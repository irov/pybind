#include "pybind/adapter/constructor_adapter.hpp"

#include "pybind/extract.hpp"
#include "pybind/exports.hpp"

namespace pybind
{
    //////////////////////////////////////////////////////////////////////////
    constructor_adapter_interface::constructor_adapter_interface( size_t _arity )
        : m_arity( _arity )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    constructor_adapter_interface::~constructor_adapter_interface()
    {
    }
    //////////////////////////////////////////////////////////////////////////
#if defined(PYBIND_DEBUG)
    //////////////////////////////////////////////////////////////////////////
    bool constructor_adapter_interface::valid( kernel_interface * _kernel, PyObject * _args ) const
    {
        if( _kernel->tuple_check( _args ) == false )
        {
            return false;
        }

        size_t args_arity = _kernel->tuple_size( _args );

        if( m_arity != args_arity )
        {
            pybind::throw_exception( "construct have %zu args, but you call %zu params"
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
    size_t constructor_adapter_interface::arity() const
    {
        return m_arity;
    }
    //////////////////////////////////////////////////////////////////////////
}
