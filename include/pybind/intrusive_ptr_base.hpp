#pragma once

#include "pybind/exports.hpp"
#include "pybind/types.hpp"

#include "config/stdex.hpp"

namespace pybind
{
    //////////////////////////////////////////////////////////////////////////
    class intrusive_ptr_base
        : public stdex::intrusive_ptr_base
    {
    public:
        intrusive_ptr_base();
        virtual ~intrusive_ptr_base();

    public:
        uint32_t incref() override;
        void decref() override;
        uint32_t getrefcount() const override;

    public:
        void * operator new (size_t _size);
        void operator delete (void * _ptr, size_t _size);

    protected:
        uint32_t m_refcount;
    };
}