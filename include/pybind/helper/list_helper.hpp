#pragma once

#include "pybind/helper.hpp"

namespace pybind
{
    //////////////////////////////////////////////////////////////////////////
    namespace detail
    {
        //////////////////////////////////////////////////////////////////////////
        class PYBIND_API set_list_operator_t
        {
        public:
            set_list_operator_t( kernel_interface * _kernel, PyObject * _list, size_t _index );
            set_list_operator_t( set_list_operator_t && _r );
            ~set_list_operator_t();

        public:
            set_list_operator_t & operator = ( set_list_operator_t && _op );
            set_list_operator_t & operator = ( import_operator_t && _imp );

            template<class T>
            void operator = ( T && _value )
            {
                pybind::list_setitem_i( m_kernel, m_list, m_index, import_operator_t( m_kernel, std::forward<T>( _value ) ) );
            }

        public:
            operator PyObject * ();

        public:
            template<class T>
            operator T ()
            {
                return pybind::list_getitem_t( m_kernel, m_list, m_index );
            }

            template<class T>
            bool operator == ( const T & _value ) const
            {
                return pybind::list_getitem_t( m_kernel, m_list, m_index ) == _value;
            }

        protected:
            kernel_interface * m_kernel;
            PyObject * m_list;
            size_t m_index;
        };
    }
}

