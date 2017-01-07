#	pragma once

#	include "pybind/types.hpp"

namespace pybind
{
    class borrowed
    {
    };

    class invalid
    {
    };

	class base
	{
    public:
        base();
        base( const base & _obj );

    public:
        explicit base( PyObject * _obj, borrowed );
        explicit base( PyObject * _obj );

        explicit base( invalid );

        base & operator = ( const base & _obj );

        ~base();

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
        PyObject * m_obj;
	};
}