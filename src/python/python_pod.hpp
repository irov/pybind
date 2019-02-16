#pragma once

#include "config/python.hpp"

#include "pybind/types.hpp"

namespace pybind
{
    //////////////////////////////////////////////////////////////////////////
    static const uint32_t PY_OBJECT_UNWRAP = 0xFF000000;
    static const uint32_t PY_OBJECT_HOLDER = 0x00000001;
    static const uint32_t PY_OBJECT_PTR = 0x00000002;
    static const uint32_t PY_OBJECT_HASH = 0x00000004;
    static const uint32_t PY_OBJECT_WEAK = 0x00000008;

    static const uint32_t PY_OBJECT_POD = 0x00000010;
    static const uint32_t PY_OBJECT_POD4 = PY_OBJECT_POD << 0;
    static const uint32_t PY_OBJECT_POD8 = PY_OBJECT_POD << 1;
    static const uint32_t PY_OBJECT_POD16 = PY_OBJECT_POD << 2;
    static const uint32_t PY_OBJECT_POD32 = PY_OBJECT_POD << 3;
    static const uint32_t PY_OBJECT_POD64 = PY_OBJECT_POD << 4;
    //////////////////////////////////////////////////////////////////////////
    namespace detail
    {
        bool is_pod_holder( PyObject * _obj );
        bool is_pod_weak( PyObject * _obj );
        void wrap_pod_ptr( PyObject * _obj, void * _impl, bool _holder );
        void wrap_pod_weak( PyObject * _obj, void * _impl, bool _holder );
        void wrap_pod( PyObject * _obj, void ** _impl, uint32_t _size, bool _hash );
        int64_t get_pod_hash( PyObject * _obj );
        void set_pod_hash( PyObject * _obj, int64_t _hash );
        void * get_pod_impl( PyObject * _obj );
        void unwrap_pod( PyObject * _obj );
        bool is_pod_wrap( PyObject * _obj );
    }
    //////////////////////////////////////////////////////////////////////////
    class pod_python
    {
    public:
        bool initialize();
        void finalize();

    public:
        PyTypeObject * get_pod_type( uint32_t _pod, bool _hash );

    protected:
        PyTypeObject m_pod_type[5];
        PyTypeObject m_pod_hash_type[5];

        PyTypeObject m_ptr_type;
    };
}