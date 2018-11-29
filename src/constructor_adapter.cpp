#include "pybind/adapter/constructor_adapter.hpp"

#include "pybind/extract.hpp"
#include "pybind/exports.hpp"

namespace pybind
{
    //////////////////////////////////////////////////////////////////////////
    constructor_adapter_interface::constructor_adapter_interface( uint32_t _arity )
        : m_arity( _arity )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    constructor_adapter_interface::~constructor_adapter_interface()
    {
    }
    //////////////////////////////////////////////////////////////////////////
#ifndef NDEBUG
    //////////////////////////////////////////////////////////////////////////
    bool constructor_adapter_interface::valid( kernel_interface * _kernel, PyObject * _args ) const
    {
        if( _kernel->tuple_check( _args ) == false )
        {
            return false;
        }

        uint32_t args_arity = _kernel->tuple_size( _args );

        if( m_arity != args_arity )
        {
            pybind::throw_exception( "construct have %d args, but you call %d params"
                , m_arity
                , args_arity
            );

            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
#endif
    //////////////////////////////////////////////////////////////////////////
    uint32_t constructor_adapter_interface::arity() const
    {
        return m_arity;
    }
}
