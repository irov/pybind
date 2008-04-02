#	pragma once

#	include "pybind/exports.hpp"
#	include "pybind/method_parser.hpp"
#	include "pybind/types.hpp"

namespace pybind
{
	namespace detail
	{
		PYBIND_API PyObject * return_none();
		PYBIND_API PyObject * getarg( PyObject * _args, int _item);
	}

	template<class C, class F>
	struct method_call
	{
		typedef typename method_parser<F>::result f_info;
		typedef typename f_info::ret_type Ret;

		static PyObject * call( C * _obj, F f, PyObject * _arg )
		{
			return call_ret_impl<Ret>( _obj, f, _arg );
		}

		template<class R>
		static PyObject * call_ret_impl( C * _obj, F f, PyObject * _arg )
		{
			if( f_info::arity > 0 )
			{
				size_t arg_size = tuple_size(_arg);
				if( arg_size != f_info::arity )
				{
					error_message("invalid function call args is not equal %d != %d\n"
						, arg_size
						, f_info::arity
						);

					return ret_none();
				}
			}

			R result = call_impl<f_info::arity>( _obj, f, _arg );

			return ptr<R>( result );
		}

		template<>
		static PyObject * call_ret_impl<void>( C * _obj, F f, PyObject * _arg )
		{
			call_impl<f_info::arity>( _obj, f, _arg );

			return detail::return_none();
		}

		template<int i>
		static Ret call_impl( C * _obj, F f, PyObject * _arg );

		template<>
		static Ret call_impl<0>( C * _obj, F f, PyObject * _arg )
		{
			return (_obj->*f)();
		}	

		template<>
		static Ret call_impl<1>( C * _obj, F f, PyObject * _arg )
		{
			PyObject * _arg1 = detail::getarg( _arg, 0 );
		
			typename f_info::param1 param1 = extract<typename f_info::param1>( _arg1 );

			return (_obj->*f)( param1 );
		}	

		template<>
		static Ret call_impl<2>( C * _obj, F f, PyObject * _arg )
		{
			PyObject * _arg1 = detail::getarg( _arg, 0 );
			PyObject * _arg2 = detail::getarg( _arg, 1 );

			return (_obj->*f)( 
				extract<typename f_info::param1>( _arg1 ), 
				extract<typename f_info::param2>( _arg2 ) 
				);
		}	

		template<>
		static Ret call_impl<3>( C * _obj, F f, PyObject * _arg )
		{
			PyObject * _arg1 = detail::getarg( _arg, 0 );
			PyObject * _arg2 = detail::getarg( _arg, 1 );
			PyObject * _arg3 = detail::getarg( _arg, 2 );

			return (_obj->*f)( 
				extract<typename f_info::param1>( _arg1 ), 
				extract<typename f_info::param2>( _arg2 ),
				extract<typename f_info::param3>( _arg3 )
				);
		}	

		template<>
		static Ret call_impl<4>( C * _obj, F f, PyObject * _arg )
		{
			PyObject * _arg1 = detail::getarg( _arg, 0 );
			PyObject * _arg2 = detail::getarg( _arg, 1 );
			PyObject * _arg3 = detail::getarg( _arg, 2 );
			PyObject * _arg4 = detail::getarg( _arg, 3 );

			return (_obj->*f)( 
				extract<typename f_info::param1>( _arg1 ), 
				extract<typename f_info::param2>( _arg2 ),
				extract<typename f_info::param3>( _arg3 ),
				extract<typename f_info::param4>( _arg4 )
				);
		}	

		template<>
		static Ret call_impl<5>( C * _obj, F f, PyObject * _arg )
		{
			PyObject * _arg1 = detail::getarg( _arg, 0 );
			PyObject * _arg2 = detail::getarg( _arg, 1 );
			PyObject * _arg3 = detail::getarg( _arg, 2 );
			PyObject * _arg4 = detail::getarg( _arg, 3 );
			PyObject * _arg5 = detail::getarg( _arg, 4 );

			return (_obj->*f)( 
				extract<typename f_info::param1>( _arg1 ), 
				extract<typename f_info::param2>( _arg2 ),
				extract<typename f_info::param3>( _arg3 ),
				extract<typename f_info::param4>( _arg4 ),
				extract<typename f_info::param5>( _arg5 )
				);
		}	

	};
}