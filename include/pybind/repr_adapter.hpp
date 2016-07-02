#	pragma once

#   include "pybind/adapter_interface.hpp"

#   include "pybind/extract.hpp"
#   include "pybind/helper.hpp"

namespace pybind
{
	//////////////////////////////////////////////////////////////////////////
	typedef stdex::intrusive_ptr<class class_type_scope> class_type_scope_ptr;
	//////////////////////////////////////////////////////////////////////////
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
	public:
		repr_adapter( F _repr )
			: m_repr(_repr)
		{
		}

	public:
		PyObject * repr( void * _self, const class_type_scope_ptr & _scope ) override
		{
			kernel_interface * kernel = pybind::get_kernel();

			C * c = kernel->meta_cast_class_t<C>( _self, _scope );

			PyObject * py_result = (detail::return_operator_t)(*m_repr)(c);

			return py_result;
		}

	protected:
		F m_repr;
	};
}

