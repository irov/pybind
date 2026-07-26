#pragma once

#include "pybind/function_interface.hpp"

#include "pybind/call/function_call.hpp"
#include "pybind/call/function_out_call.hpp"
#include "pybind/call/function_args_call.hpp"
#include "pybind/call/function_proxy_call.hpp"
#include "pybind/call/function_proxy_args_call.hpp"
#include "pybind/call/function_kernel_call.hpp"
#include "pybind/call/function_kernel_args_call.hpp"
#include "pybind/call/function_proxy_kernel_args_call.hpp"

#include "pybind/types.hpp"

#include "stdex/function_traits.h"

#include <type_traits>

namespace pybind
{
    //////////////////////////////////////////////////////////////////////////
    template<class F>
    class function_adapter_base
        : public function_adapter_interface
    {
    public:
        function_adapter_base( const char * _name, size_t _arity, F _fn )
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
        function_adapter( const char * _name, size_t _arity, F _fn )
            : function_adapter_base<F>( _name, _arity, _fn )
        {
        }

    protected:
        PyObject * call( kernel_interface * _kernel, PyObject * _args, PyObject * _kwds ) override
        {
            (void)_kernel;
            (void)_kwds;

            F fn = this->getFn();

            PyObject * ret = function_call<F>::call( _kernel, fn, _args );

            return ret;
        }
    };
    //////////////////////////////////////////////////////////////////////////
    template<class F>
    class function_out_adapter
        : public function_adapter_base<F>
    {
    public:
        function_out_adapter( const char * _name, size_t _arity, F _fn )
            : function_adapter_base<F>( _name, _arity, _fn )
        {
        }

    protected:
        PyObject * call( kernel_interface * _kernel, PyObject * _args, PyObject * _kwds ) override
        {
            (void)_kwds;

            F fn = this->getFn();

            PyObject * ret = function_out_call<F>::call( _kernel, fn, _args );

            return ret;
        }
    };
    //////////////////////////////////////////////////////////////////////////
    template<class F>
    class function_adapter_args
        : public function_adapter_base<F>
    {
    public:
        function_adapter_args( const char * _name, size_t _arity, F _fn )
            : function_adapter_base<F>( _name, _arity, _fn )
        {
        }

    protected:
        PyObject * call( kernel_interface * _kernel, PyObject * _args, PyObject * _kwds ) override
        {
            (void)_kernel;
            (void)_kwds;

            F fn = this->getFn();

            PyObject * ret = function_args_call<F>::call( _kernel, fn, _args );

            return ret;
        }
    };
    //////////////////////////////////////////////////////////////////////////
    template<class F, class P>
    class function_proxy_adapter
        : public function_adapter_base<F>
    {
    public:
        function_proxy_adapter( const char * _name, size_t _arity, F _fn, P * _proxy )
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

            PyObject * ret = function_proxy_call<F, P>::call( _kernel, fn, _args, m_proxy );

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
        function_proxy_adapter_args( const char * _name, size_t _arity, F _fn, P * _proxy )
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

            PyObject * ret = function_proxy_args_call<F, P>::call( _kernel, fn, _args, m_proxy );

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
        function_adapter_deprecate( const char * _name, size_t _arity, F _fn, const char * _doc )
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

            _kernel->error_traceback( "function '%s' deprecated '%s'"
                , name
                , m_doc
            );

            F fn = this->getFn();

            PyObject * ret = function_call<F>::call( _kernel, fn, _args );

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
        function_kernel_adapter( const char * _name, size_t _arity, F _fn )
            : function_adapter_base<F>( _name, _arity, _fn )
        {
        }

    protected:
        PyObject * call( kernel_interface * _kernel, PyObject * _args, PyObject * _kwds ) override
        {
            (void)_kernel;
            (void)_kwds;

            F fn = this->getFn();

            PyObject * ret = function_kernel_call<F>::call( _kernel, fn, _args );

            return ret;
        }
    };
    //////////////////////////////////////////////////////////////////////////
    template<class F>
    class function_kernel_adapter_args
        : public function_adapter_base<F>
    {
    public:
        function_kernel_adapter_args( const char * _name, size_t _arity, F _fn )
            : function_adapter_base<F>( _name, _arity, _fn )
        {
        }

    protected:
        PyObject * call( kernel_interface * _kernel, PyObject * _args, PyObject * _kwds ) override
        {
            (void)_kwds;

            F fn = this->getFn();

            PyObject * ret = function_kernel_args_call<F>::call( _kernel, fn, _args );

            return ret;
        }
    };
    //////////////////////////////////////////////////////////////////////////
    template<class F>
    class function_adapter_native
        : public function_adapter_base<F>
    {
    private:
        typedef typename stdex::function_traits<F>::result f_info;

        static_assert(f_info::method == false, "[pybind] native function bind requires a function");
        static_assert(f_info::arity == 3, "[pybind] native function must have kernel, args and kwds arguments");
        static_assert(std::is_same<typename f_info::template iterator_param<0>, pybind::kernel_interface *>::value == true, "[pybind] native function first argument must be kernel_interface *");
        static_assert(std::is_same<typename f_info::template iterator_param<1>, PyObject *>::value == true, "[pybind] native function second argument must be PyObject * args");
        static_assert(std::is_same<typename f_info::template iterator_param<2>, PyObject *>::value == true, "[pybind] native function third argument must be PyObject * kwds");
        static_assert(std::is_same<typename f_info::ret_type, PyObject *>::value == true, "[pybind] native function must return PyObject *");

    public:
        function_adapter_native( const char * _name, size_t _arity, F _fn )
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
