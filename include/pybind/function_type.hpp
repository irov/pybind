#	pragma once

#	include "function_adapter.hpp"

#	include "pybind/types.hpp"
#	include "config/python.hpp"

namespace pybind
{
	struct py_function_type{
		PyObject_HEAD

		function_adapter_interface * ifunction;
	};

	py_function_type * gen_function();

	void initialize_function();
	void finalize_function();
}

