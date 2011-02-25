#	pragma once

#	include "pybind/method_adapter.hpp"

#	include "pybind/types.hpp"

#	include "config/python.hpp"

namespace pybind
{
	class method_type_scope
	{
	public:
		//method_type_scope( const char * _name, method_adapter_interface * _ifunc );
		//~method_type_scope();

	public:
		static PyObject * instance( const char * _name, method_adapter_interface * _ifunc, PyTypeObject * _type );
		
	//public:
		//const char * m_name;

	//protected:
		//method_adapter_interface * m_interface;
	};

	void initialize_methods();
	void finalize_methods();
}

