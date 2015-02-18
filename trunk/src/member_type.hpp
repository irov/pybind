#	pragma once

#	include "pybind/member_adapter.hpp"

#	include "pybind/types.hpp"

#	include "config/python.hpp"

namespace pybind
{
	struct py_member_type{
		PyObject_HEAD

		member_adapter_interface * iadapter;
	};

	class member_type_scope
	{
	public:
		static PyObject * instance( const member_adapter_interface_ptr & _iadapter  );
	};

	bool initialize_members();
	void finalize_members();
}

