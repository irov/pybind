#	include "pod_python.hpp"

#	include "config/config.hpp"

#	include "pybind/exception.hpp"
#	include "pybind/system.hpp"
#	include "pybind/logger.hpp"

namespace pybind
{
	//////////////////////////////////////////////////////////////////////////
	struct py_base_object
	{
		PyObject_HEAD
			uint32_t flag;
		int64_t hash;
	};
	//////////////////////////////////////////////////////////////////////////
	struct py_ptr_object
		: public py_base_object
	{
		void * impl;
	};
	//////////////////////////////////////////////////////////////////////////
	struct py_pod4_object
		: public py_ptr_object
	{
		uint8_t buff[4];
	};
	//////////////////////////////////////////////////////////////////////////
	struct py_pod8_object
		: public py_ptr_object
	{
		uint8_t buff[8];
	};
	//////////////////////////////////////////////////////////////////////////
	struct py_pod16_object
		: public py_ptr_object
	{
		uint8_t buff[16];
	};
	//////////////////////////////////////////////////////////////////////////
	struct py_pod32_object
		: public py_ptr_object
	{
		uint8_t buff[32];
	};
	//////////////////////////////////////////////////////////////////////////
	struct py_pod64_object
		: public py_ptr_object
	{
		uint8_t buff[64];
	};
	//////////////////////////////////////////////////////////////////////////
	namespace detail
	{
		//////////////////////////////////////////////////////////////////////////
		bool is_pod_holder( PyObject * _obj )
		{
			py_base_object * py_base = (py_base_object *)_obj;

			if( py_base->flag & PY_OBJECT_HOLDER )
			{
				return true;
			}

			return false;
		}
		//////////////////////////////////////////////////////////////////////////
		void wrap_pod_ptr( PyObject * _obj, void * _impl, bool _holder )
		{
			py_ptr_object * py_ptr = (py_ptr_object *)_obj;

			py_ptr->impl = _impl;

			py_ptr->flag |= PY_OBJECT_PTR;

			py_ptr->hash = -1;

			if( _holder == true )
			{
				py_ptr->flag |= PY_OBJECT_HOLDER;
			}
		}
		//////////////////////////////////////////////////////////////////////////
		inline void wrap_pod64( PyObject * _obj, void ** _impl )
		{
			py_pod64_object * py_pod = (py_pod64_object *)_obj;

			py_pod->flag |= PY_OBJECT_POD64;

			*_impl = (void *)py_pod->buff;
		}
		//////////////////////////////////////////////////////////////////////////
		inline void wrap_pod32( PyObject * _obj, void ** _impl )
		{
			py_pod32_object * py_pod = (py_pod32_object *)_obj;

			py_pod->flag |= PY_OBJECT_POD32;

			*_impl = (void *)py_pod->buff;
		}
		//////////////////////////////////////////////////////////////////////////
		inline void wrap_pod16( PyObject * _obj, void ** _impl )
		{
			py_pod16_object * py_pod = (py_pod16_object *)_obj;

			py_pod->flag |= PY_OBJECT_POD16;

			*_impl = (void *)py_pod->buff;
		}
		//////////////////////////////////////////////////////////////////////////
		inline void wrap_pod8( PyObject * _obj, void ** _impl )
		{
			py_pod8_object * py_pod = (py_pod8_object *)_obj;

			py_pod->flag |= PY_OBJECT_POD8;

			*_impl = (void *)py_pod->buff;
		}
		//////////////////////////////////////////////////////////////////////////
		inline void wrap_pod4( PyObject * _obj, void ** _impl )
		{
			py_pod4_object * py_pod = (py_pod4_object *)_obj;

			py_pod->flag |= PY_OBJECT_POD4;

			*_impl = (void *)py_pod->buff;
		}
		//////////////////////////////////////////////////////////////////////////
		void wrap_pod( PyObject * _obj, void ** _impl, size_t _size )
		{
			if( _size > PYBIND_OBJECT_POD_SIZE )
			{
				pybind::throw_exception( "wrap_pod obj %s size %d > max pod size %d"
					, pybind::object_str( _obj )
					, _size
					, PYBIND_OBJECT_POD_SIZE
					);

				return;
			}

			py_ptr_object * py_ptr = (py_ptr_object *)_obj;

			py_ptr->hash = -1;

			if( _size > 32 )
			{
				wrap_pod64( _obj, _impl );
			}
			else if( _size > 16 )
			{
				wrap_pod32( _obj, _impl );
			}
			else if( _size > 8 )
			{
				wrap_pod16( _obj, _impl );
			}
			else if( _size > 4 )
			{
				wrap_pod8( _obj, _impl );
			}
			else
			{
				wrap_pod4( _obj, _impl );
			}
		}
		//////////////////////////////////////////////////////////////////////////
		int64_t get_pod_hash( PyObject * _obj )
		{
			py_base_object * py_base = (py_base_object *)_obj;

			int64_t hash = py_base->hash;

			return hash;
		}
		//////////////////////////////////////////////////////////////////////////
		void set_pod_hash( PyObject * _obj, int64_t _hash )
		{
			py_base_object * py_base = (py_base_object *)_obj;

			py_base->hash = _hash;
		}
		//////////////////////////////////////////////////////////////////////////
		void * get_pod_impl( PyObject * _obj )
		{
			py_base_object * py_base = (py_base_object *)_obj;

			uint32_t flag = py_base->flag;

			if( flag & PY_OBJECT_UNWRAP )
			{
				return nullptr;
			}
			else if( flag & PY_OBJECT_PTR )
			{
				py_ptr_object * py_ptr = (py_ptr_object *)_obj;

				void * impl = py_ptr->impl;

				return impl;
			}
			else if( flag & PY_OBJECT_POD4 )
			{
				py_pod4_object * py_pod = (py_pod4_object *)_obj;

				void * impl = (void *)py_pod->buff;

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

			pybind::throw_exception( "obj %s not wrap pybind (impl)"
				, pybind::object_str( _obj )
				);

			return nullptr;
		}
		//////////////////////////////////////////////////////////////////////////
		void unwrap_pod( PyObject * _obj )
		{
			py_base_object * py_base = (py_base_object *)_obj;

			py_base->flag |= PY_OBJECT_UNWRAP;
		}
		//////////////////////////////////////////////////////////////////////////
		bool is_pod_wrap( PyObject * _obj )
		{
			py_base_object * py_base = (py_base_object *)_obj;

			if( py_base->flag & PY_OBJECT_UNWRAP )
			{
				return false;
			}

			return true;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	static void py_dealloc( PyObject * _obj )
	{
		PyTypeObject * objtype = Py_TYPE( _obj );

		objtype->tp_free( _obj );
	}
	//////////////////////////////////////////////////////////////////////////
	PyTypeObject * pod_python::get_pod_type( uint32_t _pod )
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
		else if( _pod > 4 )
		{
			py_pybind_type = &m_pod8_type;
		}
		else if( _pod > 0 )
		{
			py_pybind_type = &m_pod4_type;
		}
		else
		{
			py_pybind_type = &m_ptr_type;
		}

		Py_INCREF( (PyObject *)py_pybind_type );

		return py_pybind_type;
	}
	//////////////////////////////////////////////////////////////////////////
	static void wrap_pod64( PyObject * _obj, void ** _impl )
	{
		py_pod64_object * py_pod = (py_pod64_object *)_obj;

		py_pod->flag |= PY_OBJECT_POD64;

		*_impl = (void *)py_pod->buff;
	}
	//////////////////////////////////////////////////////////////////////////
	static void wrap_pod32( PyObject * _obj, void ** _impl )
	{
		py_pod32_object * py_pod = (py_pod32_object *)_obj;

		py_pod->flag |= PY_OBJECT_POD32;

		*_impl = (void *)py_pod->buff;
	}
	//////////////////////////////////////////////////////////////////////////
	static void wrap_pod16( PyObject * _obj, void ** _impl )
	{
		py_pod16_object * py_pod = (py_pod16_object *)_obj;

		py_pod->flag |= PY_OBJECT_POD16;

		*_impl = (void *)py_pod->buff;
	}
	//////////////////////////////////////////////////////////////////////////
	static void wrap_pod8( PyObject * _obj, void ** _impl )
	{
		py_pod8_object * py_pod = (py_pod8_object *)_obj;

		py_pod->flag |= PY_OBJECT_POD8;

		*_impl = (void *)py_pod->buff;
	}
	//////////////////////////////////////////////////////////////////////////
	static void wrap_pod4( PyObject * _obj, void ** _impl )
	{
		py_pod4_object * py_pod = (py_pod4_object *)_obj;

		py_pod->flag |= PY_OBJECT_POD4;

		*_impl = (void *)py_pod->buff;
	}
	//////////////////////////////////////////////////////////////////////////
	bool pod_python::initialize()
	{
		//////////////////////////////////////////////////////////////////////////
		PyTypeObject pod4_type =
		{
			PyVarObject_HEAD_INIT( &PyType_Type, 0 )
			"pybind_pod4_type",
			sizeof( py_pod4_object ),
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
		PyTypeObject pod8_type =
		{
			PyVarObject_HEAD_INIT( &PyType_Type, 0 )
			"pybind_pod8_type",
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
			"pybind_pod16_type",
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
			"pybind_pod32_type",
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
			"pybind_pod64_type",
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

		m_pod4_type = pod4_type;
		m_pod8_type = pod8_type;
		m_pod16_type = pod16_type;
		m_pod32_type = pod32_type;
		m_pod64_type = pod64_type;

		m_ptr_type = ptr_type;

		if( PyType_Ready( &m_ptr_type ) < 0 )
		{
			pybind::log( "invalid embedding class '%s' \n"
				, m_ptr_type.tp_name
				);

			return false;
		}

		if( PyType_Ready( &m_pod64_type ) < 0 )
		{
			pybind::log( "invalid embedding class '%s' \n"
				, m_pod64_type.tp_name
				);

			return false;
		}

		if( PyType_Ready( &m_pod32_type ) < 0 )
		{
			pybind::log( "invalid embedding class '%s' \n"
				, m_pod32_type.tp_name
				);

			return false;
		}

		if( PyType_Ready( &m_pod16_type ) < 0 )
		{
			pybind::log( "invalid embedding class '%s' \n"
				, m_pod16_type.tp_name
				);

			return false;
		}

		if( PyType_Ready( &m_pod8_type ) < 0 )
		{
			pybind::log( "invalid embedding class '%s' \n"
				, m_pod8_type.tp_name
				);

			return false;
		}

		if( PyType_Ready( &m_pod4_type ) < 0 )
		{
			pybind::log( "invalid embedding class '%s' \n"
				, m_pod4_type.tp_name
				);

			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void pod_python::finalize()
	{

	}
}