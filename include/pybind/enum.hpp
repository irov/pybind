#pragma once

#include "pybind/exports.hpp"
#include "pybind/types.hpp"
#include "pybind/const.hpp"

namespace pybind
{
    template<class T>
    class enum_
    {
    public:
        enum_( kernel_interface * _kernel, const char * _name, PyObject * _module = nullptr )
            : m_kernel( _kernel )
            , m_name( _name )
            , m_module( _module )
        {
        }

    public:
        enum_ & def( const char * _id, T _value )
        {
            pybind::def_const<T>( m_kernel, _id, _value, m_module );

            return *this;
        }

    protected:
        kernel_interface * m_kernel;
        const char * m_name;
        PyObject * m_module;
    };
}