#include "pybind/exception.hpp"

#include <cstdio>
#include <cstdarg>

namespace pybind
{
    //////////////////////////////////////////////////////////////////////////
    void throw_exception( const char * _format, ... )
    {
        va_list valist;

        va_start( valist, _format );

        char message[4096] = { 0 };
        vsprintf( message, _format, valist );

        va_end( valist );

        throw pybind_exception( message );
    }
    //////////////////////////////////////////////////////////////////////////
    void throw_exception_va( const char * _format, va_list _va )
    {
        char message[4096] = { 0 };
        vsprintf( message, _format, _va );

        throw pybind_exception( message );
    }
}