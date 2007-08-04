#	pragma once

#	include "pybind/config.hpp"

#	include "method_type.hpp"

#	include <list>

namespace pybind
{
	struct py_class_type{
		PyObject_HEAD
		void * impl;
		PyObject* dict;
	};

	class class_type_scope
	{
	public:
		class_type_scope();

	public:
		void class_type_scope::setup(
			const char * _name, 
			PyObject * _module,
			newfunc _pynew,
			destructor _pydestructor);

	public:
		void add_method( const char * _name, method_proxy_interface * _ifunc, int _arity );
		void set_module( PyObject * _module );

		void add_method_from_scope( class_type_scope * _basescope );

		PyObject * create_holder( void * _impl );

		void setup_method( py_class_type * _self );

	public:		
		typedef std::list< method_type_scope > TMethodFunction;
		TMethodFunction m_methods;
		
		PyTypeObject m_type;
		PyTypeObject m_type_holder;
		PyObject * m_module;
	};

	
}