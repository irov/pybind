#	pragma once

#	include "config/stdex.hpp"

#   include <stddef.h>

namespace pybind
{
	class adapter_interface
        : public stdex::intrusive_ptr_base<adapter_interface>
	{
	public:
        adapter_interface()
        {
        }
        
        virtual ~adapter_interface()
		{
		};

    public:
        void intrusive_ptr_destroy();

	public:
		void * operator new ( std::size_t _size );
		void operator delete (void * _ptr, std::size_t _size);
	};
    //////////////////////////////////////////////////////////////////////////
    typedef stdex::intrusive_ptr<adapter_interface> adapter_interface_ptr;
}
