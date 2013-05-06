#	pragma once

#	include "pybind/class_core.hpp"

namespace pybind
{
	class hash_adapter_interface
		: public adapter_interface
    {
	public:
		virtual size_t hash( PyObject * _obj, void * _self, class_type_scope * _scope ) = 0;
	};
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
		size_t hash( PyObject * _obj, void * _self, class_type_scope * _scope ) override
		{
			C * inst = static_cast<C*>( detail::meta_cast_scope( _self, m_scope_name, m_class_name, _scope ) );

			size_t hash = (*m_hash)( _obj, inst );

			return hash;
		}

	protected:
		F m_hash;

		size_t m_class_name;
		size_t m_scope_name;
	};
}

