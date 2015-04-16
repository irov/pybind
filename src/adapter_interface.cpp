#	include <pybind/adapter_interface.hpp>

#	include "config/config.hpp"

namespace pybind
{
	//////////////////////////////////////////////////////////////////////////
	void adapter_interface::intrusive_ptr_destroy()
	{
		delete this;
	}
	//////////////////////////////////////////////////////////////////////////
	void * adapter_interface::operator new (std::size_t _size)
	{
		return PYBIND_MALLOC(_size);
	}
	//////////////////////////////////////////////////////////////////////////
	void adapter_interface::operator delete (void * _ptr, std::size_t _size)
	{		
		(void)_size;

		PYBIND_FREE(_ptr, _size);
	}
}