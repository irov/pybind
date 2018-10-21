#include "pybind/helper/dict_helper.hpp"

namespace pybind
{
    //////////////////////////////////////////////////////////////////////////
    namespace detail
    {
        //////////////////////////////////////////////////////////////////////////
        set_dict_operator_t::set_dict_operator_t( kernel_interface * _kernel, PyObject * _dict, PyObject * _key )
            : m_kernel( _kernel )
            , m_dict( _dict )
            , m_key( _key )
        {
        }
        //////////////////////////////////////////////////////////////////////////
        set_dict_operator_t::set_dict_operator_t( const set_dict_operator_t & _r )
            : m_kernel( _r.m_kernel )
            , m_dict( _r.m_dict )
            , m_key( _r.m_key )
        {
        }
        //////////////////////////////////////////////////////////////////////////
        set_dict_operator_t::~set_dict_operator_t()
        {
        }
        //////////////////////////////////////////////////////////////////////////
        set_dict_operator_t & set_dict_operator_t::operator = ( const import_operator_t & _value )
        {
            pybind::dict_setobject_i( m_kernel, m_dict, import_operator_t( m_kernel, m_key ), _value );

            return *this;
        }
        //////////////////////////////////////////////////////////////////////////
        set_dict_operator_t::operator PyObject * ()
        {
            PyObject * py_object = m_kernel->dict_get( m_dict, m_key );

            return py_object;
        }
    }
}