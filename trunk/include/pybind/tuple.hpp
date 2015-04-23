#	pragma once

#	include "pybind/object.hpp"

namespace pybind
{
	class tuple
		: public pybind::object
	{
	public:
		tuple()
		{
		}

		tuple( PyObject * _obj )
			: pybind::object( _obj )
		{
		}

	public:
		detail::extract_operator_t operator [] ( size_t _index )
		{
			return pybind::tuple_getitem_t( m_obj, _index );
		}

	public:
		size_t size() const
		{
			return pybind::tuple_size( m_obj );
		}

		bool empty() const
		{
			return this->size() == 0;
		}
	};

	inline bool extract_value( PyObject * _obj, pybind::tuple & _value )
	{ 
		_value = pybind::tuple( _obj );

		return true;
	}
}