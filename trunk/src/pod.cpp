#	include "pod.hpp"

#	include "config/config.hpp"

#	include "pybind/exception.hpp"
#	include "pybind/system.hpp"

namespace pybind
{
	namespace detail
	{
		//////////////////////////////////////////////////////////////////////////
		struct pod_scope
		{
			PyTypeObject pod8_type;
			PyTypeObject pod16_type;
			PyTypeObject pod32_type;
			PyTypeObject pod64_type;

			PyTypeObject ptr_type;
		};
		//////////////////////////////////////////////////////////////////////////
		static pod_scope & get_pod()
		{
			static pod_scope k;

			return k;
		}
		//////////////////////////////////////////////////////////////////////////
		static void py_dealloc( PyObject * _obj )
		{
			PyTypeObject * objtype = Py_TYPE( _obj );

			objtype->tp_free( _obj );
		}
		//////////////////////////////////////////////////////////////////////////
		PyTypeObject * get_pod_type( uint32_t _pod )
		{
			pod_scope & k = get_pod();

			PyTypeObject * py_pybind_type;

			if( _pod > PYBIND_OBJECT_POD_SIZE )
			{
				return nullptr;
			}
			else if( _pod > 32 )
			{
				py_pybind_type = &k.pod64_type;
			}
			else if( _pod > 16 )
			{
				py_pybind_type = &k.pod32_type;
			}
			else if( _pod > 8 )
			{
				py_pybind_type = &k.pod16_type;
			}
			else if( _pod > 0 )
			{
				py_pybind_type = &k.pod8_type;
			}
			else
			{
				py_pybind_type = &k.ptr_type;
			}

			Py_INCREF( (PyObject *)py_pybind_type );

			return py_pybind_type;
		}
		//////////////////////////////////////////////////////////////////////////
		void * get_class_impl( PyObject * _obj )
		{
			py_base_object * py_base = (py_base_object *)_obj;

			uint32_t flag = py_base->flag;

			if( flag & PY_OBJECT_PTR )
			{
				py_ptr_object * py_ptr = (py_ptr_object *)_obj;

				void * impl = py_ptr->impl;

				return impl;
			}
			else if( flag & PY_OBJECT_POD8 )
			{
				py_pod8_object * py_pod = (py_pod8_object *)_obj;

				void * impl = (void *)py_pod->buff;

				return impl;
			}
			else if( flag & PY_OBJECT_POD16 )
			{
				py_pod16_object * py_pod = (py_pod16_object *)_obj;

				void * impl = (void *)py_pod->buff;

				return impl;
			}
			else if( flag & PY_OBJECT_POD32 )
			{
				py_pod32_object * py_pod = (py_pod32_object *)_obj;

				void * impl = (void *)py_pod->buff;

				return impl;
			}
			else if( flag & PY_OBJECT_POD64 )
			{
				py_pod64_object * py_pod = (py_pod64_object *)_obj;

				void * impl = (void *)py_pod->buff;

				return impl;
			}
			else if( flag & PY_OBJECT_UNWRAP )
			{
				return nullptr;
			}

			pybind::throw_exception( "obj %s not wrap pybind (impl)"
				, pybind::object_str( _obj )
				);

			return nullptr;
		}
	}

	//////////////////////////////////////////////////////////////////////////
	bool pod_initialize()
	{
		detail::pod_scope & k = detail::get_pod();

		//////////////////////////////////////////////////////////////////////////
		PyTypeObject pod8_type =
		{
			PyVarObject_HEAD_INIT( &PyType_Type, 0 )
			"pod8_type",
			sizeof( detail::py_pod8_object ),
			0,
			&detail::py_dealloc,                             /* tp_dealloc */
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
			sizeof( detail::py_pod16_object ),
			0,
			&detail::py_dealloc,                             /* tp_dealloc */
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
			sizeof( detail::py_pod32_object ),
			0,
			&detail::py_dealloc,                             /* tp_dealloc */
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
			sizeof( detail::py_pod64_object ),
			0,
			&detail::py_dealloc,                             /* tp_dealloc */
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
			sizeof( detail::py_ptr_object ),
			0,
			&detail::py_dealloc,                             /* tp_dealloc */
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

		k.pod8_type = pod8_type;
		k.pod16_type = pod16_type;
		k.pod32_type = pod32_type;
		k.pod64_type = pod64_type;

		k.ptr_type = ptr_type;

		if( PyType_Ready( &k.ptr_type ) < 0 )
		{
			printf( "invalid embedding class '%s' \n", k.ptr_type.tp_name );

			return false;
		}

		if( PyType_Ready( &k.pod64_type ) < 0 )
		{
			printf( "invalid embedding class '%s' \n", k.pod64_type.tp_name );

			return false;
		}

		if( PyType_Ready( &k.pod32_type ) < 0 )
		{
			printf( "invalid embedding class '%s' \n", k.pod32_type.tp_name );

			return false;
		}

		if( PyType_Ready( &k.pod16_type ) < 0 )
		{
			printf( "invalid embedding class '%s' \n", k.pod16_type.tp_name );

			return false;
		}

		if( PyType_Ready( &k.pod8_type ) < 0 )
		{
			printf( "invalid embedding class '%s' \n", k.pod8_type.tp_name );

			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void pod_finalize()
	{

	}
}