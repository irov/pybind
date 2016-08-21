#	pragma once

#	include "pybind/adapter_interface.hpp"

#   include "pybind/types.hpp"

namespace pybind
{
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
		virtual PyObject * call( void * _self, const class_type_scope_ptr & _scope, PyObject * _args, PyObject * _kwds ) = 0;

	protected:
		const char * m_name;
	};
    //////////////////////////////////////////////////////////////////////////
    typedef stdex::intrusive_ptr<method_adapter_interface> method_adapter_interface_ptr;
}

