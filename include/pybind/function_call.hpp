#	pragma once

#	include "pybind/function_parser.hpp"
#	include "pybind/extract.hpp"

namespace pybind
{
	template<class F, class Ret, int i>
	struct function_call_impl
	{
		static Ret call( F f, PyObject * _arg );
	};

	template<class F, class Ret>
	struct function_call_impl<F,Ret,0>
	{
		typedef typename function_parser<F>::result f_info;

		static Ret call( F f, PyObject * _arg )
		{
			return (f)();
		}	
	};

	template<class F, class Ret>
	struct function_call_impl<F,Ret,1>
	{
		typedef typename function_parser<F>::result f_info;

		static Ret call( F f, PyObject * _arg )
		{
			PyObject * _arg0 = tuple_getitem( _arg, 0 );

			return (f)( 
				extract<typename f_info::param0>( _arg0 ) 
				);
		}	
	};

	template<class F, class Ret>
	struct function_call_impl<F,Ret,2>
	{
		typedef typename function_parser<F>::result f_info;

		static Ret call( F f, PyObject * _arg )
		{
			PyObject * _arg0 = tuple_getitem( _arg, 0 );
			PyObject * _arg1 = tuple_getitem( _arg, 1 );

			return (f)( 
				extract<typename f_info::param0>( _arg0 ), 
				extract<typename f_info::param1>( _arg1 ) 
				);
		}
	};

	template<class F, class Ret>
	struct function_call_impl<F,Ret,3>
	{
		typedef typename function_parser<F>::result f_info;

		static Ret call( F f, PyObject * _arg )
		{
			PyObject * _arg0 = tuple_getitem( _arg, 0 );
			PyObject * _arg1 = tuple_getitem( _arg, 1 );
			PyObject * _arg2 = tuple_getitem( _arg, 2 );

			return (f)( 
				extract<typename f_info::param0>( _arg0 ), 
				extract<typename f_info::param1>( _arg1 ), 
				extract<typename f_info::param2>( _arg2 ) 
				);
		}
	};

	template<class F, class Ret>
	struct function_call_impl<F,Ret,4>
	{
		typedef typename function_parser<F>::result f_info;

		static Ret call( F f, PyObject * _arg )
		{
			PyObject * _arg0 = tuple_getitem( _arg, 0 );
			PyObject * _arg1 = tuple_getitem( _arg, 1 );
			PyObject * _arg2 = tuple_getitem( _arg, 2 );
			PyObject * _arg3 = tuple_getitem( _arg, 3 );

			return (f)( 
				extract<typename f_info::param0>( _arg0 ), 
				extract<typename f_info::param1>( _arg1 ), 
				extract<typename f_info::param2>( _arg2 ),
				extract<typename f_info::param3>( _arg3 )
				);
		}
	};

	template<class F, class Ret>
	struct function_call_impl<F,Ret,5>
	{
		typedef typename function_parser<F>::result f_info;

		static Ret call( F f, PyObject * _arg )
		{
			PyObject * _arg0 = tuple_getitem( _arg, 0 );
			PyObject * _arg1 = tuple_getitem( _arg, 1 );
			PyObject * _arg2 = tuple_getitem( _arg, 2 );
			PyObject * _arg3 = tuple_getitem( _arg, 3 );
			PyObject * _arg4 = tuple_getitem( _arg, 4 );

			return (f)( 
				extract<typename f_info::param0>( _arg0 ), 
				extract<typename f_info::param1>( _arg1 ), 
				extract<typename f_info::param2>( _arg2 ),
				extract<typename f_info::param3>( _arg3 ),
				extract<typename f_info::param4>( _arg4 )
				);
		}
	};

	template<class F, class Ret>
	struct function_call_impl<F,Ret,6>
	{
		typedef typename function_parser<F>::result f_info;

		static Ret call( F f, PyObject * _arg )
		{
			PyObject * _arg0 = tuple_getitem( _arg, 0 );
			PyObject * _arg1 = tuple_getitem( _arg, 1 );
			PyObject * _arg2 = tuple_getitem( _arg, 2 );
			PyObject * _arg3 = tuple_getitem( _arg, 3 );
			PyObject * _arg4 = tuple_getitem( _arg, 4 );
			PyObject * _arg5 = tuple_getitem( _arg, 5 );

			return (f)( 
				extract<typename f_info::param0>( _arg0 ), 
				extract<typename f_info::param1>( _arg1 ), 
				extract<typename f_info::param2>( _arg2 ),
				extract<typename f_info::param3>( _arg3 ),
				extract<typename f_info::param4>( _arg4 ),
				extract<typename f_info::param5>( _arg5 )
				);
		}
	};

	template<class F, class Ret>
	struct function_call_impl<F,Ret,7>
	{
		typedef typename function_parser<F>::result f_info;

		static Ret call( F f, PyObject * _arg )
		{
			PyObject * _arg0 = tuple_getitem( _arg, 0 );
			PyObject * _arg1 = tuple_getitem( _arg, 1 );
			PyObject * _arg2 = tuple_getitem( _arg, 2 );
			PyObject * _arg3 = tuple_getitem( _arg, 3 );
			PyObject * _arg4 = tuple_getitem( _arg, 4 );
			PyObject * _arg5 = tuple_getitem( _arg, 5 );
			PyObject * _arg6 = tuple_getitem( _arg, 6 );

			return (f)( 
				extract<typename f_info::param0>( _arg0 ), 
				extract<typename f_info::param1>( _arg1 ), 
				extract<typename f_info::param2>( _arg2 ),
				extract<typename f_info::param3>( _arg3 ),
				extract<typename f_info::param4>( _arg4 ),
				extract<typename f_info::param5>( _arg5 ),
				extract<typename f_info::param6>( _arg6 )
				);
		}
	};

	template<class F, class Ret>
	struct function_call_impl<F,Ret,8>
	{
		typedef typename function_parser<F>::result f_info;

		static Ret call( F f, PyObject * _arg )
		{
			PyObject * _arg0 = tuple_getitem( _arg, 0 );
			PyObject * _arg1 = tuple_getitem( _arg, 1 );
			PyObject * _arg2 = tuple_getitem( _arg, 2 );
			PyObject * _arg3 = tuple_getitem( _arg, 3 );
			PyObject * _arg4 = tuple_getitem( _arg, 4 );
			PyObject * _arg5 = tuple_getitem( _arg, 5 );
			PyObject * _arg6 = tuple_getitem( _arg, 6 );
			PyObject * _arg7 = tuple_getitem( _arg, 7 );

			return (f)( 
				extract<typename f_info::param0>( _arg0 ), 
				extract<typename f_info::param1>( _arg1 ), 
				extract<typename f_info::param2>( _arg2 ),
				extract<typename f_info::param3>( _arg3 ),
				extract<typename f_info::param4>( _arg4 ),
				extract<typename f_info::param5>( _arg5 ),
				extract<typename f_info::param6>( _arg6 ),
				extract<typename f_info::param7>( _arg7 )
				);
		}
	};

	template<class F, class Ret>
	struct function_call_impl<F,Ret,9>
	{
		typedef typename function_parser<F>::result f_info;

		static Ret call( F f, PyObject * _arg )
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

			return (f)( 
				extract<typename f_info::param0>( _arg0 ), 
				extract<typename f_info::param1>( _arg1 ), 
				extract<typename f_info::param2>( _arg2 ),
				extract<typename f_info::param3>( _arg3 ),
				extract<typename f_info::param4>( _arg4 ), 
				extract<typename f_info::param5>( _arg5 ), 
				extract<typename f_info::param6>( _arg6 ),
				extract<typename f_info::param7>( _arg7 ),
				extract<typename f_info::param8>( _arg8 )
				);
		}
	};

	template<class F, class Ret>
	struct function_call_impl<F,Ret,10>
	{
		typedef typename function_parser<F>::result f_info;

		static Ret call( F f, PyObject * _arg )
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

			return (f)( 
				extract<typename f_info::param0>( _arg0 ), 
				extract<typename f_info::param1>( _arg1 ), 
				extract<typename f_info::param2>( _arg2 ),
				extract<typename f_info::param3>( _arg3 ),
				extract<typename f_info::param4>( _arg4 ), 
				extract<typename f_info::param5>( _arg5 ), 
				extract<typename f_info::param6>( _arg6 ),
				extract<typename f_info::param7>( _arg7 ),
				extract<typename f_info::param8>( _arg8 ),
				extract<typename f_info::param9>( _arg9 )
				);
		}
	};

	template<class F, class Ret>
	struct function_call_ret_impl
	{
		typedef typename function_parser<F>::result f_info;

		static PyObject * call( F f, PyObject * _arg )
		{
			Ret result = function_call_impl<F,Ret,f_info::arity>::call( f, _arg );

			return ptr_throw( result );
		}
	};

	template<class F>
	struct function_call_ret_impl<F,void>
	{
		typedef typename function_parser<F>::result f_info;

		static PyObject * call( F f, PyObject * _arg )
		{
			function_call_impl<F,void,f_info::arity>::call( f, _arg );

			return ret_none();
		}
	};

	template<class F>
	struct function_call
	{
		typedef typename function_parser<F>::result f_info;

		static PyObject * call( F f, PyObject * _arg, const char * _tag )
        {
			size_t arg_size = (_arg)?tuple_size(_arg):0;
            size_t fn_arity = f_info::arity;

			if( arg_size != fn_arity )
			{
				error_message("invalid function call '%s' args is not equal %d != %d\n"
					, _tag
					, arg_size
					, fn_arity
					);

				return nullptr;
			}

			try
			{
				PyObject * ret = function_call_ret_impl<F,typename f_info::ret_type>::call( f, _arg );

				return ret;
			}
			catch( const pybind_exception & )
			{
                error_message("invalid function call '%s'\n"
                    , _tag
                    );
			}

			return nullptr;
		}
	};
}

