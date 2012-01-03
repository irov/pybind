#	pragma once

#	include "pybind/function_adapter.hpp"

#	include "pybind/types.hpp"
#	include "pybind/exports.hpp"

#	include "pybind/system.hpp"

namespace pybind
{
	namespace detail
	{
		PYBIND_API function_adapter_interface * extract_adapter_py_function( PyObject * _self );
		PYBIND_API PyObject * function_noargs( PyObject * _self );
		PYBIND_API PyObject * function_args( PyObject * _self, PyObject * _args );
		PYBIND_API PyObject * function_kwds( PyObject * _self, PyObject * _args, PyObject * _kwds );

		PYBIND_API PyObject * create_function_adapter( function_adapter_interface * _adapter, bool _native );

		PYBIND_API void store_function_adapter( function_adapter_interface * _adapter );
	}

	void initialize_function();
	void finalize_function();

	void def_function_adapter( function_adapter_interface * _adapter, bool _native, PyObject * _module = 0 )
	{
		PyObject * py_func = detail::create_function_adapter( _adapter, _native );

		if( _module == 0 )
		{
			_module = get_currentmodule();
		}

		const char * name = _adapter->getName();

		module_addobject( _module, name, py_func );
	}

	template<class F>
	function_adapter_interface * create_function_adapter( const char * _name, F _function )
	{
		function_adapter_interface * adapter =
			new function_adapter<F>(_function, _name);

		return adapter;
	}

	template<class F>
	void def_function( const char * _name, F _function, PyObject * _module = 0 )
	{
		function_adapter_interface * adapter = create_function_adapter( _name, _function );

		def_function_adapter( ifunction, false, _module );

		detail::store_function_adapter( ifunction );
	}

	template<class F>
	void def_function_native( const char * _name, F _cfunc, PyObject * _module = 0 )
	{
		function_adapter_interface * ifunction =
			new function_adapter_native<F>(_function, _name);

		def_function_adapter( ifunction, true, _module );

		detail::store_function_adapter( ifunction );
	}
}