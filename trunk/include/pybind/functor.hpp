#	pragma once

#	include "pybind/function_parser.hpp"
#	include "pybind/functor_adapter.hpp"

#	include "pybind/types.hpp"
#	include "pybind/exports.hpp"

namespace pybind
{
	namespace detail
	{
		PYBIND_API void def_functor(			
			functor_adapter_interface * _adapter,
			PyObject * _module );
	}

	template<class C, class F>
	void def_functor( const char * _name, C * _self, F _method, PyObject * _module = 0 )
	{
		typedef typename function_parser<F>::result t_info;

		functor_adapter_interface * imethod =
			new functor_proxy_adapter<typename t_info::class_type, F>(_self, _method, _name);

		detail::def_functor(
			imethod,
            _module
			);
	}

}

