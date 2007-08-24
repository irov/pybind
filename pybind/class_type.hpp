#	pragma once

#	include "pybind/config.hpp"

#	include "method_type.hpp"

#	include <list>
#	include <map>
#	include <string>

namespace pybind
{
	class class_type_scope;
	class constructor;

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
		~class_type_scope();

	public:
		void class_type_scope::setup(
			const char * _name, 
			PyObject * _module,
			newfunc _pynew,
			destructor _pydestructor);

	public:
		const char * getName();

		void set_module( PyObject * _module );

		void * construct( PyObject * _args );
		void def_init( constructor * _ctr );

		void add_bases( class_type_scope * _scope );

		void add_method( const char * _name, method_proxy_interface * _ifunc, int _arity );
		void add_method_from_scope( class_type_scope * _basescope );

		void add_base( const char * _name, class_type_scope * _base, pybind_metacast _cast );

		PyObject * create_holder( void * _impl );
		void * metacast( const char * name, void * _impl );

		void setup_method( py_class_type * _self );
		void update_method_self( py_class_type * _self, void * _impl );

	public:		
		typedef std::list<method_type_scope> TMethodFunction;
		TMethodFunction m_methods;
	
		typedef std::pair<class_type_scope *, pybind_metacast> TPairMetacast;
		typedef std::map<std::string, TPairMetacast> TMapBases;
		TMapBases m_bases;

		constructor * m_constructor;

		PyTypeObject m_type;
		PyTypeObject m_type_holder;
		PyObject * m_module;
	};

	
}