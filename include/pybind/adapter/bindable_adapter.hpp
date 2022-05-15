#pragma once

#include "pybind/adapter/base_adapter.hpp"

namespace pybind
{
    //////////////////////////////////////////////////////////////////////////
    class bindable_adapter_interface
        : public base_adapter_interface
    {
    public:
        bindable_adapter_interface( const char * _name )
            : base_adapter_interface( _name )
        {
        }

        ~bindable_adapter_interface() override
        {
        }

    public:
        virtual void clear( kernel_interface * _kernel, void * _impl, const class_type_scope_interface_ptr & _scope ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef intrusive_ptr<bindable_adapter_interface> bindable_adapter_interface_ptr;
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

            self->clearEmbed();
        }
    };
}
