#	pragma once

#   include "pybind/adapter_interface.hpp"


namespace pybind
{
	class hash_adapter_interface
		: public adapter_interface
    {
	public:
		virtual uint32_t hash( PyObject * _obj, void * _self, const class_type_scope_ptr & _scope ) = 0;
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
            m_class_name = class_info<C*>();
            m_scope_name = class_info<C>();
		}

	public:
		uint32_t hash( PyObject * _obj, void * _self, const class_type_scope_ptr & _scope ) override
		{
			C * inst = static_cast<C*>( detail::meta_cast_scope( _self, m_scope_name, m_class_name, _scope ) );

			uint32_t hash = (*m_hash)( _obj, inst );

			return hash;
		}

	protected:
		F m_hash;

		size_t m_class_name;
		size_t m_scope_name;
	};
}

