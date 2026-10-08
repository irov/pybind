#pragma once

#include "pybind/types.hpp"

#include "tinypy/tinypy.hpp"

namespace pybind
{
    //////////////////////////////////////////////////////////////////////////
    class tinypy_arguments_scope
    {
    public:
        tinypy_arguments_scope( tinypy_vm_t * _vm, tinypy_value_t * const * _items, size_t _count );
        ~tinypy_arguments_scope();

    public:
        PyObject * get() const;

    private:
        tinypy_arguments_scope( const tinypy_arguments_scope & );
        tinypy_arguments_scope & operator = ( const tinypy_arguments_scope & );

    protected:
        tinypy_value_t * m_args;
    };
    //////////////////////////////////////////////////////////////////////////
}
