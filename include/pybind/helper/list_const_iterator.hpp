#pragma once

#include "pybind/extract_operator.hpp"

namespace pybind
{
    class PYBIND_API list_const_iterator
    {
    public:
        typedef uint32_t size_type;

    public:
        list_const_iterator( kernel_interface * _kernel, PyObject * _obj, size_type _index );

    public:
        list_const_iterator( const list_const_iterator & _r );
        ~list_const_iterator();

    public:
        detail::extract_operator_t operator -> () const;
        detail::extract_operator_t operator * () const;

    public:
        list_const_iterator & operator ++ ();
        list_const_iterator operator ++ ( int );
        list_const_iterator & operator -- ();
        list_const_iterator operator -- ( int );

    public:
        bool operator == ( const list_const_iterator & _it ) const;
        bool operator != ( const list_const_iterator & _it ) const;

    protected:
        kernel_interface * m_kernel;
        PyObject * m_obj;
        size_type m_index;
    };
}