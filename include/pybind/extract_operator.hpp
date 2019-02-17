#pragma once

#include "pybind/import_operator.hpp"

namespace pybind
{
    //////////////////////////////////////////////////////////////////////////
    namespace detail
    {
        //////////////////////////////////////////////////////////////////////////
        class extract_operator_t
        {
        public:
            extract_operator_t( const extract_operator_t & _r )
                : m_kernel( _r.m_kernel )
                , m_obj( _r.m_obj )
            {
                if( m_kernel != nullptr )
                {
                    m_kernel->incref( m_obj );
                }
            }

            extract_operator_t( extract_operator_t && _op )
                : m_kernel( _op.m_kernel )
                , m_obj( _op.m_obj )
            {
                _op.m_kernel = nullptr;
                _op.m_obj = nullptr;
            }

            extract_operator_t( kernel_interface * _kernel, PyObject * _obj )
                : m_kernel( _kernel )
                , m_obj( _obj )
            {
                if( m_kernel != nullptr )
                {
                    m_kernel->incref( m_obj );
                }
            }

            extract_operator_t( kernel_interface * _kernel, PyObject * _obj, pybind::borrowed_t )
                : m_kernel( _kernel )
                , m_obj( _obj )
            {
            }

            ~extract_operator_t()
            {
                if( m_kernel != nullptr )
                {
                    m_kernel->decref( m_obj );
                }
            }

        public:
            PyObject * ptr() const
            {
                return m_obj;
            }

            kernel_interface * kernel() const
            {
                return m_kernel;
            }

        public:
            operator PyObject * () const
            {
                return m_obj;
            }

            template<class T>
            operator T ()
            {
                return pybind::extract<T>( m_kernel, m_obj );
            }

            operator import_operator_t ()
            {
                return import_operator_t( m_kernel, m_obj );
            }

            template<class T>
            bool operator == ( const T & _value )
            {
                return pybind::extract<T>( m_kernel, m_obj ) == _value;
            }

        protected:
            kernel_interface * m_kernel;
            PyObject * m_obj;

        private:
            extract_operator_t & operator = ( const extract_operator_t & );
        };
    }
}