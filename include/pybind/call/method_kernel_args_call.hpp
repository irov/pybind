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
        template<uint32_t ... I>
        static Ret call( kernel_interface * _kernel, C * _obj, F f, PyObject * _arg, std::integer_sequence<uint32_t, I...> )
        {
            return (_obj->*f)(_kernel
                , tuple_getitem_t( _kernel, _arg, I ) ...
                , make_args_t( _kernel, _arg, sizeof ... (I) )
                );
        }
    };

    template<class C, class F, uint32_t Arity, class Ret>
    struct method_kernel_args_call_ret_impl
    {
        static PyObject * call( kernel_interface * _kernel, C * _obj, F f, PyObject * _arg )
        {
            PyObject * result = detail::return_operator_t( _kernel, method_kernel_args_call_impl<C, F, Ret>::call( _kernel, _obj, f, _arg, std::make_integer_sequence<uint32_t, Arity>() ) );

            return result;
        }
    };

    template<class C, class F, uint32_t Arity>
    struct method_kernel_args_call_ret_impl<C, F, Arity, void>
    {
        static PyObject * call( kernel_interface * _kernel, C * _obj, F f, PyObject * _arg )
        {
            method_kernel_args_call_impl<C, F, void>::call( _kernel, _obj, f, _arg, std::make_integer_sequence<uint32_t, Arity>() );

            return _kernel->ret_none();
        }
    };

    template<class C, class F>
    struct method_kernel_args_call
    {
        static PyObject * call( kernel_interface * _kernel, C * _self, F f, PyObject * _arg )
        {
            typedef typename stdex::function_traits<F>::result f_info;

            static_assert(std::is_same<typename f_info::template iterator_param<0>, pybind::kernel_interface *>::value == true, "[pybind] use kernel bind");
            static_assert(std::is_same<typename f_info::template reverse_iterator_param<0>, const pybind::args &>::value == true, "[pybind] add args");

#ifndef NDEBUG
            uint32_t arg_size = (_arg != nullptr) ? _kernel->tuple_size( _arg ) : 0;
            uint32_t fn_arity = f_info::arity;

            if( arg_size + 2 < fn_arity )
            {
                pybind::throw_exception( "invalid method call args is not equal %d < %d\n"
                    , arg_size + 1
                    , fn_arity
                );

                return nullptr;
            }
            
            if constexpr( std::is_same<typename f_info::template reverse_iterator_param<2>, const pybind::object &>::value == true )
            {
                PyObject * py_cb = _kernel->tuple_getitem( _arg, fn_arity - 3 );

                if( _kernel->is_callable( py_cb ) == false && _kernel->is_none( py_cb ) == false )
                {
                    pybind::throw_exception( "invalid args function call cb is not callable '%s' type '%s'"
                        , _kernel->object_repr( py_cb ).c_str()
                        , _kernel->object_repr_type( py_cb ).c_str()
                    );

                    return nullptr;
                }
            }
#endif

            PyObject * ret = method_kernel_args_call_ret_impl<C, F, f_info::arity - 2, typename f_info::ret_type>::call( _kernel, _self, f, _arg );

            return ret;
        }
    };
}

