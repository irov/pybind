#include "pybind/debug.hpp"

namespace pybind
{
    //////////////////////////////////////////////////////////////////////////
    static observer_bind_call * g_observer = nullptr;
    //////////////////////////////////////////////////////////////////////////
    observer_bind_call * set_observer_bind_call( observer_bind_call * _observer )
    {
        observer_bind_call * old_observer = g_observer;
        g_observer = _observer;
        return old_observer;
    }
    //////////////////////////////////////////////////////////////////////////
    observer_bind_call * get_observer_bind_call()
    {
        return g_observer;
    }
    //////////////////////////////////////////////////////////////////////////
    void notify_begin_bind_call( kernel_interface * _kernel, const char * _className, const char * _functionName, PyObject * _args, PyObject * _kwds )
    {
        if( g_observer == nullptr )
        {
            return;
        }

        g_observer->begin_bind_call( _kernel, _className, _functionName, _args, _kwds );
    }
    //////////////////////////////////////////////////////////////////////////
    void notify_end_bind_call( kernel_interface * _kernel, const char * _className, const char * _functionName, PyObject * _args, PyObject * _kwds )
    {
        if( g_observer == nullptr )
        {
            return;
        }

        g_observer->end_bind_call( _kernel, _className, _functionName, _args, _kwds );
    }
}