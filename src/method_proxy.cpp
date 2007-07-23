#	include "pybind/method_proxy.hpp"

#	include "pybind/class_type.hpp"

namespace pybind
{
	namespace detail
	{
		void * class_type_impl( py_class_type * _class_type )
		{
			return _class_type->impl;
		}
	}
}
