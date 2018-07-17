#include "pybind/logger.hpp"

#include <stdio.h>
#include <stdarg.h>

namespace pybind
{
	//////////////////////////////////////////////////////////////////////////
	static pybind_logger_t s_logger = nullptr;
	static void * s_user = nullptr;
	//////////////////////////////////////////////////////////////////////////
	void set_logger( pybind_logger_t _logger, void * _user )
	{
		s_logger = _logger;
		s_user = _user;
	}
	//////////////////////////////////////////////////////////////////////////
	pybind_logger_t get_logger()
	{
		return s_logger;
	}
	//////////////////////////////////////////////////////////////////////////
	void * get_logger_user()
	{ 
		return s_user;
	}
	//////////////////////////////////////////////////////////////////////////
	void log( const char * _format, ... )
	{
		if( s_logger == nullptr )
		{
			return;
		}

		va_list argList;

		va_start( argList, _format );

		char str[4096] = {0};
		vsprintf( str, _format, argList );

		va_end( argList );		

		s_logger( s_user, str );
	}
}