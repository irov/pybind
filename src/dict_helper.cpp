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
            if( m_key != nullptr )
            {
                m_kernel->incref( m_key );
            }
        }
        //////////////////////////////////////////////////////////////////////////
        set_dict_operator_t::set_dict_operator_t( const set_dict_operator_t & _r )
            : m_kernel( _r.m_kernel )
            , m_dict( _r.m_dict )
            , m_key( _r.m_key )
        {
            if( m_key != nullptr )
            {
                m_kernel->incref( m_key );
            }
        }
        //////////////////////////////////////////////////////////////////////////
        set_dict_operator_t::set_dict_operator_t( set_dict_operator_t && _r )
            : m_kernel( _r.m_kernel )
            , m_dict( _r.m_dict )
            , m_key( _r.m_key )
        {
            _r.m_kernel = nullptr;
            _r.m_dict = nullptr;
            _r.m_key = nullptr;
        }
        //////////////////////////////////////////////////////////////////////////
        set_dict_operator_t::~set_dict_operator_t()
        {
            if( m_key != nullptr )
            {
                m_kernel->decref( m_key );
            }
        }
        //////////////////////////////////////////////////////////////////////////
        set_dict_operator_t & set_dict_operator_t::operator = ( set_dict_operator_t && _op )
        {
            // As a dict item: assigning another item stores its value.
            PyObject * value = _op;

            pybind::dict_setobject_i( m_kernel, m_dict, m_key, import_operator_t( m_kernel, value ) );

            return *this;
        }
        //////////////////////////////////////////////////////////////////////////
        set_dict_operator_t & set_dict_operator_t::operator = ( import_operator_t && _op )
        {
            pybind::dict_setobject_i( m_kernel, m_dict, m_key, std::forward<import_operator_t>( _op ) );

            return *this;
        }
        //////////////////////////////////////////////////////////////////////////
        set_dict_operator_t::operator PyObject * () const
        {
            PyObject * py_object = m_kernel->dict_get( m_dict, m_key );

            // As dict_get_i and operator T: a missing key reads as None.
            if( py_object == nullptr )
            {
                PyObject * py_none = m_kernel->get_none();

                return py_none;
            }

            return py_object;
        }
        //////////////////////////////////////////////////////////////////////////
    }
}