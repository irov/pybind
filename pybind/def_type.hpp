#	pragma once

#	include "pybind/types.hpp"
#	include "pybind/config.hpp"

namespace pybind
{
	struct py_function_type{
		PyObject_HEAD
			pybind_callback f;
	};
}