#pragma once

#include "pybind/adapter_interface.hpp"

#include "pybind/types.hpp"

namespace pybind
{
    //////////////////////////////////////////////////////////////////////////
    class kernel_interface;
    //////////////////////////////////////////////////////////////////////////
    class function_adapter_interface
        : public adapter_interface
    {
    public:
        function_adapter_interface( const char * _name, uint32_t _arity )
            : m_name( _name )
            , m_arity( _arity )
        {
        }

        ~function_adapter_interface() override
        {
        }

    public:
        const char * getName() const
        {
            return m_name;
        }

        uint32_t getArity() const
        {
            return m_arity;
        }

    public:
        virtual PyObject * call( kernel_interface * _kernel, PyObject * _args, PyObject * _kwds ) = 0;

    protected:
        const char * m_name;
        uint32_t m_arity;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef stdex::intrusive_ptr<function_adapter_interface> function_adapter_interface_ptr;
    //////////////////////////////////////////////////////////////////////////
}