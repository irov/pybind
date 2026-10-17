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
        set_list_operator_t & set_list_operator_t::operator = ( const set_list_operator_t & _op )
        {
            // As a list item: assigning another item stores its value.
            PyObject * value = _op.m_kernel->list_getitem( _op.m_list, _op.m_index );

            pybind::list_setitem_i( m_kernel, m_list, m_index, detail::import_operator_t( m_kernel, value ) );

            return *this;
        }
        //////////////////////////////////////////////////////////////////////////
        set_list_operator_t & set_list_operator_t::operator = ( detail::import_operator_t && _imp )
        {
            pybind::list_setitem_i( m_kernel, m_list, m_index, std::forward<detail::import_operator_t>( _imp ) );

            return *this;
        }
        //////////////////////////////////////////////////////////////////////////
        set_list_operator_t::operator PyObject * () const
        {
            PyObject * obj = m_kernel->list_getitem( m_list, m_index );

            return obj;
        }
    }
}

