#pragma once

#include "pybind/types.hpp"
#include "pybind/helper.hpp"

namespace pybind
{
    class PYBIND_API dict_pair_value
    {
    public:
        typedef uint32_t size_type;

    public:
        dict_pair_value( kernel_interface * _kernel, PyObject * _key, PyObject * _value );

    public:
        dict_pair_value( const dict_pair_value & _r );
        ~dict_pair_value();

    public:
        dict_pair_value & operator = ( const dict_pair_value & _r );

    public:
        bool operator == ( const dict_pair_value & _it ) const;
        bool operator != ( const dict_pair_value & _it ) const;

    public:
        detail::extract_operator_t key() const;
        detail::extract_operator_t value() const;

    protected:
        kernel_interface * m_kernel;

        PyObject * m_key;
        PyObject * m_value;
    };
}
