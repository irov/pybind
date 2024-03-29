#pragma once

#include "pybind/adapter/base_adapter.hpp"

#include "pybind/call/method_call.hpp"
#include "pybind/call/method_proxy_call.hpp"

namespace pybind
{
    //////////////////////////////////////////////////////////////////////////
    typedef intrusive_ptr<class class_type_scope_interface> class_type_scope_interface_ptr;
    //////////////////////////////////////////////////////////////////////////
    class getattro_adapter_interface
        : public base_adapter_interface
    {
    public:
        getattro_adapter_interface( const char * _name )
            : base_adapter_interface( _name )
        {
        }

        ~getattro_adapter_interface() override
        {
        }

    public:
        virtual PyObject * call( kernel_interface * _kernel, void * _self, const class_type_scope_interface_ptr & _scope, PyObject * _key ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef intrusive_ptr<getattro_adapter_interface> getattro_adapter_interface_ptr;
    //////////////////////////////////////////////////////////////////////////
    template<class C, class F>
    class getattro_adapter
        : public getattro_adapter_interface
    {
    public:
        getattro_adapter( const char * _name, F _fn )
            : getattro_adapter_interface( _name )
            , m_fn( _fn )
        {
        }

    protected:
        PyObject * call( kernel_interface * _kernel, void * _impl, const class_type_scope_interface_ptr & _scope, PyObject * _key ) override
        {
            C * self = _kernel->meta_cast_class_t<C>( _impl, _scope );

            PyObject * py_result = detail::return_operator_t( _kernel, (self->*m_fn)(
                detail::extract_operator_t( _kernel, _key )
                ) );

            return py_result;
        }

    protected:
        F m_fn;
    };
    //////////////////////////////////////////////////////////////////////////
    template<class C, class F>
    class getattro_adapter_depricated
        : public getattro_adapter<C, F>
    {
    public:
        getattro_adapter_depricated( const char * _name, F _fn, const char * _doc )
            : getattro_adapter<C, F>( _name, _fn )
            , m_doc( _doc )
        {
        }

    protected:
        PyObject * call( kernel_interface * _kernel, void * _impl, const class_type_scope_interface_ptr & _scope, PyObject * _key ) override
        {
            const char * scopeName = _kernel->get_class_type_info_t<C>();

            const char * name = this->getName();

            _kernel->error_traceback( "method '%s::%s' depricated '%s'"
                , scopeName
                , name
                , m_doc
            );

            PyObject * ret = getattro_adapter<C, F>::call( _kernel, _impl, _scope, _key );

            return ret;
        }

    protected:
        const char * m_doc;
    };
    //////////////////////////////////////////////////////////////////////////
    template<class C, class P, class F>
    class getattro_adapter_proxy
        : public getattro_adapter_interface
    {
    public:
        getattro_adapter_proxy( const char * _name, F _fn, P * _proxy )
            : getattro_adapter_interface( _name )
            , m_fn( _fn )
            , m_proxy( _proxy )
        {
        }

    protected:
        PyObject * call( kernel_interface * _kernel, void * _impl, const class_type_scope_interface_ptr & _scope, PyObject * _key ) override
        {
            C * self = _kernel->meta_cast_class_t<C>( _impl, _scope );

            PyObject * py_result = detail::return_operator_t( _kernel, (m_proxy->*m_fn)(self
                , detail::extract_operator_t( _kernel, _key )
                ) );

            return py_result;
        }

    protected:
        F m_fn;
        P * m_proxy;
    };
    //////////////////////////////////////////////////////////////////////////
    template<class C, class F>
    class getattro_adapter_proxy_function
        : public getattro_adapter_interface
    {
    public:
        getattro_adapter_proxy_function( const char * _name, F _fn )
            : getattro_adapter_interface( _name )
            , m_fn( _fn )
        {
        }

    protected:
        PyObject * call( kernel_interface * _kernel, void * _impl, const class_type_scope_interface_ptr & _scope, PyObject * _key ) override
        {
            C * self = _kernel->meta_cast_class_t<C>( _impl, _scope );

            PyObject * py_result = detail::return_operator_t( _kernel, (*m_fn)(self
                , detail::extract_operator_t( _kernel, _key )
                ) );

            return py_result;
        }

    protected:
        F m_fn;
    };
}

