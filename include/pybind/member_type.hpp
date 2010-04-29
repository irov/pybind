#	pragma once

#	include "pybind/member_adapter.hpp"

#	include "pybind/types.hpp"

#	include "config/python.hpp"

namespace pybind
{
	struct py_member_type{
		PyObject_HEAD
		member_adapter_interface * iadpter;
	};

	class member_type_scope
	{
	public:
		member_type_scope( const char * _name, member_adapter_interface * _ifunc );

		~member_type_scope();

	public:
		PyObject * instance();

	public:
		const char * m_name;

	protected:
		member_adapter_interface * m_interface;
		PyMethodDef m_getmethod;
		PyMethodDef m_setmethod;
	};

	void initialize_members();
}

