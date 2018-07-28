#include "pybind/list.hpp"

namespace pybind
{
    //////////////////////////////////////////////////////////////////////////
    list::list( kernel_interface * _kernel )
        : pybind::object( _kernel, pybind::list_new( 0 ), pybind::borrowed() )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    list::list( const list & _list )
        : pybind::object( _list.kernel(), _list.ptr() )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    list::list( pybind::invalid _iv )
        : object( _iv )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    list::list( kernel_interface * _kernel, size_type _size )
        : pybind::object( _kernel, pybind::list_new( _size ), pybind::borrowed() )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    list::list( kernel_interface * _kernel, PyObject * _obj, pybind::borrowed _br )
        : pybind::object( _kernel, _obj, _br )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    list::list( kernel_interface * _kernel, PyObject * _obj )
        : pybind::object( _kernel, _obj )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    detail::extract_operator_t list::operator [] ( size_type _index ) const
    {
        PyObject * py_item = pybind::list_getitem( m_obj, _index );

        return detail::extract_operator_t( m_kernel, py_item );
    }
    //////////////////////////////////////////////////////////////////////////
    detail::set_list_operator_t list::operator [] ( size_type _index )
    {
        return detail::set_list_operator_t( m_kernel, m_obj, _index );
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
        return pybind::list_size( m_obj );
    }
    //////////////////////////////////////////////////////////////////////////
    bool list::empty() const
    {
        return this->size() == 0;
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
    bool list_check_t( const pybind::object & _obj )
    {
        return pybind::list_check( _obj.ptr() );
    }
    //////////////////////////////////////////////////////////////////////////
    pybind::list make_invalid_list_t()
    {
        return pybind::list( invalid() );
    }
    //////////////////////////////////////////////////////////////////////////
    pybind::list extract_specialized<pybind::list>::operator () ( kernel_interface * _kernel, PyObject * _obj ) const
    {
        pybind::list value( _kernel );

        if( pybind::extract_value( _kernel, _obj, value, true ) == false )
        {
            const std::type_info & tinfo = typeid(pybind::list);

            const char * type_name = tinfo.name();

            pybind::log( "extract_value<T>: extract invalid %s:%s not cast to '%s'"
                , pybind::object_repr( _obj )
                , pybind::object_repr_type( _obj )
                , type_name
            );
        }

        return value;
    }
}