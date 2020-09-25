#pragma once

#include "pybind/extract_operator.hpp"
#include "pybind/string_view.hpp"

#include "pybind/helper/tuple_const_iterator.hpp"

namespace pybind
{
    //////////////////////////////////////////////////////////////////////////
    class args
    {
    public:
        typedef uint32_t size_type;

    public:
        args();
        args( const args & _r );
        args( args && _r );
        args( kernel_interface * _kernel, PyObject * _args );
        args( kernel_interface * _kernel, PyObject * _args, pybind::borrowed_t );

    public:
        ~args();

    public:
        args & operator = ( const args & _args );

    public:
        void reset();

    public:
        size_type size() const;

    public:
        detail::extract_operator_t operator [] ( size_type _index ) const;

    public:
        operator PyObject * () const
        {
            return m_args;
        }

    public:
        PyObject * ptr() const
        {
            return m_args;
        }

    public:
        string_view repr() const;

    public:
        typedef tuple_const_iterator const_iterator;

    public:
        const_iterator begin() const;
        const_iterator end() const;

    protected:
        kernel_interface * m_kernel;
        PyObject * m_args;
    };
}