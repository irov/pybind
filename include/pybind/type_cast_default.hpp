#	pragma once

#	include <pybind/types.hpp>

namespace pybind
{
	namespace detail
	{
		PYBIND_API bool extract_float( PyObject * _obj, float & _value );
	}
}
