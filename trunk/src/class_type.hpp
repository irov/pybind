#	pragma once
 
#	include "pybind/exports.hpp"
#	include "pybind/types.hpp"

#	include <list>
#	include <vector>

#	include <typeinfo>

namespace pybind
{
	class class_type_scope;
	class method_type_scope;
	class member_type_scope;

    class call_adapter_interface;
    class repr_adapter_interface;
	class method_adapter_interface;
	class member_adapter_interface;
	class convert_adapter_interface;
    class hash_adapter_interface;
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

		PYBIND_API void * check_registred_class( PyObject * _obj, size_t _info );

		PYBIND_API PyObject * alloc_class( PyTypeObject * _type, PyObject * _args, PyObject * _kwds, void * _impl, bool _holder );
	}

	class class_type_scope
	{
	public:
		class_type_scope( const char * _name, size_t _type_name, PyObject * _module, void * _user, pybind_new _pynew, pybind_destructor _pydestructor, bool _pod );
		~class_type_scope();

	public:
		void initialize();
        void finalize();

	public:
		const char * get_name() const;
		size_t get_type() const;

        void * get_user() const;

        PyTypeObject * get_typemodule() const;

		void set_module( PyObject * _module );

		void * construct( PyObject * _obj, PyObject * _args );
		void def_init( constructor * _ctr );

		void add_method( const char * _name, method_adapter_interface * _ifunc );
		void add_member( const char * _name, member_adapter_interface * _imember );
        void add_base( size_t _name, class_type_scope * _base, pybind_metacast _cast );

		void set_convert( convert_adapter_interface * _iconvert );
		convert_adapter_interface * get_convert();

        void set_call( method_adapter_interface * _icall );
		void set_repr( repr_adapter_interface * _irepr );
        void set_hash( hash_adapter_interface * _ihash );
        void set_compare( compare_adapter_interface * _iadapter );
		void set_getattro( method_adapter_interface * _igetattro );
		void set_mapping( method_adapter_interface * _imapping );
		void set_sequence( method_adapter_interface * _isequence );
        

		PyObject * create_holder( void * _impl );
		PyObject * create_impl( void * _impl );
		PyObject * create_pod( void ** _impl );

		void * metacast( size_t name, void * _impl );
		void unwrap( PyObject * _obj );
		void type_initialize( PyTypeObject * _type );

    public:
        void incref();
        bool decref();

	public:
        const char * m_name;
        size_t m_type;

        size_t m_refcount;

		typedef std::list<const char *> TVectorMembers;
		TVectorMembers m_members;

		typedef std::list<const char *> TVectorMethods;
		TVectorMethods m_methods;

        struct Metacast
        {
            bool setup;
		    class_type_scope * scope;
            pybind_metacast cast;
        };

		typedef std::vector<Metacast> TBases;
		TBases m_bases;

		constructor * m_pyconstructor;

        void * m_user;
		pybind_new m_pynew;
		pybind_destructor m_pydestructor;

		convert_adapter_interface * m_convert;

        method_adapter_interface * m_call;
		repr_adapter_interface * m_repr;
        hash_adapter_interface * m_hash;
        compare_adapter_interface * m_compare;
		method_adapter_interface * m_getattro;

		method_adapter_interface * m_sequence;
		method_adapter_interface * m_mapping;

		PyTypeObject * m_pytypeobject;

		PyObject * m_module;

        bool m_pod;

#	ifdef PYBIND_VISIT_OBJECTS
    public:
		void addObject( PyObject * _obj );
		void removeObject( PyObject * _obj );

		void visit_objects( pybind_visit_objects * _visitor );

	protected:
		typedef std::list<PyObject *> TListObjects;
		TListObjects m_objects;
#	endif
	};

	bool initialize_classes();	
	void finalize_classes();
}

