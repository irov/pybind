#	pragma once

#	include "pybind/adapter_interface.hpp"

namespace pybind
{
	//////////////////////////////////////////////////////////////////////////
	class destroy_adapter_interface
		: public adapter_interface
	{
	public:
		virtual void call( const class_type_scope_ptr & _scope, void * impl ) = 0;
	};
	//////////////////////////////////////////////////////////////////////////
	typedef stdex::intrusive_ptr<destroy_adapter_interface> destroy_adapter_interface_ptr;
}
