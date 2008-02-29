#	pragma once

#	include "pybind/exports.hpp"
#	include "pybind/def_parser.hpp"
#	include "pybind/extract.hpp"

#	include "pybind/method_call.hpp"

namespace pybind
{
	template<class F>
	struct def_call
	{
		typedef typename def_parser<F>::result f_info;
		typedef typename f_info::ret_type Ret;

		static PyObject * call( F f, PyObject * _arg )
		{
			return call_ret_impl<typename f_info::ret_type>( f, _arg );
		}

		template<class R>
		static PyObject * call_ret_impl( F f, PyObject * _arg )
		{
			R result = call_impl<f_info::arity>( f, _arg );

			return ptr( result );
		}

		template<>
		static PyObject * call_ret_impl<void>( F f, PyObject * _arg )
		{
			call_impl<f_info::arity>( f, _arg );

			return detail::return_none();
		}

		template<int i>
		static Ret call_impl( F f, PyObject * _arg );

		template<>
		static Ret call_impl<0>( F f, PyObject * _arg )
		{
			return (f)();
		}	

		template<>
		static Ret call_impl<1>( F f, PyObject * _arg )
		{
			typedef typename f_info::param1 param1;
			
			PyObject * _arg1 = detail::getarg( _arg, 0 );

			return (f)( extract<param1>( _arg1 ) );
		}	

		template<>
		static Ret call_impl<2>( F f, PyObject * _arg )
		{
			typedef typename f_info::param1 param1;
			typedef typename f_info::param2 param2;

			PyObject * _arg1 = detail::getarg( _arg, 0 );
			PyObject * _arg2 = detail::getarg( _arg, 1 );

			return (f)( extract<param1>( _arg1 ), extract<param2>( _arg2 ) );
		}	

		template<>
		static Ret call_impl<3>( F f, PyObject * _arg )
		{
			typedef typename f_info::param1 param1;
			typedef typename f_info::param2 param2;
			typedef typename f_info::param3 param3;

			PyObject * _arg1 = detail::getarg( _arg, 0 );
			PyObject * _arg2 = detail::getarg( _arg, 1 );
			PyObject * _arg3 = detail::getarg( _arg, 2 );

			return (f)( extract<param1>( _arg1 ), extract<param2>( _arg2 ), extract<param3>( _arg3 ) );
		}	
	};
}