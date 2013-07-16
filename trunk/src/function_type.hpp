#	pragma once

#	include "pybind/function_adapter.hpp"

#	include "pybind/types.hpp"
#	include "config/python.hpp"

namespace pybind
{
	struct py_function_type{
		PyObject_HEAD

		function_adapter_interface_ptr iadapter;
	};

	py_function_type * gen_function();

	bool initialize_function();
	void finalize_function();
}

