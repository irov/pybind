#pragma once

#include "pybind/helper.hpp"

namespace pybind
{
    //////////////////////////////////////////////////////////////////////////
    namespace detail
    {
        //////////////////////////////////////////////////////////////////////////
        class set_tuple_operator_t
        {
        public:
            set_tuple_operator_t( kernel_interface * _kernel, PyObject * _tuple, uint32_t _index )
                : m_kernel( _kernel )
                , m_tuple( _tuple )
                , m_index( _index )
            {
            }

            set_tuple_operator_t( const set_tuple_operator_t & _r )
                : m_kernel( _r.m_kernel )
                , m_tuple( _r.m_tuple )
                , m_index( _r.m_index )
            {
            }

            ~set_tuple_operator_t()
            {
            }

        public:
            operator PyObject * ()
            {
                PyObject * obj = pybind::tuple_getitem( m_tuple, m_index );

                return obj;
            }

            template<class T>
            operator T ()
            {
                return pybind::tuple_getitem_t( m_kernel, m_tuple, m_index );
            }

            template<class T>
            bool operator == ( const T & _value )
            {
                return pybind::tuple_getitem_t( m_kernel, m_tuple, m_index ) == _value;
            }

        protected:
            kernel_interface * m_kernel;
            PyObject * m_tuple;
            uint32_t m_index;
        };
    }
}

