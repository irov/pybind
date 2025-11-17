#include "pybind/helper/list_helper.hpp"

namespace pybind
{
    //////////////////////////////////////////////////////////////////////////
    namespace detail
    {
        //////////////////////////////////////////////////////////////////////////
        set_list_operator_t::set_list_operator_t( kernel_interface * _kernel, PyObject * _list, size_t _index )
            : m_kernel( _kernel )
            , m_list( _list )
            , m_index( _index )
        {
        }
        //////////////////////////////////////////////////////////////////////////
        set_list_operator_t::set_list_operator_t( set_list_operator_t && _r )
            : m_kernel( _r.m_kernel )
            , m_list( _r.m_list )
            , m_index( _r.m_index )
        {
        }
        //////////////////////////////////////////////////////////////////////////
        set_list_operator_t::~set_list_operator_t()
        {
        }
        //////////////////////////////////////////////////////////////////////////
        set_list_operator_t & set_list_operator_t::operator = ( set_list_operator_t && _op )
        {
            m_kernel = _op.m_kernel;
            m_list = _op.m_list;
            m_index = _op.m_index;

            return *this;
        }
        //////////////////////////////////////////////////////////////////////////
        set_list_operator_t & set_list_operator_t::operator = ( detail::import_operator_t && _imp )
        {
            pybind::list_setitem_i( m_kernel, m_list, m_index, std::forward<detail::import_operator_t>( _imp ) );

            return *this;
        }
        //////////////////////////////////////////////////////////////////////////
        set_list_operator_t::operator PyObject * ()
        {
            PyObject * obj = m_kernel->list_getitem( m_list, m_index );

            return obj;
        }
    }
}

