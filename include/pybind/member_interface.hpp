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
    class member_adapter_interface
        : public base_adapter_interface
    {
    public:
        member_adapter_interface( const char * _name )
            : base_adapter_interface( _name )
        {
        }

        ~member_adapter_interface() override
        {
        }

    public:
        virtual PyObject * get( kernel_interface * _kernel, void * _self, const class_type_scope_interface_ptr & _scope ) = 0;
        virtual int set( kernel_interface * _kernel, void * _self, PyObject * _args, const class_type_scope_interface_ptr & _scope ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef intrusive_ptr<member_adapter_interface> member_adapter_interface_ptr;
    //////////////////////////////////////////////////////////////////////////
}

