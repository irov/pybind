#pragma once

#include "pybind/object.hpp"

#include "pybind/dict.hpp"

namespace pybind
{
    class PYBIND_API module
        : public object
    {
    public:
        module();

        module( const base & _base );
        module( const object & _obj );
        module( const module & _module );

    public:
        module( kernel_interface * _kernel, PyObject * _obj, pybind::borrowed );
        module( kernel_interface * _kernel, PyObject * _obj );

    public:
        template<class T>
        bool has_attr( const T & _name ) const
        {
            return this->has_attr_i( detail::import_operator_t( m_kernel, _name ) );
        }

        template<class T>
        detail::extract_operator_t get_attr( const T & _name ) const
        {
            return this->get_attr_i( detail::import_operator_t( m_kernel, _name ) );
        }

    public:
        bool has_attr_i( const detail::import_operator_t & _name ) const;
        detail::extract_operator_t get_attr_i( const detail::import_operator_t & _name ) const;

    public:
        pybind::dict get_dict() const;
    };
}