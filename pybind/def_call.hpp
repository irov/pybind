#	pragma once

#	include "pybind/exports.hpp"
#	include "pybind/def_parser.hpp"
#	include "pybind/from_py_to_cpp_parser.hpp"

#	include "method_call.hpp"

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

			return from_py_to_cpp_parser< R >::wrapp( result );
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
			typedef from_py_to_cpp_parser< typename f_info::param1 > parser1;

			PyObject * _arg1 = detail::getarg( _arg, 0 );

			return (f)( parser1::value( _arg1 ) );
		}	

		template<>
		static Ret call_impl<2>( F f, PyObject * _arg )
		{
			typedef from_py_to_cpp_parser< typename f_info::param1 > parser1;
			typedef from_py_to_cpp_parser< typename f_info::param2 > parser2;

			PyObject * _arg1 = detail::getarg( _arg, 0 );
			PyObject * _arg2 = detail::getarg( _arg, 1 );

			return (f)( parser1::value( _arg1 ) , parser2::value( _arg2 ) );
		}	
	};
}