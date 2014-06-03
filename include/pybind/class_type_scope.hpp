#	pragma once
 
#	include "pybind/exports.hpp"
#	include "pybind/types.hpp"

#   include "stdex/intrusive_ptr_base.h"
#   include "stdex/intrusive_ptr.h"

#	ifdef PYBIND_VISIT_OBJECTS
#	include <list>
#   endif

#	ifndef PYBIND_TYPE_OBJECT_POOL_COUNT
#	define PYBIND_TYPE_OBJECT_POOL_COUNT 16
#	endif

#	include <vector>

#	include <typeinfo>

namespace pybind
{
	typedef stdex::intrusive_ptr<class class_type_scope> class_type_scope_ptr;
	typedef stdex::intrusive_ptr<class method_type_scope> method_type_scope_ptr;
	typedef stdex::intrusive_ptr<class member_type_scope> member_type_scope_ptr;

	typedef stdex::intrusive_ptr<class repr_adapter_interface> repr_adapter_interface_ptr;
    typedef stdex::intrusive_ptr<class method_adapter_interface> method_adapter_interface_ptr;
    typedef stdex::intrusive_ptr<class member_adapter_interface> member_adapter_interface_ptr;
    typedef stdex::intrusive_ptr<class convert_adapter_interface> convert_adapter_interface_ptr;
    typedef stdex::intrusive_ptr<class hash_adapter_interface> hash_adapter_interface_ptr;
    typedef stdex::intrusive_ptr<class compare_adapter_interface> compare_adapter_interface_ptr;
	
    typedef stdex::intrusive_ptr<class constructor_adapter_interface> constructor_adapter_interface_ptr;

	namespace detail
	{
        PYBIND_API bool is_class( PyObject * _obj );
        PYBIND_API void * get_class_impl( PyObject * _obj );

        PYBIND_API bool reg_class_type_scope( size_t _info, const class_type_scope_ptr & _scope );
        PYBIND_API const class_type_scope_ptr & get_class_type_scope( size_t _info );

        PYBIND_API void visit_types_scope( visitor_class_type_scope * _getter );

        PYBIND_API void * get_scope_user( const class_type_scope_ptr & _scope );

		PYBIND_API const class_type_scope_ptr & get_class_scope( PyTypeObject * _obj );
		PYBIND_API void * unwrap( PyObject * _obj );
		PYBIND_API void wrap( PyObject * _obj, void * _impl, bool _holder );
		PYBIND_API bool is_wrap( PyObject * _obj );

		PYBIND_API void * check_registred_class( PyObject * _obj, size_t _info );

		PYBIND_API PyObject * alloc_class( PyTypeObject * _type, PyObject * _args, PyObject * _kwds, void * _impl, bool _holder );

        PYBIND_API void * meta_cast_scope( void * _self, size_t _scope_name, size_t _class_name, const class_type_scope_ptr & scope );
	}

	class class_type_scope
        : public stdex::intrusive_ptr_base
	{
	public:
		class_type_scope( const char * _name, size_t _typeId, PyObject * _module, void * _user, pybind_new _pynew, pybind_destructor _pydestructor, bool _pod );
		~class_type_scope();

	public:
		void initialize();
        void finalize();

	public:
		void finalize_pool();

	public:
		const char * get_name() const;
		size_t get_type_id() const;
		bool is_pod() const;

        void * get_user() const;

        PyTypeObject * get_typemodule() const;

		void set_module( PyObject * _module );

		void * construct( PyObject * _obj, PyObject * _args );
		void def_init( const constructor_adapter_interface_ptr & _ctr );

		void add_method( const char * _name, const method_adapter_interface_ptr & _ifunc );
		void add_member( const char * _name, const member_adapter_interface_ptr & _imember );
        void add_base( size_t _name, const class_type_scope_ptr & _base, pybind_metacast _cast );

		void set_convert( const convert_adapter_interface_ptr & _iconvert );
		const convert_adapter_interface_ptr & get_convert();

        void set_call( const method_adapter_interface_ptr & _icall );
		void set_repr( const repr_adapter_interface_ptr & _irepr );
        void set_hash( const hash_adapter_interface_ptr & _ihash );
        void set_compare( const compare_adapter_interface_ptr &  _iadapter );
		void set_getattro( const method_adapter_interface_ptr & _igetattro );
		void set_mapping( const method_adapter_interface_ptr & _imapping );
		void set_sequence( const method_adapter_interface_ptr & _isequence );
        

		PyObject * create_holder( void * _impl );
		PyObject * create_pod( void ** _impl, size_t & _size );

		void * metacast( size_t name, void * _impl );
		void unwrap( PyObject * _obj );
		void type_initialize( PyTypeObject * _type );

	public:
		bool poolObject( PyObject * _obj );
		PyObject * unpoolObject();

    public:
        void addObject( PyObject * _obj );
        void removeObject( PyObject * _obj );

        size_t getObjectCount() const;

    protected:
        void destroy() override;

	public:
        const char * m_name;
        size_t m_typeId;

        struct Metacast
        {
            bool setup;
		    class_type_scope_ptr scope;
            pybind_metacast cast;
        };

		typedef std::vector<Metacast> TBases;
		TBases m_bases;
        
        void * m_user;
		pybind_new m_pynew;
		pybind_destructor m_pydestructor;

        constructor_adapter_interface_ptr m_pyconstructor;

		convert_adapter_interface_ptr m_convert;

        method_adapter_interface_ptr m_call;
		repr_adapter_interface_ptr m_repr;
        hash_adapter_interface_ptr m_hash;
        compare_adapter_interface_ptr m_compare;
		method_adapter_interface_ptr m_getattro;

		method_adapter_interface_ptr m_sequence;
		method_adapter_interface_ptr m_mapping;

		PyTypeObject * m_pytypeobject;

		PyObject * m_module;

        bool m_pod;

        size_t m_objectCount;

		
		PyObject * m_poolObjects[PYBIND_TYPE_OBJECT_POOL_COUNT];
		size_t m_poolCount;

#	ifdef PYBIND_VISIT_OBJECTS
    public:
		void visit_objects( pybind_visit_objects * _visitor );

	protected:
		typedef std::vector<PyObject *> TVectorObjects;
		TVectorObjects m_objects;
#	endif
	};
    //////////////////////////////////////////////////////////////////////////
    typedef stdex::intrusive_ptr<class_type_scope> class_type_scope_ptr;

	bool initialize_classes();	
	void finalize_classes();
	void finalize_classes_pool();
}
