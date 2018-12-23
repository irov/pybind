#pragma once

#include "pybind/extract.hpp"
#include "pybind/helper.hpp"

#include "stdex/function_traits.h"

#include <type_traits>

namespace pybind
{
    template<class F, class Ret>
    struct function_call_impl
    {
        template<uint32_t ... I>
        static Ret call( kernel_interface * _kernel, F f, PyObject * _arg, std::integer_sequence<uint32_t, I...> )
        {
            return (*f)(
                tuple_getitem_t( _kernel, _arg, I ) ...
                );
        }
    };

    template<class F, uint32_t Arity, class Ret>
    struct function_call_ret_impl
    {
        static PyObject * call( kernel_interface * _kernel, F f, PyObject * _arg )
        {
            PyObject * py_result = detail::return_operator_t( _kernel, function_call_impl<F, Ret>::call( _kernel, f, _arg, std::make_integer_sequence<uint32_t, Arity>() ) );

            return py_result;
        }
    };

    template<class F, uint32_t Arity>
    struct function_call_ret_impl<F, Arity, void>
    {
        static PyObject * call( kernel_interface * _kernel, F f, PyObject * _arg )
        {
            function_call_impl<F, void>::call( _kernel, f, _arg, std::make_integer_sequence<uint32_t, Arity>() );

            return _kernel->ret_none();
        }
    };

    template<class F>
    struct function_call
    {
        typedef typename stdex::function_traits<F>::result f_info;

        static PyObject * call( kernel_interface * _kernel, F f, PyObject * _arg )
        {
            uint32_t arg_size = (_arg != nullptr) ? _kernel->tuple_size( _arg ) : 0;
            uint32_t fn_arity = f_info::arity;

            static_assert(std::is_same<typename f_info::first_param, pybind::kernel_interface *>::value == false, "[pybind] use kernel bind");
            static_assert(std::is_same<typename f_info::last_param, const pybind::args &>::value == false, "[pybind] use args bind");

            if( arg_size != fn_arity )
            {
                pybind::throw_exception( "invalid function call args is not equal %d != %d\n"
                    , (uint32_t)arg_size
                    , (uint32_t)fn_arity
                );

                return nullptr;
            }

            PyObject * ret = function_call_ret_impl<F, f_info::arity, typename f_info::ret_type>::call( _kernel, f, _arg );

            return ret;
        }
    };
}

