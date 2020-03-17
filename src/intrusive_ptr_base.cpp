#include "pybind/intrusive_ptr_base.hpp"

#include "config/config.hpp"

namespace pybind
{
    //////////////////////////////////////////////////////////////////////////
    intrusive_ptr_base::intrusive_ptr_base()
        : m_allocator( nullptr )
        , m_refcount( 0 )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    intrusive_ptr_base::~intrusive_ptr_base()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void intrusive_ptr_base::set_allocator( allocator_interface * _allocator )
    {
        m_allocator = _allocator;
    }
    //////////////////////////////////////////////////////////////////////////
    allocator_interface * intrusive_ptr_base::get_allocator() const
    {
        return m_allocator;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t intrusive_ptr_base::incref()
    {
        ++m_refcount;

        return m_refcount;
    }
    //////////////////////////////////////////////////////////////////////////
    void intrusive_ptr_base::decref()
    {
        if( --m_refcount == 0 )
        {
            this->~intrusive_ptr_base();

            m_allocator->free( this );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t intrusive_ptr_base::getrefcount() const
    {
        return m_refcount;
    }
}
