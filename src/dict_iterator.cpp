#include "pybind/helper/dict_iterator.hpp"

namespace pybind
{
    //////////////////////////////////////////////////////////////////////////
    dict_iterator::dict_iterator( kernel_interface * _kernel, PyObject * _obj )
        : m_kernel( _kernel )
        , m_obj( _obj )
        , m_pos( 0 )
        , m_key( nullptr )
        , m_value( nullptr )
    {
        this->operator ++();
    }
    //////////////////////////////////////////////////////////////////////////
    dict_iterator::dict_iterator( kernel_interface * _kernel, PyObject * _obj, t_dict_iterator_end_tag )
        : m_kernel( _kernel )
        , m_obj( _obj )
        , m_pos( ~0u )
        , m_key( nullptr )
        , m_value( nullptr )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    dict_iterator::dict_iterator( const dict_iterator & _r )
        : m_kernel( _r.m_kernel )
        , m_obj( _r.m_obj )
        , m_pos( _r.m_pos )
        , m_key( _r.m_key )
        , m_value( _r.m_value )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    dict_iterator::~dict_iterator()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    dict_iterator & dict_iterator::operator = ( const dict_iterator & _r )
    {
        this->m_kernel = _r.m_kernel;
        this->m_obj = _r.m_obj;
        this->m_pos = _r.m_pos;
        this->m_key = _r.m_key;
        this->m_value = _r.m_value;

        return *this;
    }
    //////////////////////////////////////////////////////////////////////////
    dict_pair_value dict_iterator::operator -> () const
    {
        return dict_pair_value( m_kernel, m_key, m_value );
    }
    //////////////////////////////////////////////////////////////////////////
    dict_pair_value dict_iterator::operator * () const
    {
        return dict_pair_value( m_kernel, m_key, m_value );
    }
    //////////////////////////////////////////////////////////////////////////
    bool dict_iterator::operator == ( const dict_iterator & _it ) const
    {
        if( m_obj != _it.m_obj )
        {
            return false;
        }

        if( m_pos != _it.m_pos )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool dict_iterator::operator != ( const dict_iterator & _it ) const
    {
        return !this->operator ==( _it );
    }
    //////////////////////////////////////////////////////////////////////////        
    dict_iterator & dict_iterator::operator ++ ()
    {
        if( m_kernel->dict_next( m_obj, m_pos, &m_key, &m_value ) == false )
        {
            m_pos = ~0u;
        }

        return *this;
    }
    //////////////////////////////////////////////////////////////////////////
    dict_iterator dict_iterator::operator ++ ( int )
    {
        dict_iterator tmp = *this;
        ++*this;

        return tmp;
    }
    //////////////////////////////////////////////////////////////////////////
    detail::extract_operator_t dict_iterator::key() const
    {
        return detail::extract_operator_t( m_kernel, m_key );
    }
    //////////////////////////////////////////////////////////////////////////
    detail::extract_operator_t dict_iterator::value() const
    {
        return detail::extract_operator_t( m_kernel, m_value );
    }
}
