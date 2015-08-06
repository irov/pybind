#	pragma once

#	include "pybind/object.hpp"

namespace pybind
{
	class dict
		: public pybind::object
	{
	public:		
		dict()
		{
			m_obj = pybind::dict_new();
		}
		
		explicit dict( PyObject * _obj )
			: pybind::object( _obj )
		{
		}

	public:
		bool contains( const char * _name ) const
		{
			return pybind::dict_contains( m_obj, _name );
		}

		detail::extract_operator_t operator [] ( const char * _name ) const
		{
			PyObject * py_object = pybind::dict_get( m_obj, _name );

			return detail::extract_operator_t(py_object);
		}

		void remove( const char * _name ) const
		{
			pybind::dict_remove( m_obj, _name );
		}

	public:
		size_t size() const
		{
			return pybind::dict_size( m_obj );
		}

		bool empty() const
		{
			return this->size() == 0;
		}
	};
	//////////////////////////////////////////////////////////////////////////
	inline pybind::dict make_invalid_dict_t()
	{
		return pybind::dict( nullptr );
	}
	//////////////////////////////////////////////////////////////////////////
	inline bool extract_value( PyObject * _obj, pybind::dict & _value )
	{ 
		if( _obj == nullptr )
		{
			return false;
		}

		if( pybind::dict_check( _obj ) == false )
		{
			return false;
		}

		_value = pybind::dict( _obj );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	inline PyObject * ptr_throw( const pybind::dict & _value )
	{
		PyObject * obj = _value.ptr();

		pybind::incref( obj );

		return obj;
	}
}