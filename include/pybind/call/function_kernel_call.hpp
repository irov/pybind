#pragma once

#include "pybind/extract.hpp"
#include "pybind/helper.hpp"

#include "stdex/function_traits.h"

#include <type_traits>

namespace pybind
{
    template<class F, class Ret>
    struct function_kernel_call_impl
    {
        template<uint32_t ... I>
        static Ret call( kernel_interface * _kernel, F f, PyObject * _arg, std::integer_sequence<uint32_t, I...> )
        {
            return (*f)(_kernel
                , tuple_getitem_t( _kernel, _arg, I ) ...
                );
        }
    };

    template<class F, uint32_t Arity, class Ret>
    struct function_kernel_call_ret_impl
    {
        static PyObject * call( kernel_interface * _kernel, F f, PyObject * _arg )
        {
            PyObject * py_result = detail::return_operator_t( _kernel, function_kernel_call_impl<F, Ret>::call( _kernel, f, _arg, std::make_integer_sequence<uint32_t, Arity>() ) );

            return py_result;
        }
    };

    template<class F, uint32_t Arity>
    struct function_kernel_call_ret_impl<F, Arity, void>
    {
        static PyObject * call( kernel_interface * _kernel, F f, PyObject * _arg )
        {
            function_kernel_call_impl<F, void>::call( _kernel, f, _arg, std::make_integer_sequence<uint32_t, Arity>() );

            return _kernel->ret_none();
        }
    };

    template<class F>
    struct function_kernel_call
    {
        static PyObject * call( kernel_interface * _kernel, F f, PyObject * _arg )
        {
            typedef typename stdex::function_traits<F>::result f_info;

            static_assert(std::is_same<typename f_info::template iterator_param<0>, pybind::kernel_interface *>::value == true, "[pybind] add kernel");
            static_assert(std::is_same<typename f_info::template reverse_iterator_param<0>, const pybind::args &>::value == false, "[pybind] use args bind");

#if defined(PYBIND_DEBUG)
            uint32_t arg_size = (_arg != nullptr) ? _kernel->tuple_size( _arg ) : 0;
            uint32_t fn_arity = f_info::arity;

            if( arg_size + 1 != fn_arity )
            {
                pybind::throw_exception( "invalid function call args is not equal %d != %d (%s)"
                    , arg_size + 1
                    , fn_arity
                    , _kernel->object_repr( _arg ).c_str()
                );

                return nullptr;
            }
#endif

            PyObject * ret = function_kernel_call_ret_impl<F, f_info::arity - 1, typename f_info::ret_type>::call( _kernel, f, _arg );

            return ret;
        }
    };
}

