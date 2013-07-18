#	pragma once

#   include "stdex/intrusive_ptr_base.h"
#   include "stdex/intrusive_ptr.h"

#   include <stddef.h>

namespace pybind
{
	class adapter_interface
        : public stdex::intrusive_ptr_base
	{
	public:
        adapter_interface()
        {
        }
        
        virtual ~adapter_interface()
		{
		};

    protected:
        void destroy() override
        {
            delete this;
        }
	};
    //////////////////////////////////////////////////////////////////////////
    typedef stdex::intrusive_ptr<adapter_interface> adapter_interface_ptr;
}
