#	pragma once

#   include "stdex/intrusive_ptr_base.h"
#   include "stdex/intrusive_ptr.h"

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
        void intrusive_ptr_destroy()
        {
            delete this;
        }
	};
    //////////////////////////////////////////////////////////////////////////
    typedef stdex::intrusive_ptr<adapter_interface> adapter_interface_ptr;
}
