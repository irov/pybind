#pragma once

#include "pybind/adapter_interface.hpp"

#include "pybind/types.hpp"

namespace pybind
{
    //////////////////////////////////////////////////////////////////////////
    class kernel_interface;
    //////////////////////////////////////////////////////////////////////////
    typedef stdex::intrusive_ptr<class class_type_scope_interface> class_type_scope_interface_ptr;
    //////////////////////////////////////////////////////////////////////////
    class member_adapter_interface
        : public adapter_interface
    {
    public:
        member_adapter_interface( const char * _name )
            : m_name( _name )
        {
        }

        ~member_adapter_interface() override
        {
        }

    public:
        inline const char * getName() const
        {
            return m_name;
        }

    public:
        virtual PyObject * get( kernel_interface * _kernel, void * _self, const class_type_scope_interface_ptr & _scope ) = 0;
        virtual int set( kernel_interface * _kernel, void * _self, PyObject * _args, const class_type_scope_interface_ptr & _scope ) = 0;

    protected:
        const char * m_name;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef stdex::intrusive_ptr<member_adapter_interface> member_adapter_interface_ptr;
    //////////////////////////////////////////////////////////////////////////
}

