#	include "pybind/tuple.hpp"

namespace pybind
{
	//////////////////////////////////////////////////////////////////////////
	tuple::tuple()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	tuple::tuple( PyObject * _obj, borrowed _br )
		: pybind::object( _obj, _br )

	{
	}
	//////////////////////////////////////////////////////////////////////////
	tuple::tuple( PyObject * _obj )
		: pybind::object( _obj )
	{
	}
	//////////////////////////////////////////////////////////////////////////
	detail::extract_operator_t tuple::operator[]( size_t _index ) const
	{
		return pybind::tuple_getitem_t( m_obj, _index );
	}
	//////////////////////////////////////////////////////////////////////////
	size_t tuple::size() const
	{
		return pybind::tuple_size( m_obj );
	}
	//////////////////////////////////////////////////////////////////////////
	bool tuple::empty() const
	{
		return this->size() == 0;
	}
	//////////////////////////////////////////////////////////////////////////	
	pybind::tuple make_tuple_t()
	{
		PyObject * py_tuple = pybind::tuple_new( 0 );

		if( py_tuple == nullptr )
		{
			pybind::throw_exception( "make_tuple_t 0" );
		}

		return pybind::tuple( py_tuple, pybind::borrowed() );
	}
	//////////////////////////////////////////////////////////////////////////
	pybind::tuple make_tuple_t( const detail::import_operator_t & _t0 )
	{
		PyObject * py_tuple = pybind::tuple_new( 1 );

		if( py_tuple == nullptr )
		{
			pybind::throw_exception( "make_tuple_t 1" );
		}

		pybind::tuple_setitem_t( py_tuple, 0, _t0 );

		return pybind::tuple( py_tuple, pybind::borrowed() );
	}
	//////////////////////////////////////////////////////////////////////////
	pybind::tuple make_tuple_t( const detail::import_operator_t & _t0, const detail::import_operator_t & _t1 )
	{
		PyObject * py_tuple = pybind::tuple_new( 2 );

		if( py_tuple == nullptr )
		{
			pybind::throw_exception( "make_tuple_t 2" );
		}

		pybind::tuple_setitem_t( py_tuple, 0, _t0 );
		pybind::tuple_setitem_t( py_tuple, 1, _t1 );

		return pybind::tuple( py_tuple, pybind::borrowed() );
	}
	//////////////////////////////////////////////////////////////////////////
	pybind::tuple make_tuple_t( const detail::import_operator_t & _t0, const detail::import_operator_t & _t1, const detail::import_operator_t & _t2 )
	{
		PyObject * py_tuple = pybind::tuple_new( 3 );

		if( py_tuple == nullptr )
		{
			pybind::throw_exception( "make_tuple_t 3" );
		}

		pybind::tuple_setitem_t( py_tuple, 0, _t0 );
		pybind::tuple_setitem_t( py_tuple, 1, _t1 );
		pybind::tuple_setitem_t( py_tuple, 2, _t2 );

		return pybind::tuple( py_tuple, pybind::borrowed() );
	}
	//////////////////////////////////////////////////////////////////////////
	pybind::tuple make_tuple_t( const detail::import_operator_t & _t0, const detail::import_operator_t & _t1, const detail::import_operator_t & _t2, const detail::import_operator_t & _t3 )
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

		return pybind::tuple( py_tuple, pybind::borrowed() );
	}
	//////////////////////////////////////////////////////////////////////////
	pybind::tuple make_tuple_t( const detail::import_operator_t & _t0, const detail::import_operator_t & _t1, const detail::import_operator_t & _t2, const detail::import_operator_t & _t3, const detail::import_operator_t & _t4 )
	{
		PyObject * py_tuple = pybind::tuple_new( 5 );

		if( py_tuple == nullptr )
		{
			pybind::throw_exception( "make_tuple_t 5" );
		}

		pybind::tuple_setitem_t( py_tuple, 0, _t0 );
		pybind::tuple_setitem_t( py_tuple, 1, _t1 );
		pybind::tuple_setitem_t( py_tuple, 2, _t2 );
		pybind::tuple_setitem_t( py_tuple, 3, _t3 );
		pybind::tuple_setitem_t( py_tuple, 4, _t4 );

		return pybind::tuple( py_tuple, pybind::borrowed() );
	}
	pybind::tuple make_tuple_t( const detail::import_operator_t & _t0, const detail::import_operator_t & _t1, const detail::import_operator_t & _t2, const detail::import_operator_t & _t3, const detail::import_operator_t & _t4, const detail::import_operator_t & _t5 )
	{ 
		PyObject * py_tuple = pybind::tuple_new( 6 );

		if( py_tuple == nullptr )
		{
			pybind::throw_exception( "make_tuple_t 6" );
		}

		pybind::tuple_setitem_t( py_tuple, 0, _t0 );
		pybind::tuple_setitem_t( py_tuple, 1, _t1 );
		pybind::tuple_setitem_t( py_tuple, 2, _t2 );
		pybind::tuple_setitem_t( py_tuple, 3, _t3 );
		pybind::tuple_setitem_t( py_tuple, 4, _t4 );
		pybind::tuple_setitem_t( py_tuple, 5, _t5 );

		return pybind::tuple( py_tuple, pybind::borrowed() );
	}
	pybind::tuple make_tuple_t( const detail::import_operator_t & _t0, const detail::import_operator_t & _t1, const detail::import_operator_t & _t2, const detail::import_operator_t & _t3, const detail::import_operator_t & _t4, const detail::import_operator_t & _t5, const detail::import_operator_t & _t6 )
	{
		PyObject * py_tuple = pybind::tuple_new( 7 );

		if( py_tuple == nullptr )
		{
			pybind::throw_exception( "make_tuple_t 7" );
		}

		pybind::tuple_setitem_t( py_tuple, 0, _t0 );
		pybind::tuple_setitem_t( py_tuple, 1, _t1 );
		pybind::tuple_setitem_t( py_tuple, 2, _t2 );
		pybind::tuple_setitem_t( py_tuple, 3, _t3 );
		pybind::tuple_setitem_t( py_tuple, 4, _t4 );
		pybind::tuple_setitem_t( py_tuple, 5, _t5 );
		pybind::tuple_setitem_t( py_tuple, 6, _t6 );

		return pybind::tuple( py_tuple, pybind::borrowed() );
	}
	pybind::tuple make_tuple_t( const detail::import_operator_t & _t0, const detail::import_operator_t & _t1, const detail::import_operator_t & _t2, const detail::import_operator_t & _t3, const detail::import_operator_t & _t4, const detail::import_operator_t & _t5, const detail::import_operator_t & _t6, const detail::import_operator_t & _t7 )
	{ 
		PyObject * py_tuple = pybind::tuple_new( 8 );

		if( py_tuple == nullptr )
		{
			pybind::throw_exception( "make_tuple_t 8" );
		}

		pybind::tuple_setitem_t( py_tuple, 0, _t0 );
		pybind::tuple_setitem_t( py_tuple, 1, _t1 );
		pybind::tuple_setitem_t( py_tuple, 2, _t2 );
		pybind::tuple_setitem_t( py_tuple, 3, _t3 );
		pybind::tuple_setitem_t( py_tuple, 4, _t4 );
		pybind::tuple_setitem_t( py_tuple, 5, _t5 );
		pybind::tuple_setitem_t( py_tuple, 6, _t6 );
		pybind::tuple_setitem_t( py_tuple, 7, _t7 );

		return pybind::tuple( py_tuple, pybind::borrowed() );
	}
	//////////////////////////////////////////////////////////////////////////
	pybind::tuple make_invalid_tuple_t()
	{
		return pybind::tuple( nullptr, pybind::borrowed() );
	}
}