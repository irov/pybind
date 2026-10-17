#include "python_pod.hpp"
#include "python_system.hpp"

#include "pybind/kernel_interface.hpp"
#include "pybind/class_type_scope_interface.hpp"
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

        // As the pod of py_pod_hash_object and of the tinypy payload: a struct
        // of 8-byte members is constructed in it.
        alignas( 8 ) uint8_t buff[4 << I];
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
        static bool release_instance( PyObject * _obj )
        {
            // Every bound type deallocates through its pod base, which subtype_dealloc
            // reaches once it cleared the instance; unlike tp_del, this keeps bound
            // objects collectable in cycles and covers Python subclasses.
            kernel_interface * kernel = pybind::get_kernel();

            if( kernel == nullptr )
            {
                return true;
            }

            void * impl = helper::get_pod_impl( _obj );

            if( impl == nullptr || helper::is_pod_weak( _obj ) == true )
            {
                return true;
            }

            PyTypeObject * objtype = pybind::object_type( _obj );

            // An object of a class this kernel has no scope for, as a removed one
            // or one of a previous kernel, whose type ids name other classes of
            // this kernel, is not released.
            if( kernel->get_class_scope( objtype ) == nullptr )
            {
                return true;
            }

            // As the tinypy backend, whose payload keeps the scope: the scope that
            // created the object releases it, which holds once its class is the
            // one of another bound class. It is held, as the C++ code may remove
            // it.
            typeid_t typeId = helper::get_pod_type_id( _obj );
            class_type_scope_interface_ptr scope = kernel->get_class_type_scope( typeId );

            if( scope == nullptr )
            {
                return true;
            }

            // As slot_tp_del: the object is alive while C++ code runs, which keeps
            // the pending error intact. It is unbound first, as the tinypy backend
            // clears the impl of its payload, so an instance C++ code resurrects
            // does not release the object again.
            _obj->ob_refcnt = 1;

            helper::unwrap_pod( _obj );

            PyObject * exception;
            PyObject * value;
            PyObject * traceback;
            PyErr_Fetch( &exception, &value, &traceback );

            try
            {
                scope->clear_bindable( impl );
                scope->call_destructor( _obj, impl );
                scope->decref_smart_pointer( impl );
            }
            catch( const pybind::pybind_exception & _ex )
            {
                PyErr_SetString( PyExc_RuntimeError, _ex.what() );
                PyErr_WriteUnraisable( (PyObject *)objtype );
            }

            // As subtype_dealloc after tp_del: an object C++ code did not resurrect
            // drops the weak references, the slots and the attributes it was given
            // while the code ran, which subtype_dealloc cleared before.
            if( _obj->ob_refcnt == 1 )
            {
                if( objtype->tp_weaklistoffset != 0 )
                {
                    PyWeakReference ** list = (PyWeakReference **)PyObject_GET_WEAKREFS_LISTPTR( _obj );

                    while( *list != nullptr )
                    {
                        _PyWeakref_ClearRef( *list );
                    }
                }

                if( objtype->tp_clear != nullptr )
                {
                    objtype->tp_clear( _obj );
                }
            }

            PyErr_Restore( exception, value, traceback );

            if( --_obj->ob_refcnt == 0 )
            {
                return true;
            }

            // Resurrected by C++ code: subtype_dealloc untracked the instance and
            // releases the type once this returns, so the surviving instance is
            // tracked again and keeps a reference of its own.
            if( PyType_IS_GC( objtype ) != 0 )
            {
                PyObject_GC_Track( _obj );
            }

            Py_INCREF( objtype );

            return false;
        }
        //////////////////////////////////////////////////////////////////////////
        static void py_dealloc( PyObject * _obj )
        {
            if( detail::release_instance( _obj ) == false )
            {
                return;
            }

            PyTypeObject * objtype = pybind::object_type( _obj );

            objtype->tp_free( _obj );
        }
        //////////////////////////////////////////////////////////////////////////
        static void raise_cannot_create( const char * _name )
        {
            // As a type without tp_new and the tinypy backend; an error a
            // constructor raised stays.
            if( PyErr_Occurred() != nullptr )
            {
                return;
            }

            PyErr_Format( PyExc_TypeError, "cannot create '%s' instances"
                , _name
            );
        }
        //////////////////////////////////////////////////////////////////////////
        static PyObject * py_new_class( PyTypeObject * _type, PyObject * _args, PyObject * _kwds )
        {
            kernel_interface * kernel = pybind::get_kernel();

            const class_type_scope_interface_ptr & scope = kernel->get_class_scope( _type );

            if( scope == nullptr )
            {
                detail::raise_cannot_create( _type->tp_name );

                return nullptr;
            }

            PyObject * py_self = PyType_GenericAlloc( _type, 0 );

            if( py_self == nullptr )
            {
                return nullptr;
            }

            try
            {
                void * impl = scope->call_new( py_self, _args, _kwds );

                if( impl == nullptr )
                {
                    pybind::decref( py_self );

                    detail::raise_cannot_create( scope->get_name() );

                    return nullptr;
                }

                helper::wrap_pod_ptr( py_self, impl, false, scope->get_type_id() );

                scope->incref_smart_pointer( impl );

                return py_self;
            }
            catch( const pybind::pybind_exception & _ex )
            {
                pybind::decref( py_self );

                pybind::error_message( "obj '%s' py_new_class scope '%s' exception: %s"
                    , kernel->object_str( (PyObject *)_type ).c_str()
                    , scope->get_name()
                    , _ex.what()
                );
            }

            return nullptr;
        }
        //////////////////////////////////////////////////////////////////////////
        static PyObject * py_new_pod( PyTypeObject * _type, PyObject * _args, PyObject * _kwds )
        {
            kernel_interface * kernel = pybind::get_kernel();

            const class_type_scope_interface_ptr & scope = kernel->get_class_scope( _type );

            if( scope == nullptr )
            {
                detail::raise_cannot_create( _type->tp_name );

                return nullptr;
            }

            PyObject * py_self = PyType_GenericAlloc( _type, 0 );

            if( py_self == nullptr )
            {
                return nullptr;
            }

            void * buff = nullptr;
            size_t pod_size = scope->get_pod_size();
            bool pod_hash = scope->get_pod_hash();

            typeid_t typeId = scope->get_type_id();

            helper::wrap_pod( py_self, &buff, pod_size, pod_hash, typeId );

            try
            {
                void * impl = scope->call_new( py_self, _args, _kwds );

                if( impl == nullptr )
                {
                    helper::unwrap_pod( py_self );

                    pybind::decref( py_self );

                    detail::raise_cannot_create( scope->get_name() );

                    return nullptr;
                }

                scope->incref_smart_pointer( impl );

                return py_self;
            }
            catch( const pybind::pybind_exception & _ex )
            {
                helper::unwrap_pod( py_self );

                pybind::decref( py_self );

                pybind::error_message( "obj '%s' py_new_pod scope '%s' exception: %s"
                    , kernel->object_str( (PyObject *)_type ).c_str()
                    , scope->get_name()
                    , _ex.what()
                );
            }

            return nullptr;
        }
        //////////////////////////////////////////////////////////////////////////
        static PyObject * get_object_class_descriptor()
        {
            PyObject * py_descr = PyDict_GetItemString( PyBaseObject_Type.tp_dict, "__class__" );

            return py_descr;
        }
        //////////////////////////////////////////////////////////////////////////
        static PyObject * py_get_class( PyObject * _obj, void * _closure )
        {
            (void)_closure;

            PyObject * py_descr = detail::get_object_class_descriptor();

            PyObject * py_class = Py_TYPE( py_descr )->tp_descr_get( py_descr, _obj, (PyObject *)Py_TYPE( _obj ) );

            return py_class;
        }
        //////////////////////////////////////////////////////////////////////////
        static int py_set_class( PyObject * _obj, PyObject * _value, void * _closure )
        {
            (void)_closure;

            // As the tinypy backend: the methods of another bound class would run
            // on this object, so only a class of the same bound class is taken.
            kernel_interface * kernel = pybind::get_kernel();

            if( kernel != nullptr && _value != nullptr && PyType_Check( _value ) != 0 )
            {
                class_type_scope_interface * scope = kernel->get_class_scope( Py_TYPE( _obj ) ).get();
                class_type_scope_interface * value_scope = kernel->get_class_scope( (PyTypeObject *)_value ).get();

                if( scope != value_scope )
                {
                    PyErr_Format( PyExc_TypeError, "__class__ assignment: '%s' and '%s' are different bound classes"
                        , Py_TYPE( _obj )->tp_name
                        , ((PyTypeObject *)_value)->tp_name
                    );

                    return -1;
                }
            }

            PyObject * py_descr = detail::get_object_class_descriptor();

            int result = Py_TYPE( py_descr )->tp_descr_set( py_descr, _obj, _value );

            return result;
        }
        //////////////////////////////////////////////////////////////////////////
        static PyGetSetDef py_pod_getset[] = {
            {const_cast<char *>("__class__"), &detail::py_get_class, &detail::py_set_class, nullptr, nullptr},
            {nullptr}
        };
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
        bool is_pod_object( PyObject * _obj )
        {
            // The solid base of a bound type and of its Python subclasses is a
            // pod type, whose dealloc releases the object; any other object has
            // no pybind flag, and this holds once its scope is removed too.
            PyTypeObject * type = Py_TYPE( _obj );

            while( type != nullptr && ( type->tp_flags & Py_TPFLAGS_HEAPTYPE ) != 0 )
            {
                type = type->tp_base;
            }

            if( type == nullptr )
            {
                return false;
            }

            return type->tp_dealloc == &detail::py_dealloc;
        }
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
        bool has_pod_hash( PyObject * _obj )
        {
            py_base_object * py_base = (py_base_object *)_obj;

            return (py_base->flag & PY_OBJECT_HASH) != 0;
        }
        //////////////////////////////////////////////////////////////////////////
        void wrap_pod_ptr( PyObject * _obj, void * _impl, bool _holder, typeid_t _typeId )
        {
            py_ptr_object * py_ptr = (py_ptr_object *)_obj;

            py_ptr->impl = _impl;

            py_ptr->flag |= PY_OBJECT_PTR;
            py_ptr->flag |= _typeId << PY_OBJECT_TYPE_ID_SHIFT;

            if( _holder == true )
            {
                py_ptr->flag |= PY_OBJECT_HOLDER;
            }
        }
        //////////////////////////////////////////////////////////////////////////
        void wrap_pod_weak( PyObject * _obj, void * _impl, bool _holder, typeid_t _typeId )
        {
            py_ptr_object * py_ptr = (py_ptr_object *)_obj;

            py_ptr->impl = _impl;

            py_ptr->flag |= PY_OBJECT_PTR;
            py_ptr->flag |= PY_OBJECT_WEAK;
            py_ptr->flag |= _typeId << PY_OBJECT_TYPE_ID_SHIFT;

            if( _holder == true )
            {
                py_ptr->flag |= PY_OBJECT_HOLDER;
            }
        }
        //////////////////////////////////////////////////////////////////////////
        void wrap_pod( PyObject * _obj, void ** _impl, size_t _size, bool _hash, typeid_t _typeId )
        {
            py_base_object * py_base = (py_base_object *)_obj;

            py_base->flag |= _typeId << PY_OBJECT_TYPE_ID_SHIFT;

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
        typeid_t get_pod_type_id( PyObject * _obj )
        {
            py_base_object * py_base = (py_base_object *)_obj;

            typeid_t typeId = ( py_base->flag & PY_OBJECT_TYPE_ID ) >> PY_OBJECT_TYPE_ID_SHIFT;

            return typeId;
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
    static PyTypeObject __make_pod_type( const char * _name, size_t _size, newfunc _new )
    {
        // PyType_Ready names tp_new with __new__, which bound types and their
        // Python subclasses inherit.
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
            detail::py_pod_getset,                                          /* tp_getset */
            0,                         /* tp_base */
            0,                                          /* tp_dict */
            0,                                          /* tp_descr_get */
            0,                                          /* tp_descr_set */
            0,                                          /* tp_dictoffset */
            0,                                          /* tp_init */
            0,                                          /* tp_alloc */
            _new,                                 /* tp_new */
            0,                               /* tp_free */
        };

        return pod_type;
    }
    //////////////////////////////////////////////////////////////////////////
    // As builtin types: object.__subclasses__ refers to readied types past
    // Py_Finalize, so they live as long as the process, and a kernel that is
    // initialized again finds them ready.
    PyTypeObject pod_python::m_pod_type[7];
    PyTypeObject pod_python::m_pod_hash_type[7];
    PyTypeObject pod_python::m_ptr_type;
    //////////////////////////////////////////////////////////////////////////
    bool pod_python::initialize()
    {
        if( ( m_ptr_type.tp_flags & Py_TPFLAGS_READY ) == 0 )
        {
            m_pod_type[0] = __make_pod_type( "pybind_pod4_type", sizeof( py_pod_object<0> ), &detail::py_new_pod );
            m_pod_type[1] = __make_pod_type( "pybind_pod8_type", sizeof( py_pod_object<1> ), &detail::py_new_pod );
            m_pod_type[2] = __make_pod_type( "pybind_pod16_type", sizeof( py_pod_object<2> ), &detail::py_new_pod );
            m_pod_type[3] = __make_pod_type( "pybind_pod32_type", sizeof( py_pod_object<3> ), &detail::py_new_pod );
            m_pod_type[4] = __make_pod_type( "pybind_pod64_type", sizeof( py_pod_object<4> ), &detail::py_new_pod );
            m_pod_type[5] = __make_pod_type( "pybind_pod128_type", sizeof( py_pod_object<5> ), &detail::py_new_pod );
            m_pod_type[6] = __make_pod_type( "pybind_pod256_type", sizeof( py_pod_object<6> ), &detail::py_new_pod );

            m_pod_hash_type[0] = __make_pod_type( "pybind_pod4_hash_type", sizeof( py_pod_hash_object<0> ), &detail::py_new_pod );
            m_pod_hash_type[1] = __make_pod_type( "pybind_pod8_hash_type", sizeof( py_pod_hash_object<1> ), &detail::py_new_pod );
            m_pod_hash_type[2] = __make_pod_type( "pybind_pod16_hash_type", sizeof( py_pod_hash_object<2> ), &detail::py_new_pod );
            m_pod_hash_type[3] = __make_pod_type( "pybind_pod32_hash_type", sizeof( py_pod_hash_object<3> ), &detail::py_new_pod );
            m_pod_hash_type[4] = __make_pod_type( "pybind_pod64_hash_type", sizeof( py_pod_hash_object<4> ), &detail::py_new_pod );
            m_pod_hash_type[5] = __make_pod_type( "pybind_pod128_hash_type", sizeof( py_pod_hash_object<5> ), &detail::py_new_pod );
            m_pod_hash_type[6] = __make_pod_type( "pybind_pod256_hash_type", sizeof( py_pod_hash_object<6> ), &detail::py_new_pod );

            m_ptr_type = __make_pod_type( "pybind_ptr_type", sizeof( py_ptr_object ), &detail::py_new_class );
        }

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
