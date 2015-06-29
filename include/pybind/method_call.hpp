#	pragma once

#	include "pybind/exports.hpp"
#	include "pybind/function_parser.hpp"
#	include "pybind/types.hpp"
#	include "pybind/system.hpp"
#	include "pybind/extract.hpp"

namespace pybind
{
	template<class C, class F,class Ret, int i>
	struct method_call_impl
	{
		static Ret call( C * _obj, F f, PyObject * _arg );
	};

	template<class C, class F,class Ret>
	struct method_call_impl<C,F,Ret,0>
	{
		static Ret call( C * _obj, F f, PyObject * _arg )
		{
            (void)_arg;

			return (_obj->*f)();
		}
	};

	template<class C, class F,class Ret>
	struct method_call_impl<C,F,Ret,1>
	{
		typedef typename function_parser<F>::result f_info;

		static Ret call( C * _obj, F f, PyObject * _arg )
		{
			PyObject * _arg0 = tuple_getitem( _arg, 0 );

			return (_obj->*f)( 
				extract_throw<typename f_info::param0>( _arg0 )
				);
		}
	};

	template<class C, class F,class Ret>
	struct method_call_impl<C,F,Ret,2>
	{
		typedef typename function_parser<F>::result f_info;

		static Ret call( C * _obj, F f, PyObject * _arg )
		{
			PyObject * _arg0 = tuple_getitem( _arg, 0 );
			PyObject * _arg1 = tuple_getitem( _arg, 1 );

			return (_obj->*f)( 
				extract_throw<typename f_info::param0>( _arg0 ),
				extract_throw<typename f_info::param1>( _arg1 )
				);
		}	
	};

	template<class C, class F,class Ret>
	struct method_call_impl<C,F,Ret,3>
	{
		typedef typename function_parser<F>::result f_info;

		static Ret call( C * _obj, F f, PyObject * _arg )
		{
			PyObject * _arg0 = tuple_getitem( _arg, 0 );
			PyObject * _arg1 = tuple_getitem( _arg, 1 );
			PyObject * _arg2 = tuple_getitem( _arg, 2 );

			return (_obj->*f)( 
				extract_throw<typename f_info::param0>( _arg0 ),
				extract_throw<typename f_info::param1>( _arg1 ),
				extract_throw<typename f_info::param2>( _arg2 )
				);
		}	
	};

	template<class C, class F,class Ret>
	struct method_call_impl<C,F,Ret,4>
	{
		typedef typename function_parser<F>::result f_info;

		static Ret call( C * _obj, F f, PyObject * _arg )
		{
			PyObject * _arg0 = tuple_getitem( _arg, 0 );
			PyObject * _arg1 = tuple_getitem( _arg, 1 );
			PyObject * _arg2 = tuple_getitem( _arg, 2 );
			PyObject * _arg3 = tuple_getitem( _arg, 3 );

			return (_obj->*f)( 
				extract_throw<typename f_info::param0>( _arg0 ),
				extract_throw<typename f_info::param1>( _arg1 ),
				extract_throw<typename f_info::param2>( _arg2 ),
				extract_throw<typename f_info::param3>( _arg3 )
				);
		}
	};

	template<class C, class F,class Ret>
	struct method_call_impl<C,F,Ret,5>
	{
		typedef typename function_parser<F>::result f_info;

		static Ret call( C * _obj, F f, PyObject * _arg )
		{
			PyObject * _arg0 = tuple_getitem( _arg, 0 );
			PyObject * _arg1 = tuple_getitem( _arg, 1 );
			PyObject * _arg2 = tuple_getitem( _arg, 2 );
			PyObject * _arg3 = tuple_getitem( _arg, 3 );
			PyObject * _arg4 = tuple_getitem( _arg, 4 );

			return (_obj->*f)( 
				extract_throw<typename f_info::param0>( _arg0 ),
				extract_throw<typename f_info::param1>( _arg1 ),
				extract_throw<typename f_info::param2>( _arg2 ),
				extract_throw<typename f_info::param3>( _arg3 ),
				extract_throw<typename f_info::param4>( _arg4 )
				);
		}
	};

	template<class C, class F,class Ret>
	struct method_call_impl<C,F,Ret,6>
	{
		typedef typename function_parser<F>::result f_info;

		static Ret call( C * _obj, F f, PyObject * _arg )
		{
			PyObject * _arg0 = tuple_getitem( _arg, 0 );
			PyObject * _arg1 = tuple_getitem( _arg, 1 );
			PyObject * _arg2 = tuple_getitem( _arg, 2 );
			PyObject * _arg3 = tuple_getitem( _arg, 3 );
			PyObject * _arg4 = tuple_getitem( _arg, 4 );
			PyObject * _arg5 = tuple_getitem( _arg, 5 );
			
			return (_obj->*f)( 
				extract_throw<typename f_info::param0>( _arg0 ),
				extract_throw<typename f_info::param1>( _arg1 ),
				extract_throw<typename f_info::param2>( _arg2 ),
				extract_throw<typename f_info::param3>( _arg3 ),
				extract_throw<typename f_info::param4>( _arg4 ),
				extract_throw<typename f_info::param5>( _arg5 )
				);
		}
	};

	template<class C, class F,class Ret>
	struct method_call_impl<C,F,Ret,7>
	{
		typedef typename function_parser<F>::result f_info;

		static Ret call( C * _obj, F f, PyObject * _arg )
		{
			PyObject * _arg0 = tuple_getitem( _arg, 0 );
			PyObject * _arg1 = tuple_getitem( _arg, 1 );
			PyObject * _arg2 = tuple_getitem( _arg, 2 );
			PyObject * _arg3 = tuple_getitem( _arg, 3 );
			PyObject * _arg4 = tuple_getitem( _arg, 4 );
			PyObject * _arg5 = tuple_getitem( _arg, 5 );
			PyObject * _arg6 = tuple_getitem( _arg, 6 );

			return (_obj->*f)( 
				extract_throw<typename f_info::param0>( _arg0 ),
				extract_throw<typename f_info::param1>( _arg1 ),
				extract_throw<typename f_info::param2>( _arg2 ),
				extract_throw<typename f_info::param3>( _arg3 ),
				extract_throw<typename f_info::param4>( _arg4 ),
				extract_throw<typename f_info::param5>( _arg5 ),
				extract_throw<typename f_info::param6>( _arg6 )
				);
		}
	};

	template<class C, class F,class Ret>
	struct method_call_impl<C,F,Ret,8>
	{
		typedef typename function_parser<F>::result f_info;

		static Ret call( C * _obj, F f, PyObject * _arg )
		{
			PyObject * _arg0 = tuple_getitem( _arg, 0 );
			PyObject * _arg1 = tuple_getitem( _arg, 1 );
			PyObject * _arg2 = tuple_getitem( _arg, 2 );
			PyObject * _arg3 = tuple_getitem( _arg, 3 );
			PyObject * _arg4 = tuple_getitem( _arg, 4 );
			PyObject * _arg5 = tuple_getitem( _arg, 5 );
			PyObject * _arg6 = tuple_getitem( _arg, 6 );
			PyObject * _arg7 = tuple_getitem( _arg, 7 );

			return (_obj->*f)( 
				extract_throw<typename f_info::param0>( _arg0 ),
				extract_throw<typename f_info::param1>( _arg1 ),
				extract_throw<typename f_info::param2>( _arg2 ),
				extract_throw<typename f_info::param3>( _arg3 ),
				extract_throw<typename f_info::param4>( _arg4 ),
				extract_throw<typename f_info::param5>( _arg5 ),
				extract_throw<typename f_info::param6>( _arg6 ),
				extract_throw<typename f_info::param7>( _arg7 )
				);
		}
	};

	template<class C, class F,class Ret>
	struct method_call_impl<C,F,Ret,9>
	{
		typedef typename function_parser<F>::result f_info;

		static Ret call( C * _obj, F f, PyObject * _arg )
		{
			PyObject * _arg0 = tuple_getitem( _arg, 0 );
			PyObject * _arg1 = tuple_getitem( _arg, 1 );
			PyObject * _arg2 = tuple_getitem( _arg, 2 );
			PyObject * _arg3 = tuple_getitem( _arg, 3 );
			PyObject * _arg4 = tuple_getitem( _arg, 4 );
			PyObject * _arg5 = tuple_getitem( _arg, 5 );
			PyObject * _arg6 = tuple_getitem( _arg, 6 );
			PyObject * _arg7 = tuple_getitem( _arg, 7 );
			PyObject * _arg8 = tuple_getitem( _arg, 8 );

			return (_obj->*f)( 
				extract_throw<typename f_info::param0>( _arg0 ),
				extract_throw<typename f_info::param1>( _arg1 ),
				extract_throw<typename f_info::param2>( _arg2 ),
				extract_throw<typename f_info::param3>( _arg3 ),
				extract_throw<typename f_info::param4>( _arg4 ),
				extract_throw<typename f_info::param5>( _arg5 ),
				extract_throw<typename f_info::param6>( _arg6 ),
				extract_throw<typename f_info::param7>( _arg7 ),
				extract_throw<typename f_info::param8>( _arg8 )
				);
		}
	};

	template<class C, class F,class Ret>
	struct method_call_impl<C,F,Ret,10>
	{
		typedef typename function_parser<F>::result f_info;

		static Ret call( C * _obj, F f, PyObject * _arg )
		{
			PyObject * _arg0 = tuple_getitem( _arg, 0 );
			PyObject * _arg1 = tuple_getitem( _arg, 1 );
			PyObject * _arg2 = tuple_getitem( _arg, 2 );
			PyObject * _arg3 = tuple_getitem( _arg, 3 );
			PyObject * _arg4 = tuple_getitem( _arg, 4 );
			PyObject * _arg5 = tuple_getitem( _arg, 5 );
			PyObject * _arg6 = tuple_getitem( _arg, 6 );
			PyObject * _arg7 = tuple_getitem( _arg, 7 );
			PyObject * _arg8 = tuple_getitem( _arg, 8 );
			PyObject * _arg9 = tuple_getitem( _arg, 9 );

			return (_obj->*f)( 
				extract_throw<typename f_info::param0>( _arg0 ),
				extract_throw<typename f_info::param1>( _arg1 ),
				extract_throw<typename f_info::param2>( _arg2 ),
				extract_throw<typename f_info::param3>( _arg3 ),
				extract_throw<typename f_info::param4>( _arg4 ),
				extract_throw<typename f_info::param5>( _arg5 ),
				extract_throw<typename f_info::param6>( _arg6 ),
				extract_throw<typename f_info::param7>( _arg7 ),
				extract_throw<typename f_info::param8>( _arg8 ),
				extract_throw<typename f_info::param9>( _arg9 )
				);
		}
	};

	template<class C, class F,class Ret>
	struct method_call_ret_impl
	{
		typedef typename function_parser<F>::result f_info;

		static PyObject * call( C * _obj, F f, PyObject * _arg )
		{
			Ret result = method_call_impl<C,F,Ret,f_info::arity>::call( _obj, f, _arg );

			return ptr_throw( result );
		}
	};

	template<class C, class F>
	struct method_call_ret_impl<C,F,void>
	{
		typedef typename function_parser<F>::result f_info;

		static PyObject * call( C * _obj, F f, PyObject * _arg )
		{
			method_call_impl<C,F,void,f_info::arity>::call( _obj, f, _arg );

			return ret_none();
		}
	};

	template<class C, class F>
	struct method_call
	{
		typedef typename function_parser<F>::result f_info;

		static PyObject * call( C * _self, F f, PyObject * _arg )
		{
			uint32_t arg_size = (_arg) ? pybind::tuple_size( _arg ) : 0;
			uint32_t fn_arity = f_info::arity;

			if( arg_size != fn_arity )
			{
				pybind::throw_exception("invalid method call args is not equal %d != %d\n"
					, arg_size
					, fn_arity
					);

				return nullptr;
			}

			PyObject * py_ret = method_call_ret_impl<C,F,typename f_info::ret_type>::call( _self, f, _arg );

			return py_ret;		
		}
	};
}

