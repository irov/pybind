#pragma once

#include <stddef.h>

namespace pybind
{
    class allocator_interface
    {
    public:
        virtual void * malloc( size_t _size ) = 0;
        virtual void * calloc( size_t _num, size_t _size ) = 0;
        virtual void * realloc( void * _ptr, size_t _size ) = 0;
        virtual void free( void * _ptr ) = 0;
    };
}
