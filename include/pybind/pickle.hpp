#pragma once

#include "pybind/types.hpp"
#include "pybind/exports.hpp"

namespace pybind
{
    class kernel_interface;

    PYBIND_API bool pickle( kernel_interface * _kernel, PyObject * _obj, PyObject * _types, void * _buffer, size_t _capacity, size_t * const _size );
    PYBIND_API PyObject * unpickle( kernel_interface * _kernel, const void * _buffer, size_t _capacity, PyObject * _types );
}