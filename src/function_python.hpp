#	pragma once

#	include "pybind/function_interface.hpp"

#	include "config/python.hpp"

namespace pybind
{
	class function_python
	{
	public:
		bool initialize();
		void finalize();

	public:
		PyObject * create_function_adapter( const function_adapter_interface_ptr & _adapter, bool _native );

	protected:
		PyTypeObject m_function_type;

		PyMethodDef m_method_native;
		PyMethodDef m_method_args;
		PyMethodDef m_method_noargs;
	};
}
