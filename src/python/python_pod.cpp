#include "python_pod.hpp"
#include "python_system.hpp"

#include "pybind/kernel_interface.hpp"
#include "pybind/exception.hpp"

#include "config/config.hpp"

namespace pybind
{
    //////////////////////////////////////////////////////////////////////////
    struct py_base_object
    {
        PyObject_HEAD

            uint32_t flag;
    };
    //////////////////////////////////////////////////////////////////////////
    struct py_ptr_object
    {
        PyObject_HEAD

            uint32_t flag;

        void * impl;
    };
    //////////////////////////////////////////////////////////////////////////
    template<size_t I>
    struct py_pod_object
    {
        PyObject_HEAD

            uint32_t flag;

        uint8_t buff[4 << I];
    };
    //////////////////////////////////////////////////////////////////////////
    struct py_hash_object
    {
        PyObject_HEAD

            uint32_t flag;

        int64_t hash;
    };
    //////////////////////////////////////////////////////////////////////////
    template<size_t I>
    struct py_pod_hash_object
    {
        PyObject_HEAD

            uint32_t flag;

        int64_t hash;

        uint8_t buff[4 << I];
    };
    //////////////////////////////////////////////////////////////////////////
    namespace detail
    {
        //////////////////////////////////////////////////////////////////////////
        static void py_dealloc( PyObject * _obj )
        {
            PyTypeObject * objtype = pybind::object_type( _obj );

            objtype->tp_free( _obj );
        }
        //////////////////////////////////////////////////////////////////////////
        template<size_t I>
        static void wrap_pod_I( PyObject * _obj, void ** _impl )
        {
            py_pod_object<I> * py_pod = (py_pod_object<I> *)_obj;

            py_pod->flag |= PY_OBJECT_POD << I;

            *_impl = (void *)py_pod->buff;
        }
        //////////////////////////////////////////////////////////////////////////
        template<size_t I>
        static void wrap_pod_hash_I( PyObject * _obj, void ** _impl )
        {
            py_pod_hash_object<I> * py_pod_hash = (py_pod_hash_object<I> *)_obj;

            py_pod_hash->flag |= PY_OBJECT_POD << I;
            py_pod_hash->flag |= PY_OBJECT_HASH;
            py_pod_hash->hash = -1;

            *_impl = (void *)py_pod_hash->buff;
        }
        //////////////////////////////////////////////////////////////////////////
        static constexpr uint32_t s_pod_type_matrix[256] = {
            0, 0, 0, 0,
            1, 1, 1, 1,
            2, 2, 2, 2, 2, 2, 2, 2,
            3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
            4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
            5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
            6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6
        };
        //////////////////////////////////////////////////////////////////////////
        typedef void(*t_wrap_pod4_f)(PyObject *, void **);
        //////////////////////////////////////////////////////////////////////////
        static t_wrap_pod4_f s_pod_type_generator[] =
        {
            wrap_pod_I<0>,
            wrap_pod_I<1>,
            wrap_pod_I<2>,
            wrap_pod_I<3>,
            wrap_pod_I<4>,
            wrap_pod_I<5>,
            wrap_pod_I<6>
        };
        //////////////////////////////////////////////////////////////////////////
        static t_wrap_pod4_f s_pod_hash_type_generator[] =
        {
            wrap_pod_hash_I<0>,
            wrap_pod_hash_I<1>,
            wrap_pod_hash_I<2>,
            wrap_pod_hash_I<3>,
            wrap_pod_hash_I<4>,
            wrap_pod_hash_I<5>,
            wrap_pod_hash_I<6>
        };
        //////////////////////////////////////////////////////////////////////////
    }
    //////////////////////////////////////////////////////////////////////////
    namespace helper
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
        bool is_pod_weak( PyObject * _obj )
        {
            py_base_object * py_base = (py_base_object *)_obj;

            if( py_base->flag & PY_OBJECT_WEAK )
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

            if( _holder == true )
            {
                py_ptr->flag |= PY_OBJECT_HOLDER;
            }
        }
        //////////////////////////////////////////////////////////////////////////
        void wrap_pod_weak( PyObject * _obj, void * _impl, bool _holder )
        {
            py_ptr_object * py_ptr = (py_ptr_object *)_obj;

            py_ptr->impl = _impl;

            py_ptr->flag |= PY_OBJECT_PTR;
            py_ptr->flag |= PY_OBJECT_WEAK;

            if( _holder == true )
            {
                py_ptr->flag |= PY_OBJECT_HOLDER;
            }
        }
        //////////////////////////////////////////////////////////////////////////
        void wrap_pod( PyObject * _obj, void ** _impl, size_t _size, bool _hash )
        {
            uint32_t pod_matrix_index = detail::s_pod_type_matrix[_size - 1];

            if( _hash == false )
            {
                detail::t_wrap_pod4_f pod_generator = detail::s_pod_type_generator[pod_matrix_index];

                (*pod_generator)(_obj, _impl);
            }
            else
            {
                detail::t_wrap_pod4_f pod_hash_generator = detail::s_pod_hash_type_generator[pod_matrix_index];

                (*pod_hash_generator)(_obj, _impl);
            }
        }
        //////////////////////////////////////////////////////////////////////////
        int64_t get_pod_hash( PyObject * _obj )
        {
            py_hash_object * py_base = (py_hash_object *)_obj;

            int64_t hash = py_base->hash;

            return hash;
        }
        //////////////////////////////////////////////////////////////////////////
        void set_pod_hash( PyObject * _obj, int64_t _hash )
        {
            py_hash_object * py_base = (py_hash_object *)_obj;

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

            if( flag & PY_OBJECT_PTR )
            {
                py_ptr_object * py_ptr = (py_ptr_object *)_obj;

                void * impl = py_ptr->impl;

                return impl;
            }

            if( flag & PY_OBJECT_HASH )
            {
                if( flag & PY_OBJECT_POD4 )
                {
                    py_pod_hash_object<0> * py_pod = (py_pod_hash_object<0> *)_obj;

                    void * impl = (void *)py_pod->buff;

                    return impl;
                }
                else if( flag & PY_OBJECT_POD8 )
                {
                    py_pod_hash_object<1> * py_pod = (py_pod_hash_object<1> *)_obj;

                    void * impl = (void *)py_pod->buff;

                    return impl;
                }
                else if( flag & PY_OBJECT_POD16 )
                {
                    py_pod_hash_object<2> * py_pod = (py_pod_hash_object<2> *)_obj;

                    void * impl = (void *)py_pod->buff;

                    return impl;
                }
                else if( flag & PY_OBJECT_POD32 )
                {
                    py_pod_hash_object<3> * py_pod = (py_pod_hash_object<3> *)_obj;

                    void * impl = (void *)py_pod->buff;

                    return impl;
                }
                else if( flag & PY_OBJECT_POD64 )
                {
                    py_pod_hash_object<4> * py_pod = (py_pod_hash_object<4> *)_obj;

                    void * impl = (void *)py_pod->buff;

                    return impl;
                }
                else if( flag & PY_OBJECT_POD128 )
                {
                    py_pod_hash_object<5> * py_pod = (py_pod_hash_object<5> *)_obj;

                    void * impl = (void *)py_pod->buff;

                    return impl;
                }
                else if( flag & PY_OBJECT_POD256 )
                {
                    py_pod_hash_object<6> * py_pod = (py_pod_hash_object<6> *)_obj;

                    void * impl = (void *)py_pod->buff;

                    return impl;
                }
            }
            else
            {
                if( flag & PY_OBJECT_POD4 )
                {
                    py_pod_object<0> * py_pod = (py_pod_object<0> *)_obj;

                    void * impl = (void *)py_pod->buff;

                    return impl;
                }
                else if( flag & PY_OBJECT_POD8 )
                {
                    py_pod_object<1> * py_pod = (py_pod_object<1> *)_obj;

                    void * impl = (void *)py_pod->buff;

                    return impl;
                }
                else if( flag & PY_OBJECT_POD16 )
                {
                    py_pod_object<2> * py_pod = (py_pod_object<2> *)_obj;

                    void * impl = (void *)py_pod->buff;

                    return impl;
                }
                else if( flag & PY_OBJECT_POD32 )
                {
                    py_pod_object<3> * py_pod = (py_pod_object<3> *)_obj;

                    void * impl = (void *)py_pod->buff;

                    return impl;
                }
                else if( flag & PY_OBJECT_POD64 )
                {
                    py_pod_object<4> * py_pod = (py_pod_object<4> *)_obj;

                    void * impl = (void *)py_pod->buff;

                    return impl;
                }
                else if( flag & PY_OBJECT_POD128 )
                {
                    py_pod_object<5> * py_pod = (py_pod_object<5> *)_obj;

                    void * impl = (void *)py_pod->buff;

                    return impl;
                }
                else if( flag & PY_OBJECT_POD256 )
                {
                    py_pod_object<6> * py_pod = (py_pod_object<6> *)_obj;

                    void * impl = (void *)py_pod->buff;

                    return impl;
                }
            }

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
        //////////////////////////////////////////////////////////////////////////
    }
    //////////////////////////////////////////////////////////////////////////
    PyTypeObject * pod_python::get_pod_type( size_t _pod, bool _hash )
    {
        PyTypeObject * py_pybind_type;

        if( _pod > PYBIND_OBJECT_POD_SIZE )
        {
            return nullptr;
        }

        if( _pod == 0 )
        {
            py_pybind_type = &m_ptr_type;
        }
        else if( _hash == false )
        {
            if( _pod > 128 )
            {
                py_pybind_type = &m_pod_type[6];
            }
            else if( _pod > 64 )
            {
                py_pybind_type = &m_pod_type[5];
            }
            else if( _pod > 32 )
            {
                py_pybind_type = &m_pod_type[4];
            }
            else if( _pod > 16 )
            {
                py_pybind_type = &m_pod_type[3];
            }
            else if( _pod > 8 )
            {
                py_pybind_type = &m_pod_type[2];
            }
            else if( _pod > 4 )
            {
                py_pybind_type = &m_pod_type[1];
            }
            else
            {
                py_pybind_type = &m_pod_type[0];
            }
        }
        else
        {
            if( _pod > 128 )
            {
                py_pybind_type = &m_pod_hash_type[6];
            }
            else if( _pod > 64 )
            {
                py_pybind_type = &m_pod_hash_type[5];
            }
            else if( _pod > 32 )
            {
                py_pybind_type = &m_pod_hash_type[4];
            }
            else if( _pod > 16 )
            {
                py_pybind_type = &m_pod_hash_type[3];
            }
            else if( _pod > 8 )
            {
                py_pybind_type = &m_pod_hash_type[2];
            }
            else if( _pod > 4 )
            {
                py_pybind_type = &m_pod_hash_type[1];
            }
            else
            {
                py_pybind_type = &m_pod_hash_type[0];
            }
        }

        pybind::incref( (PyObject *)py_pybind_type );

        return py_pybind_type;
    }
    //////////////////////////////////////////////////////////////////////////
    static PyTypeObject __make_pod_type( const char * _name, size_t _size )
    {
        PyTypeObject pod_type =
        {
            PyVarObject_HEAD_INIT( nullptr, 0 )
            _name,
            (Py_ssize_t)_size,
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

        return pod_type;
    }
    //////////////////////////////////////////////////////////////////////////
    bool pod_python::initialize()
    {
        m_pod_type[0] = __make_pod_type( "pybind_pod4_type", sizeof( py_pod_object<0> ) );
        m_pod_type[1] = __make_pod_type( "pybind_pod8_type", sizeof( py_pod_object<1> ) );
        m_pod_type[2] = __make_pod_type( "pybind_pod16_type", sizeof( py_pod_object<2> ) );
        m_pod_type[3] = __make_pod_type( "pybind_pod32_type", sizeof( py_pod_object<3> ) );
        m_pod_type[4] = __make_pod_type( "pybind_pod64_type", sizeof( py_pod_object<4> ) );
        m_pod_type[5] = __make_pod_type( "pybind_pod128_type", sizeof( py_pod_object<5> ) );
        m_pod_type[6] = __make_pod_type( "pybind_pod256_type", sizeof( py_pod_object<6> ) );

        m_pod_hash_type[0] = __make_pod_type( "pybind_pod4_hash_type", sizeof( py_pod_hash_object<0> ) );
        m_pod_hash_type[1] = __make_pod_type( "pybind_pod8_hash_type", sizeof( py_pod_hash_object<1> ) );
        m_pod_hash_type[2] = __make_pod_type( "pybind_pod16_hash_type", sizeof( py_pod_hash_object<2> ) );
        m_pod_hash_type[3] = __make_pod_type( "pybind_pod32_hash_type", sizeof( py_pod_hash_object<3> ) );
        m_pod_hash_type[4] = __make_pod_type( "pybind_pod64_hash_type", sizeof( py_pod_hash_object<4> ) );
        m_pod_hash_type[5] = __make_pod_type( "pybind_pod128_hash_type", sizeof( py_pod_hash_object<5> ) );
        m_pod_hash_type[6] = __make_pod_type( "pybind_pod256_hash_type", sizeof( py_pod_hash_object<6> ) );

        m_ptr_type = __make_pod_type( "pybind_ptr_type", sizeof( py_ptr_object ) );

        if( PyType_Ready( &m_ptr_type ) < 0 )
        {
            return false;
        }

        for( size_t i = 0; i != 7; ++i )
        {
            if( PyType_Ready( &m_pod_type[i] ) < 0 )
            {
                return false;
            }
        }

        for( size_t i = 0; i != 7; ++i )
        {
            if( PyType_Ready( &m_pod_hash_type[i] ) < 0 )
            {
                return false;
            }
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void pod_python::finalize()
    {

    }
    //////////////////////////////////////////////////////////////////////////
}