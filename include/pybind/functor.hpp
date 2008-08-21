#	pragma once

#	include "pybind/types.hpp"

#	include "pybind/method_parser.hpp"
#	include "pybind/functor_proxy.hpp"

namespace pybind
{
	namespace detail
	{
		PYBIND_API void def_functor(
			const char * _name,
			functor_proxy_interface * _imethod,
			pybind_cfunction _cfunc,
			int _hasargs, 
			PyObject * _module );
	}

	template<class C, class F>
	void functor( const char * _name, C * _self, F _method, PyObject * _module = 0 )
	{
		typedef typename method_parser<F>::result t_info;

		functor_proxy_interface * imethod =
			new functor_proxy<typename t_info::class_type, F>(_name, _self, _method);

		detail::def_functor(
			_name,
			imethod,
			( t_info::arity > 0 ) ? (pybind_cfunction)&functor_proxy<C,F>::method1 : (pybind_cfunction)&functor_proxy<C,F>::method0,
			t_info::arity,
			_module
			);
	}

}

