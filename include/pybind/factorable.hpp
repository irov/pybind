#pragma once

#include "pybind/exports.hpp"
#include "pybind/types.hpp"

#include "config/stdex.hpp"

namespace pybind
{
    //////////////////////////////////////////////////////////////////////////
    class allocator_interface;
    //////////////////////////////////////////////////////////////////////////
    class factorable
        : public stdex::intrusive_ptr_base
    {
    public:
        factorable();
        virtual ~factorable();

    public:
        void set_allocator( allocator_interface * _allocator );
        allocator_interface * get_allocator() const;

    public:
        uint32_t incref() noexcept override;
        void decref() noexcept override;
        uint32_t getrefcount() const noexcept override;

    protected:
        allocator_interface * m_allocator;

        uint32_t m_refcount;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef intrusive_ptr<factorable> factorable_ptr;
    //////////////////////////////////////////////////////////////////////////
}