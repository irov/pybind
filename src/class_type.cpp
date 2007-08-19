#	include "pybind/class_type.hpp"
#	include "pybind/class_scope.hpp"

#	include "pybind/system.hpp"

#	include "pybind/config.hpp"

#	include <list>

namespace pybind
{
	static PyTypeObject py_empty_type = {
		PyObject_HEAD_INIT(NULL)
		0,                         /*ob_size*/
		0,				           /*tp_name*/
		0,				           /*tp_basicsize*/
		0,                         /*tp_itemsize*/
		0, /*tp_dealloc*/
		0,                         /*tp_print*/
		0,                         /*tp_getattr*/
		0,                         /*tp_setattr*/
		0,                         /*tp_compare*/
		0,                         /*tp_repr*/
		0,                         /*tp_as_number*/
		0,                         /*tp_as_sequence*/
		0,                         /*tp_as_mapping*/
		0,                         /*tp_hash */
		0,                         /*tp_call*/
		0,                         /*tp_str*/
		0,                         /*tp_getattro*/
		0,                         /*tp_setattro*/
		0,                         /*tp_as_buffer*/
		0, /*tp_flags*/
		0,           /* tp_doc */
		0,		               /* tp_traverse */
		0,		               /* tp_clear */
		0,		               /* tp_richcompare */
		0,		               /* tp_weaklistoffset */
		0,		               /* tp_iter */
		0,		               /* tp_iternext */
		0,             /* tp_methods */
		0,             /* tp_members */
		0,           /* tp_getset */
		0,                         /* tp_base */
		0,                         /* tp_dict */
		0,                         /* tp_descr_get */
		0,                         /* tp_descr_set */
		0,                         /* tp_dictoffset */
		0,      /* tp_init */
		0,                         /* tp_alloc */
		0,                 /* tp_new */
	};

	static void py_dealloc( PyObject * _obj )
	{
		_obj->ob_type->tp_free( _obj );
	}

	//////////////////////////////////////////////////////////////////////////
	class_type_scope::class_type_scope()
		: m_type( py_empty_type )
	{
	}

	static int _pyinitproc(PyObject * _self, PyObject *_args, PyObject *)
	{
		py_class_type * ct = (py_class_type*)_self;
		
		PyObject * py_dict = PyObject_GetAttrString( (PyObject *)_self, "__dict__");

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
		Py_INCREF( dict );
		return 0;
	}

	static PyGetSetDef instance_getsets[] = {
		{"__dict__",  instance_get_dict,  instance_set_dict, NULL, 0},
		{0, 0, 0, 0, 0}
	};

	static int
		class_setattro(PyObject *obj, PyObject *name, PyObject* value)
	{
		py_class_type* inst = (py_class_type*)obj;

		char *str = PyString_AsString( name );

		Py_INCREF( value );

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

		char * str = PyString_AsString( name );

		if( PyObject * item = PyDict_GetItem( inst->dict, name ) )
		{
			Py_INCREF( item );

			return item;

		}

		PyObject * pResult = PyObject_GenericGetAttr( obj, name );
		return pResult;
	}

	void class_type_scope::setup(
		const char * _name, 
		PyObject * _module,
		newfunc _pynew,
		destructor _pydestructor)
	{
		if( _module == 0 )
		{
			m_module = get_currentmodule();
		}
		else
		{
			m_module = _module;
		}

		m_type.tp_name = _name;
		m_type.tp_basicsize = sizeof( py_class_type );
		m_type.tp_doc = "Embedding class from cpp";
		m_type.tp_new = _pynew;
		m_type.tp_init = &_pyinitproc;
		m_type.tp_dealloc = _pydestructor;
		m_type.tp_flags = Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE;
		m_type.tp_getset = instance_getsets;
		m_type.tp_setattro = &class_setattro;
		m_type.tp_getattro = &class_getattro;
		

		m_type_holder = m_type;
		m_type_holder.tp_dealloc = &py_dealloc;

		if( PyType_Ready(&m_type) < 0 )
		{
			printf("invalid embedding class '%s' \n", m_type.tp_name );					
		}

		if( PyType_Ready(&m_type_holder) < 0 )
		{
			printf("invalid embedding class holder '%s' \n", m_type_holder.tp_name );					
		}

		class_scope::reg_class_type( &m_type );

		PyModule_AddObject( m_module, m_type.tp_name, (PyObject*)&m_type );
	}

	static PyObject * method_call_callback0( PyObject * _method )
	{
		py_method_type * md = (py_method_type *)_method;
		return md->ifunc->call( md->impl );
	}

	static PyObject * method_call_callback1( PyObject * _method, PyObject * _args )
	{
		py_method_type * md = (py_method_type *)_method;
		return md->ifunc->call( md->impl, _args );
	}
	//////////////////////////////////////////////////////////////////////////
	void class_type_scope::set_module( PyObject * _module )
	{
		m_module = _module;
	}
	//////////////////////////////////////////////////////////////////////////
	void class_type_scope::add_method( const char * _name, method_proxy_interface * _ifunc, int _arity )
	{
		m_methods.push_back( method_type_scope() );
		method_type_scope & method = m_methods.back();

		pybind_cfunction cf = (_arity)? 
			(pybind_cfunction)&method_call_callback1:
			(pybind_cfunction)&method_call_callback0;

		method.setup( &m_type, _name, _ifunc, cf, _arity );
	}
	//////////////////////////////////////////////////////////////////////////
	void class_type_scope::add_method_from_scope( class_type_scope * _basescope )
	{
		m_methods.insert( 
			m_methods.end(), 
			_basescope->m_methods.begin(),
			_basescope->m_methods.end() );
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
	void class_type_scope::setup_method( py_class_type * _self )
	{
		_self->dict = PyDict_New();

		for( TMethodFunction::iterator
			it = m_methods.begin(),
			it_end = m_methods.end();
		it != it_end;
		++it)
		{
			//PyObject * py_method = PyMethod_New( method.m_mdfunc, (PyObject*)_self, (PyObject *)&m_type );
			PyObject * py_method = it->instance( _self );

			int res = PyObject_SetAttrString( (PyObject*)_self, it->m_name, py_method );
			Py_DECREF( py_method );

			if( res )
			{
				check_error();
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	PyObject * class_type_scope::create_holder( void * _impl )
	{
		py_class_type *self = 
			(py_class_type *)m_type_holder.tp_alloc( &m_type_holder, 0 );

		self->impl = _impl;
		self->scope = this;

		setup_method( self );

		return (PyObject*)self;
	}

}