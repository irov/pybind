#	pragma once

#	include "pybind/function_parser.hpp"
#	include "pybind/function_adapter.hpp"

#	include "pybind/types.hpp"
#	include "pybind/exports.hpp"

#	include "pybind/system.hpp"

namespace pybind
{
	namespace detail
	{
		PYBIND_API void def_function_adapter( const function_adapter_interface_ptr & _adapter, bool _native, PyObject * _module );
        PYBIND_API PyObject * create_function_adapter( const function_adapter_interface_ptr & _adapter, bool _native );
	}

	template<class F>
	void def_function( const char * _name, F _function, PyObject * _module = 0 )
	{
		typedef typename function_parser<F>::result t_info;

        function_adapter_interface_ptr adapter =
			new function_adapter<F>(_name, t_info::arity, _function);

		detail::def_function_adapter( adapter, false, _module );
	}

    template<class F>
    PyObject * create_function( const char * _name, F _function )
    {
		typedef typename function_parser<F>::result t_info;

        function_adapter_interface_ptr adapter =
			new function_adapter<F>(_name, t_info::arity, _function);

        PyObject * py_function = detail::create_function_adapter( adapter, false );

        return py_function;
    }

	template<class F>
	void def_function_native( const char * _name, F _function, PyObject * _module = 0 )
	{
		typedef typename function_parser<F>::result t_info;

		function_adapter_interface_ptr adapter =
			new function_adapter_native<F>(_name, t_info::arity, _function);

		detail::def_function_adapter( adapter, true, _module );		
	}

    template<class F>
    PyObject * create_function_native( const char * _name, F _function )
    {
		typedef typename function_parser<F>::result t_info;

        function_adapter_interface_ptr adapter =
			new function_adapter_native<F>(_name, t_info::arity, _function);

        PyObject * py_function = detail::create_function_adapter( adapter, true );

        return py_function;
    }
}