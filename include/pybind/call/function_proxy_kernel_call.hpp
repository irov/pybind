#pragma once

#include "pybind/exports.hpp"
#include "pybind/types.hpp"
#include "pybind/system.hpp"
#include "pybind/extract.hpp"
#include "pybind/helper.hpp"

#include "stdex/function_traits.h"

namespace pybind
{
	template<class C, class F, class Ret, int i>
	struct function_proxy_kernel_call_impl
	{
		static Ret call( kernel_interface * _kernel, C * _obj, F f, PyObject * _arg );
	};

	template<class C, class F, class Ret>
	struct function_proxy_kernel_call_impl<C, F, Ret, 2>
	{
		static Ret call( kernel_interface * _kernel, C * _obj, F f, PyObject * _arg )
		{
			(void)_arg;

			return (*f)(_kernel, _obj
				);
		}
	};

	template<class C, class F, class Ret>
	struct function_proxy_kernel_call_impl<C, F, Ret, 3>
	{
		static Ret call( kernel_interface * _kernel, C * _obj, F f, PyObject * _arg )
		{
			return (*f)(_kernel, _obj
				, tuple_getitem_t( _kernel, _arg, 0 )
				);
		}
	};

	template<class C, class F, class Ret>
	struct function_proxy_kernel_call_impl<C, F, Ret, 4>
	{
		static Ret call( kernel_interface * _kernel, C * _obj, F f, PyObject * _arg )
		{
			return (*f)(_kernel, _obj
				, tuple_getitem_t( _kernel, _arg, 0 )
				, tuple_getitem_t( _kernel, _arg, 1 )
				);
		}
	};

	template<class C, class F, class Ret>
	struct function_proxy_kernel_call_impl<C, F, Ret, 5>
	{
		static Ret call( kernel_interface * _kernel, C * _obj, F f, PyObject * _arg )
		{
			return (*f)(_kernel, _obj
				, tuple_getitem_t( _kernel, _arg, 0 )
				, tuple_getitem_t( _kernel, _arg, 1 )
				, tuple_getitem_t( _kernel, _arg, 2 )
				);
		}
	};

	template<class C, class F, class Ret>
	struct function_proxy_kernel_call_impl<C, F, Ret, 6>
	{
		static Ret call( kernel_interface * _kernel, C * _obj, F f, PyObject * _arg )
		{
			return (*f)(_kernel, _obj
				, tuple_getitem_t( _kernel, _arg, 0 )
				, tuple_getitem_t( _kernel, _arg, 1 )
				, tuple_getitem_t( _kernel, _arg, 2 )
				, tuple_getitem_t( _kernel, _arg, 3 )
				);
		}
	};

	template<class C, class F, class Ret>
	struct function_proxy_kernel_call_impl<C, F, Ret, 7>
	{
		static Ret call( kernel_interface * _kernel, C * _obj, F f, PyObject * _arg )
		{
			return (*f)(_kernel, _obj
				, tuple_getitem_t( _kernel, _arg, 0 )
				, tuple_getitem_t( _kernel, _arg, 1 )
				, tuple_getitem_t( _kernel, _arg, 2 )
				, tuple_getitem_t( _kernel, _arg, 3 )
				, tuple_getitem_t( _kernel, _arg, 4 )
				);
		}
	};

	template<class C, class F, class Ret>
	struct function_proxy_kernel_call_ret_impl
	{
		typedef typename stdex::function_traits<F>::result f_info;

		static PyObject * call( kernel_interface * _kernel, C * _obj, F f, PyObject * _arg )
		{
			PyObject * py_result = detail::return_operator_t( _kernel, function_proxy_kernel_call_impl<C, F, Ret, f_info::arity>::call( _kernel, _obj, f, _arg ) );

			return py_result;
		}
	};

	template<class C, class F>
	struct function_proxy_kernel_call_ret_impl<C, F, void>
	{
		typedef typename stdex::function_traits<F>::result f_info;

		static PyObject * call( kernel_interface * _kernel, C * _obj, F f, PyObject * _arg )
		{
			function_proxy_kernel_call_impl<C, F, void, f_info::arity>::call( _kernel, _obj, f, _arg );

			return pybind::ret_none();
		}
	};

	template<class C, class F>
	struct function_proxy_kernel_call
	{
		typedef typename stdex::function_traits<F>::result f_info;

		static PyObject * call( kernel_interface * _kernel, C * _obj, F f, PyObject * _arg )
		{
            uint32_t arg_size = pybind::tuple_size( _arg );
            uint32_t fn_arity = f_info::arity;

			if( arg_size + 2 != fn_arity )
			{
				pybind::throw_exception( "invalid proxy function call args is not equal %d != %d\n"
					, (uint32_t)arg_size
					, (uint32_t)fn_arity
					);

				return nullptr;
			}

			PyObject * ret = function_proxy_kernel_call_ret_impl<C, F, typename f_info::ret_type>::call( _kernel, _obj, f, _arg );

			return ret;
		}
	};
}
