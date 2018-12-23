#pragma once

#include "pybind/adapter_interface.hpp"

#include "pybind/bindable.hpp"

namespace pybind
{
    //////////////////////////////////////////////////////////////////////////
    class bindable_adapter_interface
        : public adapter_interface
    {
    public:
        bindable_adapter_interface( const char * _name )
            : m_name( _name )
        {
        }

        ~bindable_adapter_interface() override
        {
        }

    public:
        inline const char * getName() const
        {
            return m_name;
        }

    public:
        virtual void clear( kernel_interface * _kernel, void * _impl, const class_type_scope_interface_ptr & _scope ) = 0;

    protected:
        const char * m_name;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef stdex::intrusive_ptr<bindable_adapter_interface> bindable_adapter_interface_ptr;
    //////////////////////////////////////////////////////////////////////////
    template<class C>
    class bindable_adapter
        : public bindable_adapter_interface
    {
    public:
        bindable_adapter( const char * _name )
            : bindable_adapter_interface( _name )
        {
        }

    protected:
        void clear( kernel_interface * _kernel, void * _impl, const class_type_scope_interface_ptr & _scope ) override
        {
            C * self = _kernel->meta_cast_class_t<C>( _impl, _scope );

            self->clear();
        }
    };
}
