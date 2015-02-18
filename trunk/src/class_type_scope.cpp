#	include "pybind/class_type_scope.hpp"

#	include "pybind/repr_adapter.hpp"
#	include "pybind/hash_adapter.hpp"
#	include "pybind/convert_adapter.hpp"
#	include "pybind/compare_adapter.hpp"
#	include "pybind/constructor_adapter.hpp"

#	include "pybind/exception.hpp"

#	include "pybind/system.hpp"
#	include "config/python.hpp"

#	include "method_type.hpp"
#	include "member_type.hpp"

#   include "static_var.hpp"

#	include "pybind/debug.hpp"

namespace pybind
{
	//////////////////////////////////////////////////////////////////////////
	struct py_pod64_object
	{
		PyObject_HEAD
			char buff[PYBIND_OBJECT_POD64_SIZE];
	};
	//////////////////////////////////////////////////////////////////////////	
    STATIC_DECLARE(PyObject *, s_pybind_object_impl);
    STATIC_DECLARE(PyObject *, s_pybind_class_type_scope);
    STATIC_DECLARE(PyObject *, s_pybind_object_holder);
	//////////////////////////////////////////////////////////////////////////
	static void py_pod64_dealloc( PyObject * _obj )
	{
		PyTypeObject * objtype = Py_TYPE(_obj);

		objtype->tp_free( _obj );
	}
    //////////////////////////////////////////////////////////////////////////
	STATIC_DECLARE_VALUE_BEGIN(PyTypeObject, s_pod64_type)
	{
		PyVarObject_HEAD_INIT(&PyType_Type, 0)
			"pod64_type",
			sizeof(py_pod64_object),
			0,
			&py_pod64_dealloc,                             /* tp_dealloc */
			0,                    /* tp_print */
			0,                                          /* tp_getattr */
			0,                                          /* tp_setattr */
			0,                                          /* tp_compare */
			0,                                /* tp_repr */
			0,                          /* tp_as_number */
			0,                        /* tp_as_sequence */
			0,                         /* tp_as_mapping */
			0,                      /* tp_hash */
			0,                                          /* tp_call */
			0,                                 /* tp_str */
			0,                    /* tp_getattro */
			0,                                          /* tp_setattro */
			0,                          /* tp_as_buffer */
			Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,              /* tp_flags */
			0,                                 /* tp_doc */
			0,                                          /* tp_traverse */
			0,                                          /* tp_clear */
			0,            /* tp_richcompare */
			0,                                          /* tp_weaklistoffset */
			0,                                          /* tp_iter */
			0,                                          /* tp_iternext */
			0,                             /* tp_methods */
			0,                                          /* tp_members */
			0,                                          /* tp_getset */
			0,                         /* tp_base */
			0,                                          /* tp_dict */
			0,                                          /* tp_descr_get */
			0,                                          /* tp_descr_set */
			0,                                          /* tp_dictoffset */
			0,                                          /* tp_init */
			0,                                          /* tp_alloc */
			0,                                 /* tp_new */
			0,                               /* tp_free */
	}
	STATIC_DECLARE_VALUE_END();
	//////////////////////////////////////////////////////////////////////////
	static void py_dealloc( PyObject * _obj )
	{
		PyTypeObject * objtype = Py_TYPE(_obj);

		objtype->tp_free( _obj );
	}
	//////////////////////////////////////////////////////////////////////////
	STATIC_DECLARE_VALUE_BEGIN(PyTypeObject, s_base_type)
	{
		PyVarObject_HEAD_INIT(&PyType_Type, 0)
		"pybind_base_type",
		sizeof(PyObject),
		0,
		&py_dealloc,                             /* tp_dealloc */
		0,                    /* tp_print */
		0,                                          /* tp_getattr */
		0,                                          /* tp_setattr */
		0,                                          /* tp_compare */
		0,                                /* tp_repr */
		0,                          /* tp_as_number */
		0,                        /* tp_as_sequence */
		0,                         /* tp_as_mapping */
		0,                      /* tp_hash */
		0,                                          /* tp_call */
		0,                                 /* tp_str */
		0,                    /* tp_getattro */
		0,                                          /* tp_setattro */
		0,                          /* tp_as_buffer */
		Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,              /* tp_flags */
		0,                                 /* tp_doc */
		0,                                          /* tp_traverse */
		0,                                          /* tp_clear */
		0,            /* tp_richcompare */
		0,                                          /* tp_weaklistoffset */
		0,                                          /* tp_iter */
		0,                                          /* tp_iternext */
		0,                             /* tp_methods */
		0,                                          /* tp_members */
		0,                                          /* tp_getset */
		0,                         /* tp_base */
		0,                                          /* tp_dict */
		0,                                          /* tp_descr_get */
		0,                                          /* tp_descr_set */
		0,                                          /* tp_dictoffset */
		0,                                          /* tp_init */
		0,                                          /* tp_alloc */
		0,                                 /* tp_new */
		0,                               /* tp_free */
	}
	STATIC_DECLARE_VALUE_END();
	//////////////////////////////////////////////////////////////////////////
	namespace detail
	{
		static const uint32_t invalid_info = 0;
        //////////////////////////////////////////////////////////////////////////
		static class_type_scope_ptr & modify_class_type_scope( uint32_t _info )
		{
			static class_type_scope_ptr g_class_type_scope[PYBIND_TYPE_COUNT];

			class_type_scope_ptr & scope = g_class_type_scope[_info];

			return scope;
		}		
		//////////////////////////////////////////////////////////////////////////
		static void initialize_class_type_scope()
		{
			for( uint32_t index = 0; index != PYBIND_TYPE_COUNT; ++index )
			{
				modify_class_type_scope( index ) = nullptr;
			}
		}
		//////////////////////////////////////////////////////////////////////////
		static void finalize_class_type_scope()
		{
			for( uint32_t index = 0; index != PYBIND_TYPE_COUNT; ++index )
			{
				class_type_scope_ptr & scope = modify_class_type_scope( index );

				if( scope != nullptr )
				{
					scope->finalize();
					scope = nullptr;
				}
			}
		}
        //////////////////////////////////////////////////////////////////////////
        bool reg_class_type_scope( uint32_t _info, const class_type_scope_ptr & _scope )
        {
			if( _info == invalid_info )
			{
				return false;
			}

            modify_class_type_scope( _info ) = _scope;

			return true;
        }
        //////////////////////////////////////////////////////////////////////////
        const class_type_scope_ptr & get_class_type_scope( uint32_t _info )
        {
            const class_type_scope_ptr & scope = modify_class_type_scope( _info );

            return scope;	
        }
		//////////////////////////////////////////////////////////////////////////
		const class_type_scope_ptr & get_invalid_scope()
		{
			const class_type_scope_ptr & scope = modify_class_type_scope( invalid_info );

			return scope;	
		}
        //////////////////////////////////////////////////////////////////////////
        void visit_types_scope( visitor_class_type_scope * _getter )
        {
            for( uint32_t index = 0; index != PYBIND_TYPE_COUNT; ++index )
            {
				const class_type_scope_ptr & scope_ptr = detail::get_class_type_scope( index );

                if( scope_ptr == nullptr )
                {
                    continue;
                }

                class_type_scope * scope = scope_ptr.get();

                _getter->visit_scope( scope );
            }
        }
		//////////////////////////////////////////////////////////////////////////
		bool is_class( PyObject * _obj )
		{
			PyTypeObject * objtype = Py_TYPE(_obj);

			if( PyType_IsSubtype( objtype, &STATIC_VAR(s_base_type) ) == 1 )
			{
				return true;
			}

			return false;
		}
		//////////////////////////////////////////////////////////////////////////
		void * get_class_impl( PyObject * _obj )
		{
			PyObject * py_impl = PyObject_GetAttr( _obj, STATIC_VAR(s_pybind_object_impl) );

			if( py_impl == nullptr )
			{	
				pybind::throw_exception("obj %s not wrap pybind (impl)"
					, pybind::object_str( _obj )
					);

				return nullptr;
			}

			Py_DECREF( py_impl );
			
			PyTypeObject * py_self_type = Py_TYPE(py_impl);

			if( py_self_type == &STATIC_VAR(s_pod64_type) )
			{
				py_pod64_object * py_pod64 = (py_pod64_object *)py_impl;

				void * buff = py_pod64->buff;

				return buff;
			}

			if( is_none( py_impl ) == true )
			{
				return nullptr;
			}
            
            if( void_ptr_check( py_impl ) == false )
            {
				pybind::throw_exception("obj %s incorrect wrap pybind"
					, pybind::object_str( _obj )
					);

                return nullptr;
            }

            void * impl = void_ptr_get( py_impl );				

			return impl;
		}
		//////////////////////////////////////////////////////////////////////////
		const class_type_scope_ptr & get_class_scope( PyTypeObject * _type )
		{
			PyObject * py_scope = PyObject_GetAttr( (PyObject*)_type, STATIC_VAR(s_pybind_class_type_scope) );

			if( py_scope == nullptr )
			{
				pybind::throw_exception("obj %s not wrap pybind (scope)"
					, pybind::object_str( (PyObject *)_type )
					);
				
                const class_type_scope_ptr & scope_null = detail::get_class_type_scope( 0 );

                return scope_null;
			}

			Py_DECREF( py_scope );

            uint32_t id;
            if( pybind::extract_value( py_scope, id ) == false )
            {
				pybind::throw_exception("obj %s incorrect wrap pybind (scope)"
					, pybind::object_str( (PyObject *)_type )
					);

				const class_type_scope_ptr & scope_null = detail::get_invalid_scope();

                return scope_null;
            }        

			const class_type_scope_ptr & scope = detail::get_class_type_scope( id );

            if( scope == nullptr )
            {
				pybind::throw_exception("obj %s incorrect wrap pybind (id %d)"
					, pybind::object_str( (PyObject *)_type )
					, id
					);

				const class_type_scope_ptr & scope_null = detail::get_invalid_scope();

                return scope_null;
            }
            
			return scope;
		}
		//////////////////////////////////////////////////////////////////////////
		void * unwrap( PyObject * _obj )
		{
			void * impl = pybind::detail::get_class_impl( _obj );

            if( PyObject_SetAttr( _obj, STATIC_VAR(s_pybind_object_impl), Py_None ) == -1 )
            {
                pybind::throw_exception("unwrap obj %s not pybind"
					, pybind::object_str( _obj )
					);

                return nullptr;
            }

			return impl;
		}
		//////////////////////////////////////////////////////////////////////////
		bool is_wrap( PyObject * _obj )
		{
			PyObject * py_impl = PyObject_GetAttr( _obj, STATIC_VAR(s_pybind_object_impl) );

			if( py_impl == nullptr )
			{
				pybind::throw_exception("is_wrap obj %s not pybind"
					, pybind::object_str( _obj )
					);

				return false;
			}

			Py_DECREF( py_impl );

			bool result = void_ptr_check( py_impl );			

			return result;
		}
		//////////////////////////////////////////////////////////////////////////
		static void wrap( PyObject * _obj, void * _impl, bool _holder )
		{
			PyObject * py_impl = void_ptr_new( _impl );
            
			if( PyObject_SetAttr( _obj, STATIC_VAR(s_pybind_object_impl), py_impl ) == -1 )
            {
				pybind::throw_exception("wrap obj %s python error (impl)"
					, pybind::object_str( _obj )
					);

                return;
            }

			Py_DECREF( py_impl );

            PyObject * py_holder = get_bool( _holder );
	
			if( PyObject_SetAttr( _obj, STATIC_VAR(s_pybind_object_holder), py_holder ) == -1 )
            {
				pybind::throw_exception("wrap obj %s python error (holder)"
					, pybind::object_str( _obj )
					);

                return;
            }
		}
		//////////////////////////////////////////////////////////////////////////
		static bool wrap_pod64( PyObject * _obj, void ** _impl, size_t & _size )
		{
			PyObject * py_impl = PyType_GenericAlloc( &STATIC_VAR(s_pod64_type), 0 );

			if( PyObject_SetAttr( _obj, STATIC_VAR(s_pybind_object_impl), py_impl ) == -1 )
            {
				pybind::throw_exception("wrap_pod64 obj %s python error (impl)"
					, pybind::object_str( _obj )
					);

                return false;
            }

			Py_DECREF( py_impl );

			py_pod64_object * py_pod64 = (py_pod64_object *)py_impl;

			*_impl = (void *)py_pod64->buff;

			if( PyObject_SetAttr( _obj, STATIC_VAR(s_pybind_object_holder), Py_False ) == -1 )
            {
				pybind::throw_exception("wrap_pod64 obj %s python error (holder)" 
					, pybind::object_str( _obj )
					);

                return false;
            }

			_size = PYBIND_OBJECT_POD64_SIZE;

			return true;
		}
		//////////////////////////////////////////////////////////////////////////
		static bool is_holder( PyObject * _obj )
		{
			PyObject * py_holder = PyObject_GetAttr( _obj, STATIC_VAR(s_pybind_object_holder) );

			if( py_holder == nullptr )
			{
				pybind::throw_exception("is_holder obj %s not pybind (holder)"
					, pybind::object_str( _obj )
					);

				return false;
			}

			Py_DECREF( py_holder );
			
			if( pybind::bool_check( py_holder ) == false )
			{
				pybind::throw_exception("is_holder obj %s incorrect wrap (holder)"
					, pybind::object_str( _obj )
					);

				return false;
			}

			bool result = (py_holder == Py_True);

			return result;
		}
		//////////////////////////////////////////////////////////////////////////
		void * check_registred_class( PyObject * _obj, uint32_t _info )
		{
			PyTypeObject * py_type = Py_TYPE(_obj);

			const class_type_scope_ptr & type_scope = detail::get_class_type_scope( _info );

			if( PyType_IsSubtype( py_type, type_scope->m_pytypeobject ) == 0 )
			{
				return nullptr;
			}

			void * impl = detail::get_class_impl( _obj );

			return impl;
		}
        //////////////////////////////////////////////////////////////////////////
        void * meta_cast_scope( void * _self, uint32_t _typeId, uint32_t _name, const class_type_scope_ptr & scope )
        {
            uint32_t class_name = scope->get_type_id();

            if( class_name == _typeId )
            {
                return _self;
            }

            void * impl_cast = scope->metacast( _name, _self );

            return impl_cast;
        }
		//////////////////////////////////////////////////////////////////////////
		static PyObject * alloc_class( PyTypeObject * _type, PyObject * _args, PyObject * _kwds )
		{
            (void)_args;
            (void)_kwds;

			PyObject * py_self = PyBaseObject_Type.tp_alloc(_type, 0);

			if( py_self == nullptr )
			{
				if (PyErr_Occurred())
				{
					PyErr_Print();
				}
				
				return nullptr;
			}			

			return py_self;
		}
    }
    //////////////////////////////////////////////////////////////////////////
    static PyObject * py_callfunc( PyObject * _obj, PyObject * _args, PyObject * _kwds )
    {
        void * impl = pybind::detail::get_class_impl( _obj );

        if( impl == nullptr )
        {
            pybind::error_message( "pybind: callfunc unbind object" );

            return nullptr;
        }

		PyTypeObject * objtype = Py_TYPE(_obj);
        const class_type_scope_ptr & scope = pybind::detail::get_class_scope( objtype );

		const method_adapter_interface_ptr & adapter = scope->m_call;

		try
		{
			DEBUG_PYBIND_NOTIFY_BEGIN_BIND_CALL( scope->get_name(), adapter->getName(), _args, _kwds );
			PyObject * ret = adapter->call( impl, scope, _args, _kwds );
			DEBUG_PYBIND_NOTIFY_END_BIND_CALL( scope->get_name(), adapter->getName(), _args, _kwds );
			
			return ret;
		}
		catch( const pybind_exception & _ex )
		{
			pybind::error_message("obj %s py_callfunc invalid call '%s' error '%s'\n"
				, pybind::object_str( _obj )
				, adapter->getName()
				, _ex.what()
				);
		}

		return nullptr;
    }
	//////////////////////////////////////////////////////////////////////////
	static PyObject * py_reprfunc( PyObject * _obj )
	{
		void * impl = pybind::detail::get_class_impl( _obj );

		if( impl == nullptr )
		{
			pybind::error_message( "pybind: reprfunc unbind object" );

			return nullptr;
		}

		PyTypeObject * objtype = Py_TYPE(_obj);
		const class_type_scope_ptr & scope = pybind::detail::get_class_scope( objtype );

		const repr_adapter_interface_ptr & adapter = scope->m_repr;

		try
		{
			PyObject * ret = adapter->repr( _obj, impl, scope );

			return ret;
		}
		catch( const pybind_exception & _ex )
		{
			pybind::error_message("obj %s py_reprfunc invalid call error '%s'\n"
				, pybind::object_str( _obj )
				, _ex.what()
				);
		}

		return nullptr;
	}
    //////////////////////////////////////////////////////////////////////////
    static long py_hash( PyObject * _obj )
    {
        void * impl = pybind::detail::get_class_impl( _obj );

        if( impl == nullptr )
        {
            pybind::error_message( "pybind: reprfunc unbind object" );

            return 0;
        }

		PyTypeObject * objtype = Py_TYPE(_obj);
        const class_type_scope_ptr & scope = pybind::detail::get_class_scope( objtype );

		const hash_adapter_interface_ptr & adapter = scope->m_hash;

		try
		{
			uint32_t hash = adapter->hash( _obj, impl, scope );

			return (long)hash;
		}
		catch( const pybind_exception & _ex )
		{
			pybind::error_message("obj %s py_hash invalid call '%s' error '%s'\n"
				, pybind::object_str( _obj )
				, _ex.what()
				);
		}

		return 0;
    }
    //////////////////////////////////////////////////////////////////////////
    static PyObject * py_richcompare( PyObject * _obj, PyObject * _compare, int _op )
    {
        void * impl = pybind::detail::get_class_impl( _obj );

        if( impl == nullptr )
        {
            pybind::error_message( "py_richcompare %s compare unbind object"
				, pybind::object_str( _obj )
				);

            Py_INCREF(Py_NotImplemented);

            return Py_NotImplemented;
        }

		PyTypeObject * objtype = Py_TYPE(_obj);
        const class_type_scope_ptr & scope = pybind::detail::get_class_scope( objtype );

        PybindOperatorCompare pybind_op = POC_Less;

        switch( _op )
        {
        case Py_LT:
            {
                pybind_op = POC_Less;
            }break;            
        case Py_LE:
            {
                pybind_op = POC_Lessequal;
            }break;
        case Py_EQ:
            {
                pybind_op = POC_Equal;
            }break;
        case Py_NE:
            {
                pybind_op = POC_Notequal;
            }break;
        case Py_GT:
            {
                pybind_op = POC_Great;
            }break;
        case Py_GE:
            {
                pybind_op = POC_Greatequal;
            }break;
        default:
            {
				pybind::error_message("invalid compare op '%d'\n"
                    , _op
                    );
            }break;
        }

        try
        {
			bool test_result;

            if( scope->m_compare->compare( _obj, impl, scope, _compare, pybind_op, test_result ) == false )
            {
                Py_INCREF(Py_NotImplemented);

                return Py_NotImplemented;
            }

			PyObject * py_result = pybind::ret_bool( test_result );

			return py_result;
        }
        catch( const pybind_exception & _ex )
        {
            pybind::error_message("obj %s py_richcompare %s invalid compare %s\n"
				, pybind::object_str( _obj )
				, pybind::object_str( _obj )
				, _ex.what()
                );            
        }

		return nullptr;
    }
	//////////////////////////////////////////////////////////////////////////
	static PyObject * py_getattro( PyObject * _obj, PyObject * _key )
	{
		void * impl = pybind::detail::get_class_impl( _obj );

		if( impl == nullptr )
		{
			pybind::error_message( "getattro %s unbind object" 
				, pybind::object_str( _obj )
				);

			return nullptr;
		}

		PyTypeObject * objtype = Py_TYPE(_obj);
		const class_type_scope_ptr & scope = pybind::detail::get_class_scope( objtype );

		const method_adapter_interface_ptr & adapter = scope->m_getattro;

		try
		{
			PyObject * attr = PyTuple_Pack( 1, _key );
			DEBUG_PYBIND_NOTIFY_BEGIN_BIND_CALL( scope->get_name(), adapter->getName(), attr, nullptr );
			PyObject * res = adapter->call( impl, scope, attr, nullptr );
			DEBUG_PYBIND_NOTIFY_END_BIND_CALL( scope->get_name(), adapter->getName(), attr, nullptr );
			Py_DECREF( attr );

			return res;
		}
		catch( const pybind_exception & _ex )
		{
			pybind::error_message("obj %s py_getattro invalid call '%s' error '%s'\n"
				, pybind::object_str( _obj )
				, adapter->getName()
				, _ex.what()
				);
		}

		return nullptr;
	}
	//////////////////////////////////////////////////////////////////////////
	static PyObject * py_subscript( PyObject * _obj, PyObject * _key )
	{
		void * impl = pybind::detail::get_class_impl( _obj );

		if( impl == nullptr )
		{
			pybind::error_message( "pybind: subscript unbind object" );

			return nullptr;
		}

		PyTypeObject * objtype = Py_TYPE(_obj);
		const class_type_scope_ptr & scope = pybind::detail::get_class_scope( objtype );

		const method_adapter_interface_ptr & adapter = scope->m_mapping;

		try
		{
			PyObject * attr = PyTuple_Pack( 1, _key );
			DEBUG_PYBIND_NOTIFY_BEGIN_BIND_CALL( scope->get_name(), adapter->getName(), attr, nullptr );
			PyObject * res = adapter->call( impl, scope, attr, nullptr );
			DEBUG_PYBIND_NOTIFY_END_BIND_CALL( scope->get_name(), adapter->getName(), attr, nullptr );
			Py_DECREF( attr );

			return res;
		}
		catch( const pybind_exception & _ex )
		{
			pybind::error_message("obj %s py_subscript invalid call '%s' error '%s'\n"
				, pybind::object_str( _obj )
				, adapter->getName()
				, _ex.what()
				);
		}

		return nullptr;
	}
	//////////////////////////////////////////////////////////////////////////
	static PyMappingMethods py_as_mapping = {
		(lenfunc)0,		/* mp_length */
		(binaryfunc)py_subscript,		/* mp_subscript */
		(objobjargproc)0,	/* mp_ass_subscript */
	};
	//////////////////////////////////////////////////////////////////////////
	static PyObject * py_item_get( PyObject * _obj, Py_ssize_t _index )
	{
		void * impl = pybind::detail::get_class_impl( _obj );

		if( impl == nullptr )
		{
			pybind::error_message( "pybind: subscript unbind object" );

			return nullptr;
		}

		PyTypeObject * objtype = Py_TYPE(_obj);
		const class_type_scope_ptr & scope = pybind::detail::get_class_scope( objtype );

		const method_adapter_interface_ptr & adapter = scope->m_sequence_get;

		try
		{
			PyObject * py_index = pybind::ptr( _index );
			PyObject * attr = PyTuple_Pack( 1, py_index );
			pybind::decref( py_index );

			DEBUG_PYBIND_NOTIFY_BEGIN_BIND_CALL( scope->get_name(), adapter->getName(), attr, nullptr );
			PyObject * res = adapter->call( impl, scope, attr, nullptr );
			DEBUG_PYBIND_NOTIFY_END_BIND_CALL( scope->get_name(), adapter->getName(), attr, nullptr );

			Py_DECREF( attr );

			return res;
		}
		catch( const pybind_exception & _ex )
		{
			pybind::error_message("obj %s py_item_get invalid call '%s' error '%s'\n"
				, pybind::object_str( _obj )
				, adapter->getName()
				, _ex.what()
				);
		}

		return nullptr;
	}
	//////////////////////////////////////////////////////////////////////////
	static int py_item_set( PyObject * _obj, Py_ssize_t _index, PyObject * _value )
	{
		void * impl = pybind::detail::get_class_impl( _obj );

		if( impl == nullptr )
		{
			pybind::error_message( "pybind: subscript unbind object" );

			return -1;
		}

		PyTypeObject * objtype = Py_TYPE(_obj);
		const class_type_scope_ptr & scope = pybind::detail::get_class_scope( objtype );

		const method_adapter_interface_ptr & adapter = scope->m_sequence_set;

		try
		{
			PyObject * py_index = pybind::ptr( _index );
			PyObject * attr = PyTuple_Pack( 2, py_index, _value );
			pybind::decref( py_index );			

			DEBUG_PYBIND_NOTIFY_BEGIN_BIND_CALL( scope->get_name(), adapter->getName(), attr, nullptr );
			PyObject * res = adapter->call( impl, scope, attr, nullptr );
			DEBUG_PYBIND_NOTIFY_END_BIND_CALL( scope->get_name(), adapter->getName(), attr, nullptr );

			Py_DECREF( attr );
			Py_DECREF( res );

			return 0;
		}
		catch( const pybind_exception & _ex )
		{
			pybind::error_message("obj %s py_item_set invalid call '%s' error '%s'\n"
				, pybind::object_str( _obj )
				, adapter->getName()
				, _ex.what()
				);
		}

		return -1;
	}
	//////////////////////////////////////////////////////////////////////////
	static PySequenceMethods py_as_sequence = {
		(lenfunc)0,                       /* sq_length */
		(binaryfunc)0,                    /* sq_concat */
		(ssizeargfunc)0,                  /* sq_repeat */
		(ssizeargfunc)py_item_get,                    /* sq_item */
		(ssizessizeargfunc)0,              /* sq_slice */
		(ssizeobjargproc)py_item_set,             /* sq_ass_item */
		(ssizessizeobjargproc)0,       /* sq_ass_slice */
		(objobjproc)0,                  /* sq_contains */
		(binaryfunc)0,            /* sq_inplace_concat */
		(ssizeargfunc)0,          /* sq_inplace_repeat */
	};
	//////////////////////////////////////////////////////////////////////////
	static PyObject * py_new_class( PyTypeObject * _type, PyObject * _args, PyObject * _kwds )
	{
		const class_type_scope_ptr & scope = pybind::detail::get_class_scope( _type );
		
		try
		{
			PyObject * py_self = detail::alloc_class( _type, _args, _kwds );

			void * impl = (*scope->m_pynew)( scope, py_self, _args, _kwds );

			if( impl == nullptr )
			{
				Py_DECREF( py_self );

				return nullptr;
			}

			pybind::detail::wrap( py_self, impl, false );

			scope->addObject( py_self );

			return py_self;
		}
		catch( const pybind_exception & _ex )
		{
			pybind::error_message("obj %s py_new_class scope '%s' error '%s'\n"
				, pybind::object_str( (PyObject *)_type )
				, scope->get_name()
				, _ex.what()
				);
		}

		return nullptr;
	}
	//////////////////////////////////////////////////////////////////////////
	static void py_del_class( PyObject * _obj )
	{
		PyTypeObject * objtype = Py_TYPE(_obj);

		const class_type_scope_ptr & scope = pybind::detail::get_class_scope( objtype );

		try
		{
			scope->removeObject( _obj );
		
			bool holder = pybind::detail::is_holder( _obj );

			if( holder == false )
			{
				void * impl = pybind::detail::get_class_impl( _obj );

				(scope->m_pydestructor)( scope, impl );            		
			}
		}
		catch( const pybind_exception & _ex )
		{
			pybind::error_message("obj %s py_del_class scope %s error '%s'\n"
				, pybind::object_str( _obj )
				, scope->get_name()
				, _ex.what()
				);
		}
	}
    //////////////////////////////////////////////////////////////////////////
    static PyObject * py_new_pod( PyTypeObject * _type, PyObject * _args, PyObject * _kwds )
    {
        const class_type_scope_ptr & scope = pybind::detail::get_class_scope(_type);

		try
		{
			PyObject * py_self = detail::alloc_class( _type, _args, _kwds );

			void * buff = nullptr;
			size_t size;
			pybind::detail::wrap_pod64( py_self, &buff, size );

			void * impl = (*scope->m_pynew)( scope, py_self, _args, _kwds );

			(void)impl;

			scope->addObject( py_self );
        
			return py_self;
		}
		catch( const pybind_exception & _ex )
		{
			pybind::error_message("obj %s py_new_class scope '%s' error '%s'\n"
				, pybind::object_str( (PyObject *)_type )
				, scope->get_name()
				, _ex.what()
				);
		}

		return nullptr;
    }
	//////////////////////////////////////////////////////////////////////////
	static void py_del_pod( PyObject * _obj )
	{
		PyTypeObject * objtype = Py_TYPE(_obj);

		const class_type_scope_ptr & scope = pybind::detail::get_class_scope( objtype );

		try
		{
			scope->removeObject( _obj );

			void * impl = pybind::detail::get_class_impl( _obj );

			(scope->m_pydestructor)( scope, impl ); 
		}
		catch( const pybind_exception & _ex )
		{
			pybind::error_message("obj %s py_del_pod scope '%s' error '%s'\n"
				, pybind::object_str( _obj )
				, scope->get_name()
				, _ex.what()
				);
		}
	}
	//////////////////////////////////////////////////////////////////////////
	class_type_scope::class_type_scope( const char * _name, uint32_t _typeId, PyObject * _module, void * _user, pybind_new _pynew, pybind_destructor _pydestructor, bool _pod )
		: m_name(_name)
		, m_typeId(_typeId)
		, m_module(_module)
        , m_user(_user)
        , m_objectCount(0)
		, m_pynew(_pynew)
		, m_pydestructor(_pydestructor)
        , m_pod(_pod)
		, m_pytypeobject(nullptr)
		, m_basesCount(0)
	{
        //Py_INCREF( STATIC_VAR(s_pybind_class_type_scope) );
        //Py_INCREF( STATIC_VAR(s_pybind_class_type_scope) ); //Double - memory leak! yeaaa
	}
	//////////////////////////////////////////////////////////////////////////
	class_type_scope::~class_type_scope()
	{        
	}
	//////////////////////////////////////////////////////////////////////////
	void class_type_scope::initialize()
	{
		if( m_module == nullptr )
		{
			m_module = pybind::get_currentmodule();

            if( m_module == nullptr )
            {
				pybind::throw_exception("scope %s initialize not setup python module"
					, m_name
					);

				return;
            }
		}

#   if PYBIND_PYTHON_VERSION < 300
		PyObject * py_name = PyString_InternFromString( m_name );
#	else
		PyObject * py_name = PyUnicode_InternFromString( m_name );
#	endif

		PyObject * py_bases;

		if( m_basesCount > 0 )
		{
			Py_ssize_t py_basesCount = (Py_ssize_t)m_basesCount;
			py_bases = PyTuple_New( py_basesCount );
			Py_ssize_t index = 0;

			for( uint32_t i = 0; i != m_basesCount; ++i )
			{
                Metacast & mc = m_bases[i];

				PyTypeObject * py_base = mc.scope->m_pytypeobject;

				Py_INCREF( (PyObject *)py_base );
				PyTuple_SetItem( py_bases, index++, (PyObject*)py_base );
			}
		}
		else
		{
			py_bases = PyTuple_New( 1 );

            PyTypeObject * base_type = &STATIC_VAR(s_base_type);
			Py_INCREF( (PyObject *)base_type );
			PyTuple_SetItem( py_bases, 0, (PyObject *)base_type );
		}

		PyObject * py_dict = PyDict_New();
		
        PyObject * py_pybind_class_type = pybind::ptr( m_typeId );
                
		PyDict_SetItem( py_dict, STATIC_VAR(s_pybind_class_type_scope), py_pybind_class_type );
		Py_DECREF( py_pybind_class_type );
		
		PyObject * py_args = PyTuple_Pack( 3, py_name, py_bases, py_dict );
		Py_DECREF( py_name );
		Py_DECREF( py_bases );
		Py_DECREF( py_dict );
		
		m_pytypeobject = (PyTypeObject *)PyType_Type.tp_call( (PyObject*)&PyType_Type, py_args, 0 );
		Py_DECREF( py_args );

        if( m_pod == true )
        {
            m_pytypeobject->tp_new = py_new_pod;
            m_pytypeobject->tp_del = py_del_pod;
        }
        else
        {
		    m_pytypeobject->tp_new = py_new_class;
		    m_pytypeobject->tp_del = py_del_class;
        }

		PyType_Modified( m_pytypeobject );
       
        Py_INCREF( (PyObject*)m_pytypeobject );
		PyModule_AddObject( m_module, m_pytypeobject->tp_name, (PyObject*)m_pytypeobject );
	}
    //////////////////////////////////////////////////////////////////////////
    void class_type_scope::finalize()
    {
        PyDict_Clear( m_pytypeobject->tp_dict );

        Py_DECREF( (PyObject*)m_pytypeobject );

        m_pyconstructor = nullptr;

        m_convert = nullptr;
        m_call = nullptr;
        m_repr = nullptr;
        m_hash = nullptr;
        m_getattro = nullptr;
        m_mapping = nullptr;
        m_sequence_get = nullptr;
		m_sequence_set = nullptr;
        m_compare = nullptr;

		for( uint32_t i = 0; i != m_basesCount; ++i )
		{
			Metacast & m = m_bases[i];

			m.info = 0;
			m.cast = nullptr;
			m.scope = nullptr;			
		}
    }
	//////////////////////////////////////////////////////////////////////////
	void class_type_scope::type_initialize( PyTypeObject * _type )
	{
		_type->tp_del = py_del_class;
	}
	//////////////////////////////////////////////////////////////////////////
	const char * class_type_scope::get_name() const
	{
		return m_name;
	}
	//////////////////////////////////////////////////////////////////////////
	uint32_t class_type_scope::get_type_id() const
	{
		return m_typeId;
	}
	//////////////////////////////////////////////////////////////////////////
	bool class_type_scope::is_pod() const
	{
		return m_pod;
	}
    //////////////////////////////////////////////////////////////////////////
    void * class_type_scope::get_user() const
    {
        return m_user;
    }
    //////////////////////////////////////////////////////////////////////////
    PyTypeObject * class_type_scope::get_typemodule() const
    {
        return m_pytypeobject;
    }
	//////////////////////////////////////////////////////////////////////////
	void class_type_scope::set_module( PyObject * _module )
	{
		m_module = _module;
	}
	//////////////////////////////////////////////////////////////////////////
	namespace detail
	{
		class FCharCmp
		{
		public:
			FCharCmp( const char * _name )
				: m_name(_name)
			{
			}

		public:
			bool operator () ( const char * _name ) const
			{
				return strcmp( m_name, _name ) == 0;
			}

		protected:
			const char * m_name;
		};
	}
	//////////////////////////////////////////////////////////////////////////
	void class_type_scope::add_method( const method_adapter_interface_ptr & _ifunc )
	{
		PyObject * py_type_method = method_type_scope::instance( _ifunc, m_pytypeobject );

		const char * name = _ifunc->getName();

		if( PyDict_SetItemString( m_pytypeobject->tp_dict, name, py_type_method ) == -1 )
		{
			pybind::throw_exception("scope %s add_method %s python error"
				, this->m_name
				, name
				);

            return;
		}

		Py_DECREF( py_type_method );
	}
	//////////////////////////////////////////////////////////////////////////
	void class_type_scope::add_member( const member_adapter_interface_ptr & _imember )
	{
		PyObject * py_member = member_type_scope::instance( _imember );

		const char * name = _imember->getName();

		if( PyDict_SetItemString( m_pytypeobject->tp_dict, name, py_member ) == -1 )
		{
			pybind::throw_exception("scope %s add_member %s python error"
				, this->m_name
				, name
				);

            return;
		}

		Py_DECREF( py_member );
	}
	//////////////////////////////////////////////////////////////////////////
	void class_type_scope::set_convert( const convert_adapter_interface_ptr & _iconvert )
	{
		m_convert = _iconvert;
	}
	//////////////////////////////////////////////////////////////////////////
	const convert_adapter_interface_ptr & class_type_scope::get_convert()
	{
		return m_convert;
	}
    //////////////////////////////////////////////////////////////////////////
    void class_type_scope::set_call( const method_adapter_interface_ptr & _icall )
    {
        m_call = _icall;

        m_pytypeobject->tp_call = &py_callfunc;
    }
	//////////////////////////////////////////////////////////////////////////
	void class_type_scope::set_repr( const repr_adapter_interface_ptr & _irepr )
	{
		m_repr = _irepr;

		m_pytypeobject->tp_repr = &py_reprfunc;
	}
    //////////////////////////////////////////////////////////////////////////
    void class_type_scope::set_hash( const hash_adapter_interface_ptr & _ihash )
    {
        m_hash = _ihash;

        m_pytypeobject->tp_hash = (hashfunc)&py_hash;
    }
    //////////////////////////////////////////////////////////////////////////
    void class_type_scope::set_compare( const compare_adapter_interface_ptr & _icompare )
    {
        m_compare = _icompare;

        m_pytypeobject->tp_richcompare = &py_richcompare;
    }
	//////////////////////////////////////////////////////////////////////////
	void class_type_scope::set_getattro( const method_adapter_interface_ptr & _igetattro )
	{
		m_getattro = _igetattro;

		m_pytypeobject->tp_getattro = &py_getattro;
	}
	//////////////////////////////////////////////////////////////////////////
	void class_type_scope::set_mapping( const method_adapter_interface_ptr & _imapping )
	{
		m_mapping = _imapping;

		m_pytypeobject->tp_as_mapping = &py_as_mapping;
	}
	//////////////////////////////////////////////////////////////////////////
	void class_type_scope::set_sequence_get( const method_adapter_interface_ptr & _isequence )
	{
		m_sequence_get = _isequence;
		
		m_pytypeobject->tp_as_sequence = &py_as_sequence;
	}
	//////////////////////////////////////////////////////////////////////////
	void class_type_scope::set_sequence_set( const method_adapter_interface_ptr & _isequence )
	{
		m_sequence_set = _isequence;

		m_pytypeobject->tp_as_sequence = &py_as_sequence;
	}
	//////////////////////////////////////////////////////////////////////////
	void * class_type_scope::construct( PyObject * _obj, PyObject * _args )
	{
        if( m_pyconstructor == nullptr )
        {
            return nullptr;
        }

		void * obj = m_pyconstructor->call( _obj, _args );

		return obj;
	}
	//////////////////////////////////////////////////////////////////////////
	void class_type_scope::def_init( const constructor_adapter_interface_ptr & _ctr )
	{
		m_pyconstructor = _ctr;
	}
	//////////////////////////////////////////////////////////////////////////
	void class_type_scope::add_base( uint32_t _info, const class_type_scope_ptr & _base, pybind_metacast _cast )
	{
		if( m_basesCount == PYBIND_BASES_COUNT )
		{
			pybind::throw_exception("pybind scope %s maximize bases count PYBIND_BASES_COUNT is %d"
				, this->m_name
				, PYBIND_BASES_COUNT
				);

			return;
		}

        Metacast mc;
		mc.info = _info;
        mc.scope = _base;
        mc.cast = _cast;
				
		uint32_t newId = m_basesCount++;
		m_bases[newId] = mc;
	}
	//////////////////////////////////////////////////////////////////////////
	void * class_type_scope::metacast( uint32_t _info, void * _impl )
	{
		for( uint32_t i = 0; i != m_basesCount; ++i )
		{
			Metacast & mc = m_bases[i];

			if( mc.info != _info )
			{
				continue;
			}

			void * obj = mc.cast( _impl );

			return obj;
		}

		for( uint32_t i = 0; i != m_basesCount; ++i )
		{
            Metacast & mc = m_bases[i];

			void * down_impl = mc.cast( _impl );

			void * obj = mc.scope->metacast( _info, down_impl );

			if( obj == nullptr )
			{
				continue;
			}
			
			return obj;
		}

		return nullptr;
	}
	//////////////////////////////////////////////////////////////////////////
	PyObject * class_type_scope::create_holder( void * _impl )
	{
		if( m_pod == true )
		{
			return nullptr;	
		}
		
		PyObject * py_self = pybind::detail::alloc_class( m_pytypeobject, nullptr, nullptr );

		pybind::detail::wrap( py_self, _impl, true );

	    this->addObject( py_self );

		return py_self;
	}
	//////////////////////////////////////////////////////////////////////////
	PyObject * class_type_scope::create_pod( void ** _impl, size_t & _size )
	{
		if( m_pod == false )
		{
			return nullptr;
		}

		PyObject * py_self = pybind::detail::alloc_class( m_pytypeobject, nullptr, nullptr );

		pybind::detail::wrap_pod64( py_self, _impl, _size );

	    this->addObject( py_self );

		return py_self;
	}
	//////////////////////////////////////////////////////////////////////////
	void class_type_scope::addObject( PyObject * _obj )
	{
        (void)_obj;

        ++m_objectCount;
	}
	//////////////////////////////////////////////////////////////////////////
	void class_type_scope::removeObject( PyObject * _obj )
	{
        (void)_obj;

        --m_objectCount;
	}
    //////////////////////////////////////////////////////////////////////////
    uint32_t class_type_scope::getObjectCount() const
    {
        return m_objectCount;
    }
    //////////////////////////////////////////////////////////////////////////
    void class_type_scope::intrusive_ptr_destroy()
    {
        delete this;
    }
	//////////////////////////////////////////////////////////////////////////
	void * class_type_scope::operator new ( size_t _size )
	{
		return PYBIND_MALLOC(_size);
	}
	//////////////////////////////////////////////////////////////////////////
	void class_type_scope::operator delete ( void * _ptr, size_t _size )
	{		
		(void)_size;

		PYBIND_FREE(_ptr, _size);
	}
    //////////////////////////////////////////////////////////////////////////
#	ifdef PYBIND_VISIT_OBJECTS
    void class_type_scope::visit_objects( pybind_visit_objects * _visitor )
	{
		for( TVectorObjects::iterator
			it = m_objects.begin(),
			it_end = m_objects.end();
		it != it_end;
		++it )
		{
            PyObject * obj = *it;

			_visitor->visit_object( obj );
		}
	}
#	endif
	//////////////////////////////////////////////////////////////////////////
	bool initialize_classes()
	{
		detail::initialize_class_type_scope();

#   if PYBIND_PYTHON_VERSION > 300
        STATIC_VAR(s_pybind_object_impl) = pybind::unicode_from_utf8( "__pybind_object_impl" );
        STATIC_VAR(s_pybind_class_type_scope) = pybind::unicode_from_utf8( "__pybind_class_type_scope" );
        STATIC_VAR(s_pybind_object_holder) = pybind::unicode_from_utf8( "__pybind_object_holder" );
#   else
        STATIC_VAR(s_pybind_object_impl) = pybind::string_from_char( "__pybind_object_impl" );
        STATIC_VAR(s_pybind_class_type_scope) = pybind::string_from_char( "__pybind_class_type_scope" );
        STATIC_VAR(s_pybind_object_holder) = pybind::string_from_char( "__pybind_object_holder" );
#   endif

		PyTypeObject * pod64_type = &STATIC_VAR(s_pod64_type);

		if( PyType_Ready( pod64_type ) < 0 )
		{
			printf("invalid embedding class '%s' \n", STATIC_VAR(s_pod64_type).tp_name );

            return false;
		}

		PyTypeObject * pybind_base_type = &STATIC_VAR(s_base_type);

		if( PyType_Ready( pybind_base_type ) < 0 )
		{
			printf("invalid embedding class '%s' \n", STATIC_VAR(s_base_type).tp_name );

            return false;
		}

		PyObject * builtins = pybind::get_builtins();
		
		PyObject * dir_bltin = pybind::module_dict( builtins );
		
		PyObject * pybind_base_obj = (PyObject *)pybind_base_type;

		pybind::dict_setstring( dir_bltin, "pybind_base_type", pybind_base_obj );

		Py_DECREF( pybind_base_obj );
		Py_DECREF( builtins );
		
        return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void finalize_classes()
	{
		detail::finalize_class_type_scope();

		Py_DECREF(STATIC_VAR(s_pybind_object_impl));
		Py_DECREF(STATIC_VAR(s_pybind_class_type_scope));
		Py_DECREF(STATIC_VAR(s_pybind_object_holder));
	}
}
