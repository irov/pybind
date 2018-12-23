#pragma once

#include "pybind/functor_interface.hpp"

#include "pybind/call/method_call.hpp"
#include "pybind/call/method_kernel_call.hpp"
#include "pybind/call/method_args_call.hpp"
#include "pybind/call/method_kernel_args_call.hpp"

namespace pybind
{
    //////////////////////////////////////////////////////////////////////////
    template<class C, class M>
    class functor_proxy_adapter
        : public functor_adapter_interface
    {
    public:
        functor_proxy_adapter( const char * _name, uint32_t _arity, C * _self, M _method )
            : functor_adapter_interface( _name, _arity )
            , m_self( _self )
            , m_method( _method )
        {
        }

    public:
        PyObject * call( kernel_interface * _kernel, PyObject * _args, PyObject * _kwds ) override
        {
            (void)_kwds;

            PyObject * ret = method_call<C, M>::call( _kernel, m_self, m_method, _args );

            return ret;
        }

    protected:
        C * m_self;
        M m_method;
    };
    //////////////////////////////////////////////////////////////////////////
    template<class C, class M>
    class functor_proxy_deprecated_adapter
        : public functor_adapter_interface
    {
    public:
        functor_proxy_deprecated_adapter( const char * _name, uint32_t _arity, C * _self, M _method, const char * _doc )
            : functor_adapter_interface( _name, _arity )
            , m_self( _self )
            , m_method( _method )
            , m_doc( _doc )
        {
        }

    public:
        PyObject * call( kernel_interface * _kernel, PyObject * _args, PyObject * _kwds ) override
        {
            (void)_kwds;

            const char * name = this->getName();

            _kernel->error_traceback( "functor '%s' deprecated '%s'"
                , name
                , m_doc
            );

            PyObject * ret = method_call<C, M>::call( _kernel, m_self, m_method, _args );

            return ret;
        }

    protected:
        C * m_self;
        M m_method;
        const char * m_doc;
    };
    //////////////////////////////////////////////////////////////////////////
    template<class C, class FI, class FD, class M>
    class functor_proxy_adapter_ptr
        : public functor_adapter_interface
    {
    public:
        functor_proxy_adapter_ptr( const char * _name, uint32_t _arity, C * _self, FI _incref, FD _decref, M _method )
            : functor_adapter_interface( _name, _arity )
            , m_self( _self )
            , m_incref( _incref )
            , m_decref( _decref )
            , m_method( _method )
        {
            (*m_incref)(m_self);
        }

        ~functor_proxy_adapter_ptr()
        {
            (*m_decref)(m_self);
        }

    public:
        PyObject * call( kernel_interface * _kernel, PyObject * _args, PyObject * _kwds ) override
        {
            (void)_kwds;

            PyObject * ret = method_call<C, M>::call( _kernel, m_self, m_method, _args );

            return ret;
        }

    protected:
        C * m_self;
        FI m_incref;
        FD m_decref;
        M m_method;
    };
    //////////////////////////////////////////////////////////////////////////
    template<class C, class M>
    class functor_proxy_kernel_adapter
        : public functor_adapter_interface
    {
    public:
        functor_proxy_kernel_adapter( const char * _name, uint32_t _arity, C * _self, M _method )
            : functor_adapter_interface( _name, _arity )
            , m_self( _self )
            , m_method( _method )
        {
        }

    public:
        PyObject * call( kernel_interface * _kernel, PyObject * _args, PyObject * _kwds ) override
        {
            (void)_kwds;

            PyObject * ret = method_kernel_call<C, M>::call( _kernel, m_self, m_method, _args );

            return ret;
        }

    protected:
        C * m_self;
        M m_method;
    };
    //////////////////////////////////////////////////////////////////////////
    template<class C, class M>
    class functor_proxy_adapter_args
        : public functor_adapter_interface
    {
    public:
        functor_proxy_adapter_args( const char * _name, uint32_t _arity, C * _self, M _method )
            : functor_adapter_interface( _name, _arity )
            , m_self( _self )
            , m_method( _method )
        {
        }

    public:
        PyObject * call( kernel_interface * _kernel, PyObject * _args, PyObject * _kwds ) override
        {
            (void)_kwds;

            PyObject * ret = method_args_call<C, M>::call( _kernel, m_self, m_method, _args );

            return ret;
        }

    protected:
        C * m_self;
        M m_method;
    };
    //////////////////////////////////////////////////////////////////////////
    template<class C, class M>
    class functor_proxy_kernel_adapter_args
        : public functor_adapter_interface
    {
    public:
        functor_proxy_kernel_adapter_args( const char * _name, uint32_t _arity, C * _self, M _method )
            : functor_adapter_interface( _name, _arity )
            , m_self( _self )
            , m_method( _method )
        {
        }

    public:
        PyObject * call( kernel_interface * _kernel, PyObject * _args, PyObject * _kwds ) override
        {
            (void)_kwds;

            PyObject * ret = method_kernel_args_call<C, M>::call( _kernel, m_self, m_method, _args );

            return ret;
        }

    protected:
        C * m_self;
        M m_method;
    };
}

