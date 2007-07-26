#	pragma once

#	include "pybind/types.hpp"

#	include "pybind/def_parser.hpp"
#	include "pybind/def_proxy.hpp"

namespace pybind
{
	namespace detail
	{
		PYBIND_API void def_function( 
			const char * _name, 
			pybind_callback _callback, 
			pybind_cfunction _cfunc, 
			int _hasargs, 
			PyObject * _module );
	}

	template<class F>
	void def( const char * _name, F _cfunc, PyObject * _module = 0 )
	{
		typedef typename def_parser<F>::result f_info;

		detail::def_function(
			_name,
			(pybind_callback)_cfunc,
			( f_info::arity > 0 ) ? (pybind_cfunction)&def_proxy<F>::method1 : (pybind_cfunction)&def_proxy<F>::method0,
			f_info::arity,
			_module
			);
	}

}