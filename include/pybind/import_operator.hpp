#	pragma once

#	include "pybind/system.hpp"
#	include "pybind/extract.hpp"

#	include <stddef.h>

namespace pybind
{
    //////////////////////////////////////////////////////////////////////////
    namespace detail
    {
        //////////////////////////////////////////////////////////////////////////
        class import_operator_t
        {
        public:
            import_operator_t()
                : m_kernel( nullptr )
                , m_obj( nullptr )
            {
            }

            import_operator_t( const import_operator_t  & _op )
                : m_kernel( _op.m_kernel )
                , m_obj( _op.m_obj )
            {
                pybind::incref( m_obj );
            }

            import_operator_t( kernel_interface * _kernel, PyObject * _value )
                : m_kernel( _kernel )
                , m_obj( _value )
            {
                pybind::incref( m_obj );
            }

        public:
            template<class T>
            import_operator_t( kernel_interface * _kernel, const T & _value )
                : m_kernel( _kernel )
                , m_obj( ptr_throw_specialized<T>()(_kernel, _value) )
            {
            }

        public:
            ~import_operator_t()
            {
                pybind::decref( m_obj );
            }

        public:
            operator PyObject * () const
            {
                return m_obj;
            }

        public:
            PyObject * ptr() const
            {
                return m_obj;
            }

        protected:
            kernel_interface * m_kernel;
            PyObject * m_obj;

        private:
            import_operator_t & operator = ( const import_operator_t & );
        };
    }
}