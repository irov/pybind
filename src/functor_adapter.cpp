#	include "pybind/functor_adapter.hpp"

#	include "functor_type.hpp"

namespace pybind
{
	namespace detail
	{
		functor_adapter_interface * extract_from_py_functor( PyObject * _self )
		{
			py_functor_type * functor = (py_functor_type*)_self;

			return functor->iadapter;
		}
	}
}
