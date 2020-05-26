#pragma once

#include "exports.hpp"

#include <exception>
#include <cstdarg>

namespace pybind
{
    class pybind_exception
        : public std::exception
    {
    public:
        pybind_exception( const char * _message )
            : m_message( _message )
        {
        }

        ~pybind_exception() throw() override
        {
        }

    public:
        const char * what() const throw() override
        {
            return m_message;
        }

    protected:
        const char * m_message;
    };

    PYBIND_API void throw_exception( const char * _format, ... );
    PYBIND_API void throw_exception_va( const char * _format, va_list _va );
}

