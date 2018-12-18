#pragma once

#include "pybind/extract.hpp"

#include <cstddef>

namespace pybind
{
    //////////////////////////////////////////////////////////////////////////
    class object;
    //////////////////////////////////////////////////////////////////////////
    namespace detail
    {
        //////////////////////////////////////////////////////////////////////////
        class import_operator_t
        {
        public:
            import_operator_t();
            import_operator_t( const import_operator_t & _op );
            import_operator_t( import_operator_t && _op );
            import_operator_t( kernel_interface * _kernel, std::nullptr_t );
            import_operator_t( kernel_interface * _kernel, PyObject * _value );
            import_operator_t( const object & _obj );

        public:
            template<class T>
            import_operator_t( kernel_interface * _kernel, const T & _value )
                : m_kernel( _kernel )
                , m_obj( ptr_throw_specialized<T>()(_kernel, _value) )
            {
            }

        public:
            ~import_operator_t();

        public:
            operator PyObject * () const;

        public:
            PyObject * ptr() const;

        protected:
            kernel_interface * m_kernel;
            PyObject * m_obj;

        private:
            import_operator_t & operator = ( const import_operator_t & );
        };
    }
}