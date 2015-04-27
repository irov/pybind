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
		
		explicit tuple( PyObject * _obj )
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
	//////////////////////////////////////////////////////////////////////////	
	inline pybind::tuple make_tuple_t()
	{
		PyObject * py_tuple = pybind::tuple_new( 0 );

		if( py_tuple == nullptr )
		{
			pybind::throw_exception( "make_tuple_t 0" );
		}

		return pybind::tuple( py_tuple );
	}
	//////////////////////////////////////////////////////////////////////////
	template<class T0>
	inline pybind::tuple make_tuple_t( const T0 & _t0 )
	{
		PyObject * py_tuple = pybind::tuple_new( 1 );

		if( py_tuple == nullptr )
		{
			pybind::throw_exception( "make_tuple_t 1" );
		}

		pybind::tuple_setitem_t( py_tuple, 0, _t0 );

		return pybind::tuple(py_tuple);
	}
	//////////////////////////////////////////////////////////////////////////
	template<class T0, class T1>
	inline pybind::tuple make_tuple_t( const T0 & _t0, const T1 & _t1 )
	{
		PyObject * py_tuple = pybind::tuple_new( 2 );

		if( py_tuple == nullptr )
		{
			pybind::throw_exception( "make_tuple_t 2" );
		}

		pybind::tuple_setitem_t( py_tuple, 0, _t0 );
		pybind::tuple_setitem_t( py_tuple, 1, _t1 );

		return pybind::tuple(py_tuple);
	}
	//////////////////////////////////////////////////////////////////////////
	template<class T0, class T1, class T2>
	inline pybind::tuple make_tuple_t( const T0 & _t0, const T1 & _t1, const T2 & _t2 )
	{
		PyObject * py_tuple = pybind::tuple_new( 3 );

		if( py_tuple == nullptr )
		{
			pybind::throw_exception( "make_tuple_t 3" );
		}

		pybind::tuple_setitem_t( py_tuple, 0, _t0 );
		pybind::tuple_setitem_t( py_tuple, 1, _t1 );
		pybind::tuple_setitem_t( py_tuple, 2, _t2 );

		return pybind::tuple(py_tuple);
	}
	//////////////////////////////////////////////////////////////////////////
	template<class T0, class T1, class T2, class T3>
	inline pybind::tuple make_tuple_t( const T0 & _t0, const T1 & _t1, const T2 & _t2, const T3 & _t3 )
	{
		PyObject * py_tuple = pybind::tuple_new( 4 );

		if( py_tuple == nullptr )
		{
			pybind::throw_exception( "make_tuple_t 4" );			
		}

		pybind::tuple_setitem_t( py_tuple, 0, _t0 );
		pybind::tuple_setitem_t( py_tuple, 1, _t1 );
		pybind::tuple_setitem_t( py_tuple, 2, _t2 );
		pybind::tuple_setitem_t( py_tuple, 3, _t3 );

		return pybind::tuple(py_tuple);
	}
	//////////////////////////////////////////////////////////////////////////
	inline bool extract_value( PyObject * _obj, pybind::tuple & _value )
	{ 
		if( pybind::tuple_check( _obj ) == false )
		{
			return false;
		}

		_value = pybind::tuple( _obj );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	inline PyObject * ptr_throw( const pybind::tuple & _value )
	{
		PyObject * obj = _value.ptr();

		pybind::incref( obj );

		return obj;
	}
}