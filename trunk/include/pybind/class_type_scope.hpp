#	pragma once

#	include "pybind/exports.hpp"
#	include "pybind/types.hpp"
#	include "pybind/exception.hpp"
#	include "pybind/detail.hpp"

#	include "config/stdex.hpp"

namespace pybind
{
	//////////////////////////////////////////////////////////////////////////
	typedef stdex::intrusive_ptr<class class_type_scope> class_type_scope_ptr;
	//////////////////////////////////////////////////////////////////////////
	typedef stdex::intrusive_ptr<class method_type_scope> method_type_scope_ptr;
	typedef stdex::intrusive_ptr<class member_type_scope> member_type_scope_ptr;

	typedef stdex::intrusive_ptr<class new_adapter_interface> new_adapter_interface_ptr;
	typedef stdex::intrusive_ptr<class destroy_adapter_interface> destroy_adapter_interface_ptr;
	typedef stdex::intrusive_ptr<class repr_adapter_interface> repr_adapter_interface_ptr;
	typedef stdex::intrusive_ptr<class method_adapter_interface> method_adapter_interface_ptr;
	typedef stdex::intrusive_ptr<class member_adapter_interface> member_adapter_interface_ptr;
	typedef stdex::intrusive_ptr<class convert_adapter_interface> convert_adapter_interface_ptr;
	typedef stdex::intrusive_ptr<class hash_adapter_interface> hash_adapter_interface_ptr;
	typedef stdex::intrusive_ptr<class compare_adapter_interface> compare_adapter_interface_ptr;
	typedef stdex::intrusive_ptr<class constructor_adapter_interface> constructor_adapter_interface_ptr;

	typedef stdex::intrusive_ptr<class getattro_adapter_interface> getattro_adapter_interface_ptr;
	typedef stdex::intrusive_ptr<class mapping_adapter_interface> mapping_adapter_interface_ptr;
	typedef stdex::intrusive_ptr<class sequence_get_adapter_interface> sequence_get_adapter_interface_ptr;
	typedef stdex::intrusive_ptr<class sequence_set_adapter_interface> sequence_set_adapter_interface_ptr;
	//////////////////////////////////////////////////////////////////////////
	class PYBIND_API class_type_scope
		: public stdex::intrusive_ptr_base<class_type_scope>
	{
	public:
		class_type_scope( const char * _name, uint32_t _typeId, void * _user, const new_adapter_interface_ptr & _pynew, const destroy_adapter_interface_ptr & _pydestructor, uint32_t _pod );
		~class_type_scope();

	public:
		void initialize( PyObject * _module );
		void finalize();

	public:
		const char * get_name() const;
		uint32_t get_type_id() const;
		uint32_t is_pod() const;

		void * get_user() const;

		template<class T>
		T * get_user_t() const
		{
			void * ptr = this->get_user();

			return static_cast<T *>(ptr);
		}

		PyTypeObject * get_typemodule() const;

		void * construct( PyObject * _obj, PyObject * _args );
		
		void add_method( const method_adapter_interface_ptr & _ifunc );
		void add_member( const member_adapter_interface_ptr & _imember );
		void add_base( uint32_t _info, const class_type_scope_ptr & _scope, pybind_metacast _cast );

		template<class B>
		void add_base_t( pybind_metacast _cast )
		{
			uint32_t tptrinfo = detail::class_info<B*>();
			uint32_t tinfo = detail::class_info<B>();

			if( detail::has_class_type_scope( tinfo ) == false )
			{
				pybind::throw_exception( "class_type_scope_interface::add_base_t %s not bind base type %s"
					, this->get_name()
					, detail::get_class_type_info( tinfo )
					);

				return;
			}

			const class_type_scope_ptr & basescope = detail::get_class_type_scope( tinfo );

			this->add_base( tptrinfo, basescope, _cast );
		}

		void set_construct( const constructor_adapter_interface_ptr & _iconstruct );
		void set_convert( const convert_adapter_interface_ptr & _iconvert );
		void set_call( const method_adapter_interface_ptr & _icall );
		void set_repr( const repr_adapter_interface_ptr & _irepr );
		void set_hash( const hash_adapter_interface_ptr & _ihash );
		void set_compare( const compare_adapter_interface_ptr &  _iadapter );
		void set_getattro( const getattro_adapter_interface_ptr & _igetattro );
		void set_mapping( const mapping_adapter_interface_ptr & _imapping );
		void set_sequence_get( const sequence_get_adapter_interface_ptr & _isequence );
		void set_sequence_set( const sequence_set_adapter_interface_ptr & _isequence );
        

		PyObject * create_class( void * _impl );
		PyObject * create_holder( void * _impl );
		PyObject * create_pod( void ** _impl, size_t _pod );

		void * metacast( uint32_t _info, void * _impl );
		void type_initialize( PyTypeObject * _type );

		bool is_instance( PyTypeObject * _type ) const;

	public:
		inline const new_adapter_interface_ptr & get_new_adapter() const;
		inline const destroy_adapter_interface_ptr & get_destroy_adapter() const;
		inline const constructor_adapter_interface_ptr & get_constructor_adapter() const;
		inline const convert_adapter_interface_ptr & get_convert_adapter() const;
		inline const method_adapter_interface_ptr & get_call_adapter() const;
		inline const repr_adapter_interface_ptr & get_repr_adapter() const;
		inline const hash_adapter_interface_ptr & get_hash_adapter() const;
		inline const compare_adapter_interface_ptr & get_compare_adapter() const;
		inline const getattro_adapter_interface_ptr & get_getattro_adapter() const;
		inline const sequence_get_adapter_interface_ptr & get_sequence_get_adapter() const;
		inline const sequence_set_adapter_interface_ptr & get_sequence_set_adapter() const;
		inline const mapping_adapter_interface_ptr & get_mapping_adapter() const;
		
    public:
        void addObject( PyObject * _obj );
		void removeObject( PyObject * _obj );

		uint32_t getObjectCount() const;

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

