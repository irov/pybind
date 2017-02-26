#	pragma once

#	include "pybind/kernel.hpp"
#	include "pybind/helper.hpp"
#	include "pybind/tuple.hpp"

namespace pybind
{
	template<class T> 
	detail::extract_operator_t internal_call_method( kernel_interface * _kernel, T * _self, const char * _name )
	{
		const pybind::class_type_scope_ptr & scope = _kernel->class_scope<T>();

		tuple args = pybind::make_tuple_t( _kernel );

		PyObject * py_result = _kernel->call_method( _self, scope, _name, args );

		return py_result;
	}
	//////////////////////////////////////////////////////////////////////////
	template<class T, class A0>
	detail::extract_operator_t internal_call_method( kernel_interface * _kernel, T * _self, const char * _name, const A0 & _a0 )
	{
		const pybind::class_type_scope_ptr & scope = _kernel->class_scope<T>();

		tuple args = pybind::make_tuple_t( _kernel, _a0 );

		PyObject * py_result = _kernel->call_method( _self, scope, _name, args );

		return py_result;
	}
	//////////////////////////////////////////////////////////////////////////
	template<class T, class A0, class A1>
	detail::extract_operator_t internal_call_method( kernel_interface * _kernel, T * _self, const char * _name, const A0 & _a0, const A1 & _a1 )
	{
		const pybind::class_type_scope_ptr & scope = _kernel->class_scope<T>();

		tuple args = pybind::make_tuple_t( _kernel, _a0, _a1 );

		PyObject * py_result = _kernel->call_method( _self, scope, _name, args );

		return py_result;
	}
}