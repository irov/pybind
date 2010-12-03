#	include "pybind/class_type.hpp"
#	include "pybind/method_type.hpp"
#	include "pybind/member_type.hpp"
#	include "pybind/repr_adapter.hpp"
#	include "pybind/constructor.hpp"

#	include "pybind/system.hpp"
#	include "config/python.hpp"

namespace pybind
{
	//////////////////////////////////////////////////////////////////////////
	struct py_class_type{
		PyObject_HEAD
		void * impl;
		bool holder;
		class_type_scope * scope;
	};
	//////////////////////////////////////////////////////////////////////////
	static void py_dealloc( PyObject * _obj )
	{
		py_class_type * self = (py_class_type *)_obj;

		if( self->holder )
		{			
			_obj->ob_type->tp_free( _obj );
		}
		else
		{
			self->scope->m_pydestructor( _obj );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	static PyObject * py_reprfunc( PyObject * _obj )
	{
		py_class_type* inst = (py_class_type*)_obj;

		if( inst->impl == 0 )
		{
			error_message( "class_call: unbind object" );
			return 0;
		}

		return inst->scope->m_repr->repr( _obj, inst->impl, inst->scope );
	}
	//////////////////////////////////////////////////////////////////////////
	static PyObject * py_getattro( PyObject * _obj, PyObject * _key )
	{
		py_class_type* inst = (py_class_type*)_obj;

		if( inst->impl == 0 )
		{
			error_message( "class_call: unbind object" );
			return 0;
		}

		PyObject * attr = PyTuple_Pack( 1, _key );
		PyObject * res = inst->scope->m_getattro->call( inst->impl, inst->scope, attr );
		Py_DECREF( attr );

		return res;
	}
	//////////////////////////////////////////////////////////////////////////
	static PyObject * py_subscript( PyObject * _obj, PyObject * _key )
	{
		py_class_type* inst = (py_class_type*)_obj;

		if( inst->impl == 0 )
		{
			error_message( "class_call: unbind object" );
			return 0;
		}

		PyObject * attr = PyTuple_Pack( 1, _key );
		PyObject * res = inst->scope->m_mapping->call( inst->impl, inst->scope, attr );
		Py_DECREF( attr );

		return res;
	}
	//////////////////////////////////////////////////////////////////////////
	static PyMappingMethods py_as_mapping = {
		(lenfunc)0,		/* mp_length */
		(binaryfunc)py_subscript,		/* mp_subscript */
		(objobjargproc)0,	/* mp_ass_subscript */
	};
	//////////////////////////////////////////////////////////////////////////
	static PyTypeObject s_class_base_type = {
		PyVarObject_HEAD_INIT(&PyType_Type, 0)
		"pybind_class_base_type",
		0,
		0,
		0,					/* tp_dealloc */
		0,					/* tp_print */
		0,					/* tp_getattr */
		0,					/* tp_setattr */
		0,					/* tp_compare */
		0,					/* tp_repr */
		0,					/* tp_as_number */
		0,					/* tp_as_sequence */
		0,					/* tp_as_mapping */
		0,					/* tp_hash */
		0,					/* tp_call */
		0,					/* tp_str */
		0,					/* tp_getattro */
		0,					/* tp_setattro */
		0,					/* tp_as_buffer */
		Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /* tp_flags */
		0,					/* tp_doc */
		0,					/* tp_traverse */
		0,					/* tp_clear */
		0,					/* tp_richcompare */
		0,					/* tp_weaklistoffset */
		0,					/* tp_iter */
		0,					/* tp_iternext */
		0,					/* tp_methods */
		0,					/* tp_members */
		0,					/* tp_getset */
		0,					/* tp_base */
		0,					/* tp_dict */
		0,					/* tp_descr_get */
		0,					/* tp_descr_set */
	};
	//////////////////////////////////////////////////////////////////////////
	namespace detail
	{
		//////////////////////////////////////////////////////////////////////////
		typedef std::list<PyTypeObject *> TListTypeDef;
		typedef std::map<const char *, class_type_scope *> TMapTypeScope;
		//////////////////////////////////////////////////////////////////////////
		static TListTypeDef s_listTypeDef;
		static TMapTypeScope s_mapTypeScope;
		//////////////////////////////////////////////////////////////////////////
		bool is_class( PyObject * _obj )
		{
			return PyType_IsSubtype( _obj->ob_type, &s_class_base_type ) == 1;
		}
		//////////////////////////////////////////////////////////////////////////
		void * get_class_impl( PyObject * _obj )
		{
			py_class_type * self = (py_class_type *)_obj;
			return self->impl;
		}
		//////////////////////////////////////////////////////////////////////////
		class_type_scope * get_class_scope( PyObject * _obj )
		{
			py_class_type * self = (py_class_type *)_obj;
			return self->scope;
		}
		//////////////////////////////////////////////////////////////////////////
		void * unwrap( PyObject * _obj )
		{
			py_class_type * self = (py_class_type *)_obj;
			
			void * return_impl = self->impl;
			self->impl = 0;

			return return_impl;
		}
		//////////////////////////////////////////////////////////////////////////
		void wrap( PyObject * _obj, void * _impl )
		{
			py_class_type * self = (py_class_type*)_obj;
			self->impl = _impl;
		}
		//////////////////////////////////////////////////////////////////////////
		void * check_registred_class( PyObject * _obj, const std::type_info & _info )
		{
			PyObject * py_type = PyObject_Type( _obj );

			class_type_scope * type_scope = get_class_type_scope( _info );

			if( PyType_IsSubtype( (PyTypeObject *)py_type, type_scope->m_pytypeobject ) )
			{
				return get_class_impl( _obj );
			}

			return 0;
		}
		//////////////////////////////////////////////////////////////////////////
		PyObject * alloc_class( PyTypeObject * _type, PyObject * _args, void * _impl, const std::type_info & _tinfo )
		{
			py_class_type *self = 
				(py_class_type *)_type->tp_alloc( _type, 0 );

			class_type_scope * scope = get_class_type_scope( _tinfo );

			if( self != NULL )
			{
				self->impl = _impl;
				self->scope = scope;
				self->holder = false;
			}

			return (PyObject *)self;
		}
		//////////////////////////////////////////////////////////////////////////
		void * dealloc_class( PyObject * _obj )
		{
			py_class_type * self = (py_class_type *)(_obj);
			void * impl = self->impl;

			_obj->ob_type->tp_free( (PyObject*)self );

			return impl;		
		}
		//////////////////////////////////////////////////////////////////////////
		void reg_class_type( PyTypeObject * _type )
		{
			s_listTypeDef.push_back( _type );
		}
		//////////////////////////////////////////////////////////////////////////
		void reg_class_type_scope( const std::type_info & _info, class_type_scope * _scope )
		{
			const char * info_name = _info.name();
			s_mapTypeScope[ info_name ] = _scope;
		}
		//////////////////////////////////////////////////////////////////////////
		class_type_scope * get_class_type_scope( const std::type_info & _info )
		{
			const char * info_name = _info.name();
			TMapTypeScope::iterator it_find = s_mapTypeScope.find( info_name );

			if( it_find == s_mapTypeScope.end() )
			{
				return 0;
			}

			return it_find->second;		
		}
	}
	//////////////////////////////////////////////////////////////////////////
	class_type_scope::class_type_scope( const char * _name, const char * _type_name )
		: m_name(_name)
		, m_type(_type_name)
		, m_pyconstructor(0)
		, m_pydestructor(0)
		, m_pytypeobject(0)
		, m_convert(0)
		, m_repr(0)
		, m_getattro(0)
		, m_mapping(0)
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

		delete m_pyconstructor;

		Py_XDECREF( m_pytypeobject );
		delete m_pytypeobject;
	}
	//////////////////////////////////////////////////////////////////////////
	void class_type_scope::setup( PyObject * _module, newfunc _pynew, destructor _pydestructor )
	{
		if( _module == 0 )
		{
			m_module = get_currentmodule();
		}
		else
		{
			m_module = _module;
		}
		
		m_pytypeobject = new PyTypeObject();

		m_pydestructor = _pydestructor;

		m_pytypeobject->tp_name = m_name;
		m_pytypeobject->tp_basicsize = sizeof( py_class_type );
		m_pytypeobject->tp_new = _pynew;
		m_pytypeobject->tp_dealloc = &py_dealloc;
		m_pytypeobject->tp_flags = Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE /*| Py_TPFLAGS_HEAPTYPE*/;

		if( m_bases.empty() )
		{
			Py_INCREF(&s_class_base_type);
			m_pytypeobject->tp_base = &s_class_base_type;
			m_pytypeobject->tp_bases = PyTuple_Pack( 1, &s_class_base_type );
		}
		else
		{
			PyObject * py_bases = PyTuple_New( m_bases.size() );

			int index = 0;

			for( TMapBases::iterator
				it = m_bases.begin(),
				it_end = m_bases.end();
			it != it_end;
			++it)
			{
				PyTypeObject * py_base = it->second.first->m_pytypeobject;
				PyTuple_SetItem( py_bases, index++, (PyObject*)py_base );
			}

			m_pytypeobject->tp_bases = py_bases;
			m_pytypeobject->tp_base = (PyTypeObject*)PyTuple_GetItem( py_bases, 0 );
		}

		if( PyType_Ready( m_pytypeobject ) < 0 )
		{
			printf("invalid embedding class '%s' \n", m_pytypeobject->tp_name );					
		}

		Py_INCREF( m_pytypeobject );

		detail::reg_class_type( m_pytypeobject );

		PyModule_AddObject( m_module, m_pytypeobject->tp_name, (PyObject*)m_pytypeobject );
		//Py_DECREF( m_pytypeobject );
	}
	//////////////////////////////////////////////////////////////////////////
	const char * class_type_scope::get_name() const
	{
		return m_type;
	}
	//////////////////////////////////////////////////////////////////////////
	void class_type_scope::set_module( PyObject * _module )
	{
		m_module = _module;
	}
	//////////////////////////////////////////////////////////////////////////
	void class_type_scope::add_method( const char * _name, method_adapter_interface * _ifunc, int _arity )
	{
		method_type_scope * method = new method_type_scope( _name, _ifunc );

		m_methods.push_back( method );

		PyObject * py_type_method = method->instance( m_pytypeobject );

		if( PyDict_SetItemString( m_pytypeobject->tp_dict, _name, py_type_method ) == -1 )
		{
			check_error();
		}

		Py_DECREF( py_type_method );
	}
	//////////////////////////////////////////////////////////////////////////
	void class_type_scope::add_member( const char * _name, member_adapter_interface * _imember )
	{
		member_type_scope * member = new member_type_scope( _name, _imember );

		m_members.push_back( member );

		PyObject * py_member = member->instance();

		if( PyDict_SetItemString( m_pytypeobject->tp_dict, member->m_name, py_member ) == -1 )
		{
			check_error();
		}

		Py_DECREF( py_member );
	}
	//////////////////////////////////////////////////////////////////////////
	void class_type_scope::add_convert( convert_adapter_interface * _iconvert )
	{
		m_convert = _iconvert;
	}
	//////////////////////////////////////////////////////////////////////////
	convert_adapter_interface * class_type_scope::get_convert()
	{
		return m_convert;
	}
	//////////////////////////////////////////////////////////////////////////
	void class_type_scope::add_repr( repr_adapter_interface * _irepr )
	{
		m_repr = _irepr;

		m_pytypeobject->tp_repr = &py_reprfunc;
	}
	//////////////////////////////////////////////////////////////////////////
	void class_type_scope::add_getattro( method_adapter_interface * _igetattro )
	{
		m_getattro = _igetattro;

		m_pytypeobject->tp_getattro = &py_getattro;
	}
	//////////////////////////////////////////////////////////////////////////
	void class_type_scope::add_mapping( method_adapter_interface * _imapping )
	{
		m_mapping = _imapping;

		m_pytypeobject->tp_as_mapping = &py_as_mapping;
	}
	//////////////////////////////////////////////////////////////////////////
	void * class_type_scope::construct( PyObject * _args )
	{
		return m_pyconstructor->call( _args );
	}
	//////////////////////////////////////////////////////////////////////////
	void class_type_scope::def_init( constructor * _ctr )
	{
		delete m_pyconstructor;
		m_pyconstructor = _ctr;
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

		if( it_find != m_bases.end() )
		{
			return it_find->second.second( _impl );
		}
		
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
	//////////////////////////////////////////////////////////////////////////
	PyObject * class_type_scope::create_holder( void * _impl )
	{
		py_class_type *self = (py_class_type *)PyType_GenericAlloc( m_pytypeobject, 0 );

		self->impl = _impl;
		self->scope = this;
		self->holder = true;

		return (PyObject*)self;
	}
	//////////////////////////////////////////////////////////////////////////
	PyObject * class_type_scope::create_impl( void * _impl )
	{
		py_class_type *self = 
			(py_class_type *)PyType_GenericAlloc( m_pytypeobject, 0 );

		self->impl = _impl;
		self->scope = this;
		self->holder = false;

		return (PyObject*)self;
	}
	//////////////////////////////////////////////////////////////////////////
	void initialize_classes()
	{
		if( PyType_Ready( &s_class_base_type ) < 0 )
		{
			printf("invalid embedding class '%s' \n", s_class_base_type.tp_name );
		}
	}
}
