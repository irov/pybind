#	pragma once

#	include "pybind/member_adapter.hpp"

#	include "pybind/types.hpp"

#	include "config/python.hpp"

namespace pybind
{
	struct py_member_type{
		PyObject_HEAD

		const char * name;
		member_adapter_interface_ptr iadpter;
	};

	class member_type_scope
	{
	public:
		static PyObject * instance( const char * _name, const member_adapter_interface_ptr & _iadpter  );
	};

	bool initialize_members();
	void finalize_members();
}

