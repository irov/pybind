#	pragma once

#	include "pybind/functor_adapter.hpp"

#	include "pybind/types.hpp"
#	include "config/python.hpp"

namespace pybind
{
	struct py_functor_type{
		PyObject_HEAD
		functor_adapter_interface * iadapter;
	};

	py_functor_type * gen_functor();

	bool initialize_functor();
	void finalize_functor();
}

