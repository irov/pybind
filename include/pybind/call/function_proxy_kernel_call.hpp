#pragma once

#include "pybind/exports.hpp"
#include "pybind/types.hpp"
#include "pybind/extract.hpp"
#include "pybind/helper.hpp"

#include "stdex/function_traits.h"

#include <type_traits>

namespace pybind
{
    template<class P, class F, class Ret>
    struct function_proxy_kernel_call_impl
    {
        template<size_t ... I>
        static Ret call( kernel_interface * _kernel, P * _proxy, F f, PyObject * _arg, std::integer_sequence<size_t, I...> )
        {
            return (*f)(_kernel, _proxy
                , tuple_getitem_t( _kernel, _arg, I ) ...
                );
        }
    };

    template<class P, class F, size_t Arity, class Ret>
    struct function_proxy_kernel_call_ret_impl
    {
        static PyObject * call( kernel_interface * _kernel, P * _proxy, F f, PyObject * _arg )
        {
            PyObject * py_result = detail::return_operator_t( _kernel, function_proxy_kernel_call_impl<P, F, Ret>::call( _kernel, _proxy, f, _arg, std::make_integer_sequence<size_t, Arity>() ) );

            return py_result;
        }
    };

    template<class P, class F, size_t Arity>
    struct function_proxy_kernel_call_ret_impl<P, F, Arity, void>
    {
        static PyObject * call( kernel_interface * _kernel, P * _proxy, F f, PyObject * _arg )
        {
            function_proxy_kernel_call_impl<P, F, void>::call( _kernel, _proxy, f, _arg, std::make_integer_sequence<size_t, Arity>() );

            return _kernel->ret_none();
        }
    };

    template<class P, class F>
    struct function_proxy_kernel_call
    {
        static PyObject * call( kernel_interface * _kernel, P * _proxy, F f, PyObject * _arg )
        {
            typedef typename stdex::function_traits<F>::result f_info;

            static_assert(std::is_same<typename f_info::template iterator_param<0>, pybind::kernel_interface *>::value == true, "[pybind] add kernel");
            static_assert(std::is_same<typename f_info::template reverse_iterator_param<0>, const pybind::args &>::value == false, "[pybind] use args bind");

#if defined(PYBIND_DEBUG)
            size_t arg_size = _kernel->tuple_size( _arg );
            size_t fn_arity = f_info::arity;

            if( arg_size + 2 != fn_arity )
            {
                pybind::throw_exception( "invalid function proxy kernel call. Args is not equal %d != %d (%s)"
                    , arg_size + 2
                    , fn_arity
                    , _kernel->object_repr( _arg ).c_str()
                );

                return nullptr;
            }
#endif

            PyObject * ret = function_proxy_kernel_call_ret_impl<P, F, f_info::arity - 2, typename f_info::ret_type>::call( _kernel, _proxy, f, _arg );

            return ret;
        }
    };
}

