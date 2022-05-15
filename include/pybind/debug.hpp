#pragma once

#include "exports.hpp"
#include "types.hpp"

namespace pybind
{
    PYBIND_API observer_bind_call * set_observer_bind_call( observer_bind_call * _observer );
    PYBIND_API observer_bind_call * get_observer_bind_call();
    PYBIND_API void notify_begin_bind_call( kernel_interface * _kernel, const char * _className, const char * _functionName, bool _silent, PyObject * _args, PyObject * _kwds );
    PYBIND_API void notify_end_bind_call( kernel_interface * _kernel, const char * _className, const char * _functionName, bool _silent, PyObject * _args, PyObject * _kwds );

#ifdef PYBIND_CALL_DEBUG
#	define DEBUG_PYBIND_NOTIFY_BEGIN_BIND_CALL( Kernel, ClassName, FunctionName, Silent, Args, Kwds )\
	pybind::notify_begin_bind_call( Kernel, ClassName, FunctionName, Silent, Args, Kwds )
#	define DEBUG_PYBIND_NOTIFY_END_BIND_CALL( Kernel, ClassName, FunctionName, Silent, Args, Kwds )\
	pybind::notify_end_bind_call( Kernel, ClassName, FunctionName, Silent, Args, Kwds )
#else
#	define DEBUG_PYBIND_NOTIFY_BEGIN_BIND_CALL( Kernel, ClassName, FunctionName, Silent, Args, Kwds )
#	define DEBUG_PYBIND_NOTIFY_END_BIND_CALL( Kernel, ClassName, FunctionName, Silent, Args, Kwds )
#endif
}