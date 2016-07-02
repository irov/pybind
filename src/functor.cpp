#	include "pybind/functor.hpp"

#	include "pybind/system.hpp"

namespace pybind
{
	namespace detail
	{
		//////////////////////////////////////////////////////////////////////////
		void def_functor( const functor_adapter_interface_ptr & _adapter, bool _native, PyObject * _module )
		{
			kernel_interface * kernel = pybind::get_kernel();

			kernel->def_functor_adapter( _adapter, _native, _module );
		}
	}
}
