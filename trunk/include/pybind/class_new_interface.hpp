#	pragma once

#	include "pybind/types.hpp"

#	include "config/stdex.hpp"

namespace pybind
{
	//////////////////////////////////////////////////////////////////////////
	class class_new_interface
		: public stdex::intrusive_ptr_base < class_new_interface >
	{
	public:
		virtual void * call( const pybind::class_type_scope_ptr & _scope, PyObject * _obj, PyObject * _args, PyObject * _kwds ) = 0;
	};
	//////////////////////////////////////////////////////////////////////////
	typedef stdex::intrusive_ptr<class_new_interface> class_new_interface_ptr;
	//////////////////////////////////////////////////////////////////////////
	class class_destructor_interface
		: public stdex::intrusive_ptr_base < class_destructor_interface >
	{
	public:
		virtual void * call( const class_type_scope_ptr & _scope, void * impl ) = 0;
	};
	//////////////////////////////////////////////////////////////////////////
	typedef stdex::intrusive_ptr<class_destructor_interface> class_destructor_interface_ptr;
}
