#include "pybind/list.hpp"

namespace pybind
{
    //////////////////////////////////////////////////////////////////////////
    list::list()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    list::list( kernel_interface * _kernel )
        : pybind::object( _kernel, _kernel->list_new( 0 ), pybind::borrowed )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    list::list( kernel_interface * _kernel, size_type _size )
        : pybind::object( _kernel, _kernel->list_new( _size ), pybind::borrowed )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    list::list( const list & _list )
        : pybind::object( _list )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    list::list( list && _list )
        : pybind::object( std::move( _list ) )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    list::list( const object & _obj )
        : pybind::object( _obj )
    { 
    }
    //////////////////////////////////////////////////////////////////////////
    list::list( object && _obj )
        : pybind::object( std::move( _obj ) )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    list::list( pybind::invalid_t _iv )
        : object( _iv )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    list::list( kernel_interface * _kernel, PyObject * _obj, pybind::borrowed_t _br )
        : pybind::object( _kernel, _obj, _br )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    list::list( kernel_interface * _kernel, PyObject * _obj )
        : pybind::object( _kernel, _obj )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    list::~list()
    {
    }
    //////////////////////////////////////////////////////////////////////////    
    list & list::operator = ( const list & _obj )
    {
        this->base::operator = ( _obj );

        return *this;
    }
    //////////////////////////////////////////////////////////////////////////
    list & list::operator = ( list && _obj )
    {
        this->base::operator = ( std::move( _obj ) );

        return *this;
    }
    //////////////////////////////////////////////////////////////////////////
    detail::extract_operator_t list::operator [] ( size_type _index ) const
    {
        PyObject * py_item = m_kernel->list_getitem( m_obj, _index );

        return detail::extract_operator_t( m_kernel, py_item );
    }
    //////////////////////////////////////////////////////////////////////////
    detail::set_list_operator_t list::operator [] ( size_type _index )
    {
        return detail::set_list_operator_t( m_kernel, m_obj, _index );
    }
    //////////////////////////////////////////////////////////////////////////
    list & list::append( const pybind::object & _item )
    {
        PyObject * py_item = _item.ptr();

        m_kernel->list_appenditem( m_obj, py_item );

        return *this;
    }
    //////////////////////////////////////////////////////////////////////////
    list & list::append_i( const detail::import_operator_t & _t )
    {
        pybind::list_appenditem_i( m_kernel, m_obj, _t );

        return *this;
    }
    //////////////////////////////////////////////////////////////////////////
    list::size_type list::size() const
    {
        return m_kernel->list_size( m_obj );
    }
    //////////////////////////////////////////////////////////////////////////
    bool list::empty() const
    {
        size_type size = this->size();

        return size == 0;
    }
    //////////////////////////////////////////////////////////////////////////
    list::const_iterator list::begin() const
    {
        return const_iterator( m_kernel, m_obj, 0 );
    }
    //////////////////////////////////////////////////////////////////////////
    list::const_iterator list::end() const
    {
        size_type size = this->size();

        return const_iterator( m_kernel, m_obj, size );
    }
    //////////////////////////////////////////////////////////////////////////
    pybind::list make_invalid_list_t()
    {
        return pybind::list( pybind::invalid );
    }
    //////////////////////////////////////////////////////////////////////////
    pybind::list extract_specialized<pybind::list>::operator () ( kernel_interface * _kernel, PyObject * _obj ) const
    {
        pybind::list value( pybind::invalid );
        pybind::extract_value( _kernel, _obj, value, true );

        return value;
    }
    //////////////////////////////////////////////////////////////////////////
}