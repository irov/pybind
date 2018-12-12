#pragma once

#include "pybind/extract_operator.hpp"

namespace pybind
{
    class PYBIND_API tuple_const_iterator
    {
    public:
        typedef uint32_t size_type;

    public:
        tuple_const_iterator( kernel_interface * _kernel, PyObject * _obj, size_type _index );

    public:
        tuple_const_iterator( const tuple_const_iterator & _iterator );
        ~tuple_const_iterator();

    public:
        tuple_const_iterator & operator = ( const tuple_const_iterator & _iterator );

    public:
        detail::extract_operator_t operator -> () const;
        detail::extract_operator_t operator * () const;

    public:
        tuple_const_iterator & operator ++ ();
        tuple_const_iterator operator ++ ( int );
        tuple_const_iterator & operator -- ();
        tuple_const_iterator operator -- ( int );

    public:
        bool operator == ( const tuple_const_iterator & _it ) const;
        bool operator != ( const tuple_const_iterator & _it ) const;

    protected:
        kernel_interface * m_kernel;
        PyObject * m_obj;
        size_type m_index;
    };
}