#	pragma once

#   include "pybind/adapter_interface.hpp"

namespace pybind
{
	//////////////////////////////////////////////////////////////////////////
	typedef stdex::intrusive_ptr<class class_type_scope_interface> class_type_scope_interface_ptr;
	//////////////////////////////////////////////////////////////////////////
	class hash_adapter_interface
		: public adapter_interface
    {
	public:
		virtual int64_t hash( kernel_interface * _kernel, void * _self, const class_type_scope_interface_ptr & _scope ) = 0;
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
		int64_t hash( kernel_interface * _kernel, void * _self, const class_type_scope_interface_ptr & _scope ) override
		{			
			uint32_t class_name = _kernel->class_info<C*>();
			uint32_t scope_name = _kernel->class_info<C>();

			void * impl = _kernel->meta_cast_scope( _self, scope_name, class_name, _scope );

			C * inst = static_cast<C*>(impl);

			int64_t hash = (*m_hash)(inst);

			return hash;
		}

	protected:
		F m_hash;
	};
}

