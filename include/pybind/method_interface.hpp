#pragma once

#include "pybind/adapter_interface.hpp"

#include "pybind/types.hpp"

namespace pybind
{
	//////////////////////////////////////////////////////////////////////////
	class kernel_interface;
	//////////////////////////////////////////////////////////////////////////
	typedef stdex::intrusive_ptr<class class_type_scope_interface> class_type_scope_interface_ptr;
	//////////////////////////////////////////////////////////////////////////
	class method_adapter_interface
		: public adapter_interface
	{
	public:
		method_adapter_interface( const char * _name )
			: m_name(_name)
		{
		}

	public:
		inline const char * getName() const
		{
			return m_name;
		}

	public:
		virtual PyObject * call( kernel_interface * _kernel, void * _self, const class_type_scope_interface_ptr & _scope, PyObject * _args, PyObject * _kwds ) = 0;

	protected:
		const char * m_name;
	};
    //////////////////////////////////////////////////////////////////////////
    typedef stdex::intrusive_ptr<method_adapter_interface> method_adapter_interface_ptr;
}

