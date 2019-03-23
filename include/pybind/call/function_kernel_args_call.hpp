#pragma once

#include "pybind/extract.hpp"
#include "pybind/helper.hpp"

#include "stdex/function_traits.h"

#include <type_traits>

namespace pybind
{
    template<class F, class Ret>
    struct function_kernel_args_call_impl
    {
        template<uint32_t ... I>
        static Ret call( kernel_interface * _kernel, F f, PyObject * _arg, std::integer_sequence<uint32_t, I...> )
        {
            return (*f)(_kernel
                , tuple_getitem_t( _kernel, _arg, I ) ...
                , make_args_t( _kernel, _arg, sizeof ... (I) )
                );
        }
    };

    template<class F, uint32_t Arity, class Ret>
    struct function_kernel_args_call_ret_impl
    {
        static PyObject * call( kernel_interface * _kernel, F f, PyObject * _arg )
        {
            PyObject * py_result = detail::return_operator_t( _kernel, function_kernel_args_call_impl<F, Ret>::call( _kernel, f, _arg, std::make_integer_sequence<uint32_t, Arity>() ) );

            return py_result;
        }
    };

    template<class F, uint32_t Arity>
    struct function_kernel_args_call_ret_impl<F, Arity, void>
    {
        static PyObject * call( kernel_interface * _kernel, F f, PyObject * _arg )
        {
            function_kernel_args_call_impl<F, void>::call( _kernel, f, _arg, std::make_integer_sequence<uint32_t, Arity>() );

            return _kernel->ret_none();
        }
    };

    template<class F>
    struct function_kernel_args_call
    {
        static PyObject * call( kernel_interface * _kernel, F f, PyObject * _arg )
        {
			typedef typename stdex::function_traits<F>::result f_info;

            uint32_t arg_size = (_arg != nullptr) ? _kernel->tuple_size( _arg ) : 0;
            uint32_t fn_arity = f_info::arity;

            static_assert(std::is_same<typename f_info::first_param, pybind::kernel_interface *>::value == true, "[pybind] use kernel bind");
            static_assert(std::is_same<typename f_info::last_param, const pybind::args &>::value == true, "[pybind] add args");

            if( arg_size + 2 < fn_arity )
            {
                pybind::throw_exception( "invalid args function call args is not equal %u < %u\n"
                    , (uint32_t)arg_size + 2
                    , (uint32_t)fn_arity
                );

                return nullptr;
            }

            PyObject * ret = function_kernel_args_call_ret_impl<F, f_info::arity - 2, typename f_info::ret_type>::call( _kernel, f, _arg );

            return ret;
        }
    };
}
