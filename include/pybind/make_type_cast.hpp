#pragma once

#include "pybind/type_cast.hpp"

#include "pybind/kernel_interface.hpp"

namespace pybind
{
    template<class T>
    type_cast_ptr make_type_cast( kernel_interface * _kernel )
    {
        allocator_interface * allocator = _kernel->get_allocator();

        return allocator->newT<T>();
    }
}