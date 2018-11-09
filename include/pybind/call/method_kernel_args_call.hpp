#pragma once

#include "pybind/exports.hpp"
#include "pybind/types.hpp"
#include "pybind/extract.hpp"
#include "pybind/helper.hpp"

#include "stdex/function_traits.h"

#include <type_traits>

namespace pybind
{
    template<class C, class F, class Ret>
    struct method_kernel_args_call_impl
    {
        template<size_t ... I>
        static Ret call( kernel_interface * _kernel, C * _obj, F f, PyObject * _arg, std::index_sequence<I...> )
        {
            return (_obj->*f)(_kernel
                , tuple_getitem_t( _kernel, _arg, I ) ...
                , make_args_t( _kernel, _arg, std::index_sequence<I...>::size() )
                );
        }
    };

    template<class C, class F, size_t Arity, class Ret>
    struct method_kernel_args_call_ret_impl
    {
        static PyObject * call( kernel_interface * _kernel, C * _obj, F f, PyObject * _arg )
        {
            PyObject * result = detail::return_operator_t( _kernel, method_kernel_args_call_impl<C, F, Ret>::call( _kernel, _obj, f, _arg, std::make_index_sequence<Arity>() ) );

            return result;
        }
    };

    template<class C, class F, size_t Arity>
    struct method_kernel_args_call_ret_impl<C, F, Arity, void>
    {
        static PyObject * call( kernel_interface * _kernel, C * _obj, F f, PyObject * _arg )
        {
            method_kernel_args_call_impl<C, F, void>::call( _kernel, _obj, f, _arg, std::make_index_sequence<Arity>() );

            return _kernel->ret_none();
        }
    };

    template<class C, class F>
    struct method_kernel_args_call
    {
        typedef typename stdex::function_traits<F>::result f_info;

        static PyObject * call( kernel_interface * _kernel, C * _self, F f, PyObject * _arg )
        {
            uint32_t arg_size = (_arg) ? _kernel->tuple_size( _arg ) : 0;
            uint32_t fn_arity = f_info::arity;

            static_assert(std::is_same<typename f_info::first_param, pybind::kernel_interface *>::value == true, "[pybind] use kernel bind");
            static_assert(std::is_same<typename f_info::last_param, const pybind::args &>::value == true, "[pybind] add args");

            if( arg_size + 2 < fn_arity )
            {
                pybind::throw_exception( "invalid method call args is not equal %d < %d\n"
                    , arg_size + 1
                    , fn_arity
                );

                return nullptr;
            }

            PyObject * ret = method_kernel_args_call_ret_impl<C, F, f_info::arity - 2, typename f_info::ret_type>::call( _kernel, _self, f, _arg );

            return ret;
        }
    };
}

