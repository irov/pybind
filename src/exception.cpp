#	include "pybind/exception.hpp"

#	include <stdio.h>
#	include <stdarg.h>

namespace pybind
{
    //////////////////////////////////////////////////////////////////////////
    void throw_exception( const char * _format, ... )
    {
		va_list argList;

		va_start(argList, _format);

		char str[2048];
		int size = vsnprintf( str, 2048 - 1, _format, argList );

		va_end(argList);

        throw pybind_exception(str);
    }
}