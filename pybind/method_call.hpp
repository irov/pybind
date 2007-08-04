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
			return call_ret_impl<typename f_info::ret_type>( _obj, f, _arg );
		}

		template<class R>
		static PyObject * call_ret_impl( C * _obj, F f, PyObject * _arg )
		{
			R result = call_impl<f_info::arity>( _obj, f, _arg );

			return ptr( result );
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

			return (_obj->*f)( extract<typename f_info::param1>( _arg1 ) , extract<typename f_info::param2>( _arg2 ) );
		}	
	};
}