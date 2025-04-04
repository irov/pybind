#pragma once

#include "pybind/types.hpp"
#include "pybind/helper.hpp"

#include "pybind/helper/dict_pair_value.hpp"

namespace pybind
{
    class t_dict_iterator_end_tag
    {
    };

    class PYBIND_API dict_iterator
    {
    public:
        typedef size_t size_type;

    public:
        dict_iterator( kernel_interface * _kernel, PyObject * _obj );
        dict_iterator( kernel_interface * _kernel, PyObject * _obj, t_dict_iterator_end_tag );

    public:
        dict_iterator( const dict_iterator & _r );
        ~dict_iterator();

    public:
        dict_iterator & operator = ( const dict_iterator & _r );

    public:
        dict_pair_value operator -> () const;
        dict_pair_value operator * () const;

    public:
        bool operator == ( const dict_iterator & _it ) const;
        bool operator != ( const dict_iterator & _it ) const;

    public:
        dict_iterator & operator ++ ();
        dict_iterator operator ++ ( int );

    public:
        detail::extract_operator_t key() const;
        detail::extract_operator_t value() const;

    protected:
        kernel_interface * m_kernel;

        PyObject * m_obj;
        size_type m_pos;

        PyObject * m_key;
        PyObject * m_value;
    };
}
