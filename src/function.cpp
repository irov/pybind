#	include "pybind/function.hpp"

#	include "pybind/system.hpp"

namespace pybind
{
	namespace detail
	{
        //////////////////////////////////////////////////////////////////////////
		void def_function_adapter( const function_adapter_interface_ptr & _adapter, bool _native, PyObject * _module )
		{
			kernel_interface * kernel = pybind::get_kernel();

			kernel->def_function_adapter( _adapter, _native, _module );
		}
	}
}

