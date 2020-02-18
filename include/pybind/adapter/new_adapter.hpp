#pragma once

#include "pybind/adapter_interface.hpp"

#include "pybind/kernel.hpp"

namespace pybind
{
    //////////////////////////////////////////////////////////////////////////
    class new_adapter_interface
        : public adapter_interface
    {
    public:
        virtual void * call( kernel_interface * _kernel, const class_type_scope_interface_ptr & _scope, PyObject * _obj, PyObject * _args, PyObject * _kwds ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef stdex::intrusive_ptr<new_adapter_interface> new_adapter_interface_ptr;
    //////////////////////////////////////////////////////////////////////////
}
