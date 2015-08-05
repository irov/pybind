#	pragma once

#   include "pybind/adapter_interface.hpp"

namespace pybind
{
	//////////////////////////////////////////////////////////////////////////
	typedef stdex::intrusive_ptr<class class_type_scope> class_type_scope_ptr;
	//////////////////////////////////////////////////////////////////////////
	class hash_adapter_interface
		: public adapter_interface
    {
	public:
		virtual long hash( void * _self, const class_type_scope_ptr & _scope ) = 0;
	};
    //////////////////////////////////////////////////////////////////////////
    typedef stdex::intrusive_ptr<hash_adapter_interface> hash_adapter_interface_ptr;
	//////////////////////////////////////////////////////////////////////////
	template<class C, class F>
	class hash_adapter
		: public hash_adapter_interface
	{
	public:
		hash_adapter( F _hash )
			: m_hash(_hash)
		{
		}

	public:
		long hash( void * _self, const class_type_scope_ptr & _scope ) override
		{
			uint32_t class_name = detail::class_info<C*>();
			uint32_t scope_name = detail::class_info<C>();

			C * inst = static_cast<C*>( detail::meta_cast_scope( _self, scope_name, class_name, _scope ) );

			long hash = (*m_hash)( inst );

			return hash;
		}

	protected:
		F m_hash;
	};
}

