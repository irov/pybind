#	pragma once

#	include "pybind/adapter_interface.hpp"

namespace pybind
{
	//////////////////////////////////////////////////////////////////////////
	class new_adapter_interface
		: public adapter_interface
	{
	public:
		virtual void * call( pybind::kernel_interface * _kernel, const pybind::class_type_scope_ptr & _scope, PyObject * _obj, PyObject * _args, PyObject * _kwds ) = 0;
	};
	//////////////////////////////////////////////////////////////////////////
	typedef stdex::intrusive_ptr<new_adapter_interface> new_adapter_interface_ptr;
}
