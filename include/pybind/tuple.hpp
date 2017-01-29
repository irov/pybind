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
	//////////////////////////////////////////////////////////////////////////
	PYBIND_API pybind::tuple make_tuple_args_t( const detail::args_operator_t & _args );
	PYBIND_API pybind::tuple make_tuple_args_t( const detail::import_operator_t & _t0, const detail::args_operator_t & _args );
	PYBIND_API pybind::tuple make_tuple_args_t( const detail::import_operator_t & _t0, const detail::import_operator_t & _t1, const detail::args_operator_t & _args );
	PYBIND_API pybind::tuple make_tuple_args_t( const detail::import_operator_t & _t0, const detail::import_operator_t & _t1, const detail::import_operator_t & _t2, const detail::args_operator_t & _args );
	PYBIND_API pybind::tuple make_tuple_args_t( const detail::import_operator_t & _t0, const detail::import_operator_t & _t1, const detail::import_operator_t & _t2, const detail::import_operator_t & _t3, const detail::args_operator_t & _args );
	PYBIND_API pybind::tuple make_tuple_args_t( const detail::import_operator_t & _t0, const detail::import_operator_t & _t1, const detail::import_operator_t & _t2, const detail::import_operator_t & _t3, const detail::import_operator_t & _t4, const detail::args_operator_t & _args );
	PYBIND_API pybind::tuple make_tuple_args_t( const detail::import_operator_t & _t0, const detail::import_operator_t & _t1, const detail::import_operator_t & _t2, const detail::import_operator_t & _t3, const detail::import_operator_t & _t4, const detail::import_operator_t & _t5, const detail::args_operator_t & _args );
	PYBIND_API pybind::tuple make_tuple_args_t( const detail::import_operator_t & _t0, const detail::import_operator_t & _t1, const detail::import_operator_t & _t2, const detail::import_operator_t & _t3, const detail::import_operator_t & _t4, const detail::import_operator_t & _t5, const detail::import_operator_t & _t6, const detail::args_operator_t & _args );
	PYBIND_API pybind::tuple make_tuple_args_t( const detail::import_operator_t & _t0, const detail::import_operator_t & _t1, const detail::import_operator_t & _t2, const detail::import_operator_t & _t3, const detail::import_operator_t & _t4, const detail::import_operator_t & _t5, const detail::import_operator_t & _t6, const detail::import_operator_t & _t7, const detail::args_operator_t & _args );
	//////////////////////////////////////////////////////////////////////////
    template<class C>
    inline pybind::tuple make_tuple_container_t( const C & _c )
    {
        typename C::size_type size = _c.size();

        PyObject * py_tuple = pybind::tuple_new( size );

        for( typename C::size_type i = 0; i != size; ++i )
        {
            pybind::tuple_setitem_t( py_tuple, i, _c[i] );
        }

        return pybind::tuple( py_tuple, pybind::borrowed() );
    }
    //////////////////////////////////////////////////////////////////////////
	PYBIND_API pybind::tuple make_invalid_tuple_t();
}