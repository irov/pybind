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
    struct method_kernel_call_impl
    {
        template<size_t ... I>
        static Ret call( kernel_interface * _kernel, C * _obj, F f, PyObject * _arg, std::integer_sequence<size_t, I...> )
        {
            return (_obj->*f)(_kernel
                , tuple_getitem_t( _kernel, _arg, I ) ...
                );
        }
    };

    template<class C, class F, size_t Arity, class Ret>
    struct method_kernel_call_ret_impl
    {
        static PyObject * call( kernel_interface * _kernel, C * _obj, F f, PyObject * _arg )
        {
            PyObject * py_result = detail::return_operator_t( _kernel, method_kernel_call_impl<C, F, Ret>::call( _kernel, _obj, f, _arg, std::make_integer_sequence<size_t, Arity>() ) );

            return py_result;
        }
    };

    template<class C, class F, size_t Arity>
    struct method_kernel_call_ret_impl<C, F, Arity, void>
    {
        static PyObject * call( kernel_interface * _kernel, C * _obj, F f, PyObject * _arg )
        {
            method_kernel_call_impl<C, F, void>::call( _kernel, _obj, f, _arg, std::make_integer_sequence<size_t, Arity>() );

            return _kernel->ret_none();
        }
    };

    template<class C, class F>
    struct method_kernel_call
    {
        static PyObject * call( kernel_interface * _kernel, C * _self, F f, PyObject * _arg )
        {
            typedef typename stdex::function_traits<F>::result f_info;

            static_assert(std::is_same<typename f_info::template iterator_param<0>, pybind::kernel_interface *>::value == true, "[pybind] use kernel bind");
            static_assert(std::is_same<typename f_info::template reverse_iterator_param<0>, const pybind::args &>::value == false, "[pybind] use args bind");

#if defined(PYBIND_DEBUG)
            size_t arg_size = (_arg != nullptr) ? _kernel->tuple_size( _arg ) : 0;
            size_t fn_arity = f_info::arity;

            if( arg_size + 1 != fn_arity )
            {
                pybind::throw_exception( "invalid method call args is not equal %d != %d (%s)"
                    , arg_size + 1
                    , fn_arity
                    , _kernel->object_repr( _arg ).c_str()
                );

                return nullptr;
            }
#endif

            PyObject * py_ret = method_kernel_call_ret_impl<C, F, f_info::arity - 1, typename f_info::ret_type>::call( _kernel, _self, f, _arg );

            return py_ret;
        }
    };
}

