#pragma once

#include "pybind/exports.hpp"
#include "pybind/types.hpp"
#include "pybind/extract.hpp"
#include "pybind/helper.hpp"

#include "stdex/function_traits.h"

#include <type_traits>

namespace pybind
{
    template<class P, class C, class F, class Ret>
    struct method_proxy_kernel_call_impl
    {
        template<size_t ... I>
        static Ret call( kernel_interface * _kernel, P * _proxy, C * _obj, F f, PyObject * _arg, std::integer_sequence<uint32_t, I...> )
        {
            return (_proxy->*f)(_kernel, _obj
                , tuple_getitem_t( _kernel, _arg, I ) ...
                );
        }
    };

    template<class P, class C, class F, uint32_t Arity, class Ret>
    struct method_proxy_kernel_call_ret_impl
    {
        static PyObject * call( kernel_interface * _kernel, P * _proxy, C * _obj, F f, PyObject * _arg )
        {
            PyObject * py_result = detail::return_operator_t( _kernel, method_proxy_kernel_call_impl<P, C, F, Ret>::call( _kernel, _proxy, _obj, f, _arg, std::make_integer_sequence<uint32_t, Arity>() ) );

            return py_result;
        }
    };

    template<class P, class C, class F, uint32_t Arity>
    struct method_proxy_kernel_call_ret_impl<P, C, F, Arity, void>
    {
        static PyObject * call( kernel_interface * _kernel, P * _proxy, C * _obj, F f, PyObject * _arg )
        {
            method_proxy_kernel_call_impl<P, C, F, void>::call( _kernel, _proxy, _obj, f, _arg, std::make_integer_sequence<uint32_t, Arity>() );

            return _kernel->ret_none();
        }
    };

    template<class P, class C, class F>
    struct method_proxy_kernel_call
    {
        typedef typename stdex::function_traits<F>::result f_info;

        static PyObject * call( kernel_interface * _kernel, P * _proxy, C * _obj, F f, PyObject * _arg )
        {
            uint32_t arg_size = _kernel->tuple_size( _arg );
            uint32_t fn_arity = f_info::arity;

            static_assert(std::is_same<typename f_info::first_param, pybind::kernel_interface *>::value == true, "[pybind] use kernel bind");
            static_assert(std::is_same<typename f_info::last_param, const pybind::args &>::value == false, "[pybind] use args bind");

            if( arg_size + 2 != fn_arity )
            {
                pybind::throw_exception( "invalid proxy method call kernel args is not equal %d + addition %d != %d\n"
                    , (uint32_t)arg_size
                    , (uint32_t)2
                    , (uint32_t)fn_arity
                );

                return nullptr;
            }

            PyObject * ret = method_proxy_kernel_call_ret_impl<P, C, F, f_info::arity - 2, typename f_info::ret_type>::call( _kernel, _proxy, _obj, f, _arg );

            return ret;
        }
    };
}

