#	pragma once

#	include "pybind/exports.hpp"
#	include "pybind/types.hpp"
#	include "pybind/system.hpp"
#	include "pybind/extract.hpp"
#	include "pybind/helper.hpp"

#	include "stdex/function_traits.h"

namespace pybind
{
	template<class C, class F,class Ret, int i>
	struct method_args_call_impl
	{
		static Ret call( C * _obj, F f, PyObject * _arg );
	};

	template<class C, class F,class Ret>
	struct method_args_call_impl<C, F, Ret, 1>
	{
		static Ret call( C * _obj, F f, PyObject * _arg )
		{			
			return (_obj->*f)( 
				make_args_t( _arg, 0 )
				);
		}
	};

	template<class C, class F,class Ret>
	struct method_args_call_impl<C, F, Ret, 2>
	{
		static Ret call( C * _obj, F f, PyObject * _arg )
		{
			return (_obj->*f)(
				tuple_getitem_t( _arg, 0 )
				, make_args_t( _arg, 1 )
				);
		}	
	};

	template<class C, class F, class Ret>
	struct method_args_call_impl<C, F, Ret, 3>
	{
		static Ret call( C * _obj, F f, PyObject * _arg )
		{
			return (_obj->*f)(
				tuple_getitem_t( _arg, 0 ),
				tuple_getitem_t( _arg, 1 )
				, make_args_t( _arg, 2 )
				);
		}
	};

	template<class C, class F, class Ret>
	struct method_args_call_impl<C, F, Ret, 4>
	{
		static Ret call( C * _obj, F f, PyObject * _arg )
		{
			return (_obj->*f)(
				tuple_getitem_t( _arg, 0 ),
				tuple_getitem_t( _arg, 1 ),
				tuple_getitem_t( _arg, 2 )
				, make_args_t( _arg, 3 )
				);
		}
	};

	template<class C, class F, class Ret>
	struct method_args_call_impl<C, F, Ret, 5>
	{
		static Ret call( C * _obj, F f, PyObject * _arg )
		{
			return (_obj->*f)(
				tuple_getitem_t( _arg, 0 ),
				tuple_getitem_t( _arg, 1 ),
				tuple_getitem_t( _arg, 2 ),
				tuple_getitem_t( _arg, 3 )
				, make_args_t( _arg, 4 )
				);
		}
	};

	template<class C, class F, class Ret>
	struct method_args_call_impl<C, F, Ret, 6>
	{
		static Ret call( C * _obj, F f, PyObject * _arg )
		{
			return (_obj->*f)(
				tuple_getitem_t( _arg, 0 ),
				tuple_getitem_t( _arg, 1 ),
				tuple_getitem_t( _arg, 2 ),
				tuple_getitem_t( _arg, 3 ),
				tuple_getitem_t( _arg, 4 )
				, make_args_t( _arg, 5 )
				);
		}
	};

	template<class C, class F, class Ret>
	struct method_args_call_impl<C, F, Ret, 7>
	{
		static Ret call( C * _obj, F f, PyObject * _arg )
		{
			return (_obj->*f)(
				tuple_getitem_t( _arg, 0 ),
				tuple_getitem_t( _arg, 1 ),
				tuple_getitem_t( _arg, 2 ),
				tuple_getitem_t( _arg, 3 ),
				tuple_getitem_t( _arg, 4 ),
				tuple_getitem_t( _arg, 5 )
				, make_args_t( _arg, 6 )
				);
		}
	};

	template<class C, class F, class Ret>
	struct method_args_call_impl<C, F, Ret, 8>
	{
		static Ret call( C * _obj, F f, PyObject * _arg )
		{
			return (_obj->*f)(
				tuple_getitem_t( _arg, 0 ),
				tuple_getitem_t( _arg, 1 ),
				tuple_getitem_t( _arg, 2 ),
				tuple_getitem_t( _arg, 3 ),
				tuple_getitem_t( _arg, 4 ),
				tuple_getitem_t( _arg, 5 ),
				tuple_getitem_t( _arg, 6 )
				, make_args_t( _arg, 7 )
				);
		}
	};

	template<class C, class F, class Ret>
	struct method_args_call_impl<C, F, Ret, 9>
	{
		static Ret call( C * _obj, F f, PyObject * _arg )
		{
			return (_obj->*f)(
				tuple_getitem_t( _arg, 0 ),
				tuple_getitem_t( _arg, 1 ),
				tuple_getitem_t( _arg, 2 ),
				tuple_getitem_t( _arg, 3 ),
				tuple_getitem_t( _arg, 4 ),
				tuple_getitem_t( _arg, 5 ),
				tuple_getitem_t( _arg, 6 ),
				tuple_getitem_t( _arg, 7 )
				, make_args_t( _arg, 8 )
				);
		}
	};

	template<class C, class F, class Ret>
	struct method_args_call_impl<C, F, Ret, 10>
	{
		static Ret call( C * _obj, F f, PyObject * _arg )
		{
			return (_obj->*f)(
				tuple_getitem_t( _arg, 0 ),
				tuple_getitem_t( _arg, 1 ),
				tuple_getitem_t( _arg, 2 ),
				tuple_getitem_t( _arg, 3 ),
				tuple_getitem_t( _arg, 4 ),
				tuple_getitem_t( _arg, 5 ),
				tuple_getitem_t( _arg, 6 ),
				tuple_getitem_t( _arg, 7 ),
				tuple_getitem_t( _arg, 8 ),
				tuple_getitem_t( _arg, 9 )
				, make_args_t( _arg, 10 )
				);
		}
	};

	template<class C, class F,class Ret>
	struct method_args_call_ret_impl
	{
		typedef typename stdex::function_traits<F>::result f_info;

		static PyObject * call( C * _obj, F f, PyObject * _arg )
		{
			PyObject * py_result = (detail::return_operator_t)method_args_call_impl<C, F, Ret, f_info::arity>::call( _obj, f, _arg );

			return py_result;
		}
	};

	template<class C, class F>
	struct method_args_call_ret_impl<C, F, void>
	{
		typedef typename stdex::function_traits<F>::result f_info;

		static PyObject * call( C * _obj, F f, PyObject * _arg )
		{
			method_args_call_impl<C, F, void, f_info::arity>::call( _obj, f, _arg );

			return ret_none();
		}
	};

	template<class C, class F>
	struct method_args_call
	{
		typedef typename stdex::function_traits<F>::result f_info;

		static PyObject * call( C * _self, F f, PyObject * _arg )
		{
			uint32_t arg_size = (_arg) ? pybind::tuple_size( _arg ) : 0;
			uint32_t fn_arity = f_info::arity;

			if( arg_size + 1 < fn_arity )
			{
				pybind::throw_exception("invalid method call args is not equal %d != %d\n"
					, arg_size
					, fn_arity
					);

				return nullptr;
			}

			PyObject * py_ret = method_args_call_ret_impl<C,F,typename f_info::ret_type>::call( _self, f, _arg );

			return py_ret;		
		}
	};
}

