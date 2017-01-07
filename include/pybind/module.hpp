#	pragma once

#	include "pybind/object.hpp"

#	include "pybind/dict.hpp"

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
		explicit module( PyObject * _obj, pybind::borrowed );
		explicit module( PyObject * _obj );

	public:
		bool has_attr( const detail::import_operator_t & _name ) const;
		pybind::object get_attr( const detail::import_operator_t & _name ) const;

	public:
		pybind::dict get_dict() const;
	};
}