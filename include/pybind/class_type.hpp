#	pragma once
 
#	include "config/python.hpp"

#	include "pybind/types.hpp"

#	include <list>
#	include <map>
#	include <string>

namespace pybind
{
	class class_type_scope;
	class method_type_scope;
	class member_type_scope;

	class method_adapter_interface;
	class member_adapter_interface;
	class repr_adapter_interface;
	
	class constructor;

	struct py_class_type{
		PyObject_HEAD
		void * impl;
		bool holder;
		class_type_scope * scope;
	};

	class class_type_scope
	{
	public:
		class_type_scope( const char * _name, const char * _type_name );
		~class_type_scope();

	public:
		void setup( PyObject * _module, newfunc _pynew, destructor _pydestructor );

	public:
		const char * get_name() const;
		void set_module( PyObject * _module );

		void * construct( PyObject * _args );
		void def_init( constructor * _ctr );

		void add_method( const char * _name, method_adapter_interface * _ifunc, int _arity );

		void add_member( const char * _name, member_adapter_interface * _imember );
		void add_member_from_scope( class_type_scope * _basescope );

		void add_repr( repr_adapter_interface * _irepr );

		void add_base( const char * _name, class_type_scope * _base, pybind_metacast _cast );

		PyObject * create_holder( void * _impl );
		PyObject * create_impl( void * _impl );

		void * metacast( const char * name, void * _impl );

		void unwrap( py_class_type * _self );

		static bool is_class( PyTypeObject * _type );

	public:		
		typedef std::list<method_type_scope *> TMethods;
		TMethods m_methods;

		typedef std::list<member_type_scope *> TMembers;
		TMembers m_members;
	
		typedef std::pair<class_type_scope *, pybind_metacast> TPairMetacast;
		typedef std::map<const char *, TPairMetacast> TMapBases;
		TMapBases m_bases;

		constructor * m_pyconstructor;
		destructor m_pydestructor;
		repr_adapter_interface * m_repr;

		const char * m_name;
		const char * m_type;

		PyTypeObject m_pytypeobject;

		PyObject * m_module;
	};

	void initialize_classes();	
}

