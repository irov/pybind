#include "pybind/dict.hpp"

namespace pybind
{
    //////////////////////////////////////////////////////////////////////////
    dict::dict( kernel_interface * _kernel )
        : pybind::object( _kernel, _kernel->dict_new(), pybind::borrowed )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    dict::dict( const dict & _dict )
        : pybind::object( _dict.kernel(), _dict.ptr() )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    dict::dict( dict && _dict )
        : pybind::object( std::move( _dict ) )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    dict::dict( pybind::invalid_t _iv )
        : object( _iv )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    dict::dict( kernel_interface * _kernel, dict::size_type _presized )
        : pybind::object( _kernel, _kernel->dict_new_presized( _presized ), pybind::borrowed )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    dict::dict( kernel_interface * _kernel, PyObject * _obj, pybind::borrowed_t _br )
        : pybind::object( _kernel, _obj, _br )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    dict::dict( kernel_interface * _kernel, PyObject * _obj )
        : pybind::object( _kernel, _obj )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    dict::~dict()
    {
    }
    //////////////////////////////////////////////////////////////////////////    
    dict & dict::operator = ( const dict & _obj )
    {
        this->base::operator = ( _obj );

        return *this;
    }
    //////////////////////////////////////////////////////////////////////////
    dict & dict::operator = ( dict && _obj )
    {
        this->base::operator = ( std::move( _obj ) );

        return *this;
    }
    //////////////////////////////////////////////////////////////////////////
    dict::iterator dict::begin() const
    {
        return dict_iterator( m_kernel, m_obj );
    }
    //////////////////////////////////////////////////////////////////////////
    dict::iterator dict::end() const
    {
        return dict::iterator( m_kernel, m_obj, t_dict_iterator_end_tag() );
    }
    //////////////////////////////////////////////////////////////////////////
    bool dict::exist_i( const detail::import_operator_t & _name ) const
    {
        return m_kernel->dict_exist( m_obj, _name );
    }
    //////////////////////////////////////////////////////////////////////////
    detail::extract_operator_t dict::get_i( const detail::import_operator_t & _name ) const
    {
        PyObject * py_attr = m_kernel->dict_get( m_obj, _name );

        return detail::extract_operator_t( m_kernel, py_attr );
    }
    //////////////////////////////////////////////////////////////////////////
    detail::set_dict_operator_t dict::set_i( const detail::import_operator_t & _name )
    {
        return detail::set_dict_operator_t( m_kernel, m_obj, _name );
    }
    //////////////////////////////////////////////////////////////////////////
    void dict::remove_i( const detail::import_operator_t & _name ) const
    {
        m_kernel->dict_remove( m_obj, _name );
    }
    //////////////////////////////////////////////////////////////////////////
    dict::size_type dict::size() const
    {
        return m_kernel->dict_size( m_obj );
    }
    //////////////////////////////////////////////////////////////////////////
    bool dict::empty() const
    {
        return this->size() == 0;
    }
    //////////////////////////////////////////////////////////////////////////
    pybind::dict make_dict_t( pybind::kernel_interface * _kernel )
    {
        return pybind::dict( _kernel );
    }
    //////////////////////////////////////////////////////////////////////////
    pybind::dict make_invalid_dict_t()
    {
        return pybind::dict( pybind::invalid );
    }
    //////////////////////////////////////////////////////////////////////////
    pybind::dict extract_specialized<pybind::dict>::operator () ( kernel_interface * _kernel, PyObject * _obj ) const
    {
        pybind::dict value( pybind::invalid );

        if( pybind::extract_value( _kernel, _obj, value, true ) == false )
        {
            const std::type_info & tinfo = typeid(pybind::dict);

            const char * type_name = tinfo.name();

            _kernel->log( "extract_value<T>: extract invalid '%s:%s' not cast to '%s'"
                , _kernel->object_repr( _obj ).c_str()
                , _kernel->object_repr_type( _obj ).c_str()
                , type_name
            );
        }

        return value;
    }
    //////////////////////////////////////////////////////////////////////////
}