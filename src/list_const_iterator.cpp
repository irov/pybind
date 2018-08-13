#include "pybind/helper/list_const_iterator.hpp"

namespace pybind
{
    //////////////////////////////////////////////////////////////////////////
    list_const_iterator::list_const_iterator( kernel_interface * _kernel, PyObject * _obj, size_type _index )
        : m_kernel( _kernel )
        , m_obj( _obj )
        , m_index( _index )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    list_const_iterator::list_const_iterator( const list_const_iterator & _r )
        : m_kernel( _r.m_kernel )
        , m_obj( _r.m_obj )
        , m_index( _r.m_index )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    list_const_iterator::~list_const_iterator()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    detail::extract_operator_t list_const_iterator::operator -> () const
    {
        PyObject * py_item = pybind::list_getitem( m_obj, m_index );

        return detail::extract_operator_t( m_kernel, py_item );
    }

    detail::extract_operator_t list_const_iterator::operator * () const
    {
        PyObject * py_item = pybind::list_getitem( m_obj, m_index );

        return detail::extract_operator_t( m_kernel, py_item );
    }
    //////////////////////////////////////////////////////////////////////////
    list_const_iterator & list_const_iterator::operator ++ ()
    {
        ++m_index;

        return *this;
    }
    //////////////////////////////////////////////////////////////////////////
    list_const_iterator list_const_iterator::operator ++ ( int )
    {
        list_const_iterator tmp = *this;
        ++*this;
        return tmp;
    }
    //////////////////////////////////////////////////////////////////////////
    list_const_iterator & list_const_iterator::operator -- ()
    {
        --m_index;

        return *this;
    }
    //////////////////////////////////////////////////////////////////////////
    list_const_iterator list_const_iterator::operator -- ( int )
    {
        list_const_iterator tmp = *this;
        --*this;
        return tmp;
    }
    //////////////////////////////////////////////////////////////////////////
    bool list_const_iterator::operator == ( const list_const_iterator & _it ) const
    {
        return m_index == _it.m_index;
    }
    //////////////////////////////////////////////////////////////////////////
    bool list_const_iterator::operator != ( const list_const_iterator & _it ) const
    {
        return !this->operator == ( _it );
    }
}
