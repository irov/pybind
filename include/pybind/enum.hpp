#pragma once

#include "pybind/exports.hpp"
#include "pybind/types.hpp"

namespace pybind
{
    namespace detail
    {
        PYBIND_API void add_module_enum( kernel_interface * _kernel, PyObject * _module, const char * _id, uint32_t _value );
    }

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
            detail::add_module_enum( m_kernel, m_module, _id, (uint32_t)_value );

            return *this;
        }

    protected:
        kernel_interface * m_kernel;
        const char * m_name;
        PyObject * m_module;
    };
}