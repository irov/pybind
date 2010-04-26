#	include "pybind/class_type.hpp"
#	include "pybind/class_scope.hpp"
#	include "pybind/constructor.hpp"

#	include "pybind/system.hpp"

#	include "config/python.hpp"

#	include <list>

namespace pybind
{
	static void py_dealloc( PyObject * _obj )
	{
		py_class_type * self = (py_class_type *)_obj;

		Py_XDECREF( self->dict );
		self->dict = 0;

		_obj->ob_type->tp_free( _obj );
	}

	//////////////////////////////////////////////////////////////////////////
	class_type_scope::class_type_scope()
		: m_type( 0 )
		, m_type_holder( 0 )
		, m_constructor( 0 )
	{
	}

	//////////////////////////////////////////////////////////////////////////
	class_type_scope::~class_type_scope()
	{
		for( TMethods::iterator
			it = m_methods.begin(),
			it_end = m_methods.end();
		it != it_end;
		++it )
		{
			delete *it;
		}

		for( TMembers::iterator
			it = m_members.begin(),
			it_end = m_members.end();
		it != it_end;
		++it )
		{
			delete *it;
		}

		delete m_constructor;

		delete m_type;
		delete m_type_holder;		
	}

	static int _pyinitproc(PyObject * _self, PyObject *_args, PyObject *)
	{
		py_class_type * ct = (py_class_type*)_self;
		return 1;
	}

	static PyObject* instance_get_dict(PyObject* op, void*)
	{
		py_class_type* inst = (py_class_type*)op;
		Py_INCREF( inst->dict );
		return inst->dict;
	}

	static int instance_set_dict(PyObject* op, PyObject* dict, void*)
	{
		py_class_type* inst = (py_class_type*)op;
		PyObject* last = inst->dict;
		inst->dict = dict;
		Py_DECREF( last );
		//Py_INCREF( dict );
		return 0;
	}

	static PyGetSetDef instance_getsets[] = {
		{"__dict__", instance_get_dict, instance_set_dict, NULL, 0},
		{0, 0, 0, 0, 0}
	};

	static int
		class_setattro(PyObject *obj, PyObject *name, PyObject* value)
	{
		py_class_type* inst = (py_class_type*)obj;

		if( inst->impl == 0 )
		{
			error_message( "class_setattro: unbind object" );

			return -1;
		}

		if( PyObject_GenericSetAttr( obj, name, value ) == 1 )
		{
			return 1;
		}

		int res = PyDict_SetItem( inst->dict, name, value );

		if( res )
		{
			check_error();
		}

		return res;
	}

	static PyObject *
		class_getattro(PyObject *obj, PyObject *name )
	{
		py_class_type* inst = (py_class_type*)obj;

		if( inst->impl == 0 )
		{
			error_message( "class_getattro: unbind object" );
			return 0;
		}

		if( PyObject * generic_attr = PyObject_GenericGetAttr( obj, name ) )
		{
			return generic_attr;
		}

		PyObject * attr = PyDict_GetItem( inst->dict, name );	

		if( attr == 0)
		{
			return 0;
		}

		Py_INCREF( attr );
		return attr;
	}

	static PyObject *
		class_call(PyObject *obj, PyObject *args, PyObject *keyvalues)
	{
		py_class_type* inst = (py_class_type*)obj;

		if( inst->impl == 0 )
		{
			error_message( "class_call: unbind object" );
			return 0;
		}

		PyObject * caller = PyDict_GetItemString( inst->dict, "__call__" );
		if (!caller) 
		{
			PyErr_SetString(PyExc_TypeError, "Object is not defined");
			return NULL;
		}
		if (keyvalues)
			return ask( caller, "(OOO)", obj, args, keyvalues );

		return ask( caller, "(OO)", obj, args );
	}

	void class_type_scope::setup(
		const char * _name, 
		const char * _type,
		PyObject * _module,
		newfunc _pynew,
		destructor _pydestructor)
	{
		m_type_name = _type;

		if( _module == 0 )
		{
			m_module = get_currentmodule();
		}
		else
		{
			m_module = _module;
		}

		{
			//PyObject * type_args = Py_BuildValue( "sOO", _name, PyTuple_Pack( 1, &PyBaseObject_Type ), PyDict_New() );
			//m_type = (PyTypeObject*)PyType_Type.tp_new( &PyType_Type, type_args, 0 );
			//Py_DECREF( type_args );

			m_type = new PyTypeObject();

			//m_type = (PyTypeObject*)PyType_GenericAlloc( &PyType_Type, 0 );

			m_type->tp_name = _name;
			m_type->tp_basicsize = sizeof( py_class_type );
			//m_type->tp_doc = class_type_doc;
			m_type->tp_alloc = PyType_GenericAlloc;
			m_type->tp_new = _pynew;
			m_type->tp_init = &_pyinitproc;
			m_type->tp_dealloc = _pydestructor;
			m_type->tp_flags = Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE /*| Py_TPFLAGS_HEAPTYPE*/;
			m_type->tp_getset = instance_getsets;
			m_type->tp_setattro = &class_setattro;
			m_type->tp_getattro = &class_getattro;
			m_type->tp_call = &class_call;
			m_type->tp_dictoffset = offsetof( py_class_type, dict );
			m_type->tp_base = &PyBaseObject_Type;
			m_type->tp_bases = PyTuple_Pack( 1, &PyBaseObject_Type ); 

			if( PyType_Ready( m_type ) < 0 )
			{
				printf("invalid embedding class '%s' \n", m_type->tp_name );					
			}

			Py_INCREF( m_type );
		}

		{
			//PyObject * type_args = Py_BuildValue( "sOO", _name, PyTuple_Pack( 1, &PyBaseObject_Type ), PyDict_New() );
			//m_type_holder = (PyTypeObject*)PyType_Type.tp_new( &PyType_Type, type_args, 0 );
			//Py_DECREF( type_args );

			//m_type_holder = (PyTypeObject*)PyType_GenericAlloc( &PyType_Type, 0 );

			m_type_holder = new PyTypeObject();

			m_type_holder->tp_name = _name;
			m_type_holder->tp_basicsize = sizeof( py_class_type );
			//m_type_holder->tp_doc = class_type_holder_doc;
			m_type_holder->tp_alloc = PyType_GenericAlloc;
			m_type_holder->tp_new = 0;
			m_type_holder->tp_init = &_pyinitproc;
			m_type_holder->tp_dealloc = &py_dealloc;
			m_type_holder->tp_flags = Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE /*| Py_TPFLAGS_HEAPTYPE*/;
			m_type_holder->tp_getset = instance_getsets;
			m_type_holder->tp_setattro = &class_setattro;
			m_type_holder->tp_getattro = &class_getattro;
			m_type_holder->tp_call = &class_call;
			m_type_holder->tp_dictoffset = offsetof( py_class_type, dict );
			m_type_holder->tp_base = &PyBaseObject_Type;
			m_type_holder->tp_bases = PyTuple_Pack( 1, &PyBaseObject_Type ); 

			if( PyType_Ready( m_type_holder ) < 0 )
			{
				printf("invalid embedding class holder '%s' \n", m_type_holder->tp_name );					
			}

			Py_INCREF( m_type_holder );
		}

		class_scope::reg_class_type( m_type );

		PyModule_AddObject( m_module, m_type->tp_name, (PyObject*)m_type );
		//Py_DECREF( m_type );
	}

	const char * class_type_scope::getName() const
	{
		return m_type_name;
	}

	static PyObject * method_call_callback0( PyObject * _method )
	{
		py_method_type * md = (py_method_type *)_method;

		if( md->impl == 0 )
		{
			error_message( "method_call_callback0: unbind object" );
			return 0;
		}

		return md->ifunc->call( md->impl, md->scope );
	}

	static PyObject * method_call_callback1( PyObject * _method, PyObject * _args )
	{
		py_method_type * md = (py_method_type *)_method;

		if( md->impl == 0 )
		{
			error_message( "method_call_callback1: unbind object" );
			return 0;
		}

		return md->ifunc->call( md->impl, md->scope, _args );
	}
	//////////////////////////////////////////////////////////////////////////
	void class_type_scope::set_module( PyObject * _module )
	{
		m_module = _module;
	}
	//////////////////////////////////////////////////////////////////////////
	void class_type_scope::add_method( const char * _name, method_adapter_interface * _ifunc, int _arity )
	{
		pybind_cfunction cf = (_arity)? 
			(pybind_cfunction)&method_call_callback1:
		(pybind_cfunction)&method_call_callback0;

		method_type_scope * method = new method_type_scope( m_type, _name, _ifunc, cf, _arity );

		m_methods.push_back( method );
	}
	//////////////////////////////////////////////////////////////////////////
	void class_type_scope::add_method_from_scope( class_type_scope * _basescope )
	{
		m_methodsBase.insert( 
			m_methodsBase.end(), 
			_basescope->m_methods.begin(),
			_basescope->m_methods.end() );

		m_methodsBase.insert( 
			m_methodsBase.end(), 
			_basescope->m_methodsBase.begin(),
			_basescope->m_methodsBase.end() );
	}
	//////////////////////////////////////////////////////////////////////////
	void class_type_scope::add_member( const char * _name, member_adapter_interface * _imember )
	{
		member_type_scope * member = new member_type_scope( _name, _imember );

		m_members.push_back( member );
	}
	//////////////////////////////////////////////////////////////////////////
	void class_type_scope::add_member_from_scope( class_type_scope * _basescope )
	{
		m_membersBase.insert( 
			m_membersBase.end(), 
			_basescope->m_members.begin(),
			_basescope->m_members.end() );

		m_membersBase.insert( 
			m_membersBase.end(), 
			_basescope->m_membersBase.begin(),
			_basescope->m_membersBase.end() );
	}
	//////////////////////////////////////////////////////////////////////////
	void * class_type_scope::construct( PyObject * _args )
	{
		return m_constructor->call( _args );
	}
	//////////////////////////////////////////////////////////////////////////
	void class_type_scope::def_init( constructor * _ctr )
	{
		delete m_constructor;
		m_constructor = _ctr;
	}
	//////////////////////////////////////////////////////////////////////////
	void class_type_scope::add_base( const char * _name, class_type_scope * _base, pybind_metacast _cast )
	{
		m_bases[ _name ] = std::make_pair( _base, _cast );
	}
	//////////////////////////////////////////////////////////////////////////
	void * class_type_scope::metacast( const char * _name, void * _impl )
	{
		TMapBases::iterator it_find = m_bases.find( _name );

		if( it_find == m_bases.end() )
		{
			for( TMapBases::iterator 
				it = m_bases.begin(),
				it_end = m_bases.end();
			it != it_end;
			++it)
			{
				TPairMetacast & pair = it->second;
				void * down_impl = pair.second(_impl);
				if( void * result = pair.first->metacast( _name, down_impl ) )
				{
					return result;
				}
			}

			return 0;
		}

		return it_find->second.second( _impl );
	}
	//////////////////////////////////////////////////////////////////////////
	void class_type_scope::unwrap( py_class_type * _self )
	{
		_self->impl = 0;

		//this->update_attributes_self( _self, 0 );

		Py_XDECREF( _self->dict );
		_self->dict = 0;
	}
	//////////////////////////////////////////////////////////////////////////
	bool class_type_scope::is_class( PyTypeObject * _type )
	{
		return _type->tp_setattro == &class_setattro;
	}
	//////////////////////////////////////////////////////////////////////////
	void class_type_scope::setup_attributes( py_class_type * _self )
	{
		_self->dict = PyDict_New();

		for( TMethods::iterator
			it = m_methods.begin(),
			it_end = m_methods.end();
		it != it_end;
		++it)
		{
			method_type_scope * method_type = *it;

			PyObject * py_method = method_type->instance( _self );

			if( PyDict_SetItemString( _self->dict, method_type->m_name, py_method ) == -1 )
			{
				//Py_DECREF( py_method );
				Py_DECREF( _self );

				check_error();
			}

			Py_DECREF( py_method );
		}

		for( TMethods::iterator
			it = m_methodsBase.begin(),
			it_end = m_methodsBase.end();
		it != it_end;
		++it)
		{
			method_type_scope * method_type = *it;

			PyObject * py_method = method_type->instance( _self );

			if( PyDict_SetItemString( _self->dict, method_type->m_name, py_method ) == -1 )
			{
				//Py_DECREF( py_method );
				Py_DECREF( _self );

				check_error();
			}

			Py_DECREF( py_method );
		}

		for( TMembers::iterator
			it = m_members.begin(),
			it_end = m_members.end();
		it != it_end;
		++it)
		{
			member_type_scope * member_type = *it;

			PyObject * py_member = member_type->instance( _self );

			if( PyDict_SetItemString( _self->dict, method_type->m_name, py_member ) == -1 )
			{
				Py_DECREF( _self );

				check_error();
			}

			Py_DECREF( py_member );
		}

		for( TMethods::iterator
			it = m_methodsBase.begin(),
			it_end = m_methodsBase.end();
		it != it_end;
		++it)
		{
			member_type_scope * member_type = *it;

			PyObject * py_member = member_type->instance( _self );

			if( PyDict_SetItemString( _self->dict, method_type->m_name, py_member ) == -1 )
			{
				Py_DECREF( _self );

				check_error();
			}

			Py_DECREF( py_member );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void class_type_scope::update_attributes_self( py_class_type * _self, void * _impl )
	{
		for( TMethods::iterator
			it = m_methods.begin(),
			it_end = m_methods.end();
		it != it_end;
		++it)
		{
			method_type_scope * method_type = *it;

			PyObject * py_object = PyDict_GetItemString( _self->dict, method_type->m_name );
			PyCFunctionObject * py_function = (PyCFunctionObject *)py_object;
			py_method_type * py_method = (py_method_type *)py_function->m_self;
			py_method->impl = _impl;
		}

		for( TMethods::iterator
			it = m_methodsBase.begin(),
			it_end = m_methodsBase.end();
		it != it_end;
		++it)
		{
			method_type_scope * method_type = *it;

			PyObject * py_object = PyDict_GetItemString( _self->dict, method_type->m_name );
			PyCFunctionObject * py_function = (PyCFunctionObject *)py_object;
			py_method_type * py_method = (py_method_type *)py_function->m_self;
			py_method->impl = _impl;
		}

		for( TMembers::iterator
			it = m_members.begin(),
			it_end = m_members.end();
		it != it_end;
		++it)
		{
			member_type_scope * member_type = *it;

			PyObject * py_object = PyDict_GetItemString( _self->dict, member_type->m_name );
			PyGetSetDescrObject * py_function = (PyGetSetDescrObject *)py_object;
			py_method_type * py_method = (py_method_type *)py_function->m_self;
			py_method->impl = _impl;
		}

		for( TMethods::iterator
			it = m_methodsBase.begin(),
			it_end = m_methodsBase.end();
		it != it_end;
		++it)
		{
			method_type_scope * method_type = *it;

			PyObject * py_object = PyDict_GetItemString( _self->dict, method_type->m_name );
			PyCFunctionObject * py_function = (PyCFunctionObject *)py_object;
			py_method_type * py_method = (py_method_type *)py_function->m_self;
			py_method->impl = _impl;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	PyObject * class_type_scope::create_holder( void * _impl )
	{
		py_class_type *self = (py_class_type *)PyType_GenericAlloc( m_type_holder, 0 );

		//Py_DECREF( &m_type_holder );

		self->impl = _impl;
		self->scope = this;

		setup_attributes( self );

		//Py_INCREF( self );

		return (PyObject*)self;
	}
	//////////////////////////////////////////////////////////////////////////
	PyObject * class_type_scope::create_impl( void * _impl )
	{
		py_class_type *self = 
			(py_class_type *)PyType_GenericAlloc( m_type, 0 );

		//Py_DECREF( &m_type_holder );

		self->impl = _impl;
		self->scope = this;

		setup_attributes( self );

		//Py_INCREF( self );

		return (PyObject*)self;
	}

}
