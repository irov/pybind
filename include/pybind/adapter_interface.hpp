#pragma once

#include "config/stdex.hpp"

#include <stddef.h>

namespace pybind
{
    class adapter_interface
        : public stdex::intrusive_ptr_base
    {
    public:
        adapter_interface() noexcept
        {
        }

        virtual ~adapter_interface()
        {
        };

    public:
        void destroy() override;

    public:
        void * operator new (size_t _size);
        void operator delete (void * _ptr, size_t _size);
    };
    //////////////////////////////////////////////////////////////////////////
    typedef stdex::intrusive_ptr<adapter_interface> adapter_interface_ptr;
    //////////////////////////////////////////////////////////////////////////
}
