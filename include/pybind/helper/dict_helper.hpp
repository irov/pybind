#pragma once

#include "pybind/object.hpp"

namespace pybind
{
    //////////////////////////////////////////////////////////////////////////
    namespace detail
    {
        //////////////////////////////////////////////////////////////////////////
        class PYBIND_API set_dict_operator_t
        {
        public:
            set_dict_operator_t( kernel_interface * _kernel, PyObject * _dict, PyObject * _key );
            set_dict_operator_t( const set_dict_operator_t & _r );
            ~set_dict_operator_t();

        public:
            set_dict_operator_t & operator = ( const import_operator_t & _value );

            template<class T>
            set_dict_operator_t & operator = ( const T & _value )
            {
                return this->operator = ( import_operator_t( m_kernel, _value ) );
            }

        public:
            operator PyObject * ();

        public:
            template<class T>
            operator T ()
            {
                return pybind::dict_get_t( m_kernel, m_dict, m_key );
            }

            template<class T>
            bool operator == ( const T & _value )
            {
                return pybind::dict_get_t( m_kernel, m_dict, m_key ) == _value;
            }

        protected:
            kernel_interface * m_kernel;

            PyObject * m_dict;
            PyObject * m_key;
        };
    }
}