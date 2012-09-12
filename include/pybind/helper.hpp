#   pragma once

#   include <string.h>

#   include <functional>

namespace pybind
{
    //////////////////////////////////////////////////////////////////////////
    struct less_char
        : public std::binary_function<const char *, const char *, bool>
    {
        bool operator()(const char * _left, const char * _right) const
        {
            return strcmp( _left, _right ) < 0;
        }
    };
}