#pragma once

#include "pybind/factorable.hpp"

namespace pybind
{
    class adapter_interface
        : public factorable
    {
    public:
        adapter_interface();
        ~adapter_interface() override;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef intrusive_ptr<adapter_interface> adapter_interface_ptr;
    //////////////////////////////////////////////////////////////////////////
}
