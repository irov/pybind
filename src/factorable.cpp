#include "pybind/factorable.hpp"

#include "pybind/allocator_interface.hpp"

#include "config/config.hpp"

namespace pybind
{
    //////////////////////////////////////////////////////////////////////////
    factorable::factorable()
        : m_allocator( nullptr )
        , m_refcount( 0 )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    factorable::~factorable()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void factorable::set_allocator( allocator_interface * _allocator )
    {
        m_allocator = _allocator;
    }
    //////////////////////////////////////////////////////////////////////////
    allocator_interface * factorable::get_allocator() const
    {
        return m_allocator;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t factorable::incref()
    {
        ++m_refcount;

        return m_refcount;
    }
    //////////////////////////////////////////////////////////////////////////
    void factorable::decref()
    {
        if( --m_refcount == 0 )
        {
            m_allocator->deleteT( this );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t factorable::getrefcount() const
    {
        return m_refcount;
    }
}
