#	pragma once

#	include "pybind/functor_adapter.hpp"

#	include "pybind/types.hpp"
#	include "pybind/exports.hpp"

#	include "stdex/function_traits.h"

namespace pybind
{
	namespace detail
	{
		PYBIND_API void def_functor( const functor_adapter_interface_ptr & _adapter, bool _native, PyObject * _module );
	}

	template<class C, class F>
	void def_functor( const char * _name, C * _self, F _method, PyObject * _module = 0 )
	{
		typedef typename stdex::function_traits<F>::result t_info;

		functor_adapter_interface_ptr imethod =
			new functor_proxy_adapter<typename t_info::class_type, F>(_name, t_info::arity, _self, _method );

		detail::def_functor( imethod, false, _module );
	}

	template<class C, class F>
	void def_functor_args( const char * _name, C * _self, F _method, PyObject * _module = 0 )
	{
		typedef typename stdex::function_traits<F>::result t_info;

		functor_adapter_interface_ptr imethod =
			new functor_proxy_adapter_args<typename t_info::class_type, F>( _name, t_info::arity, _self, _method );

		detail::def_functor( imethod, false, _module );
	}
}

