#	pragma once

#	include "pybind/types.hpp"

#	include "pybind/function_parser.hpp"
#	include "pybind/def_adapter.hpp"

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

	void initialize_def();
	void finalize_def();

	template<class F>
	void def( const char * _name, F _cfunc, PyObject * _module = 0 )
	{
		typedef typename function_parser<F>::result f_info;

		detail::def_function(
			_name,
			(pybind_callback)_cfunc,
			( f_info::arity > 0 ) ? (pybind_cfunction)&def_adapter<F>::method1 : (pybind_cfunction)&def_adapter<F>::method0,
			f_info::arity,
			_module
			);
	}

}

