#	pragma once

#	include "pybind/adapter/functor_adapter.hpp"

#	include "pybind/types.hpp"
#	include "pybind/exports.hpp"

#	include "stdex/function_traits.h"

namespace pybind
{
	template<class C, class F>
	void def_functor( kernel_interface * _kernel, const char * _name, C * _self, F _method, PyObject * _module = 0 )
	{
		typedef typename stdex::function_traits<F>::result t_info;

		functor_adapter_interface_ptr adapter =
			new functor_proxy_adapter<typename t_info::class_type, F>(_name, t_info::arity, _self, _method );

		_kernel->def_functor_adapter( adapter, false, _module );		
	}

	template<class C, class F>
	void def_functor_args( kernel_interface * _kernel, const char * _name, C * _self, F _method, PyObject * _module = 0 )
	{
		typedef typename stdex::function_traits<F>::result t_info;

		functor_adapter_interface_ptr adapter =
			new functor_proxy_adapter_args<typename t_info::class_type, F>( _name, t_info::arity, _self, _method );

		_kernel->def_functor_adapter( adapter, false, _module );
	}
}

