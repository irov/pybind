#	pragma once

#	include "pybind/extract.hpp"
#	include "pybind/helper.hpp"

#	include "stdex/function_traits.h"

namespace pybind
{
	template<class F, class Ret, int i>
	struct function_kernel_call_impl
	{
		static Ret call( kernel_interface * _kernel, F f, PyObject * _arg );
	};

	template<class F, class Ret>
	struct function_kernel_call_impl<F,Ret,1>
	{
		static Ret call( kernel_interface * _kernel, F f, PyObject * _arg )
		{
			(void)_arg;

			return (f)(_kernel);
		}	
	};

	template<class F, class Ret>
	struct function_kernel_call_impl<F,Ret,2>
	{
		static Ret call( kernel_interface * _kernel, F f, PyObject * _arg )
		{
			return (f)(_kernel,
				tuple_getitem_t( _kernel, _arg, 0 )
				);
		}	
	};

	template<class F, class Ret>
	struct function_kernel_call_impl<F,Ret,3>
	{
		static Ret call( kernel_interface * _kernel, F f, PyObject * _arg )
		{
			return (f)(_kernel,
				tuple_getitem_t( _kernel, _arg, 0 ),
				tuple_getitem_t( _kernel, _arg, 1 )
				);
		}
	};

	template<class F, class Ret>
	struct function_kernel_call_impl<F,Ret,4>
	{
		static Ret call( kernel_interface * _kernel, F f, PyObject * _arg )
		{
			return (f)(_kernel,
				tuple_getitem_t( _kernel, _arg, 0 ),
				tuple_getitem_t( _kernel, _arg, 1 ),
				tuple_getitem_t( _kernel, _arg, 2 )
				);
		}
	};

	template<class F, class Ret>
	struct function_kernel_call_impl<F,Ret,5>
	{
		static Ret call( kernel_interface * _kernel, F f, PyObject * _arg )
		{
			return (f)(_kernel,
				tuple_getitem_t( _kernel, _arg, 0 ),
				tuple_getitem_t( _kernel, _arg, 1 ),
				tuple_getitem_t( _kernel, _arg, 2 ),
				tuple_getitem_t( _kernel, _arg, 3 )
				);
		}
	};

	template<class F, class Ret>
	struct function_kernel_call_impl<F,Ret,6>
	{
		static Ret call( kernel_interface * _kernel, F f, PyObject * _arg )
		{
			return (f)(_kernel,
				tuple_getitem_t( _kernel, _arg, 0 ),
				tuple_getitem_t( _kernel, _arg, 1 ),
				tuple_getitem_t( _kernel, _arg, 2 ),
				tuple_getitem_t( _kernel, _arg, 3 ),
				tuple_getitem_t( _kernel, _arg, 4 )
				);
		}
	};

	template<class F, class Ret>
	struct function_kernel_call_impl<F,Ret,7>
	{
		static Ret call( kernel_interface * _kernel, F f, PyObject * _arg )
		{
			return (f)(_kernel,
				tuple_getitem_t( _kernel, _arg, 0 ),
				tuple_getitem_t( _kernel, _arg, 1 ),
				tuple_getitem_t( _kernel, _arg, 2 ),
				tuple_getitem_t( _kernel, _arg, 3 ),
				tuple_getitem_t( _kernel, _arg, 4 ),
				tuple_getitem_t( _kernel, _arg, 5 )
				);
		}
	};

	template<class F, class Ret>
	struct function_kernel_call_impl<F,Ret,8>
	{
		static Ret call( kernel_interface * _kernel, F f, PyObject * _arg )
		{
			return (f)(_kernel,
				tuple_getitem_t( _kernel, _arg, 0 ),
				tuple_getitem_t( _kernel, _arg, 1 ),
				tuple_getitem_t( _kernel, _arg, 2 ),
				tuple_getitem_t( _kernel, _arg, 3 ),
				tuple_getitem_t( _kernel, _arg, 4 ),
				tuple_getitem_t( _kernel, _arg, 5 ),
				tuple_getitem_t( _kernel, _arg, 6 )
				);
		}
	};

	template<class F, class Ret>
	struct function_kernel_call_impl<F,Ret,9>
	{
		static Ret call( kernel_interface * _kernel, F f, PyObject * _arg )
		{
			return (f)(_kernel,
				tuple_getitem_t( _kernel, _arg, 0 ),
				tuple_getitem_t( _kernel, _arg, 1 ),
				tuple_getitem_t( _kernel, _arg, 2 ),
				tuple_getitem_t( _kernel, _arg, 3 ),
				tuple_getitem_t( _kernel, _arg, 4 ),
				tuple_getitem_t( _kernel, _arg, 5 ),
				tuple_getitem_t( _kernel, _arg, 6 ),
				tuple_getitem_t( _kernel, _arg, 7 )
				);
		}
	};

	template<class F, class Ret>
	struct function_kernel_call_impl<F,Ret,10>
	{
		static Ret call( kernel_interface * _kernel, F f, PyObject * _arg )
		{
			return (f)(_kernel,
				tuple_getitem_t( _kernel, _arg, 0 ),
				tuple_getitem_t( _kernel, _arg, 1 ),
				tuple_getitem_t( _kernel, _arg, 2 ),
				tuple_getitem_t( _kernel, _arg, 3 ),
				tuple_getitem_t( _kernel, _arg, 4 ),
				tuple_getitem_t( _kernel, _arg, 5 ),
				tuple_getitem_t( _kernel, _arg, 6 ),
				tuple_getitem_t( _kernel, _arg, 7 ),
				tuple_getitem_t( _kernel, _arg, 8 )
				);
		}
	};

	template<class F, class Ret>
	struct function_kernel_call_impl<F,Ret,11>
	{
		static Ret call( kernel_interface * _kernel, F f, PyObject * _arg )
		{
			return (f)(_kernel,
				tuple_getitem_t( _kernel, _arg, 0 ),
				tuple_getitem_t( _kernel, _arg, 1 ),
				tuple_getitem_t( _kernel, _arg, 2 ),
				tuple_getitem_t( _kernel, _arg, 3 ),
				tuple_getitem_t( _kernel, _arg, 4 ),
				tuple_getitem_t( _kernel, _arg, 5 ),
				tuple_getitem_t( _kernel, _arg, 6 ),
				tuple_getitem_t( _kernel, _arg, 7 ),
				tuple_getitem_t( _kernel, _arg, 8 ),
				tuple_getitem_t( _kernel, _arg, 9 )
				);
		}
	};

	template<class F, class Ret>
	struct function_kernel_call_ret_impl
	{
		typedef typename stdex::function_traits<F>::result f_info;

		static PyObject * call( kernel_interface * _kernel, F f, PyObject * _arg )
		{
			PyObject * py_result = detail::return_operator_t( _kernel, function_kernel_call_impl<F, Ret, f_info::arity>::call( _kernel, f, _arg ) );

			return py_result;
		}
	};

	template<class F>
	struct function_kernel_call_ret_impl<F,void>
	{
		typedef typename stdex::function_traits<F>::result f_info;

		static PyObject * call( kernel_interface * _kernel, F f, PyObject * _arg )
		{
			function_kernel_call_impl<F, void, f_info::arity>::call( _kernel, f, _arg );

			return ret_none();
		}
	};

	template<class F>
	struct function_kernel_call
	{
		typedef typename stdex::function_traits<F>::result f_info;

		static PyObject * call( kernel_interface * _kernel, F f, PyObject * _arg )
        {
			size_t arg_size = (_arg) ? pybind::tuple_size( _arg ) : 0;
			size_t fn_arity = f_info::arity;

			if( arg_size + 1 != fn_arity )
			{
				pybind::throw_exception("invalid function call args is not equal %d != %d\n"					
					, (uint32_t)arg_size + 1
					, (uint32_t)fn_arity
					);

				return nullptr;
			}

			PyObject * ret = function_kernel_call_ret_impl<F, typename f_info::ret_type>::call( _kernel, f, _arg );

			return ret;
		}
	};
}

