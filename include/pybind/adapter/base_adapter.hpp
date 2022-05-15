#pragma once

#include "pybind/adapter_interface.hpp"

namespace pybind
{
    //////////////////////////////////////////////////////////////////////////
    class base_adapter_interface
        : public adapter_interface
    {
    public:
        base_adapter_interface( const char * _name )
            : m_name( _name )
#ifdef PYBIND_CALL_DEBUG
            , m_callDebugSilent( false )
#endif
        {
        }

        ~base_adapter_interface() override
        {
        }

    public:
        inline const char * getName() const
        {
            return m_name;
        }

#ifdef PYBIND_CALL_DEBUG
        inline void setCallDebugSilent( bool _silent )
        {
            m_callDebugSilent = _silent;
        }

        inline bool getCallDebugSilent() const
        {
            return m_callDebugSilent;
        }
#endif

    protected:
        const char * m_name;

#ifdef PYBIND_CALL_DEBUG
    protected:
        bool m_callDebugSilent;
#endif
    };
}