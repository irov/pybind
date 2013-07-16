#	pragma once

#	include "pybind/adapter_interface.hpp"

#	include "pybind/extract.hpp"
#	include "pybind/exports.hpp"

namespace pybind
{
	class constructor_adapter_interface
        : public adapter_interface
	{
	public:
		constructor_adapter_interface( size_t _arity );
		virtual ~constructor_adapter_interface();

	public:
		bool valid( PyObject * _args ) const;
		size_t arity() const;

	public:
		virtual void * call( PyObject * _obj, PyObject * _args, const char * _tag ) = 0;

	protected:
		size_t m_arity;
	};
    //////////////////////////////////////////////////////////////////////////
    typedef stdex::intrusive_ptr<constructor_adapter_interface> constructor_adapter_interface_ptr;
}
