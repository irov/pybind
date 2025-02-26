#pragma once

#include "pybind/adapter/base_adapter.hpp"

#include "pybind/call/method_call.hpp"
#include "pybind/call/method_proxy_call.hpp"

namespace pybind
{
    //////////////////////////////////////////////////////////////////////////
    typedef intrusive_ptr<class class_type_scope_interface> class_type_scope_interface_ptr;
    //////////////////////////////////////////////////////////////////////////
    class number_unary_adapter_interface
        : public base_adapter_interface
    {
    public:
        number_unary_adapter_interface( const char * _name )
            : base_adapter_interface( _name )
        {
        }

        ~number_unary_adapter_interface() override
        {
        }

    public:
        virtual PyObject * call( kernel_interface * _kernel, void * _self, const class_type_scope_interface_ptr & _scope ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef intrusive_ptr<number_unary_adapter_interface> number_unary_adapter_interface_ptr;
    //////////////////////////////////////////////////////////////////////////
    template<class C, class F>
    class number_unary_adapter
        : public number_unary_adapter_interface
    {
    public:
        number_unary_adapter( const char * _name, F _fn )
            : number_unary_adapter_interface( _name )
            , m_fn( _fn )
        {
        }

    protected:
        PyObject * call( kernel_interface * _kernel, void * _impl, const class_type_scope_interface_ptr & _scope ) override
        {
            C * self = _kernel->meta_cast_class_t<C>( _impl, _scope );

            PyObject * py_result = detail::return_operator_t( _kernel, (self->*m_fn)() );

            return py_result;
        }

    protected:
        F m_fn;
    };
    //////////////////////////////////////////////////////////////////////////
    template<class C, class P, class F>
    class number_unary_adapter_proxy
        : public number_unary_adapter_interface
    {
    public:
        number_unary_adapter_proxy( const char * _name, F _fn, P * _proxy )
            : number_unary_adapter_interface( _name )
            , m_fn( _fn )
            , m_proxy( _proxy )
        {
        }

    protected:
        PyObject * call( kernel_interface * _kernel, void * _impl, const class_type_scope_interface_ptr & _scope ) override
        {
            C * self = _kernel->meta_cast_class_t<C>( _impl, _scope );

            PyObject * py_result = detail::return_operator_t( _kernel, (m_proxy->*m_fn)(self) );

            return py_result;
        }

    protected:
        F m_fn;
        P * m_proxy;
    };
    //////////////////////////////////////////////////////////////////////////
    template<class C, class F>
    class number_unary_adapter_proxy_function
        : public number_unary_adapter_interface
    {
    public:
        number_unary_adapter_proxy_function( const char * _name, F _fn )
            : number_unary_adapter_interface( _name )
            , m_fn( _fn )
        {
        }

    protected:
        PyObject * call( kernel_interface * _kernel, void * _impl, const class_type_scope_interface_ptr & _scope ) override
        {
            C * self = _kernel->meta_cast_class_t<C>( _impl, _scope );

            PyObject * py_result = detail::return_operator_t( _kernel, (*m_fn)(self) );

            return py_result;
        }

    protected:
        F m_fn;
    };
    //////////////////////////////////////////////////////////////////////////
    template<class C>
    class number_unary_adapter_operator_neg
        : public number_unary_adapter_interface
    {
    public:
        number_unary_adapter_operator_neg( const char * _name )
            : number_unary_adapter_interface( _name )
        {
        }

    protected:
        PyObject * call( kernel_interface * _kernel, void * _impl, const class_type_scope_interface_ptr & _scope ) override
        {
            C * self = _kernel->meta_cast_class_t<C>( _impl, _scope );

            PyObject * py_result = detail::return_operator_t( _kernel, (-*self) );

            return py_result;
        }
    };
    //////////////////////////////////////////////////////////////////////////
}

