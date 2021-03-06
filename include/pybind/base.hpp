#pragma once

#include "pybind/types.hpp"
#include "pybind/string_view.hpp"

namespace pybind
{
    class kernel_interface;

    class base
    {
    public:
        base();
        base( const base & _base );
        base( base && _base );

    public:
        base( std::nullptr_t );
        base( kernel_interface * _kernel, PyObject * _obj );
        base( kernel_interface * _kernel, PyObject * _obj, pybind::borrowed_t );

        explicit base( pybind::invalid_t );

    public:
        ~base();

    public:
        base & operator = ( std::nullptr_t );
        base & operator = ( const base & _obj );
        base & operator = ( base && _obj );

    public:
        kernel_interface * kernel() const;

    public:
        PyObject * ptr() const;
        PyObject * ret() const;

        void reset();
        uint32_t get_ref() const;

    public:
        bool is_invalid() const;
        bool is_valid() const;
        bool is_wrap() const;
        bool is_class() const;
        bool is_type_class() const;
        bool is_none() const;
        bool is_bool() const;
        bool is_string() const;
        bool is_unicode() const;
        bool is_list() const;
        bool is_callable() const;

    public:
        void unwrap() const;

    public:
        string_view str() const;
        string_view repr() const;

    protected:
        kernel_interface * m_kernel;
        PyObject * m_obj;
    };
}