#	pragma once

#	include "pybind/def_call.hpp"

namespace pybind
{
	namespace detail
	{
		PYBIND_API pybind_callback extract_from_py_function( PyObject * _self );
	}

	template<class F>
	class def_proxy
	{
	public:
		static PyObject * 
			method0( PyObject * _self )
		{
			pybind_callback f = detail::extract_from_py_function( _self );

			PyObject *ret = def_call<F>::call( (F)f, 0 );

			return ret;
		}

		static PyObject * 
			method1( PyObject * _self, PyObject * _args )
		{
			pybind_callback f = detail::extract_from_py_function( _self );

			PyObject *ret = def_call<F>::call( (F)f, _args );

			return ret;
		}
	};
}

