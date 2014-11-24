#	pragma once

#	include "pybind/types.hpp"
#	include "pybind/exports.hpp"

namespace pybind
{
	PYBIND_API bool pickle( PyObject * _obj, PyObject * _types, void * _buffer, size_t _capacity, size_t & _size );
	PYBIND_API PyObject * unpickle( const void * _buffer, size_t _capacity, PyObject * _types );
}
