#	pragma once

#	include "pybind/config.hpp"

#	include <vector>

namespace pybind
{
	struct py_class_type{
		PyObject_HEAD
		void * impl;
	};

	class class_type_scope
	{
	public:
		class_type_scope(
			const char * _name, 
			PyObject * _module,
			newfunc _pynew,
			destructor _pydestructor
			);

	public:
		void add_method( const PyMethodDef & md );
		void set_module( PyObject * _module );

		void add_method_from_scope( class_type_scope * _basescope );

		PyObject * create_holder( void * _impl );

		void setup_type();

	public:
		typedef std::vector< PyMethodDef > TVectorPyMethodDef;

		TVectorPyMethodDef m_vectorMethodDef;
		PyTypeObject m_type;
		PyTypeObject m_type_holder;
		PyObject * m_module;
	};

	
}