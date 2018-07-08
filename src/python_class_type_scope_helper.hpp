#pragma once

#include "pybind/types.hpp"

namespace pybind
{
    namespace helper
    {
        bool is_object_bindable( PyObject * _obj );
    }
}