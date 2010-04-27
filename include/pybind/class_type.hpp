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
		void setup(
			const char * _name, 
			const char * _type,
			PyObject * _module,
			newfunc _pynew,
			destructor _pydestructor);

	public:
		const char * getName() const;
		void set_module( PyObject * _module );

		void * construct( PyObject * _args );
		void def_init( constructor * _ctr );

		void add_bases( class_type_scope * _scope );

		void add_method( const char * _name, method_adapter_interface * _ifunc, int _arity );
		void add_method_from_scope( class_type_scope * _basescope );

		void add_member( const char * _name, member_adapter_interface * _imember );
		void add_member_from_scope( class_type_scope * _basescope );

		void add_base( const char * _name, class_type_scope * _base, pybind_metacast _cast );

		PyObject * create_holder( void * _impl );
		PyObject * create_impl( void * _impl );

		void * metacast( const char * name, void * _impl );

		void setup_attributes( py_class_type * _self );
		void update_attributes_self( py_class_type * _self, void * _impl );

		void unwrap( py_class_type * _self );

		static bool is_class( PyTypeObject * _type );

	public:		
		typedef std::list<method_type_scope *> TMethods;
		TMethods m_methods;
		TMethods m_methodsBase;

		typedef std::list<member_type_scope *> TMembers;
		TMembers m_members;
		TMembers m_membersBase;
	
		typedef std::pair<class_type_scope *, pybind_metacast> TPairMetacast;
		typedef std::map<const char *, TPairMetacast> TMapBases;
		TMapBases m_bases;

		constructor * m_constructor;

		const char * m_type_name;

		PyTypeObject * m_type;
		PyTypeObject * m_type_holder;

		PyObject * m_module;
	};

	
}

