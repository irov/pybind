#	pragma once

#	include "pybind/functor_interface.hpp"

#	include "config/python.hpp"

namespace pybind
{
	class functor_python
	{
	public:
		bool initialize();
		void finalize();

	public:
		PyObject * create_functor_adapter( const functor_adapter_interface_ptr & _adapter, bool _native );

	protected:
		PyTypeObject m_functor_type;

		PyMethodDef m_method_native;
		PyMethodDef m_method_args;
		PyMethodDef m_method_noargs;
	};
}
