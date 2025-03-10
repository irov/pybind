#pragma once

#include "pybind/adapter/base_adapter.hpp"

#include "pybind/types.hpp"

namespace pybind
{
    //////////////////////////////////////////////////////////////////////////
    class kernel_interface;
    //////////////////////////////////////////////////////////////////////////
    class functor_adapter_interface
        : public base_adapter_interface
    {
    public:
        functor_adapter_interface( const char * _name, size_t _arity )
            : base_adapter_interface( _name )
            , m_arity( _arity )
        {
        }

        ~functor_adapter_interface() override
        {
        }

    public:
        size_t getArity() const
        {
            return m_arity;
        }

    public:
        virtual PyObject * call( kernel_interface * _kernel, PyObject * _args, PyObject * _kwds ) = 0;

    protected:
        size_t m_arity;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef intrusive_ptr<class functor_adapter_interface> functor_adapter_interface_ptr;
    //////////////////////////////////////////////////////////////////////////
}

