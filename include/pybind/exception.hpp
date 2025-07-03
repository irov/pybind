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
        explicit pybind_exception( const char * _message );
        ~pybind_exception() throw() override;

    public:
        const char * what() const throw() override;

    protected:
        char m_message[4096 + 1] = {'\0'};
    };

    PYBIND_API void throw_exception( const char * _format, ... );
    PYBIND_API void throw_exception_va( const char * _format, va_list _va );
}

