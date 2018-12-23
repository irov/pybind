#pragma once

#include "pybind/adapter_interface.hpp"

#include "pybind/extract.hpp"
#include "pybind/exports.hpp"

namespace pybind
{
    class constructor_adapter_interface
        : public adapter_interface
    {
    public:
        constructor_adapter_interface( uint32_t _arity );
        ~constructor_adapter_interface() override;

    public:
        uint32_t arity() const;

    public:
        virtual void * call( kernel_interface * _kernel, PyObject * _obj, PyObject * _args ) = 0;

#ifndef NDEBUG
    protected:
        bool valid( kernel_interface * _kernel, PyObject * _args ) const;
#endif

    protected:
        uint32_t m_arity;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef stdex::intrusive_ptr<constructor_adapter_interface> constructor_adapter_interface_ptr;
    //////////////////////////////////////////////////////////////////////////
}
