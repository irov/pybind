#pragma once

#include "pybind/adapter_interface.hpp"

namespace pybind
{
    //////////////////////////////////////////////////////////////////////////
    class kernel_interface;
    //////////////////////////////////////////////////////////////////////////
    typedef stdex::intrusive_ptr<class class_type_scope_interface> class_type_scope_interface_ptr;
    //////////////////////////////////////////////////////////////////////////
    class destroy_adapter_interface
        : public adapter_interface
    {
    public:
        virtual void call( kernel_interface * _kernel, const class_type_scope_interface_ptr & _scope, void * _impl ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef stdex::intrusive_ptr<destroy_adapter_interface> destroy_adapter_interface_ptr;
}
