#	pragma once

#   include "pybind/adapter_interface.hpp"

#   include "pybind/class_info.hpp"
#   include "pybind/class_type_scope.hpp"

namespace pybind
{
	class repr_adapter_interface
		: public adapter_interface
    {
	public:
		virtual PyObject * repr( PyObject * _obj, void * _self, const class_type_scope_ptr & _scope ) = 0;
	};
    //////////////////////////////////////////////////////////////////////////
    typedef stdex::intrusive_ptr<repr_adapter_interface> repr_adapter_interface_ptr;
	//////////////////////////////////////////////////////////////////////////
	template<class C, class F>
	class repr_adapter
		: public repr_adapter_interface
	{
	public:
		repr_adapter( F _repr )
			: m_repr(_repr)
		{
            m_class_name = class_info<C*>();
            m_scope_name = class_info<C>();
		}

	public:
		PyObject * repr( PyObject * _obj, void * _self, const class_type_scope_ptr & _scope ) override
		{
			C * inst = static_cast<C*>( detail::meta_cast_scope( _self, m_scope_name, m_class_name, _scope ) );

			PyObject * py_repr = (*m_repr)( _obj, inst );

			return py_repr;
		}

	protected:
		F m_repr;

		uint32_t m_class_name;
		uint32_t m_scope_name;
	};
}

