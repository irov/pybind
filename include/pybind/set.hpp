#pragma once

#include "pybind/object.hpp"

namespace pybind
{
    class PYBIND_API set
        : public pybind::object
    {
    public:
        typedef size_t size_type;

    public:
        set();
        set( const object & _dict );
        set( object && _dict );
        set( const set & _dict );
        set( set && _dict );

    public:
        explicit set( pybind::invalid_t );
        explicit set( kernel_interface * _kernel );
        set( kernel_interface * _kernel, PyObject * _obj, pybind::borrowed_t );
        set( kernel_interface * _kernel, PyObject * _obj );

    public:
        ~set();

    public:
        set & operator = ( std::nullptr_t );
        set & operator = ( const set & _obj );
        set & operator = ( set && _obj );

    public:
        template<class T>
        bool exist( const T & _value ) const
        {
            return this->exist_i( detail::import_operator_t( m_kernel, _value ) );
        }

        template<class V>
        void add( const V & _value )
        {
            this->add_i( detail::import_operator_t( m_kernel, _value ) );
        }

    public:
        template<class T>
        void remove( const T & _value ) const
        {
            this->remove_i( detail::import_operator_t( m_kernel, _value ) );
        }

    public:
        bool exist_i( detail::import_operator_t && _value ) const;
        bool add_i( detail::import_operator_t && _value );
        bool remove_i( detail::import_operator_t && _value ) const;

    public:
        size_type size() const;
        bool empty() const;
    };
    //////////////////////////////////////////////////////////////////////////
    PYBIND_API pybind::set make_set_t( pybind::kernel_interface * _kernel );
    PYBIND_API pybind::set make_invalid_set_t();
    //////////////////////////////////////////////////////////////////////////
    template<>
    struct extract_specialized<pybind::set>
    {
        pybind::set operator () ( kernel_interface * _kernel, PyObject * _obj ) const;
    };
    //////////////////////////////////////////////////////////////////////////
}