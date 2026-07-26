#pragma once

#include "pybind/extract.hpp"
#include "pybind/helper.hpp"

#include "stdex/function_traits.h"

#include <type_traits>

namespace pybind
{
    template<class F, class Out>
    struct function_out_call_impl
    {
        template<size_t ... I>
        static void call( kernel_interface * _kernel, F f, Out * const _out, PyObject * _arg, std::integer_sequence<size_t, I...> )
        {
            (*f)(
                _out
                , tuple_getitem_t( _kernel, _arg, I ) ...
            );
        }
    };

    template<class F>
    struct function_out_call
    {
        static PyObject * call( kernel_interface * _kernel, F f, PyObject * _arg )
        {
            typedef typename stdex::function_traits<F>::result f_info;

            static_assert(f_info::method == false, "[pybind] out bind requires a function");
            static_assert(f_info::arity > 0, "[pybind] out function must have an output argument");
            static_assert(std::is_same<typename f_info::ret_type, void>::value == true, "[pybind] out function must return void");
            static_assert(std::is_same<typename f_info::template iterator_param<1>, pybind::kernel_interface *>::value == false, "[pybind] out function does not support a kernel argument");
            static_assert(std::is_same<typename f_info::template reverse_iterator_param<0>, const pybind::args &>::value == false, "[pybind] out function does not support an args argument");

            if constexpr( f_info::arity == 0 )
            {
                return nullptr;
            }
            else
            {
                typedef typename f_info::template iterator_param<0> out_param_type;
                typedef typename std::remove_cv<out_param_type>::type out_pointer_type;

                static_assert(std::is_pointer<out_pointer_type>::value == true, "[pybind] first out function argument must be T *");

                if constexpr( std::is_pointer<out_pointer_type>::value == false )
                {
                    return nullptr;
                }
                else
                {
                    typedef typename std::remove_pointer<out_pointer_type>::type out_pointee_type;
                    typedef typename std::remove_cv<out_pointee_type>::type out_type;

                    static_assert(std::is_const<out_pointee_type>::value == false, "[pybind] first out function argument must be mutable T *");
                    static_assert(std::is_volatile<out_pointee_type>::value == false, "[pybind] first out function argument must not be volatile");
                    static_assert(std::is_void<out_type>::value == false, "[pybind] out function result must not be void");
                    static_assert(std::is_default_constructible<out_type>::value == true, "[pybind] out function result must be default constructible");

                    if constexpr( std::is_void<out_type>::value == true || std::is_default_constructible<out_type>::value == false )
                    {
                        return nullptr;
                    }
                    else
                    {
#if defined(PYBIND_DEBUG)
                        size_t arg_size = (_arg != nullptr) ? _kernel->tuple_size( _arg ) : 0;
                        size_t fn_arity = f_info::arity - 1;

                        if( arg_size != fn_arity )
                        {
                            pybind::throw_exception( "invalid out function call. Args is not equal %d != %d (%s)"
                                , arg_size
                                , fn_arity
                                , _kernel->object_repr( _arg ).c_str()
                            );

                            return nullptr;
                        }
#endif

                        out_type out{};

                        function_out_call_impl<F, out_type>::call( _kernel, f, &out, _arg, std::make_integer_sequence<size_t, f_info::arity - 1>() );

                        PyObject * py_result = detail::return_operator_t( _kernel, out );

                        return py_result;
                    }
                }
            }
        }
    };
}
