#pragma once

#include "pybind/object.hpp"
#include "pybind/helper/dict_helper.hpp"
#include "pybind/helper/dict_iterator.hpp"

namespace pybind
{
    class PYBIND_API dict
        : public pybind::object
    {
    public:
        typedef size_t size_type;

    public:
        dict();
        dict( const object & _dict );
        dict( object && _dict );
        dict( const dict & _dict );
        dict( dict && _dict );

    public:
        explicit dict( pybind::invalid_t );
        explicit dict( kernel_interface * _kernel );
        dict( kernel_interface * _kernel, size_type _presized );
        dict( kernel_interface * _kernel, PyObject * _obj, pybind::borrowed_t );
        dict( kernel_interface * _kernel, PyObject * _obj );

    public:
        ~dict();

    public:
        dict & operator = ( std::nullptr_t );
        dict & operator = ( const dict & _obj );
        dict & operator = ( dict && _obj );

    public:
        typedef dict_iterator iterator;

    public:
        iterator begin() const;
        iterator end() const;

    public:
        template<class T>
        bool exist( const T & _key ) const
        {
            return this->exist_i( detail::import_operator_t( m_kernel, _key ) );
        }

        template<class T>
        detail::extract_operator_t get( const T & _key ) const
        {
            return this->get_i( detail::import_operator_t( m_kernel, _key ) );
        }

        template<class K, class T>
        T get_default( const K & _key, const T & _default ) const
        {
            if( this->exist( _key ) == false )
            {
                return _default;
            }

            T value = this->get_i( detail::import_operator_t( m_kernel, _key ) );

            return value;
        }

        template<class K, class V>
        void set( const K & _key, const V & _value )
        {
            this->set_i( detail::import_operator_t( m_kernel, _key ) ) = _value;
        }

    public:
        template<class T>
        detail::set_dict_operator_t operator [] ( const T & _key )
        {
            return this->set_i( detail::import_operator_t( m_kernel, _key ) );
        }

        template<class T>
        detail::extract_operator_t operator [] ( const T & _key ) const
        {
            return this->get_i( detail::import_operator_t( m_kernel, _key ) );
        }

    public:
        template<class T>
        void remove( const T & _key ) const
        {
            this->remove_i( detail::import_operator_t( m_kernel, _key ) );
        }

    public:
        bool exist_i( const detail::import_operator_t & _key ) const;
        detail::extract_operator_t get_i( const detail::import_operator_t & _key ) const;
        detail::set_dict_operator_t set_i( const detail::import_operator_t & _key );
        bool remove_i( const detail::import_operator_t & _key ) const;

    public:
        size_type size() const;
        bool empty() const;
    };
    //////////////////////////////////////////////////////////////////////////
    PYBIND_API pybind::dict make_dict_t( pybind::kernel_interface * _kernel );
    PYBIND_API pybind::dict make_invalid_dict_t();
    //////////////////////////////////////////////////////////////////////////
    template<>
    struct extract_specialized<pybind::dict>
    {
        pybind::dict operator () ( kernel_interface * _kernel, PyObject * _obj ) const;
    };
    //////////////////////////////////////////////////////////////////////////
}