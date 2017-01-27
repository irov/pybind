#	pragma once

#	include "pybind/types.hpp"
#	include "pybind/helper.hpp"

namespace pybind
{
    class t_dict_iterator_end_tag
    {
    };

    class PYBIND_API dict_iterator
    {
    protected:
        explicit dict_iterator( PyObject * _obj );
        explicit dict_iterator( PyObject * _obj, t_dict_iterator_end_tag );

    public:
        dict_iterator & operator = ( const dict_iterator & _it );

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
        PyObject * m_obj;
        size_t m_pos;

        PyObject * m_key;
        PyObject * m_value;

    protected:
        friend class pybind::dict;
    };
}
