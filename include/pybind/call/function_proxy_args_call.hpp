#pragma once

#include "pybind/exports.hpp"
#include "pybind/types.hpp"
#include "pybind/extract.hpp"
#include "pybind/helper.hpp"

#include "stdex/function_traits.h"

namespace pybind
{
    template<class F, class P, class Ret, int i>
    struct function_proxy_args_call_impl
    {
        static Ret call( kernel_interface * _kernel, P * _proxy, F f, PyObject * _arg );
    };

    template<class F, class P, class Ret>
    struct function_proxy_args_call_impl<F, P, Ret, 2>
    {
        static Ret call( kernel_interface * _kernel, P * _proxy, F f, PyObject * _arg )
        {
            (void)_kernel;
            (void)_arg;

            return (*f)(_proxy
                , make_args_t( _kernel, _arg, 0 )
                );
        }
    };

    template<class F, class P, class Ret>
    struct function_proxy_args_call_impl<F, P, Ret, 3>
    {
        static Ret call( kernel_interface * _kernel, P * _proxy, F f, PyObject * _arg )
        {
            return (*f)(_proxy
                , tuple_getitem_t( _kernel, _arg, 0 )
                , make_args_t( _kernel, _arg, 1 )
                );
        }
    };

    template<class F, class P, class Ret>
    struct function_proxy_args_call_impl<F, P, Ret, 4>
    {
        static Ret call( kernel_interface * _kernel, P * _proxy, F f, PyObject * _arg )
        {
            return (*f)(_proxy
                , tuple_getitem_t( _kernel, _arg, 0 )
                , tuple_getitem_t( _kernel, _arg, 1 )
                , make_args_t( _kernel, _arg, 2 )
                );
        }
    };

    template<class F, class P, class Ret>
    struct function_proxy_args_call_impl<F, P, Ret, 5>
    {
        static Ret call( kernel_interface * _kernel, P * _proxy, F f, PyObject * _arg )
        {
            return (*f)(_proxy
                , tuple_getitem_t( _kernel, _arg, 0 )
                , tuple_getitem_t( _kernel, _arg, 1 )
                , tuple_getitem_t( _kernel, _arg, 2 )
                , make_args_t( _kernel, _arg, 3 )
                );
        }
    };

    template<class F, class P, class Ret>
    struct function_proxy_args_call_impl<F, P, Ret, 6>
    {
        static Ret call( kernel_interface * _kernel, P * _proxy, F f, PyObject * _arg )
        {
            return (*f)(_proxy
                , tuple_getitem_t( _kernel, _arg, 0 )
                , tuple_getitem_t( _kernel, _arg, 1 )
                , tuple_getitem_t( _kernel, _arg, 2 )
                , tuple_getitem_t( _kernel, _arg, 3 )
                , make_args_t( _kernel, _arg, 4 )
                );
        }
    };

    template<class F, class P, class Ret>
    struct function_proxy_args_call_impl<F, P, Ret, 7>
    {
        static Ret call( kernel_interface * _kernel, P * _proxy, F f, PyObject * _arg )
        {
            return (*f)(_proxy
                , tuple_getitem_t( _kernel, _arg, 0 )
                , tuple_getitem_t( _kernel, _arg, 1 )
                , tuple_getitem_t( _kernel, _arg, 2 )
                , tuple_getitem_t( _kernel, _arg, 3 )
                , tuple_getitem_t( _kernel, _arg, 4 )
                , make_args_t( _kernel, _arg, 5 )
                );
        }
    };

    template<class F, class P, class Ret>
    struct function_proxy_args_call_ret_impl
    {
        typedef typename stdex::function_traits<F>::result f_info;

        static PyObject * call( kernel_interface * _kernel, P * _proxy, F f, PyObject * _arg )
        {
            PyObject * result = detail::return_operator_t( _kernel, function_proxy_args_call_impl<F, P, Ret, f_info::arity>::call( _kernel, _proxy, f, _arg ) );

            return result;
        }
    };

    template<class F, class P>
    struct function_proxy_args_call_ret_impl<F, P, void>
    {
        typedef typename stdex::function_traits<F>::result f_info;

        static PyObject * call( kernel_interface * _kernel, P * _proxy, F f, PyObject * _arg )
        {
            function_proxy_args_call_impl<F, P, void, f_info::arity>::call( _kernel, _proxy, f, _arg );

            return _kernel->ret_none();
        }
    };

    template<class F, class P>
    struct function_proxy_args_call
    {
        typedef typename stdex::function_traits<F>::result f_info;

        static PyObject * call( kernel_interface * _kernel, F f, PyObject * _arg, P * _proxy )
        {
            uint32_t arg_size = _kernel->tuple_size( _arg );
            uint32_t fn_arity = f_info::arity;

            if( arg_size + 2 < fn_arity )
            {
                pybind::throw_exception( "invalid proxy args function call args is not equal %d < %d\n"
                    , arg_size + 2
                    , fn_arity
                );

                return nullptr;
            }

            PyObject * ret = function_proxy_args_call_ret_impl<F, P, typename f_info::ret_type>::call( _kernel, _proxy, f, _arg );

            return ret;
        }
    };
}

