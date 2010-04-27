#	pragma once

#	include "pybind/member_adapter.hpp"

#	include "pybind/types.hpp"

#	include "config/python.hpp"

namespace pybind
{
	struct py_member_type{
		PyObject_HEAD
		member_adapter_interface * iadpter;
		void * impl;
		class_type_scope * scope;
	};

	struct py_class_type;

	void initialize_member();

	class member_type_scope
	{
	public:
		member_type_scope( const char * _name, member_adapter_interface * _ifunc );

		~member_type_scope();

	public:
		PyObject * instance( py_class_type * _obj );

	public:
		const char * m_name;

	protected:
		member_adapter_interface * m_interface;
		PyMethodDef m_getmethod;
		PyMethodDef m_setmethod;
	};
}

