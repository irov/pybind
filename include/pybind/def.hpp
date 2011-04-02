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

	namespace detail
	{
		template<class F, int Arity>
		struct def_arity
		{
			static void call( const char * _name, F _cfunc, PyObject * _module = 0 )
			{
				detail::def_function(
					_name,
					(pybind_callback)_cfunc,
					(pybind_cfunction)&def_adapter<F>::method1,
					Arity,
					_module
					);
			}
		};

		template<class F>
		struct def_arity<F,0>
		{
			static void call( const char * _name, F _cfunc, PyObject * _module = 0 )
			{
				detail::def_function(
					_name,
					(pybind_callback)_cfunc,
					(pybind_cfunction)&def_adapter<F>::method0,
					0,
					_module
					);
			}
		};
	}

	template<class F>
	void def( const char * _name, F _cfunc, PyObject * _module = 0 )
	{
		typedef typename function_parser<F>::result f_info;

		detail::def_arity<F, f_info::arity>::call( _name, _cfunc, _module );
	}
}

