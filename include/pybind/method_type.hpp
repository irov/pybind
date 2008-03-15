#	pragma once

#	include "pybind/method_proxy.hpp"

#	include "pybind/types.hpp"

#	include "config/python.hpp"

namespace pybind
{
	struct py_method_type{
		PyObject_HEAD
		method_proxy_interface * ifunc;
		void * impl;
		class_type_scope * scope;
	};

	struct py_class_type;

	class method_type_scope
	{
	public:
		method_type_scope();

		method_type_scope( const method_type_scope & _scope );
		~method_type_scope();

		void setup( PyTypeObject * _class, 
			const char * _name, 
			method_proxy_interface * _ifunc, 
			pybind_cfunction _cfunc, 
			int _hasargs );

		PyObject * instance( py_class_type * _obj );
		
	public:
		//PyObject * m_mdfunc;
		const char * m_name;

	protected:
		method_proxy_interface * m_interface;
		PyMethodDef m_method;
		PyTypeObject m_type;
	};
}