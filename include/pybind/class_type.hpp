#	pragma once
 
#	include "pybind/exports.hpp"
#	include "pybind/types.hpp"

#   include "pybind/helper.hpp"

#	include <list>
#	include <vector>
#	include <map>

#	include <typeinfo>

namespace pybind
{
	class class_type_scope;
	class method_type_scope;
	class member_type_scope;

	class method_adapter_interface;
	class member_adapter_interface;
	class convert_adapter_interface;
	class repr_adapter_interface;
    class compare_adapter_interface;
	
	class constructor;

	namespace detail
	{
		PYBIND_API bool is_class( PyObject * _obj );
		PYBIND_API void * get_class_impl( PyObject * _obj );
		PYBIND_API class_type_scope * get_class_scope( PyTypeObject * _obj );
		PYBIND_API void * unwrap( PyObject * _obj );
		PYBIND_API void wrap( PyObject * _obj, void * _impl, bool _holder );
		PYBIND_API bool is_wrap( PyObject * _obj );

		PYBIND_API void * check_registred_class( PyObject * _obj, const std::type_info & _info );

		PYBIND_API PyObject * alloc_class( PyTypeObject * _type, PyObject * _args, PyObject * _kwds, void * _impl, bool _holder );

		PYBIND_API void reg_class_type( PyTypeObject * _type );

		PYBIND_API void reg_class_type_scope( const std::type_info & _info, class_type_scope * _scope );
		PYBIND_API class_type_scope * get_class_type_scope( const std::type_info & _info );

		typedef std::vector<class_type_scope *> TVectorTypeScope;
		PYBIND_API void get_types_scope( TVectorTypeScope & _types );
	}

	class class_type_scope
	{
	public:
		class_type_scope( const char * _name, const char * _type_name );
		~class_type_scope();

	public:
		void setup( PyObject * _module, pybind_new _pynew, pybind_destructor _pydestructor );

	public:
		const char * get_name() const;
		const char * get_type() const;

		void set_module( PyObject * _module );

		void * construct( PyObject * _args );
		void def_init( constructor * _ctr );

		void add_method( const char * _name, method_adapter_interface * _ifunc );
		void add_member( const char * _name, member_adapter_interface * _imember );
        void add_base( const char * _name, class_type_scope * _base, pybind_metacast _cast );

		void set_convert( convert_adapter_interface * _iconvert );
		convert_adapter_interface * get_convert();

		void set_repr( repr_adapter_interface * _irepr );
        void set_compare( compare_adapter_interface * _iadapter );
		void set_getattro( method_adapter_interface * _igetattro );
		void set_mapping( method_adapter_interface * _imapping );
		void set_sequence( method_adapter_interface * _isequence );
        

		PyObject * create_holder( void * _impl );
		PyObject * create_impl( void * _impl );
		PyObject * create_pod( void ** _impl );

		void * metacast( const char * name, void * _impl );
		void unwrap( PyObject * _obj );
		void type_initialize( PyTypeObject * _type );

	public:
		typedef std::list<const char *> TVectorMembers;
		TVectorMembers m_members;

		typedef std::list<const char *> TVectorMethods;
		TVectorMethods m_methods;

		typedef std::pair<class_type_scope *, pybind_metacast> TPairMetacast;
		typedef std::map<const char *, TPairMetacast> TMapBases;
		TMapBases m_bases;

		constructor * m_pyconstructor;

		pybind_new m_pynew;
		pybind_destructor m_pydestructor;

		convert_adapter_interface * m_convert;
		repr_adapter_interface * m_repr;
        compare_adapter_interface * m_compare;
		method_adapter_interface * m_getattro;

		method_adapter_interface * m_sequence;
		method_adapter_interface * m_mapping;

		const char * m_name;
		const char * m_type;

		PyTypeObject * m_pytypeobject;

		PyObject * m_module;

#	ifdef PYBIND_VISIT_OBJECTS
    public:
		void incref( PyObject * _obj );
		void decref( PyObject * _obj );

		void visit_objects( pybind_visit_objects * _visitor );

	protected:
		typedef std::list<PyObject *> TListObjects;
		TListObjects m_objects;
#	endif
	};

	void initialize_classes();	
	void finalize_classes();
}

