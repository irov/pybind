#pragma once

#include "pybind/intrusive_ptr_base.hpp"

namespace pybind
{
    class adapter_interface
        : public intrusive_ptr_base
    {
    public:
        adapter_interface() noexcept;
        ~adapter_interface() override;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef stdex::intrusive_ptr<adapter_interface> adapter_interface_ptr;
    //////////////////////////////////////////////////////////////////////////
}
