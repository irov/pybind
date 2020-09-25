#pragma once

#include "pybind/types.hpp"

namespace pybind
{
    class kernel_interface;

    class string_view
    {
    public:
        string_view( kernel_interface * _kernel, PyObject * _obj );
        string_view( const string_view & _base );
        string_view( string_view && _base );
        ~string_view();

    public:
        bool is_invalid() const;
        bool is_valid() const;

    public:
        kernel_interface * kernel() const;

    public:
        const char * c_str() const;

    protected:
        kernel_interface * m_kernel;
        PyObject * m_obj;
    };
}