#	pragma once
 
#	include "pybind/exports.hpp"
#	include "pybind/types.hpp"

#	include "config/stdex.hpp"

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

        PYBIND_API bool reg_class_type_scope( uint32_t _info, const class_type_scope_ptr & _scope );
		PYBIND_API bool has_class_type_scope( uint32_t _info );
        PYBIND_API const class_type_scope_ptr & get_class_type_scope( uint32_t _info );

        PYBIND_API void visit_types_scope( visitor_class_type_scope * _getter );

        PYBIND_API void * get_scope_user( const class_type_scope_ptr & _scope );

		PYBIND_API const class_type_scope_ptr & get_class_scope( PyTypeObject * _obj );
		PYBIND_API void * unwrap( PyObject * _obj );
		PYBIND_API bool is_wrap( PyObject * _obj );

		PYBIND_API void * check_registred_class( PyObject * _obj, uint32_t _info );

		//PYBIND_API PyObject * alloc_class( PyTypeObject * _type, PyObject * _args, PyObject * _kwds, void * _impl, bool _holder );

        PYBIND_API void * meta_cast_scope( void * _self, uint32_t _scope_name, uint32_t _class_name, const class_type_scope_ptr & _scope );

		template<class T>
		inline T * meta_cast_scope_t( void * _self, uint32_t _scope_name, uint32_t _class_name, const class_type_scope_ptr & _scope )
		{
			void * impl = meta_cast_scope( _self, _scope_name, _class_name, _scope );

			T * obj = static_cast<T *>(impl);

			return obj;
		}
	}

	class class_type_scope
        : public stdex::intrusive_ptr_base<class_type_scope>
	{
	public:
		class_type_scope( const char * _name, uint32_t _typeId, void * _user, pybind_new _pynew, pybind_destructor _pydestructor, uint32_t _pod );
		~class_type_scope();

	public:
		void initialize( PyObject * _module );
        void finalize();

	public:
		const char * get_name() const;
		uint32_t get_type_id() const;
		size_t is_pod() const;

        void * get_user() const;

        PyTypeObject * get_typemodule() const;

		void * construct( PyObject * _obj, PyObject * _args );
		void def_init( const constructor_adapter_interface_ptr & _ctr );

		void add_method( const method_adapter_interface_ptr & _ifunc );
		void add_member( const member_adapter_interface_ptr & _imember );
        void add_base( uint32_t _info, const class_type_scope_ptr & _base, pybind_metacast _cast );

		void set_convert( const convert_adapter_interface_ptr & _iconvert );
		const convert_adapter_interface_ptr & get_convert();

        void set_call( const method_adapter_interface_ptr & _icall );
		void set_repr( const repr_adapter_interface_ptr & _irepr );
        void set_hash( const hash_adapter_interface_ptr & _ihash );
        void set_compare( const compare_adapter_interface_ptr &  _iadapter );
		void set_getattro( const method_adapter_interface_ptr & _igetattro );
		void set_mapping( const method_adapter_interface_ptr & _imapping );
		void set_sequence_get( const method_adapter_interface_ptr & _isequence );
		void set_sequence_set( const method_adapter_interface_ptr & _isequence );
        

		PyObject * create_class( void * _impl );
		PyObject * create_holder( void * _impl );
		PyObject * create_pod( void ** _impl, size_t _pod );

		void * metacast( uint32_t _info, void * _impl );
		void unwrap( PyObject * _obj );
		void type_initialize( PyTypeObject * _type );

    public:
        void addObject( PyObject * _obj );
        void removeObject( PyObject * _obj );

        uint32_t getObjectCount() const;

    public:
        void intrusive_ptr_destroy();

	public:
		void * operator new ( size_t _size );
		void operator delete ( void * _ptr, size_t _size );

	public:
        const char * m_name;
        uint32_t m_typeId;

        struct Metacast
        {
            uint32_t info;
		    class_type_scope_ptr scope;
            pybind_metacast cast;
        };

		Metacast m_bases[PYBIND_BASES_COUNT];
		uint32_t m_basesCount;
        
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

		method_adapter_interface_ptr m_sequence_get;
		method_adapter_interface_ptr m_sequence_set;
		method_adapter_interface_ptr m_mapping;

		PyTypeObject * m_pytypeobject;

		uint32_t m_pod;

        uint32_t m_objectCount;
	};
    //////////////////////////////////////////////////////////////////////////
    typedef stdex::intrusive_ptr<class_type_scope> class_type_scope_ptr;
	//////////////////////////////////////////////////////////////////////////
	bool initialize_classes();	
	void finalize_classes();
}

