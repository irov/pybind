#	pragma once

#	include "pybind/adapter_interface.hpp"

namespace pybind
{
	//////////////////////////////////////////////////////////////////////////
	class class_new_interface
		: public adapter_interface
	{
	public:
		virtual void * call( const pybind::class_type_scope_ptr & _scope, PyObject * _obj, PyObject * _args, PyObject * _kwds ) = 0;
	};
	//////////////////////////////////////////////////////////////////////////
	typedef stdex::intrusive_ptr<class_new_interface> class_new_interface_ptr;
	//////////////////////////////////////////////////////////////////////////
	class class_destroy_interface
		: public adapter_interface
	{
	public:
		virtual void call( const class_type_scope_ptr & _scope, void * impl ) = 0;
	};
	//////////////////////////////////////////////////////////////////////////
	typedef stdex::intrusive_ptr<class_destroy_interface> class_destroy_interface_ptr;
}
