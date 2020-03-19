#pragma once

#include "pybind/adapter_interface.hpp"

namespace pybind
{
    //////////////////////////////////////////////////////////////////////////
    class kernel_interface;
    //////////////////////////////////////////////////////////////////////////
    typedef intrusive_ptr<class class_type_scope_interface> class_type_scope_interface_ptr;
    //////////////////////////////////////////////////////////////////////////
    class new_adapter_interface
        : public adapter_interface
    {
    public:
        virtual void * call( kernel_interface * _kernel, const class_type_scope_interface_ptr & _scope, PyObject * _obj, PyObject * _args, PyObject * _kwds ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef intrusive_ptr<new_adapter_interface> new_adapter_interface_ptr;
    //////////////////////////////////////////////////////////////////////////
}
