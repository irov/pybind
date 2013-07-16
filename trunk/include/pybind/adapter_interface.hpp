#	pragma once

#   include "stdex/intrusive_ptr.h"

namespace pybind
{
	class adapter_interface
	{
	public:
        adapter_interface()
            : m_refcount(0)
        {
        }
        
        virtual ~adapter_interface()
		{
		};

    protected:
        friend void intrusive_ptr_add_ref( adapter_interface * _ptr );
        friend void intrusive_ptr_release( adapter_interface * _ptr );

    protected:
        void destroy()
        {
            delete this;
        }

    protected:
        size_t m_refcount;
	};
    //////////////////////////////////////////////////////////////////////////
    inline void intrusive_ptr_add_ref( adapter_interface * _ptr )
    {
        ++_ptr->m_refcount;
    }
    //////////////////////////////////////////////////////////////////////////
    inline void intrusive_ptr_release( adapter_interface * _ptr )
    {
        if( --_ptr->m_refcount == 0 )
        {
            _ptr->destroy();
        }
    }
    //////////////////////////////////////////////////////////////////////////
    typedef stdex::intrusive_ptr<adapter_interface> adapter_interface_ptr;
}