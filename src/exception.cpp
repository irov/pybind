#include "pybind/exception.hpp"

#include <cstring>
#include <cstdio>
#include <cstdarg>

namespace pybind
{
    //////////////////////////////////////////////////////////////////////////
    pybind_exception::pybind_exception( const char * _message )
    {
        strncpy( m_message, _message, 4096 );
        m_message[4096] = '\0';
    }
    //////////////////////////////////////////////////////////////////////////
    pybind_exception::~pybind_exception() throw()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    const char * pybind_exception::what() const throw()
    {
        return m_message;
    }
    //////////////////////////////////////////////////////////////////////////
    void throw_exception( const char * _format, ... )
    {
        va_list valist;

        va_start( valist, _format );

        char message[4096] = {'\0'};
        vsprintf( message, _format, valist );

        va_end( valist );

        throw pybind_exception( message );
    }
    //////////////////////////////////////////////////////////////////////////
    void throw_exception_va( const char * _format, va_list _va )
    {
        char message[4096] = {'\0'};
        vsprintf( message, _format, _va );

        throw pybind_exception( message );
    }
    //////////////////////////////////////////////////////////////////////////
}