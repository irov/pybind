#pragma once

#include "pybind/exports.hpp"
#include "pybind/types.hpp"
#include "pybind/extract.hpp"
#include "pybind/helper.hpp"

#include "stdex/function_traits.h"

#include <type_traits>

namespace pybind
{
    template<class F, class P, class Ret>
    struct function_proxy_args_call_impl
    {
        template<size_t ... I>
        static Ret call( kernel_interface * _kernel, P * _proxy, F f, PyObject * _arg, std::index_sequence<I...> )
        {
            return (*f)(_proxy
                , tuple_getitem_t( _kernel, _arg, I ) ...
                , make_args_t( _kernel, _arg, std::index_sequence<I...>::size() )
                );
        }
    };

    template<class F, class P, size_t Arity, class Ret>
    struct function_proxy_args_call_ret_impl
    {
        static PyObject * call( kernel_interface * _kernel, P * _proxy, F f, PyObject * _arg )
        {
            PyObject * result = detail::return_operator_t( _kernel, function_proxy_args_call_impl<F, P, Ret>::call( _kernel, _proxy, f, _arg, std::make_index_sequence<Arity>() ) );

            return result;
        }
    };

    template<class F, class P, size_t Arity>
    struct function_proxy_args_call_ret_impl<F, P, Arity, void>
    {
        static PyObject * call( kernel_interface * _kernel, P * _proxy, F f, PyObject * _arg )
        {
            function_proxy_args_call_impl<F, P, void>::call( _kernel, _proxy, f, _arg, std::make_index_sequence<Arity>() );

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

            static_assert(std::is_same<typename f_info::first_param, pybind::kernel_interface *>::value == false, "[pybind] use kernel bind");
            static_assert(std::is_same<typename f_info::last_param, const pybind::args &>::value == true, "[pybind] add args");

            if( arg_size + 2 < fn_arity )
            {
                pybind::throw_exception( "invalid proxy args function call args is not equal %d < %d\n"
                    , arg_size + 2
                    , fn_arity
                );

                return nullptr;
            }

            PyObject * ret = function_proxy_args_call_ret_impl<F, P, f_info::arity - 2, typename f_info::ret_type>::call( _kernel, _proxy, f, _arg );

            return ret;
        }
    };
}

