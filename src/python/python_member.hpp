#pragma once

#include "pybind/adapter/member_adapter.hpp"

#include "pybind/types.hpp"

#include "config/python.hpp"

namespace pybind
{
    class member_python
    {
    public:
        bool initialize();
        void finalize();

    public:
        PyObject * create_member_adapter( const member_adapter_interface_ptr & _iadapter );

    protected:
        PyTypeObject m_member_type;

        PyMethodDef m_setmethod;
        PyMethodDef m_getmethod;
    };
}

