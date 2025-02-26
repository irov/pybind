#include "pybind/debug.hpp"

#ifdef PYBIND_CALL_DEBUG

namespace pybind
{
    namespace detail
    {
        //////////////////////////////////////////////////////////////////////////
        static observer_bind_call * g_observer = nullptr;
        //////////////////////////////////////////////////////////////////////////
        static void notify_begin_bind_call( kernel_interface * _kernel, const char * _className, const char * _functionName, bool _silent, PyObject * _args, PyObject * _kwds )
        {
            if( g_observer == nullptr )
            {
                return;
            }

            if( _silent == true )
            {
                return;
            }

            g_observer->begin_bind_call( _kernel, _className, _functionName, _args, _kwds );
        }
        //////////////////////////////////////////////////////////////////////////
        static void notify_end_bind_call( kernel_interface * _kernel, const char * _className, const char * _functionName, bool _silent, PyObject * _args, PyObject * _kwds )
        {
            if( g_observer == nullptr )
            {
                return;
            }

            if( _silent == true )
            {
                return;
            }

            g_observer->end_bind_call( _kernel, _className, _functionName, _args, _kwds );
        }
        //////////////////////////////////////////////////////////////////////////
    }
    //////////////////////////////////////////////////////////////////////////
    observer_bind_call * set_observer_bind_call( observer_bind_call * _observer )
    {
        observer_bind_call * old_observer = detail::g_observer;
        detail::g_observer = _observer;

        return old_observer;
    }
    //////////////////////////////////////////////////////////////////////////
    observer_bind_call * get_observer_bind_call()
    {
        return detail::g_observer;
    }
    //////////////////////////////////////////////////////////////////////////
    call_debug_scope::call_debug_scope( kernel_interface * _kernel, const char * _className, const char * _functionName, bool _silent, PyObject * _args, PyObject * _kwds )
        : m_kernel( _kernel )
        , m_className( _className )
        , m_functionName( _functionName )
        , m_silent( _silent )
        , m_args( _args )
        , m_kwds( _kwds )
    {
        detail::notify_begin_bind_call( m_kernel, m_className, m_functionName, m_silent, m_args, m_kwds );
    }
    //////////////////////////////////////////////////////////////////////////
    call_debug_scope::~call_debug_scope()
    {
        detail::notify_end_bind_call( m_kernel, m_className, m_functionName, m_silent, m_args, m_kwds );
    }
    //////////////////////////////////////////////////////////////////////////
}

#endif