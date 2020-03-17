#include "pybind/intrusive_ptr_base.hpp"

#include "config/config.hpp"

namespace pybind
{
    //////////////////////////////////////////////////////////////////////////
    intrusive_ptr_base::intrusive_ptr_base()
        : m_refcount( 0 )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    intrusive_ptr_base::~intrusive_ptr_base()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void * intrusive_ptr_base::operator new (size_t _size)
    {
        return PYBIND_MALLOC( _size );
    }
    //////////////////////////////////////////////////////////////////////////
    void intrusive_ptr_base::operator delete (void * _ptr, size_t _size)
    {
        (void)_size;

        PYBIND_FREE( _ptr );
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
            delete this;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t intrusive_ptr_base::getrefcount() const
    {
        return m_refcount;
    }
}
