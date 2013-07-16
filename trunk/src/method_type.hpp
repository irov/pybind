#	pragma once

#	include "pybind/method_adapter.hpp"

#	include "pybind/types.hpp"

namespace pybind
{
	class method_type_scope
	{
	public:
		static PyObject * instance( const char * _name, const method_adapter_interface_ptr & _ifunc, PyTypeObject * _type );
	};

	bool initialize_methods();
	void finalize_methods();
}

