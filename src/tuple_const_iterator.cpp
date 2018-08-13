#include "pybind/helper/tuple_const_iterator.hpp"

namespace pybind
{
    //////////////////////////////////////////////////////////////////////////
    tuple_const_iterator::tuple_const_iterator( kernel_interface * _kernel, PyObject * _obj, size_type _index )
        : m_kernel( _kernel )
        , m_obj( _obj )
        , m_index( _index )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    tuple_const_iterator::tuple_const_iterator( const tuple_const_iterator & _iterator )
        : m_kernel( _iterator.m_kernel )
        , m_obj( _iterator.m_obj )
        , m_index( _iterator.m_index )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    tuple_const_iterator::~tuple_const_iterator()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    detail::extract_operator_t tuple_const_iterator::operator -> () const
    {
        PyObject * py_item = pybind::tuple_getitem( m_obj, m_index );

        return detail::extract_operator_t( m_kernel, py_item );
    }
    //////////////////////////////////////////////////////////////////////////
    detail::extract_operator_t tuple_const_iterator::operator * () const
    {
        PyObject * py_item = pybind::tuple_getitem( m_obj, m_index );

        return detail::extract_operator_t( m_kernel, py_item );
    }
    //////////////////////////////////////////////////////////////////////////
    tuple_const_iterator & tuple_const_iterator::operator ++ ()
    {
        ++m_index;

        return *this;
    }
    //////////////////////////////////////////////////////////////////////////
    tuple_const_iterator tuple_const_iterator::operator ++ ( int )
    {
        tuple_const_iterator tmp = *this;
        ++*this;
        return tmp;
    }
    //////////////////////////////////////////////////////////////////////////
    tuple_const_iterator & tuple_const_iterator::operator -- ()
    {
        --m_index;

        return *this;
    }
    //////////////////////////////////////////////////////////////////////////
    tuple_const_iterator tuple_const_iterator::operator -- ( int )
    {
        tuple_const_iterator tmp = *this;
        --*this;
        return tmp;
    }
    //////////////////////////////////////////////////////////////////////////
    bool tuple_const_iterator::operator == ( const tuple_const_iterator & _it ) const
    {
        return m_index == _it.m_index;
    }
    //////////////////////////////////////////////////////////////////////////
    bool tuple_const_iterator::operator != ( const tuple_const_iterator & _it ) const
    {
        return !this->operator == ( _it );
    }
}
