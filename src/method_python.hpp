#	pragma once

#	include "pybind/method_interface.hpp"

#	include "config/python.hpp"

namespace pybind
{
	class method_python
	{
	public:
		bool initialize();
		void finalize();

	public:
		PyObject * create_method_adapter( const method_adapter_interface_ptr & _ifunc, PyTypeObject * _type );

	protected:
		PyTypeObject m_method_caller_type;
		PyTypeObject m_method_generator_type;

		PyMemberDef m_descr_members[2];
	};
}

