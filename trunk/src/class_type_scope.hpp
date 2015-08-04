#	pragma once
 
#	include "pybind/class_type_scope_interface.hpp"

namespace pybind
{
	//////////////////////////////////////////////////////////////////////////
	typedef stdex::intrusive_ptr<class class_type_scope> class_type_scope_ptr;
	//////////////////////////////////////////////////////////////////////////
	class class_type_scope
		: public class_type_scope_interface
	{
	public:
		class_type_scope( const char * _name, uint32_t _typeId, void * _user, const new_adapter_interface_ptr & _pynew, const destroy_adapter_interface_ptr & _pydestructor, uint32_t _pod );
		~class_type_scope();

	public:
		void initialize( PyObject * _module ) override;
		void finalize() override;

	public:
		const char * get_name() const override;
		uint32_t get_type_id() const override;
		uint32_t is_pod() const override;

		void * get_user() const override;

		PyTypeObject * get_typemodule() const override;

		void * construct( PyObject * _obj, PyObject * _args ) override;
		
		void add_method( const method_adapter_interface_ptr & _ifunc ) override;
		void add_member( const member_adapter_interface_ptr & _imember ) override;
		void add_base( uint32_t _info, const class_type_scope_interface_ptr & _scope, pybind_metacast _cast ) override;

		void set_construct( const constructor_adapter_interface_ptr & _iconstruct ) override;
		void set_convert( const convert_adapter_interface_ptr & _iconvert ) override;
		void set_call( const method_adapter_interface_ptr & _icall ) override;
		void set_repr( const repr_adapter_interface_ptr & _irepr ) override;
		void set_hash( const hash_adapter_interface_ptr & _ihash ) override;
		void set_compare( const compare_adapter_interface_ptr &  _iadapter ) override;
		void set_getattro( const getattro_adapter_interface_ptr & _igetattro ) override;
		void set_mapping( const mapping_adapter_interface_ptr & _imapping );
		void set_sequence_get( const sequence_get_adapter_interface_ptr & _isequence ) override;
		void set_sequence_set( const sequence_set_adapter_interface_ptr & _isequence ) override;
        

		PyObject * create_class( void * _impl ) override;
		PyObject * create_holder( void * _impl ) override;
		PyObject * create_pod( void ** _impl, size_t _pod ) override;

		void * metacast( uint32_t _info, void * _impl ) override;
		void type_initialize( PyTypeObject * _type ) override;

		bool is_instance( PyTypeObject * _type ) const override;

	public:
		inline const new_adapter_interface_ptr & get_new_adapter() const override;
		inline const destroy_adapter_interface_ptr & get_destroy_adapter() const override;
		inline const constructor_adapter_interface_ptr & get_constructor_adapter() const override;
		inline const convert_adapter_interface_ptr & get_convert_adapter() const override;
		inline const method_adapter_interface_ptr & get_call_adapter() const override;
		inline const repr_adapter_interface_ptr & get_repr_adapter() const override;
		inline const hash_adapter_interface_ptr & get_hash_adapter() const override;
		inline const compare_adapter_interface_ptr & get_compare_adapter() const override;
		inline const getattro_adapter_interface_ptr & get_getattro_adapter() const override;
		inline const sequence_get_adapter_interface_ptr & get_sequence_get_adapter() const override;
		inline const sequence_set_adapter_interface_ptr & get_sequence_set_adapter() const override;
		inline const mapping_adapter_interface_ptr & get_mapping_adapter() const override;
		
    public:
        void addObject( PyObject * _obj ) override;
		void removeObject( PyObject * _obj ) override;

		uint32_t getObjectCount() const override;

    public:
		static void intrusive_ptr_destroy( class_type_scope * _ptr );

	public:
		void * operator new ( size_t _size );
		void operator delete ( void * _ptr, size_t _size );

	protected:
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
		new_adapter_interface_ptr m_new;
		destroy_adapter_interface_ptr m_destructor;

        constructor_adapter_interface_ptr m_constructor;

		convert_adapter_interface_ptr m_convert;

        method_adapter_interface_ptr m_call;
		repr_adapter_interface_ptr m_repr;
        hash_adapter_interface_ptr m_hash;
        compare_adapter_interface_ptr m_compare;
		getattro_adapter_interface_ptr m_getattro;
		sequence_get_adapter_interface_ptr m_sequence_get;
		sequence_set_adapter_interface_ptr m_sequence_set;
		mapping_adapter_interface_ptr m_mapping;

		PyTypeObject * m_pytypeobject;

		uint32_t m_pod;

        uint32_t m_objectCount;
	};
	//////////////////////////////////////////////////////////////////////////
	inline const new_adapter_interface_ptr & class_type_scope::get_new_adapter() const
	{
		return m_new;
	}
	//////////////////////////////////////////////////////////////////////////
	inline const destroy_adapter_interface_ptr & class_type_scope::get_destroy_adapter() const
	{
		return m_destructor;
	}
	//////////////////////////////////////////////////////////////////////////
	inline const constructor_adapter_interface_ptr & class_type_scope::get_constructor_adapter() const
	{
		return m_constructor;
	}
	//////////////////////////////////////////////////////////////////////////
	inline const convert_adapter_interface_ptr & class_type_scope::get_convert_adapter() const
	{
		return m_convert;
	}
	//////////////////////////////////////////////////////////////////////////
	inline const method_adapter_interface_ptr & class_type_scope::get_call_adapter() const
	{
		return m_call;
	}
	//////////////////////////////////////////////////////////////////////////
	inline const repr_adapter_interface_ptr & class_type_scope::get_repr_adapter() const
	{
		return m_repr;
	}
	//////////////////////////////////////////////////////////////////////////
	inline const hash_adapter_interface_ptr & class_type_scope::get_hash_adapter() const
	{
		return m_hash;
	}
	//////////////////////////////////////////////////////////////////////////
	inline const compare_adapter_interface_ptr & class_type_scope::get_compare_adapter() const
	{
		return m_compare;
	}
	//////////////////////////////////////////////////////////////////////////
	inline const getattro_adapter_interface_ptr & class_type_scope::get_getattro_adapter() const
	{
		return m_getattro;
	}
	//////////////////////////////////////////////////////////////////////////
	inline const sequence_get_adapter_interface_ptr & class_type_scope::get_sequence_get_adapter() const
	{
		return m_sequence_get;
	}
	//////////////////////////////////////////////////////////////////////////
	inline const sequence_set_adapter_interface_ptr & class_type_scope::get_sequence_set_adapter() const
	{
		return m_sequence_set;
	}
	//////////////////////////////////////////////////////////////////////////
	inline const mapping_adapter_interface_ptr & class_type_scope::get_mapping_adapter() const
	{
		return m_mapping;
	}
    //////////////////////////////////////////////////////////////////////////
    typedef stdex::intrusive_ptr<class_type_scope> class_type_scope_ptr;
}

