#pragma once

#include "pybind/exports.hpp"
#include "pybind/types.hpp"

#ifdef PYBIND_CALL_DEBUG

namespace pybind
{
    PYBIND_API observer_bind_call * set_observer_bind_call( observer_bind_call * _observer );
    PYBIND_API observer_bind_call * get_observer_bind_call();

    PYBIND_API class call_debug_scope
    {
    public:
        call_debug_scope( kernel_interface * _kernel, const char * _className, const char * _functionName, bool _silent, PyObject * _args, PyObject * _kwds );
        ~call_debug_scope();

    protected:
        kernel_interface * m_kernel;
        const char * m_className;
        const char * m_functionName;
        bool m_silent;
        PyObject * m_args;
        PyObject * m_kwds;
    };
}

#   define DEBUG_PYBIND_SET_OBSERVER_BIND_CALL( Observer ) pybind::set_observer_bind_call( Observer )
#   define DEBUG_PYBIND_GET_OBSERVER_BIND_CALL() pybind::get_observer_bind_call()
#	define DEBUG_PYBIND_NOTIFY_BIND_CALL_SCOPE( Kernel, ClassName, FunctionName, Silent, Args, Kwds ) pybind::call_debug_scope cds_##__LINE__( Kernel, ClassName, FunctionName, Silent, Args, Kwds )
#else
#   define DEBUG_PYBIND_SET_OBSERVER_BIND_CALL( Observer ) (nullptr)
#   define DEBUG_PYBIND_GET_OBSERVER_BIND_CALL() (nullptr)
#	define DEBUG_PYBIND_NOTIFY_BIND_CALL_SCOPE( Kernel, ClassName, FunctionName, Silent, Args, Kwds )
#endif