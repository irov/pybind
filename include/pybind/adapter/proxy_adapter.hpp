#pragma once

#include "pybind/adapter_interface.hpp"

#include "pybind/adapter/method_adapter.hpp"

namespace pybind
{
    //////////////////////////////////////////////////////////////////////////
    class proxy_adapter_interface
        : public adapter_interface
    {
    public:
        virtual void * get_proxy( kernel_interface * _kernel, void * _impl, const class_type_scope_interface_ptr & _scope ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef intrusive_ptr<proxy_adapter_interface> proxy_adapter_interface_ptr;
    //////////////////////////////////////////////////////////////////////////
    template<class C, class P, class F>
    class proxy_adapter
        : public proxy_adapter_interface
    {
    public:
        proxy_adapter( F _fn )
            : m_fn( _fn )
        {
        }

        ~proxy_adapter() override
        {
        }

    protected:
        void * get_proxy( kernel_interface * _kernel, void * _impl, const class_type_scope_interface_ptr & _scope ) override
        {
            C * self = _kernel->meta_cast_class_t<C>( _impl, _scope );

            P * proxy = (self->*m_fn)();

            return reinterpret_cast<void *>(proxy);
        }

    protected:
        F m_fn;
    };
    //////////////////////////////////////////////////////////////////////////
    class proxy_method_adapter
        : public method_adapter_interface
    {
    public:
        proxy_method_adapter( const char * _name, const proxy_adapter_interface_ptr & _iproxy, const method_adapter_interface_ptr & _imethod, const class_type_scope_interface_ptr & _proxy_scope )
            : method_adapter_interface( _name )
            , m_iproxy( _iproxy )
            , m_imethod( _imethod )
            , m_proxy_scope( _proxy_scope )
        {
        }

        ~proxy_method_adapter() override
        {
        }

    public:
        PyObject * call( kernel_interface * _kernel, void * _impl, const class_type_scope_interface_ptr & _scope, PyObject * _args, PyObject * _kwds ) override
        {
            void * proxy = m_iproxy->get_proxy( _kernel, _impl, _scope );

            PyObject * result = m_imethod->call( _kernel, proxy, m_proxy_scope, _args, _kwds );

            return result;
        }

    protected:
        proxy_adapter_interface_ptr m_iproxy;
        method_adapter_interface_ptr m_imethod;
        class_type_scope_interface_ptr m_proxy_scope;
    };
    //////////////////////////////////////////////////////////////////////////
}
