#pragma once

#include "config/stdex.hpp"

namespace pybind
{
    template<class T>
    using intrusive_ptr = stdex::intrusive_ptr<T>;
}