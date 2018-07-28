#pragma once

#include "pybind/function_interface.hpp"

#include "pybind/call/function_call.hpp"
#include "pybind/call/function_proxy_call.hpp"
#include "pybind/call/function_proxy_args_call.hpp"
#include "pybind/call/function_kernel_call.hpp"

#include "pybind/types.hpp"

namespace pybind
{
    //////////////////////////////////////////////////////////////////////////
    template<class F>
    class function_adapter_base
        : public function_adapter_interface
    {
    public:
        function_adapter_base( const char * _name, uint32_t _arity, F _fn )
            : function_adapter_interface( _name, _arity )
            , m_fn( _fn )
        {
        }

    protected:
        F getFn() const
        {
            return m_fn;
        }

    protected:
        F m_fn;
    };
    //////////////////////////////////////////////////////////////////////////
    template<class F>
    class function_adapter
        : public function_adapter_base<F>
    {
    public:
        function_adapter( const char * _name, uint32_t _arity, F _fn )
            : function_adapter_base<F>( _name, _arity, _fn )
        {
        }

    protected:
        PyObject * call( kernel_interface * _kernel, PyObject * _args, PyObject * _kwds ) override
        {
            (void)_kernel;
            (void)_kwds;

            F fn = this->getFn();

            PyObject *ret = function_call<F>::call( _kernel, fn, _args );

            return ret;
        }
    };
    //////////////////////////////////////////////////////////////////////////
    template<class F, class P>
    class function_proxy_adapter
        : public function_adapter_base<F>
    {
    public:
        function_proxy_adapter( const char * _name, uint32_t _arity, F _fn, P * _proxy )
            : function_adapter_base<F>( _name, _arity, _fn )
            , m_proxy( _proxy )
        {
        }

    protected:
        PyObject * call( kernel_interface * _kernel, PyObject * _args, PyObject * _kwds ) override
        {
            (void)_kernel;
            (void)_kwds;

            F fn = this->getFn();

            PyObject *ret = function_proxy_call<F, P>::call( _kernel, fn, _args, m_proxy );

            return ret;
        }

    protected:
        P * m_proxy;
    };
    //////////////////////////////////////////////////////////////////////////
    template<class F, class P>
    class function_proxy_adapter_args
        : public function_adapter_base<F>
    {
    public:
        function_proxy_adapter_args( const char * _name, uint32_t _arity, F _fn, P * _proxy )
            : function_adapter_base<F>( _name, _arity, _fn )
            , m_proxy( _proxy )
        {
        }

    protected:
        PyObject * call( kernel_interface * _kernel, PyObject * _args, PyObject * _kwds ) override
        {
            (void)_kernel;
            (void)_kwds;

            F fn = this->getFn();

            PyObject *ret = function_proxy_args_call<F, P>::call( _kernel, fn, _args, m_proxy );

            return ret;
        }

    protected:
        P * m_proxy;
    };
    //////////////////////////////////////////////////////////////////////////
    template<class F>
    class function_adapter_deprecate
        : public function_adapter_base<F>
    {
    public:
        function_adapter_deprecate( const char * _name, uint32_t _arity, F _fn, const char * _doc )
            : function_adapter_base<F>( _name, _arity, _fn )
            , m_doc( _doc )
        {
        }

    protected:
        PyObject * call( kernel_interface * _kernel, PyObject * _args, PyObject * _kwds ) override
        {
            (void)_kernel;
            (void)_kwds;

            const char * name = this->getName();

            pybind::error_traceback( "function '%s' deprecated '%s'"
                , name
                , m_doc
            );

            F fn = this->getFn();

            PyObject *ret = function_call<F>::call( _kernel, fn, _args );

            return ret;
        }

    protected:
        const char * m_doc;
    };
    //////////////////////////////////////////////////////////////////////////
    template<class F>
    class function_kernel_adapter
        : public function_adapter_base<F>
    {
    public:
        function_kernel_adapter( const char * _name, uint32_t _arity, F _fn )
            : function_adapter_base<F>( _name, _arity, _fn )
        {
        }

    protected:
        PyObject * call( kernel_interface * _kernel, PyObject * _args, PyObject * _kwds ) override
        {
            (void)_kernel;
            (void)_kwds;

            F fn = this->getFn();

            PyObject *ret = function_kernel_call<F>::call( _kernel, fn, _args );

            return ret;
        }
    };
    //////////////////////////////////////////////////////////////////////////
    template<class F>
    class function_adapter_native
        : public function_adapter_base<F>
    {
    public:
        function_adapter_native( const char * _name, uint32_t _arity, F _fn )
            : function_adapter_base<F>( _name, _arity, _fn )
        {
        }

    protected:
        PyObject * call( kernel_interface * _kernel, PyObject * _args, PyObject * _kwds ) override
        {
            F fn = this->getFn();

            PyObject * ret = fn( _kernel, _args, _kwds );

            return ret;
        }
    };
}