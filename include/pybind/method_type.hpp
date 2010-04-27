#	pragma once

#	include "pybind/method_adapter.hpp"

#	include "pybind/types.hpp"

#	include "config/python.hpp"

namespace pybind
{
	struct py_method_type{
		PyObject_HEAD
		method_adapter_interface * iadapter;
		void * impl;
		class_type_scope * scope;
	};

	struct py_class_type;

	void initialize_method();

	class method_type_scope
	{
	public:
		method_type_scope( const char * _name, 
			method_adapter_interface * _ifunc, 
			pybind_cfunction _cfunc, 
			int _hasargs );

		~method_type_scope();


	public:
		PyObject * instance( py_class_type * _obj );
		
	public:
		const char * m_name;

	protected:
		method_adapter_interface * m_interface;
		PyMethodDef m_method;
	};
}

