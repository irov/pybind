#	pragma once

#	include "exports.hpp"
#	include "types.hpp"

namespace pybind
{
	PYBIND_API void set_observer_bind_call( observer_bind_call * _observer );
	PYBIND_API void notify_begin_bind_call( const char * _className, const char * _functionName, PyObject * _args, PyObject * _kwds );
	PYBIND_API void notify_end_bind_call( const char * _className, const char * _functionName, PyObject * _args, PyObject * _kwds );

#	ifdef PYBIND_CALL_DEBUG
#	define DEBUG_PYBIND_NOTIFY_BEGIN_BIND_CALL( ClassName, FunctionName, Args, Kwds )\
	pybind::notify_begin_bind_call( ClassName, FunctionName, Args, Kwds )
#	define DEBUG_PYBIND_NOTIFY_END_BIND_CALL( ClassName, FunctionName, Args, Kwds )\
	pybind::notify_end_bind_call( ClassName, FunctionName, Args, Kwds )
#	else
#	define DEBUG_PYBIND_NOTIFY_BEGIN_BIND_CALL( ClassName, FunctionName, Args, Kwds )
#	define DEBUG_PYBIND_NOTIFY_END_BIND_CALL( ClassName, FunctionName, Args, Kwds )
#	endif
}