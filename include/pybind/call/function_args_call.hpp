#pragma once

#include "pybind/exports.hpp"
#include "pybind/types.hpp"
#include "pybind/extract.hpp"
#include "pybind/helper.hpp"

#include "stdex/function_traits.h"

#include <type_traits>

namespace pybind
{
    template<class F, class Ret>
    struct function_args_call_impl
    {
        template<uint32_t ... I>
        static Ret call( kernel_interface * _kernel, F f, PyObject * _arg, std::integer_sequence<uint32_t, I...> )
        {
            return (*f)(
                tuple_getitem_t( _kernel, _arg, I ) ...
                , make_args_t( _kernel, _arg, sizeof ... (I) )
                );
        }
    };

    template<class F, uint32_t Arity, class Ret>
    struct function_args_call_ret_impl
    {
        static PyObject * call( kernel_interface * _kernel, F f, PyObject * _arg )
        {
            PyObject * result = detail::return_operator_t( _kernel, function_args_call_impl<F, Ret>::call( _kernel, f, _arg, std::make_integer_sequence<uint32_t, Arity>() ) );

            return result;
        }
    };

    template<class F, uint32_t Arity>
    struct function_args_call_ret_impl<F, Arity, void>
    {
        static PyObject * call( kernel_interface * _kernel, F f, PyObject * _arg )
        {
            function_args_call_impl<F, void>::call( _kernel, f, _arg, std::make_integer_sequence<uint32_t, Arity>() );

            return _kernel->ret_none();
        }
    };

    template<class F>
    struct function_args_call
    {
        static PyObject * call( kernel_interface * _kernel, F f, PyObject * _arg )
        {
            typedef typename stdex::function_traits<F>::result f_info;

            static_assert(std::is_same<typename f_info::first_param, pybind::kernel_interface *>::value == false, "[pybind] use kernel bind");
            static_assert(std::is_same<typename f_info::last_param, const pybind::args &>::value == true, "[pybind] add args");

#ifndef NDEBUG
            uint32_t arg_size = _kernel->tuple_size( _arg );
            uint32_t fn_arity = f_info::arity;

            if( arg_size + 1 < fn_arity )
            {
                pybind::throw_exception( "invalid args function call args is not equal %d < %d"
                    , arg_size + 1
                    , fn_arity
                );

                return nullptr;
            }

            PyObject * py_cb = _kernel->tuple_getitem( _arg, fn_arity - 2 );

            if( _kernel->is_callable( py_cb ) == false && _kernel->is_none( py_cb ) == false )
            {
                pybind::throw_exception( "invalid args function call cb is not callable '%s' type '%s'"
                    , _kernel->object_repr( py_cb )
                    , _kernel->object_repr_type( py_cb )
                );

                return nullptr;
            }
#endif

            PyObject * ret = function_args_call_ret_impl<F, f_info::arity - 1, typename f_info::ret_type>::call( _kernel, f, _arg );

            return ret;
        }
    };
}

