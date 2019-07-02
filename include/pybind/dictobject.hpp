#pragma once

#include "pybind/exports.hpp"

#include "pybind/object.hpp"
#include "pybind/dict.hpp"

namespace pybind
{
    //////////////////////////////////////////////////////////////////////////
    PYBIND_API pybind::object dictobject_new( kernel_interface * _kernel, const pybind::dict & _dict );
    //////////////////////////////////////////////////////////////////////////
}
