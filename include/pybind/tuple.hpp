#	pragma once

#	include "pybind/object.hpp"

namespace pybind
{
	class PYBIND_API tuple
		: public pybind::object
	{
	public:		
		tuple();

	public:
		explicit tuple( PyObject * _obj, borrowed );
		explicit tuple( PyObject * _obj );

	public:
		detail::extract_operator_t operator [] ( size_t _index ) const;

	public:
		size_t size() const;
		bool empty() const;
	};
	//////////////////////////////////////////////////////////////////////////	
	PYBIND_API pybind::tuple make_tuple_t();
	PYBIND_API pybind::tuple make_tuple_t( const detail::import_operator_t & _t0 );
	PYBIND_API pybind::tuple make_tuple_t( const detail::import_operator_t & _t0, const detail::import_operator_t & _t1 );
	PYBIND_API pybind::tuple make_tuple_t( const detail::import_operator_t & _t0, const detail::import_operator_t & _t1, const detail::import_operator_t & _t2 );
	PYBIND_API pybind::tuple make_tuple_t( const detail::import_operator_t & _t0, const detail::import_operator_t & _t1, const detail::import_operator_t & _t2, const detail::import_operator_t & _t3 );
	PYBIND_API pybind::tuple make_tuple_t( const detail::import_operator_t & _t0, const detail::import_operator_t & _t1, const detail::import_operator_t & _t2, const detail::import_operator_t & _t3, const detail::import_operator_t & _t4 );
	PYBIND_API pybind::tuple make_tuple_t( const detail::import_operator_t & _t0, const detail::import_operator_t & _t1, const detail::import_operator_t & _t2, const detail::import_operator_t & _t3, const detail::import_operator_t & _t4, const detail::import_operator_t & _t5 );
	PYBIND_API pybind::tuple make_tuple_t( const detail::import_operator_t & _t0, const detail::import_operator_t & _t1, const detail::import_operator_t & _t2, const detail::import_operator_t & _t3, const detail::import_operator_t & _t4, const detail::import_operator_t & _t5, const detail::import_operator_t & _t6 );
	PYBIND_API pybind::tuple make_tuple_t( const detail::import_operator_t & _t0, const detail::import_operator_t & _t1, const detail::import_operator_t & _t2, const detail::import_operator_t & _t3, const detail::import_operator_t & _t4, const detail::import_operator_t & _t5, const detail::import_operator_t & _t6, const detail::import_operator_t & _t7 );
	PYBIND_API pybind::tuple make_invalid_tuple_t();
}