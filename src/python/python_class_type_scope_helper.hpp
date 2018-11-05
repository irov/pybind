#pragma once

#include "pybind/types.hpp"

namespace pybind
{
    namespace helper
    {
        bool is_object_bindable( kernel_interface * _kernel, PyObject * _obj );
    }
}