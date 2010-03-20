#	pragma once

#	include "pybind/method_adapter.hpp"

#	include "pybind/types.hpp"

#	include "config/python.hpp"

namespace pybind
{
	struct py_method_type{
		PyObject_HEAD
		method_adapter_interface * ifunc;
		void * impl;
		class_type_scope * scope;
	};

	struct py_class_type;

	void initialize_method();
	void finialize_method();

	class method_type_scope
	{
	public:
		method_type_scope();
		~method_type_scope();


	public:
		void setup( PyTypeObject * _class, 
			const char * _name, 
			method_adapter_interface * _ifunc, 
			pybind_cfunction _cfunc, 
			int _hasargs );

		PyObject * instance( py_class_type * _obj );
		
	public:
		//PyObject * m_mdfunc;
		const char * m_name;

	protected:
		method_adapter_interface * m_interface;
		PyMethodDef m_method;
	};
}

