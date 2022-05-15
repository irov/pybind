#pragma once

#include "pybind/adapter/base_adapter.hpp"

#include "pybind/types.hpp"

namespace pybind
{
    //////////////////////////////////////////////////////////////////////////
    class kernel_interface;
    //////////////////////////////////////////////////////////////////////////
    typedef intrusive_ptr<class class_type_scope_interface> class_type_scope_interface_ptr;
    //////////////////////////////////////////////////////////////////////////
    class method_adapter_interface
        : public base_adapter_interface
    {
    public:
        method_adapter_interface( const char * _name )
            : base_adapter_interface( _name )
        {
        }

        ~method_adapter_interface() override
        {
        }

    public:
        virtual PyObject * call( kernel_interface * _kernel, void * _impl, const class_type_scope_interface_ptr & _scope, PyObject * _args, PyObject * _kwds ) = 0;

    };
    //////////////////////////////////////////////////////////////////////////
    typedef intrusive_ptr<method_adapter_interface> method_adapter_interface_ptr;
    //////////////////////////////////////////////////////////////////////////
}

