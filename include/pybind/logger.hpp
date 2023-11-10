#pragma once

#include "exports.hpp"

#include <cstdarg>

namespace pybind
{
    //////////////////////////////////////////////////////////////////////////
    typedef void(*pybind_logger_t)(void * _impl, const char * _msg);
    //////////////////////////////////////////////////////////////////////////
    PYBIND_API void set_logger( pybind_logger_t _logger, void * _user );
    PYBIND_API pybind_logger_t get_logger();
    PYBIND_API void * get_logger_user();
    //////////////////////////////////////////////////////////////////////////
    PYBIND_API void log( const char * _format, ... );
    PYBIND_API void log_va( const char * _format, va_list _va );
    //////////////////////////////////////////////////////////////////////////
}