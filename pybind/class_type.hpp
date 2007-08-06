#	pragma once

#	include "pybind/config.hpp"

#	include "method_type.hpp"

#	include <list>
#	include <map>
#	include <string>

namespace pybind
{
	class class_type_scope;

	struct py_class_type{
		PyObject_HEAD
		void * impl;
		PyObject* dict;
		class_type_scope * scope;
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
		void set_module( PyObject * _module );

		void add_bases( class_type_scope * _scope );

		void add_method( const char * _name, method_proxy_interface * _ifunc, int _arity );
		void add_method_from_scope( class_type_scope * _basescope );

		void add_meta_cast( const char * _name, pybind_metacast cast );
		void add_meta_cast_from_scope( class_type_scope * _basescope );

		PyObject * create_holder( void * _impl );
		void * metacast( const char * name, void * _impl );

		void setup_method( py_class_type * _self );

	public:		
		typedef std::list<class_type_scope *> TListBases;
		TListBases m_bases;

		typedef std::list<method_type_scope> TMethodFunction;
		TMethodFunction m_methods;
	
		typedef std::map<std::string, pybind_metacast> TMapMetaCast;

		TMapMetaCast m_metacast;

		PyTypeObject m_type;
		PyTypeObject m_type_holder;
		PyObject * m_module;
	};

	
}