#	pragma once

#	include "pybind/types.hpp"
#	include "config/python.hpp"

namespace pybind
{
	class function_adapter_interface;

	struct py_function_type{
		PyObject_HEAD
		function_adapter_interface * ifunction;
	};

	py_function_type * gen_function();
}

