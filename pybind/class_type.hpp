#	pragma once

#	include "pybind/config.hpp"

#	include <vector>

namespace pybind
{
	struct py_class_type{
		PyObject_HEAD
		void * impl;
	};

	class module_;

	class class_type_scope
	{
	public:
		class_type_scope(
			const char * _name, 
			module_ * _module,
			newfunc _pynew,
			destructor _pydestructor
			);

	public:
		void add_method( const PyMethodDef & md );
		void set_module( module_ * _module );

		void add_method_from_scope( class_type_scope * _basescope );

		void setup_type( const type_info & _info );

	public:
		typedef std::vector< PyMethodDef > TVectorPyMethodDef;

		TVectorPyMethodDef m_vectorMethodDef;
		PyTypeObject m_type;
		PyTypeObject m_type_holder;
		module_ * m_module;
	};

	
}