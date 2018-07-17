#pragma once

#include "pybind/types.hpp"

namespace pybind
{
	class kernel_interface;

	class base
	{
    public:
		base();
        base( const base & _base );

    public:		        
		base( kernel_interface * _kernel, PyObject * _obj );
        base( kernel_interface * _kernel, PyObject * _obj, pybind::borrowed );

		explicit base( pybind::invalid );

        base & operator = ( const base & _obj );

        ~base();

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
        bool is_callable() const;

    public:
        void unwrap() const;

    public:
        const char * repr() const;

    protected:
		kernel_interface * m_kernel;
        PyObject * m_obj;
	};
}