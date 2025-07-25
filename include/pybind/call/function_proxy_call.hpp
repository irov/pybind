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
    struct function_proxy_call_impl
    {
        template<size_t ... I>
        static Ret call( kernel_interface * _kernel, P * _proxy, F f, PyObject * _arg, std::integer_sequence<size_t, I...> )
        {
            return (*f)(_proxy
                , tuple_getitem_t( _kernel, _arg, I ) ...
                );
        }
    };

    template<class F, class P, size_t Arity, class Ret>
    struct function_proxy_call_ret_impl
    {
        static PyObject * call( kernel_interface * _kernel, P * _proxy, F f, PyObject * _arg )
        {
            PyObject * py_result = detail::return_operator_t( _kernel, function_proxy_call_impl<F, P, Ret>::call( _kernel, _proxy, f, _arg, std::make_integer_sequence<size_t, Arity>() ) );

            return py_result;
        }
    };

    template<class F, class P, size_t Arity>
    struct function_proxy_call_ret_impl<F, P, Arity, void>
    {
        static PyObject * call( kernel_interface * _kernel, P * _proxy, F f, PyObject * _arg )
        {
            function_proxy_call_impl<F, P, void>::call( _kernel, _proxy, f, _arg, std::make_integer_sequence<size_t, Arity>() );

            return _kernel->ret_none();
        }
    };

    template<class F, class P>
    struct function_proxy_call
    {
        static PyObject * call( kernel_interface * _kernel, F f, PyObject * _arg, P * _proxy )
        {
            typedef typename stdex::function_traits<F>::result f_info;

            static_assert(std::is_same<typename f_info::template iterator_param<0>, pybind::kernel_interface *>::value == false, "[pybind] use kernel bind");
            static_assert(std::is_same<typename f_info::template reverse_iterator_param<0>, const pybind::args &>::value == false, "[pybind] use args bind");

#if defined(PYBIND_DEBUG)
            size_t arg_size = _kernel->tuple_size( _arg );
            size_t fn_arity = f_info::arity;

            if( arg_size + 1 != fn_arity )
            {
                pybind::throw_exception( "invalid function proxy call. Args is not equal %d != %d (%s)"
                    , arg_size + 1
                    , fn_arity
                    , _kernel->object_repr( _arg ).c_str()
                );

                return nullptr;
            }
#endif

            PyObject * ret = function_proxy_call_ret_impl<F, P, f_info::arity - 1, typename f_info::ret_type>::call( _kernel, _proxy, f, _arg );

            return ret;
        }
    };
}

