#	pragma once

#   include "pybind/adapter_interface.hpp"

#	include "pybind/function_parser.hpp"

#   include "pybind/class_type_scope_interface.hpp"

#   include "pybind/extract.hpp"
#   include "pybind/detail.hpp"

namespace pybind
{
	class repr_adapter_interface
		: public adapter_interface
    {
	public:
		virtual PyObject * repr( kernel_interface * _kernel, void * _self, const class_type_scope_interface_ptr & _scope ) = 0;
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
		}

	public:
		PyObject * repr( kernel_interface * _kernel, void * _self, const class_type_scope_interface_ptr & _scope ) override
		{
			uint32_t class_name = _kernel->class_info<C*>();
			uint32_t scope_name = _kernel->class_info<C>();

			C * c = detail::meta_cast_scope_t<C *>( _self, scope_name, class_name, _scope );

			typename f_info::ret_type result = (*m_repr)( _kernel, c );

			PyObject * py_result = pybind::ptr( result );

			return py_result;
		}

	protected:
		F m_repr;
	};
}

