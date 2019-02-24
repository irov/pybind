#include "pybind/helper/dict_pair_value.hpp"

namespace pybind
{
    //////////////////////////////////////////////////////////////////////////
    dict_pair_value::dict_pair_value( kernel_interface * _kernel, PyObject * _key, PyObject * _value )
        : m_kernel( _kernel )
        , m_key( _key )
        , m_value( _value )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    dict_pair_value::dict_pair_value( const dict_pair_value & _r )
        : m_kernel( _r.m_kernel )
        , m_key( _r.m_key )
        , m_value( _r.m_value )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    dict_pair_value::~dict_pair_value()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    dict_pair_value & dict_pair_value::operator = ( const dict_pair_value & _r )
    {
        this->m_kernel = _r.m_kernel;
        this->m_key = _r.m_key;
        this->m_value = _r.m_value;

        return *this;
    }
    //////////////////////////////////////////////////////////////////////////
    bool dict_pair_value::operator == ( const dict_pair_value & _it ) const
    {
        if( m_key != _it.m_key )
        {
            return false;
        }

        if( m_value != _it.m_value )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool dict_pair_value::operator != ( const dict_pair_value & _it ) const
    {
        return !this->operator == ( _it );
    }
    //////////////////////////////////////////////////////////////////////////
    detail::extract_operator_t dict_pair_value::key() const
    {
        return detail::extract_operator_t( m_kernel, m_key );
    }
    //////////////////////////////////////////////////////////////////////////
    detail::extract_operator_t dict_pair_value::value() const
    {
        return detail::extract_operator_t( m_kernel, m_value );
    }
}
