#	pragma once

#	include "pybind/object.hpp"

namespace pybind
{
	class PYBIND_API tuple
		: public pybind::object
	{
	public:		
		tuple( kernel_interface * _kernel );

	public:
		explicit tuple( borrowed );
		explicit tuple( kernel_interface * _kernel, PyObject * _obj, borrowed );
		explicit tuple( kernel_interface * _kernel, PyObject * _obj );

	public:
		detail::extract_operator_t operator [] ( size_t _index ) const;

	public:
		size_t size() const;
		bool empty() const;
	};
	//////////////////////////////////////////////////////////////////////////	
	template<class T0>
	pybind::tuple make_tuple_t( kernel_interface * _kernel, const T0 & _t0 )
	{
		return make_tuple_t( _kernel, detail::import_operator_t( _kernel, _t0 ) );
	}
	//////////////////////////////////////////////////////////////////////////	
	template<class T0, class T1>
	pybind::tuple make_tuple_t( kernel_interface * _kernel, const T0 & _t0, const T1 & _t1 )
	{
		return make_tuple_t( _kernel
			, detail::import_operator_t( _kernel, _t0 ) 
			, detail::import_operator_t( _kernel, _t1 )
			);
	}
	//////////////////////////////////////////////////////////////////////////	
	template<class T0, class T1, class T2>
	pybind::tuple make_tuple_t( kernel_interface * _kernel, const T0 & _t0, const T1 & _t1, const T2 & _t2 )
	{
		return make_tuple_t( _kernel
			, detail::import_operator_t( _kernel, _t0 )
			, detail::import_operator_t( _kernel, _t1 )
			, detail::import_operator_t( _kernel, _t2 )
			);
	}
	//////////////////////////////////////////////////////////////////////////	
	template<class T0, class T1, class T2, class T3>
	pybind::tuple make_tuple_t( kernel_interface * _kernel, const T0 & _t0, const T1 & _t1, const T2 & _t2, const T3 & _t3 )
	{
		return make_tuple_t( _kernel
			, detail::import_operator_t( _kernel, _t0 )
			, detail::import_operator_t( _kernel, _t1 )
			, detail::import_operator_t( _kernel, _t2 )
			, detail::import_operator_t( _kernel, _t3 )
			);
	}
	//////////////////////////////////////////////////////////////////////////	
	template<class T0, class T1, class T2, class T3, class T4>
	pybind::tuple make_tuple_t( kernel_interface * _kernel, const T0 & _t0, const T1 & _t1, const T2 & _t2, const T3 & _t3, const T4 & _t4 )
	{
		return make_tuple_t( _kernel
			, detail::import_operator_t( _kernel, _t0 )
			, detail::import_operator_t( _kernel, _t1 )
			, detail::import_operator_t( _kernel, _t2 )
			, detail::import_operator_t( _kernel, _t3 )
			, detail::import_operator_t( _kernel, _t4 )
			);
	}
	//////////////////////////////////////////////////////////////////////////	
	template<class T0, class T1, class T2, class T3, class T4, class T5>
	pybind::tuple make_tuple_t( kernel_interface * _kernel, const T0 & _t0, const T1 & _t1, const T2 & _t2, const T3 & _t3, const T4 & _t4, const T5 & _t5 )
	{
		return make_tuple_t( _kernel
			, detail::import_operator_t( _kernel, _t0 )
			, detail::import_operator_t( _kernel, _t1 )
			, detail::import_operator_t( _kernel, _t2 )
			, detail::import_operator_t( _kernel, _t3 )
			, detail::import_operator_t( _kernel, _t4 )
			, detail::import_operator_t( _kernel, _t5 )
			);
	}
	//////////////////////////////////////////////////////////////////////////	
	template<class T0, class T1, class T2, class T3, class T4, class T5, class T6>
	pybind::tuple make_tuple_t( kernel_interface * _kernel, const T0 & _t0, const T1 & _t1, const T2 & _t2, const T3 & _t3, const T4 & _t4, const T5 & _t5, const T6 & _t6 )
	{
		return make_tuple_t( _kernel
			, detail::import_operator_t( _kernel, _t0 )
			, detail::import_operator_t( _kernel, _t1 )
			, detail::import_operator_t( _kernel, _t2 )
			, detail::import_operator_t( _kernel, _t3 )
			, detail::import_operator_t( _kernel, _t4 )
			, detail::import_operator_t( _kernel, _t5 )
			, detail::import_operator_t( _kernel, _t6 )
			);
	}
	//////////////////////////////////////////////////////////////////////////	
	template<class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7>
	pybind::tuple make_tuple_t( kernel_interface * _kernel, const T0 & _t0, const T1 & _t1, const T2 & _t2, const T3 & _t3, const T4 & _t4, const T5 & _t5, const T6 & _t6, const T7 & _t7 )
	{
		return make_tuple_t( _kernel
			, detail::import_operator_t( _kernel, _t0 )
			, detail::import_operator_t( _kernel, _t1 )
			, detail::import_operator_t( _kernel, _t2 )
			, detail::import_operator_t( _kernel, _t3 )
			, detail::import_operator_t( _kernel, _t4 )
			, detail::import_operator_t( _kernel, _t5 )
			, detail::import_operator_t( _kernel, _t6 )
			, detail::import_operator_t( _kernel, _t7 )
			);
	}
	//////////////////////////////////////////////////////////////////////////	
	PYBIND_API pybind::tuple make_tuple_t( kernel_interface * _kernel );
	PYBIND_API pybind::tuple make_tuple_t( kernel_interface * _kernel, const detail::import_operator_t & _t0 );
	PYBIND_API pybind::tuple make_tuple_t( kernel_interface * _kernel, const detail::import_operator_t & _t0, const detail::import_operator_t & _t1 );
	PYBIND_API pybind::tuple make_tuple_t( kernel_interface * _kernel, const detail::import_operator_t & _t0, const detail::import_operator_t & _t1, const detail::import_operator_t & _t2 );
	PYBIND_API pybind::tuple make_tuple_t( kernel_interface * _kernel, const detail::import_operator_t & _t0, const detail::import_operator_t & _t1, const detail::import_operator_t & _t2, const detail::import_operator_t & _t3 );
	PYBIND_API pybind::tuple make_tuple_t( kernel_interface * _kernel, const detail::import_operator_t & _t0, const detail::import_operator_t & _t1, const detail::import_operator_t & _t2, const detail::import_operator_t & _t3, const detail::import_operator_t & _t4 );
	PYBIND_API pybind::tuple make_tuple_t( kernel_interface * _kernel, const detail::import_operator_t & _t0, const detail::import_operator_t & _t1, const detail::import_operator_t & _t2, const detail::import_operator_t & _t3, const detail::import_operator_t & _t4, const detail::import_operator_t & _t5 );
	PYBIND_API pybind::tuple make_tuple_t( kernel_interface * _kernel, const detail::import_operator_t & _t0, const detail::import_operator_t & _t1, const detail::import_operator_t & _t2, const detail::import_operator_t & _t3, const detail::import_operator_t & _t4, const detail::import_operator_t & _t5, const detail::import_operator_t & _t6 );
	PYBIND_API pybind::tuple make_tuple_t( kernel_interface * _kernel, const detail::import_operator_t & _t0, const detail::import_operator_t & _t1, const detail::import_operator_t & _t2, const detail::import_operator_t & _t3, const detail::import_operator_t & _t4, const detail::import_operator_t & _t5, const detail::import_operator_t & _t6, const detail::import_operator_t & _t7 );
	//////////////////////////////////////////////////////////////////////////
	PYBIND_API pybind::tuple make_tuple_args_t( kernel_interface * _kernel, const detail::args_operator_t & _args );
	PYBIND_API pybind::tuple make_tuple_args_t( kernel_interface * _kernel, const detail::import_operator_t & _t0, const detail::args_operator_t & _args );
	PYBIND_API pybind::tuple make_tuple_args_t( kernel_interface * _kernel, const detail::import_operator_t & _t0, const detail::import_operator_t & _t1, const detail::args_operator_t & _args );
	PYBIND_API pybind::tuple make_tuple_args_t( kernel_interface * _kernel, const detail::import_operator_t & _t0, const detail::import_operator_t & _t1, const detail::import_operator_t & _t2, const detail::args_operator_t & _args );
	PYBIND_API pybind::tuple make_tuple_args_t( kernel_interface * _kernel, const detail::import_operator_t & _t0, const detail::import_operator_t & _t1, const detail::import_operator_t & _t2, const detail::import_operator_t & _t3, const detail::args_operator_t & _args );
	PYBIND_API pybind::tuple make_tuple_args_t( kernel_interface * _kernel, const detail::import_operator_t & _t0, const detail::import_operator_t & _t1, const detail::import_operator_t & _t2, const detail::import_operator_t & _t3, const detail::import_operator_t & _t4, const detail::args_operator_t & _args );
	PYBIND_API pybind::tuple make_tuple_args_t( kernel_interface * _kernel, const detail::import_operator_t & _t0, const detail::import_operator_t & _t1, const detail::import_operator_t & _t2, const detail::import_operator_t & _t3, const detail::import_operator_t & _t4, const detail::import_operator_t & _t5, const detail::args_operator_t & _args );
	PYBIND_API pybind::tuple make_tuple_args_t( kernel_interface * _kernel, const detail::import_operator_t & _t0, const detail::import_operator_t & _t1, const detail::import_operator_t & _t2, const detail::import_operator_t & _t3, const detail::import_operator_t & _t4, const detail::import_operator_t & _t5, const detail::import_operator_t & _t6, const detail::args_operator_t & _args );
	PYBIND_API pybind::tuple make_tuple_args_t( kernel_interface * _kernel, const detail::import_operator_t & _t0, const detail::import_operator_t & _t1, const detail::import_operator_t & _t2, const detail::import_operator_t & _t3, const detail::import_operator_t & _t4, const detail::import_operator_t & _t5, const detail::import_operator_t & _t6, const detail::import_operator_t & _t7, const detail::args_operator_t & _args );
	//////////////////////////////////////////////////////////////////////////
	template<class C>
	inline pybind::tuple make_tuple_container_t( kernel_interface * _kernel, const C & _c )
	{
		typename C::size_type size = _c.size();

		PyObject * py_tuple = pybind::tuple_new( size );

		for( typename C::size_type i = 0; i != size; ++i )
		{
			pybind::tuple_setitem_t( _kernel, py_tuple, i, _c[i] );
		}

		return pybind::tuple( _kernel, py_tuple, pybind::borrowed() );
	}
	//////////////////////////////////////////////////////////////////////////
	PYBIND_API pybind::tuple make_invalid_tuple_t( kernel_interface * _kernel );
}