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

		pybind_exception ex;
		char * buff = ex.buff();
		vsnprintf( buff, 2048 - 1, _format, argList );

		va_end(argList);

        throw ex;
    }
}