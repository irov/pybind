#	pragma once

#	include "pybind/class_type_scope_interface.hpp"

#	include "pybind/exports.hpp"
#	include "pybind/types.hpp"
#	include "pybind/exception.hpp"
#	include "pybind/kernel.hpp"
#	include "pybind/system.hpp"

#	include "config/stdex.hpp"

namespace pybind
{
	//////////////////////////////////////////////////////////////////////////
	class python_class_type_scope
		: public class_type_scope_interface
	{
	public:
		python_class_type_scope( kernel_interface * _kernel, const char * _name, uint32_t _typeId, void * _user, const new_adapter_interface_ptr & _pynew, const destroy_adapter_interface_ptr & _pydestructor, uint32_t _pod, bool _hash );
		~python_class_type_scope();

	public:
		void initialize( PyObject * _module ) override;
		void finalize() override;

	public:
		kernel_interface * get_kernel() const override;

	public:
		const char * get_name() const override;
		uint32_t get_type_id() const override;
		uint32_t get_pod_size() const override;
		bool get_pod_hash() const override;

		void * get_user() const override;

		PyTypeObject * get_typemodule() const override;

        void add_method( const method_adapter_interface_ptr & _ifunc ) override;
		method_adapter_interface * get_method( const char * _name ) override;
		void add_member( const member_adapter_interface_ptr & _imember ) override;
		void add_base( uint32_t _info, const class_type_scope_interface_ptr & _scope, pybind_metacast _cast ) override;

		void set_construct( const constructor_adapter_interface_ptr & _iconstruct ) override;
		void set_convert( const convert_adapter_interface_ptr & _iconvert ) override;
		void set_call( const method_adapter_interface_ptr & _icall ) override;
		void set_repr( const repr_adapter_interface_ptr & _irepr ) override;
		void set_hash( const hash_adapter_interface_ptr & _ihash ) override;
		void set_compare( const compare_adapter_interface_ptr &  _iadapter ) override;
		void set_getattro( const getattro_adapter_interface_ptr & _igetattro ) override;
		void set_mapping( const mapping_adapter_interface_ptr & _imapping ) override;
		void set_sequence_get( const sequence_get_adapter_interface_ptr & _isequence ) override;
		void set_sequence_set( const sequence_set_adapter_interface_ptr & _isequence ) override;
		void set_number_add( const number_binary_adapter_interface_ptr & _iadapter ) override;
		void set_number_sub( const number_binary_adapter_interface_ptr & _iadapter ) override;
		void set_number_mul( const number_binary_adapter_interface_ptr & _iadapter ) override;
		void set_number_div( const number_binary_adapter_interface_ptr & _iadapter ) override;
		void add_number_add( uint32_t _typeId, const number_binary_adapter_interface_ptr & _iadapter ) override;
		void add_number_sub( uint32_t _typeId, const number_binary_adapter_interface_ptr & _iadapter ) override;
		void add_number_mul( uint32_t _typeId, const number_binary_adapter_interface_ptr & _iadapter ) override;
		void add_number_div( uint32_t _typeId, const number_binary_adapter_interface_ptr & _iadapter ) override;
		void set_smart_pointer( const smart_pointer_adapter_interface_ptr & _iadapter ) override;
        void set_bindable( const bindable_adapter_interface_ptr & _iadapter ) override;
        
		PyObject * create_class( void * _impl ) override;
		PyObject * create_holder( void * _impl ) override;
		PyObject * create_pod( void ** _impl ) override;

		void * meta_cast( uint32_t _info, void * _impl ) override;
        pybind::bindable * bindable_cast( void * _impl ) override;
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
		inline const mapping_adapter_interface_ptr & get_mapping_adapter() const override;
		inline const sequence_get_adapter_interface_ptr & get_sequence_get_adapter() const override;
		inline const sequence_set_adapter_interface_ptr & get_sequence_set_adapter() const override;

		inline const number_binary_adapter_interface_ptr & get_number_add_adapter() const override;
		inline const number_binary_adapter_interface_ptr & get_number_sub_adapter() const override;
		inline const number_binary_adapter_interface_ptr & get_number_mul_adapter() const override;
		inline const number_binary_adapter_interface_ptr & get_number_div_adapter() const override;

		inline const number_binary_adapter_interface_ptr & get_number_add_adapters( uint32_t _typeId ) const override;
		inline const number_binary_adapter_interface_ptr & get_number_sub_adapters( uint32_t _typeId ) const override;
		inline const number_binary_adapter_interface_ptr & get_number_mul_adapters( uint32_t _typeId ) const override;
		inline const number_binary_adapter_interface_ptr & get_number_div_adapters( uint32_t _typeId ) const override;

		inline const smart_pointer_adapter_interface_ptr & get_smart_pointer() const override;
        inline const bindable_adapter_interface_ptr & get_bindable() const override;

	public:
		void incref_smart_pointer( void * _impl ) override;
		void decref_smart_pointer( void * _impl ) override;

    public:
        void * call_new( PyObject * _obj, PyObject * _args, PyObject * _kwds ) override;
        void call_destructor( PyObject * _obj, void * _impl ) override;
        void clear_bindable(void * _impl) override;
		
    public:
        void addObject( PyObject * _obj ) override;
		void removeObject( PyObject * _obj ) override;

		uint32_t getObjectCount() const override;

    public:
		static void intrusive_ptr_destroy( python_class_type_scope * _ptr );

	public:
		void * operator new ( size_t _size );
		void operator delete ( void * _ptr, size_t _size );

	protected:
		kernel_interface * m_kernel;

        const char * m_name;
        uint32_t m_typeId;

        struct Metacast
        {
            uint32_t info;
			class_type_scope_interface_ptr scope;
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
		mapping_adapter_interface_ptr m_mapping;
		sequence_get_adapter_interface_ptr m_sequence_get;
		sequence_set_adapter_interface_ptr m_sequence_set;

		number_binary_adapter_interface_ptr m_number_add;
		number_binary_adapter_interface_ptr m_number_sub;
		number_binary_adapter_interface_ptr m_number_mul;
		number_binary_adapter_interface_ptr m_number_div;
				
		number_binary_adapter_interface_ptr m_number_adds[PYBIND_TYPE_COUNT];
		number_binary_adapter_interface_ptr m_number_subs[PYBIND_TYPE_COUNT];
		number_binary_adapter_interface_ptr m_number_muls[PYBIND_TYPE_COUNT];
		number_binary_adapter_interface_ptr m_number_divs[PYBIND_TYPE_COUNT];

		smart_pointer_adapter_interface_ptr m_smart_pointer;
        bindable_adapter_interface_ptr m_bindable;

		uint32_t m_objectCount;

		PyTypeObject * m_pytypeobject;

		uint32_t m_pod_size;
		bool m_pod_hash;
        bool m_binable_base;
	};
	//////////////////////////////////////////////////////////////////////////
	inline const new_adapter_interface_ptr & python_class_type_scope::get_new_adapter() const
	{
		return m_new;
	}
	//////////////////////////////////////////////////////////////////////////
	inline const destroy_adapter_interface_ptr & python_class_type_scope::get_destroy_adapter() const
	{
		return m_destructor;
	}
	//////////////////////////////////////////////////////////////////////////
	inline const constructor_adapter_interface_ptr & python_class_type_scope::get_constructor_adapter() const
	{
		return m_constructor;
	}
	//////////////////////////////////////////////////////////////////////////
	inline const convert_adapter_interface_ptr & python_class_type_scope::get_convert_adapter() const
	{
		return m_convert;
	}
	//////////////////////////////////////////////////////////////////////////
	inline const method_adapter_interface_ptr & python_class_type_scope::get_call_adapter() const
	{
		return m_call;
	}
	//////////////////////////////////////////////////////////////////////////
	inline const repr_adapter_interface_ptr & python_class_type_scope::get_repr_adapter() const
	{
		return m_repr;
	}
	//////////////////////////////////////////////////////////////////////////
	inline const hash_adapter_interface_ptr & python_class_type_scope::get_hash_adapter() const
	{
		return m_hash;
	}
	//////////////////////////////////////////////////////////////////////////
	inline const compare_adapter_interface_ptr & python_class_type_scope::get_compare_adapter() const
	{
		return m_compare;
	}
	//////////////////////////////////////////////////////////////////////////
	inline const getattro_adapter_interface_ptr & python_class_type_scope::get_getattro_adapter() const
	{
		return m_getattro;
	}
	//////////////////////////////////////////////////////////////////////////
	inline const mapping_adapter_interface_ptr & python_class_type_scope::get_mapping_adapter() const
	{
		return m_mapping;
	}
	//////////////////////////////////////////////////////////////////////////
	inline const sequence_get_adapter_interface_ptr & python_class_type_scope::get_sequence_get_adapter() const
	{
		return m_sequence_get;
	}
	//////////////////////////////////////////////////////////////////////////
	inline const sequence_set_adapter_interface_ptr & python_class_type_scope::get_sequence_set_adapter() const
	{
		return m_sequence_set;
	}
	//////////////////////////////////////////////////////////////////////////
	inline const number_binary_adapter_interface_ptr & python_class_type_scope::get_number_add_adapter() const
	{
		return m_number_add;
	}
	//////////////////////////////////////////////////////////////////////////
	inline const number_binary_adapter_interface_ptr & python_class_type_scope::get_number_sub_adapter() const
	{
		return m_number_sub;
	}
	//////////////////////////////////////////////////////////////////////////
	inline const number_binary_adapter_interface_ptr & python_class_type_scope::get_number_mul_adapter() const
	{
		return m_number_mul;
	}
	//////////////////////////////////////////////////////////////////////////
	inline const number_binary_adapter_interface_ptr & python_class_type_scope::get_number_div_adapter() const
	{
		return m_number_div;
	}
	//////////////////////////////////////////////////////////////////////////
	inline const number_binary_adapter_interface_ptr & python_class_type_scope::get_number_add_adapters( uint32_t _typeId ) const
	{
		return m_number_adds[_typeId];
	}
	//////////////////////////////////////////////////////////////////////////
	inline const number_binary_adapter_interface_ptr & python_class_type_scope::get_number_sub_adapters( uint32_t _typeId ) const
	{
		return m_number_subs[_typeId];
	}
	//////////////////////////////////////////////////////////////////////////
	inline const number_binary_adapter_interface_ptr & python_class_type_scope::get_number_mul_adapters( uint32_t _typeId ) const
	{
		return m_number_muls[_typeId];
	}
	//////////////////////////////////////////////////////////////////////////
	inline const number_binary_adapter_interface_ptr & python_class_type_scope::get_number_div_adapters( uint32_t _typeId ) const
	{
		return m_number_divs[_typeId];
	}
	//////////////////////////////////////////////////////////////////////////
	inline const smart_pointer_adapter_interface_ptr & python_class_type_scope::get_smart_pointer() const
	{
		return m_smart_pointer;
	}
    //////////////////////////////////////////////////////////////////////////
    inline const bindable_adapter_interface_ptr & python_class_type_scope::get_bindable() const
    {
        return m_bindable;
    }
    //////////////////////////////////////////////////////////////////////////
    typedef stdex::intrusive_ptr<python_class_type_scope> python_class_type_scope_ptr;
}

