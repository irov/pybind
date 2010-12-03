#	pragma once
 
#	include "pybind/exports.hpp"
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
	class convert_adapter_interface;
	class repr_adapter_interface;
	
	class constructor;

	namespace detail
	{
		PYBIND_API bool is_class( PyObject * _obj );
		PYBIND_API void * get_class_impl( PyObject * _obj );
		PYBIND_API class_type_scope * get_class_scope( PyObject * _obj );
		PYBIND_API void * unwrap( PyObject * _obj );
		PYBIND_API void wrap( PyObject * _obj, void * _impl );

		PYBIND_API void * check_registred_class( PyObject * _obj, const std::type_info & _info );

		PYBIND_API PyObject * alloc_class( PyTypeObject * _type, PyObject * _args, void * _impl, const std::type_info & _tinfo );
		PYBIND_API void * dealloc_class( PyObject * _obj );

		PYBIND_API void reg_class_type( PyTypeObject * _type );

		PYBIND_API void reg_class_type_scope( const std::type_info & _info, class_type_scope * _scope );
		PYBIND_API class_type_scope * get_class_type_scope( const std::type_info & _info );
	}

	class class_type_scope
	{
	public:
		class_type_scope( const char * _name, const char * _type_name );
		~class_type_scope();

	public:
		void setup( PyObject * _module, pybind_newfunc _pynew, pybind_destructor _pydestructor );

	public:
		const char * get_name() const;
		void set_module( PyObject * _module );

		void * construct( PyObject * _args );
		void def_init( constructor * _ctr );

		void add_method( const char * _name, method_adapter_interface * _ifunc, int _arity );

		void add_member( const char * _name, member_adapter_interface * _imember );
		void add_member_from_scope( class_type_scope * _basescope );

		void add_convert( convert_adapter_interface * _iconvert );
		convert_adapter_interface * get_convert();

		void add_repr( repr_adapter_interface * _irepr );
		void add_getattro( method_adapter_interface * _igetattro );
		void add_mapping( method_adapter_interface * _imapping );

		void add_base( const char * _name, class_type_scope * _base, pybind_metacast _cast );

		PyObject * create_holder( void * _impl );
		PyObject * create_impl( void * _impl );

		void * metacast( const char * name, void * _impl );
		void unwrap( PyObject * _obj );

	public:		
		typedef std::list<method_type_scope *> TMethods;
		TMethods m_methods;

		typedef std::list<member_type_scope *> TMembers;
		TMembers m_members;
	
		typedef std::pair<class_type_scope *, pybind_metacast> TPairMetacast;
		typedef std::map<const char *, TPairMetacast> TMapBases;
		TMapBases m_bases;

		constructor * m_pyconstructor;
		pybind_destructor m_pydestructor;

		convert_adapter_interface * m_convert;
		repr_adapter_interface * m_repr;
		method_adapter_interface * m_getattro;
		method_adapter_interface * m_mapping;

		const char * m_name;
		const char * m_type;

		PyTypeObject * m_pytypeobject;

		PyObject * m_module;
	};

	void initialize_classes();	
}

