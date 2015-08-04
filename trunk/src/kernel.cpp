#   include "kernel.hpp"

#	include "pybind/class_type_scope_interface.hpp"

#	include "pybind/convert_adapter.hpp"

#	include "pybind/extract.hpp"
#	include "pybind/pod.hpp"
#	include "pybind/detail.hpp"

#   include "pybind/exception.hpp"

#	include "pybind/system.hpp"

#	include "config/python.hpp"

namespace pybind
{
	//////////////////////////////////////////////////////////////////////////
	static void py_dealloc( PyObject * _obj )
	{
		PyTypeObject * objtype = Py_TYPE( _obj );

		objtype->tp_free( _obj );
	}

	//////////////////////////////////////////////////////////////////////////
	kernel::kernel()
		: m_enumerator(0)
		, m_str_pybind_class_type_scope(nullptr)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	kernel::~kernel()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	bool kernel::initialize()
	{
		for( uint32_t index = 0; index != PYBIND_TYPE_COUNT; ++index )
		{
			class_info_desc_t & desc = m_class_info_desc[index];

			desc.id = 0;
			desc.name = nullptr;
		}

#   if PYBIND_PYTHON_VERSION > 300
		m_str_pybind_class_type_scope = pybind::unicode_from_utf8( "__pybind_class_type_scope" );
#   else
		m_str_pybind_class_type_scope = pybind::string_from_char( "__pybind_class_type_scope" );
#   endif

		//////////////////////////////////////////////////////////////////////////
		PyTypeObject pod8_type =
		{
			PyVarObject_HEAD_INIT( &PyType_Type, 0 )
			"pod8_type",
			sizeof( py_pod8_object ),
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
		};
		//////////////////////////////////////////////////////////////////////////
		PyTypeObject pod16_type =
		{
			PyVarObject_HEAD_INIT( &PyType_Type, 0 )
			"pod16_type",
			sizeof( py_pod16_object ),
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
		};
		//////////////////////////////////////////////////////////////////////////
		PyTypeObject pod32_type =
		{
			PyVarObject_HEAD_INIT( &PyType_Type, 0 )
			"pod32_type",
			sizeof( py_pod32_object ),
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
		};
		//////////////////////////////////////////////////////////////////////////
		PyTypeObject pod64_type =
		{
			PyVarObject_HEAD_INIT( &PyType_Type, 0 )
			"pod64_type",
			sizeof( py_pod64_object ),
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
		};
		//////////////////////////////////////////////////////////////////////////
		PyTypeObject ptr_type =
		{
			PyVarObject_HEAD_INIT( &PyType_Type, 0 )
			"pybind_ptr_type",
			sizeof( py_ptr_object ),
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
			0,                        /* tp_alloc */
			0,                          /* tp_new */
			0,                               /* tp_free */
		};

		m_pod8_type = pod8_type;
		m_pod16_type = pod16_type;
		m_pod32_type = pod32_type;
		m_pod64_type = pod64_type;
		
		m_ptr_type = ptr_type;

		if( PyType_Ready( &m_ptr_type ) < 0 )
		{
			printf( "invalid embedding class '%s' \n", m_ptr_type.tp_name );

			return false;
		}

		if( PyType_Ready( &m_pod64_type ) < 0 )
		{
			printf( "invalid embedding class '%s' \n", m_pod64_type.tp_name );

			return false;
		}

		if( PyType_Ready( &m_pod32_type ) < 0 )
		{
			printf( "invalid embedding class '%s' \n", m_pod32_type.tp_name );

			return false;
		}

		if( PyType_Ready( &m_pod16_type ) < 0 )
		{
			printf( "invalid embedding class '%s' \n", m_pod16_type.tp_name );

			return false;
		}

		if( PyType_Ready( &m_pod8_type ) < 0 )
		{
			printf( "invalid embedding class '%s' \n", m_pod8_type.tp_name );

			return false;
		}
		
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void kernel::finalize()
	{
		for( uint32_t index = 0; index != PYBIND_TYPE_COUNT; ++index )
		{
			class_type_scope_interface_ptr & scope = m_class_type_scope[index];

			if( scope != nullptr )
			{
				scope->finalize();
				scope = nullptr;
			}

			m_class_type_scope[index] = nullptr;

			m_type_cast[index] = nullptr;

			m_class_info_desc[index];

			m_class_info_desc[index].id = 0;
			m_class_info_desc[index].name = nullptr;
		}

		pybind::decref( m_str_pybind_class_type_scope );
		m_str_pybind_class_type_scope = nullptr;
	}
	//////////////////////////////////////////////////////////////////////////
	uint32_t kernel::get_next_id()
	{
		uint32_t id = m_enumerator;

		m_enumerator++;

		return id;
	}
	//////////////////////////////////////////////////////////////////////////
	const class_info_desc_t * kernel::find_class_info_desc_name( const char * _name ) const
	{
		for( uint32_t index = 0; index != PYBIND_TYPE_COUNT; ++index )
		{
			const class_info_desc_t & desc = m_class_info_desc[index];

			if( desc.id == 0 )
			{
				continue;
			}

			if( strcmp( desc.name, _name ) != 0 )
			{
				continue;
			}

			return &desc;
		}

		return nullptr;
	}
	//////////////////////////////////////////////////////////////////////////
	const class_info_desc_t * kernel::find_class_info_desc_id( uint32_t _id ) const
	{
		for( uint32_t index = 0; index != PYBIND_TYPE_COUNT; ++index )
		{
			const class_info_desc_t & desc = m_class_info_desc[index];

			if( desc.id == 0 )
			{
				continue;
			}

			if( desc.id != _id )
			{
				continue;
			}

			return &desc;
		}

		return nullptr;
	}
	//////////////////////////////////////////////////////////////////////////
	class_info_desc_t * kernel::alloc_class_info_desc()
	{
		for( uint32_t index = 0; index != PYBIND_TYPE_COUNT; ++index )
		{
			class_info_desc_t & desc = m_class_info_desc[index];

			if( desc.id != 0 )
			{
				continue;
			}

			return &desc;
		}

		return nullptr;
	}
	//////////////////////////////////////////////////////////////////////////
	uint32_t kernel::get_class_type_id( const std::type_info & _info )
	{
		const char * info_name = _info.name();

		const class_info_desc_t * desc = this->find_class_info_desc_name( info_name );

		if( desc != nullptr )
		{
			uint32_t id = desc->id;

			return id;
		}

		class_info_desc_t * new_desc = this->alloc_class_info_desc();

		if( new_desc == nullptr )
		{
			pybind::throw_exception( "invalid create new type info" );

			return 0;
		}

		uint32_t new_id = this->get_next_id();

		new_desc->id = new_id;
		new_desc->name = info_name;

		return new_id;
	}
	//////////////////////////////////////////////////////////////////////////
	const char * kernel::get_class_type_info( uint32_t _id ) const
	{
		const class_info_desc_t * desc = this->find_class_info_desc_id( _id );

		if( desc == nullptr )
		{
			pybind::throw_exception( "invalid get class info %d"
				, _id
				);

			return nullptr;
		}

		const char * name = desc->name;

		return name;
	}
	//////////////////////////////////////////////////////////////////////////
	void kernel::register_type_info_extract( uint32_t _info, const type_cast_ptr & _cast )
	{
		m_type_cast[_info] = _cast;
	}
	//////////////////////////////////////////////////////////////////////////
	void kernel::unregister_type_info_extract( uint32_t _info )
	{
		m_type_cast[_info] = nullptr;
	}
	//////////////////////////////////////////////////////////////////////////
	type_cast * kernel::find_type_info_extract( uint32_t _info ) const
	{
		const type_cast_ptr & cast = m_type_cast[_info];

		type_cast * t = cast.get();

		return t;
	}
	//////////////////////////////////////////////////////////////////////////
	class_type_scope_interface_ptr kernel::create_new_type_scope( uint32_t _info, const char * _name, void * _user, const new_adapter_interface_ptr & _pynew, const destroy_adapter_interface_ptr & _pydestructor, uint32_t _pod )
	{ 
		class_type_scope_ptr scope = new class_type_scope( _name, _info, _user, _pynew, _pydestructor, _pod );

		m_class_type_scope[_info] = scope;

		return scope;
	}
	//////////////////////////////////////////////////////////////////////////
	bool kernel::has_class_type_scope( uint32_t _info ) const
	{
		const class_type_scope_ptr & scope = m_class_type_scope[_info];

		return scope != nullptr;
	}
	//////////////////////////////////////////////////////////////////////////
	const class_type_scope_interface_ptr & kernel::get_class_type_scope( uint32_t _info ) const
	{
		const class_type_scope_interface_ptr & scope = m_class_type_scope[_info];

		return scope;
	}
	//////////////////////////////////////////////////////////////////////////
	void kernel::visit_types_scope( visitor_class_type_scope * _getter )
	{
		for( uint32_t index = 0; index != PYBIND_TYPE_COUNT; ++index )
		{
			const class_type_scope_ptr & scope_ptr = m_class_type_scope[index];

			if( scope_ptr == nullptr )
			{
				continue;
			}

			class_type_scope * scope = scope_ptr.get();

			_getter->visit_scope( scope );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	const class_type_scope_interface_ptr & kernel::get_class_scope( PyTypeObject * _type ) const
	{
		PyObject * py_scope = PyObject_GetAttr( (PyObject*)_type, m_str_pybind_class_type_scope );

		if( py_scope == nullptr )
		{
			pybind::throw_exception( "obj %s not wrap pybind (scope)"
				, pybind::object_str( (PyObject *)_type )
				);

			return class_type_scope_interface_ptr::none();
		}

		uint32_t id;
		if( pybind::extract_value( py_scope, id ) == false )
		{
			pybind::throw_exception( "obj %s incorrect wrap pybind (scope)"
				, pybind::object_str( (PyObject *)_type )
				);

			return class_type_scope_interface_ptr::none();
		}

		Py_DECREF( py_scope );

		const class_type_scope_interface_ptr & scope = this->get_class_type_scope( id );

		if( scope == nullptr )
		{
			pybind::throw_exception( "obj %s incorrect wrap pybind (id %d)"
				, pybind::object_str( (PyObject *)_type )
				, id
				);

			return class_type_scope_interface_ptr::none();
		}				

		return scope;
	}
	//////////////////////////////////////////////////////////////////////////
	bool kernel::is_class( PyObject * _obj ) const
	{
		PyTypeObject * objtype = Py_TYPE( _obj );

		bool successful = this->is_type_class( (PyObject *)objtype );
		
		return successful;
	}
	//////////////////////////////////////////////////////////////////////////
	bool kernel::is_type_class( PyObject * _type ) const
	{
		if( PyObject_HasAttr( _type, m_str_pybind_class_type_scope ) == 0 )
		{
			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void * kernel::check_registred_class( PyObject * _obj, uint32_t _info ) const
	{
		PyTypeObject * py_type = Py_TYPE( _obj );

		const class_type_scope_ptr & type_scope = m_class_type_scope[_info];

		if( type_scope->is_instance( py_type ) == false )
		{
			return nullptr;
		}

		void * impl = detail::get_class_impl( _obj );

		return impl;
	}
	//////////////////////////////////////////////////////////////////////////
	void kernel::error_invalid_extract( PyObject * _obj, uint32_t _tinfo )
	{
		pybind::check_error();

		const char * typeinfo_name = this->get_class_type_info( _tinfo );

		const char * obj_repr = pybind::object_repr( _obj );
		const char * obj_repr_type = pybind::object_repr_type( _obj );

		if( obj_repr != nullptr )
		{
			pybind::error_message( "invalid extract from %.256s type %.256s to %.256s"
				, obj_repr
				, obj_repr_type
				, typeinfo_name
				);
		}
		else
		{
			pybind::error_message( "invalid extract from unknown object type %s to %s"
				, _obj->ob_type->tp_name
				, typeinfo_name
				);
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool kernel::instance_of_type( PyObject * _obj, uint32_t _tinfo ) const
	{
		if( this->is_class( _obj ) == false )
		{
			return false;
		}

		if( detail::is_wrap( _obj ) == false )
		{
			pybind::error_message( "instance_of_type: unbind object" );

			return false;
		}

		const class_type_scope_interface_ptr & scope = this->get_class_scope( _obj->ob_type );
		const class_type_scope_interface_ptr & cur_scope = this->get_class_type_scope( _tinfo );

		if( scope != cur_scope )
		{
			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	PyTypeObject * kernel::get_pod_type( uint32_t _pod )
	{
		PyTypeObject * py_pybind_type;

		if( _pod > PYBIND_OBJECT_POD_SIZE )
		{
			return nullptr;
		}
		else if( _pod > 32 )
		{
			py_pybind_type = &m_pod64_type;
		}
		else if( _pod > 16 )
		{
			py_pybind_type = &m_pod32_type;
		}
		else if( _pod > 8 )
		{
			py_pybind_type = &m_pod16_type;
		}
		else if( _pod > 0 )
		{
			py_pybind_type = &m_pod8_type;
		}
		else
		{
			py_pybind_type = &m_ptr_type;
		}

		Py_INCREF( (PyObject *)py_pybind_type );

		return py_pybind_type;
	}
	//////////////////////////////////////////////////////////////////////////
	PyObject * kernel::get_str_class_type_scope_() const
	{
		return m_str_pybind_class_type_scope;
	}
}