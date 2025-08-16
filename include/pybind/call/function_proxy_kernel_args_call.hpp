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
    struct function_proxy_kernel_args_call_impl
    {
        template<size_t ... I>
        static Ret call( kernel_interface * _kernel, P * _proxy, F f, PyObject * _arg, std::integer_sequence<size_t, I...> )
        {
            return (*f)(_kernel, _proxy
                , tuple_getitem_t( _kernel, _arg, I ) ...
                , make_args_t( _kernel, _arg, sizeof ... (I) )
                );
        }
    };

    template<class P, class F, size_t Arity, class Ret>
    struct function_proxy_kernel_args_call_ret_impl
    {
        static PyObject * call( kernel_interface * _kernel, P * _proxy, F f, PyObject * _arg )
        {
            PyObject * py_result = detail::return_operator_t( _kernel, function_proxy_kernel_args_call_impl<P, F, Ret>::call( _kernel, _proxy, f, _arg, std::make_integer_sequence<size_t, Arity>() ) );

            return py_result;
        }
    };

    template<class P, class F, size_t Arity>
    struct function_proxy_kernel_args_call_ret_impl<P, F, Arity, void>
    {
        static PyObject * call( kernel_interface * _kernel, P * _proxy, F f, PyObject * _arg )
        {
            function_proxy_kernel_args_call_impl<P, F, void>::call( _kernel, _proxy, f, _arg, std::make_integer_sequence<size_t, Arity>() );

            return _kernel->ret_none();
        }
    };

    template<class P, class F>
    struct function_proxy_kernel_args_call
    {
        static PyObject * call( kernel_interface * _kernel, P * _proxy, F f, PyObject * _arg )
        {
            typedef typename stdex::function_traits<F>::result f_info;

            static_assert(std::is_same<typename f_info::template iterator_param<0>, pybind::kernel_interface *>::value == true, "[pybind] add kernel");
            static_assert(std::is_same<typename f_info::template reverse_iterator_param<0>, const pybind::args &>::value == true, "[pybind] use args bind");

#if defined(PYBIND_DEBUG)
            size_t arg_size = _kernel->tuple_size( _arg );
            size_t fn_arity = f_info::arity;

            if( arg_size + 3 != fn_arity )
            {
                pybind::throw_exception( "invalid function proxy kernel args call. Args is not equal %d != %d (%s)"
                    , arg_size + 3
                    , fn_arity
                    , _kernel->object_repr( _arg ).c_str()
                );

                return nullptr;
            }

            if constexpr( std::is_same<typename f_info::template reverse_iterator_param<2>, const pybind::object &>::value == true )
            {
                PyObject * py_cb = _kernel->tuple_getitem( _arg, fn_arity - 3 );

                if( _kernel->is_callable( py_cb ) == false && _kernel->is_none( py_cb ) == false && _kernel->dict_check( py_cb ) == false )
                {
                    pybind::throw_exception( "invalid args function call cb is not callable '%s' type '%s'"
                        , _kernel->object_repr( py_cb ).c_str()
                        , _kernel->object_repr_type( py_cb ).c_str()
                    );

                    return nullptr;
                }
            }
#endif

            PyObject * ret = function_proxy_kernel_args_call_ret_impl<P, F, f_info::arity - 3, typename f_info::ret_type>::call( _kernel, _proxy, f, _arg );

            return ret;
        }
    };
}

