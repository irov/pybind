#	pragma once

#   include "pybind/extract_operator.hpp"

namespace pybind
{
    //////////////////////////////////////////////////////////////////////////
    class args
    {
    public:
        args();
        args( const args & _r );
        args( kernel_interface * _kernel, PyObject * _args );

    public:
        ~args();

    public:
        args & operator = ( const args & _args );

    public:
        void reset();
        
    public:
        size_t size() const;

    public:
        detail::extract_operator_t operator [] ( size_t _index ) const;

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

    protected:
        kernel_interface * m_kernel;
        PyObject * m_args;
    };
}