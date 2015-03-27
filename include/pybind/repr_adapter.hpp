#	pragma once

#   include "pybind/adapter_interface.hpp"

#	include "pybind/function_parser.hpp"

#   include "pybind/class_info.hpp"
#   include "pybind/class_type_scope.hpp"

#   include "pybind/extract.hpp"

namespace pybind
{
	class repr_adapter_interface
		: public adapter_interface
    {
	public:
		virtual PyObject * repr( void * _self, const class_type_scope_ptr & _scope ) = 0;
	};
    //////////////////////////////////////////////////////////////////////////
    typedef stdex::intrusive_ptr<repr_adapter_interface> repr_adapter_interface_ptr;
	//////////////////////////////////////////////////////////////////////////
	template<class C, class F>
	class repr_adapter
		: public repr_adapter_interface
	{
		typedef typename function_parser<F>::result f_info;

	public:
		repr_adapter( F _repr )
			: m_repr(_repr)
		{
            m_class_name = class_info<C*>();
            m_scope_name = class_info<C>();
		}

	public:
		PyObject * repr( void * _self, const class_type_scope_ptr & _scope ) override
		{
			C * c = detail::meta_cast_scope_t<C>( _self, m_scope_name, m_class_name, _scope );

			typename f_info::ret_type result = (*m_repr)( c );

			PyObject * py_result = pybind::ptr( result );

			return py_result;
		}

	protected:
		F m_repr;

		uint32_t m_class_name;
		uint32_t m_scope_name;
	};
}

