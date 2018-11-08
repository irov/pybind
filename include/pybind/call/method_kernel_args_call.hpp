#pragma once

#include "pybind/exports.hpp"
#include "pybind/types.hpp"
#include "pybind/extract.hpp"
#include "pybind/helper.hpp"

#include "stdex/function_traits.h"

namespace pybind
{
	template<class C, class F,class Ret, int i>
	struct method_kernel_args_call_impl
	{
		static Ret call( kernel_interface * _kernel, C * _obj, F f, PyObject * _arg );
	};

	template<class C, class F,class Ret>
	struct method_kernel_args_call_impl<C, F, Ret, 2>
	{
		static Ret call( kernel_interface * _kernel, C * _obj, F f, PyObject * _arg )
		{			
            return (_obj->*f)(_kernel,
                make_args_t( _kernel, _arg, 0 )
				);
		}
	};

	template<class C, class F,class Ret>
	struct method_kernel_args_call_impl<C, F, Ret, 3>
	{
		static Ret call( kernel_interface * _kernel, C * _obj, F f, PyObject * _arg )
		{
			return (_obj->*f)(_kernel,
				tuple_getitem_t( _kernel, _arg, 0 ),
				make_args_t( _kernel, _arg, 1 )
				);
		}	
	};

	template<class C, class F, class Ret>
	struct method_kernel_args_call_impl<C, F, Ret, 4>
	{
		static Ret call( kernel_interface * _kernel, C * _obj, F f, PyObject * _arg )
		{
			return (_obj->*f)(_kernel,
				tuple_getitem_t( _kernel, _arg, 0 ),
				tuple_getitem_t( _kernel, _arg, 1 ),
				make_args_t( _kernel, _arg, 2 )
				);
		}
	};

	template<class C, class F, class Ret>
	struct method_kernel_args_call_impl<C, F, Ret, 5>
	{
		static Ret call( kernel_interface * _kernel, C * _obj, F f, PyObject * _arg )
		{
			return (_obj->*f)(_kernel,
				tuple_getitem_t( _kernel, _arg, 0 ),
				tuple_getitem_t( _kernel, _arg, 1 ),
				tuple_getitem_t( _kernel, _arg, 2 ),
				make_args_t( _kernel, _arg, 3 )
				);
		}
	};

	template<class C, class F, class Ret>
	struct method_kernel_args_call_impl<C, F, Ret, 6>
	{
		static Ret call( kernel_interface * _kernel, C * _obj, F f, PyObject * _arg )
		{
			return (_obj->*f)(_kernel,
				tuple_getitem_t( _kernel, _arg, 0 ),
				tuple_getitem_t( _kernel, _arg, 1 ),
				tuple_getitem_t( _kernel, _arg, 2 ),
                tuple_getitem_t( _kernel, _arg, 3 ),
				make_args_t( _kernel, _arg, 4 )
				);
		}
	};

	template<class C, class F, class Ret>
	struct method_kernel_args_call_impl<C, F, Ret, 7>
	{
		static Ret call( kernel_interface * _kernel, C * _obj, F f, PyObject * _arg )
		{
			return (_obj->*f)(_kernel,
				tuple_getitem_t( _kernel, _arg, 0 ),
				tuple_getitem_t( _kernel, _arg, 1 ),
				tuple_getitem_t( _kernel, _arg, 2 ),
				tuple_getitem_t( _kernel, _arg, 3 ),
				tuple_getitem_t( _kernel, _arg, 4 )
				, make_args_t( _kernel, _arg, 5 )
				);
		}
	};

	template<class C, class F, class Ret>
	struct method_kernel_args_call_impl<C, F, Ret, 8>
	{
		static Ret call( kernel_interface * _kernel, C * _obj, F f, PyObject * _arg )
		{
			return (_obj->*f)(_kernel,
				tuple_getitem_t( _kernel, _arg, 0 ),
				tuple_getitem_t( _kernel, _arg, 1 ),
				tuple_getitem_t( _kernel, _arg, 2 ),
				tuple_getitem_t( _kernel, _arg, 3 ),
				tuple_getitem_t( _kernel, _arg, 4 ),
				tuple_getitem_t( _kernel, _arg, 5 )
				, make_args_t( _kernel, _arg, 6 )
				);
		}
	};

	template<class C, class F, class Ret>
	struct method_kernel_args_call_impl<C, F, Ret, 9>
	{
		static Ret call( kernel_interface * _kernel, C * _obj, F f, PyObject * _arg )
		{
			return (_obj->*f)(_kernel,
				tuple_getitem_t( _kernel, _arg, 0 ),
				tuple_getitem_t( _kernel, _arg, 1 ),
				tuple_getitem_t( _kernel, _arg, 2 ),
				tuple_getitem_t( _kernel, _arg, 3 ),
				tuple_getitem_t( _kernel, _arg, 4 ),
				tuple_getitem_t( _kernel, _arg, 5 ),
				tuple_getitem_t( _kernel, _arg, 6 )
				, make_args_t( _kernel, _arg, 7 )
				);
		}
	};

	template<class C, class F, class Ret>
	struct method_kernel_args_call_impl<C, F, Ret, 10>
	{
		static Ret call( kernel_interface * _kernel, C * _obj, F f, PyObject * _arg )
		{
			return (_obj->*f)(_kernel,
				tuple_getitem_t( _kernel, _arg, 0 ),
				tuple_getitem_t( _kernel, _arg, 1 ),
				tuple_getitem_t( _kernel, _arg, 2 ),
				tuple_getitem_t( _kernel, _arg, 3 ),
				tuple_getitem_t( _kernel, _arg, 4 ),
				tuple_getitem_t( _kernel, _arg, 5 ),
				tuple_getitem_t( _kernel, _arg, 6 ),
				tuple_getitem_t( _kernel, _arg, 7 )
				, make_args_t( _kernel, _arg, 8 )
				);
		}
	};

	template<class C, class F, class Ret>
	struct method_kernel_args_call_impl<C, F, Ret, 11>
	{
		static Ret call( kernel_interface * _kernel, C * _obj, F f, PyObject * _arg )
		{
			return (_obj->*f)(_kernel,
				tuple_getitem_t( _kernel, _arg, 0 ),
				tuple_getitem_t( _kernel, _arg, 1 ),
				tuple_getitem_t( _kernel, _arg, 2 ),
				tuple_getitem_t( _kernel, _arg, 3 ),
				tuple_getitem_t( _kernel, _arg, 4 ),
				tuple_getitem_t( _kernel, _arg, 5 ),
				tuple_getitem_t( _kernel, _arg, 6 ),
				tuple_getitem_t( _kernel, _arg, 7 ),
				tuple_getitem_t( _kernel, _arg, 8 )
				, make_args_t( _kernel, _arg, 9 )
				);
		}
	};

	template<class C, class F,class Ret>
	struct method_kernel_args_call_ret_impl
	{
		typedef typename stdex::function_traits<F>::result f_info;

		static PyObject * call( kernel_interface * _kernel, C * _obj, F f, PyObject * _arg )
		{
			PyObject * result = detail::return_operator_t( _kernel, method_kernel_args_call_impl<C, F, Ret, f_info::arity>::call( _kernel, _obj, f, _arg ) );

			return result;
		}
	};

	template<class C, class F>
	struct method_kernel_args_call_ret_impl<C, F, void>
	{
		typedef typename stdex::function_traits<F>::result f_info;

		static PyObject * call( kernel_interface * _kernel, C * _obj, F f, PyObject * _arg )
		{
			method_kernel_args_call_impl<C, F, void, f_info::arity>::call( _kernel, _obj, f, _arg );

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

			if( arg_size + 1 < fn_arity )
			{
                pybind::throw_exception("invalid method call args is not equal %d < %d\n"
					, arg_size + 1
					, fn_arity
					);

				return nullptr;
			}

			PyObject * ret = method_kernel_args_call_ret_impl<C,F,typename f_info::ret_type>::call( _kernel, _self, f, _arg );

			return ret;		
		}
	};
}

