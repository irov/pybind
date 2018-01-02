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
        class return_operator_t
        {
        public:
            return_operator_t( const return_operator_t & _op )
                : m_kernel( _op.m_kernel )
                , m_obj( _op.m_obj )
            {
            }

            return_operator_t( kernel_interface * _kernel, PyObject * _value )
                : m_kernel( _kernel )
                , m_obj( _value )
            {
            }

            template<class T>
            return_operator_t( kernel_interface * _kernel, const T & _value )
                : m_kernel( _kernel )
                , m_obj( ptr_throw_specialized<T>()(_kernel, _value) )
            {
            }

        public:
            operator PyObject * () const
            {
                return m_obj;
            }

        protected:
            kernel_interface * m_kernel;
            PyObject * m_obj;

        private:
            return_operator_t & operator = ( const return_operator_t & );
        };
    }
}