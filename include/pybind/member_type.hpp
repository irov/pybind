#	pragma once

#	include "pybind/member_adapter.hpp"

#	include "pybind/types.hpp"

#	include "config/python.hpp"

namespace pybind
{
	class member_type_scope
	{
	public:
		member_type_scope();
		~member_type_scope();

	public:
		void setup( PyTypeObject * _class, 
			const char * _name, 
			method_adapter_interface * _ifunc, 
			pybind_cfunction _cfunc, 
			int _hasargs );

		PyObject * instance( py_class_type * _obj );

	public:
		const char * m_name;

	protected:
		member_adapter_interface * m_interface;		
	};
}

