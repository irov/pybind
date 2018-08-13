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
        typedef uint32_t size_type;

    public:
        dict( const dict & _dict );
        dict( dict && _dict );

    public:
        explicit dict( pybind::invalid );
        explicit dict( kernel_interface * _kernel );
        dict( kernel_interface * _kernel, size_type _presized );
        dict( kernel_interface * _kernel, PyObject * _obj, pybind::borrowed );
        dict( kernel_interface * _kernel, PyObject * _obj );

    public:
        ~dict();

    public:
        dict & operator = ( const dict & _obj );
        dict & operator = ( dict && _obj );

    public:
        typedef dict_iterator iterator;

    public:
        iterator begin() const;
        iterator end() const;

    public:
        template<class T>
        bool exist( const T & _name ) const
        {
            return this->exist_i( detail::import_operator_t( m_kernel, _name ) );
        }

        template<class T>
        detail::extract_operator_t get( const T & _name ) const
        {
            return this->get_i( detail::import_operator_t( m_kernel, _name ) );
        }

    public:
        template<class T>
        detail::set_dict_operator_t operator [] ( const T & _name )
        {
            return this->set_i( detail::import_operator_t( m_kernel, _name ) );
        }

        template<class T>
        detail::extract_operator_t operator [] ( const T & _name ) const
        {
            return this->get_i( detail::import_operator_t( m_kernel, _name ) );
        }

    public:
        template<class T>
        void remove( const T & _name ) const
        {
            this->remove_i( detail::import_operator_t( m_kernel, _name ) );
        }

    public:
        bool exist_i( const detail::import_operator_t & _name ) const;
        detail::extract_operator_t get_i( const detail::import_operator_t & _name ) const;
        detail::set_dict_operator_t set_i( const detail::import_operator_t & _name );

    public:
        void remove_i( const detail::import_operator_t & _name ) const;

    public:
        size_type size() const;
        bool empty() const;
    };
    //////////////////////////////////////////////////////////////////////////
    PYBIND_API pybind::dict make_invalid_dict_t();
}