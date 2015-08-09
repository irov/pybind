#	include "pybind/helper.hpp"

#	include "pybind/tuple.hpp"

namespace pybind
{
	//////////////////////////////////////////////////////////////////////////
	detail::extract_operator_t list_getitem_t( PyObject * _list, size_t _it )
	{
		PyObject * py_item = pybind::list_getitem( _list, _it );

		return detail::extract_operator_t( py_item );
	}
	//////////////////////////////////////////////////////////////////////////
	bool list_setitem_t( PyObject * _list, size_t _it, const detail::import_operator_t & _item )
	{
		return pybind::list_setitem( _list, _it, _item );
	}
	//////////////////////////////////////////////////////////////////////////
	bool list_appenditem_t( PyObject * _obj, const detail::borrowed_import_operator_t & _item )
	{
		bool result = pybind::list_appenditem( _obj, _item );

		return result;
	}
	//////////////////////////////////////////////////////////////////////////
	bool dict_setstring_t( PyObject * _dict, const char * _name, const detail::borrowed_import_operator_t & _value )
	{
		bool result = pybind::dict_setstring( _dict, _name, _value );

		return result;
	}
	//////////////////////////////////////////////////////////////////////////
	detail::extract_operator_t tuple_getitem_t( PyObject * _tuple, size_t _it )
	{
		PyObject * py_item = pybind::tuple_getitem( _tuple, _it );
		
		return detail::extract_operator_t( py_item );
	}
	//////////////////////////////////////////////////////////////////////////
	bool tuple_setitem_t( PyObject * _tuple, size_t _it, const detail::import_operator_t & _item )
	{
		return pybind::tuple_setitem( _tuple, _it, _item );
	}
	//////////////////////////////////////////////////////////////////////////
	detail::extract_operator_t ask_tuple( PyObject * _obj, const pybind::tuple & _tuple )
	{
		PyObject * py_result = pybind::ask_native( _obj, _tuple.ptr() );

		return detail::extract_operator_t( py_result );
	}
	//////////////////////////////////////////////////////////////////////////
	detail::extract_operator_t call_t( PyObject * _obj )
	{
		pybind::tuple args = pybind::make_tuple_t();

		return pybind::ask_tuple( _obj, args );
	}
	//////////////////////////////////////////////////////////////////////////	
	detail::extract_operator_t call_t( PyObject * _obj, const detail::import_operator_t & _t0 )
	{
		pybind::tuple args = pybind::make_tuple_t( _t0 );

		return pybind::ask_tuple( _obj, args );
	}
	//////////////////////////////////////////////////////////////////////////
	detail::extract_operator_t call_t( PyObject * _obj, const detail::import_operator_t & _t0, const detail::import_operator_t & _t1 )
	{
		pybind::tuple args = pybind::make_tuple_t( _t0, _t1 );

		return pybind::ask_tuple( _obj, args );
	}
	//////////////////////////////////////////////////////////////////////////
	detail::extract_operator_t call_t( PyObject * _obj, const detail::import_operator_t & _t0, const detail::import_operator_t & _t1, const detail::import_operator_t & _t2 )
	{
		pybind::tuple args = pybind::make_tuple_t( _t0, _t1, _t2 );

		return pybind::ask_tuple( _obj, args );
	}
	//////////////////////////////////////////////////////////////////////////
	detail::extract_operator_t call_t( PyObject * _obj, const detail::import_operator_t & _t0, const detail::import_operator_t & _t1, const detail::import_operator_t & _t2, const detail::import_operator_t & _t3 )
	{
		pybind::tuple args = pybind::make_tuple_t( _t0, _t1, _t2, _t3 );

		return pybind::ask_tuple( _obj, args );
	}
	//////////////////////////////////////////////////////////////////////////
	detail::extract_operator_t call_t( PyObject * _obj, const detail::import_operator_t & _t0, const detail::import_operator_t & _t1, const detail::import_operator_t & _t2, const detail::import_operator_t & _t3, const detail::import_operator_t & _t4 )
	{
		pybind::tuple args = pybind::make_tuple_t( _t0, _t1, _t2, _t3, _t4 );

		return pybind::ask_tuple( _obj, args );
	}
	//////////////////////////////////////////////////////////////////////////
	detail::extract_operator_t call_t( PyObject * _obj, const detail::import_operator_t & _t0, const detail::import_operator_t & _t1, const detail::import_operator_t & _t2, const detail::import_operator_t & _t3, const detail::import_operator_t & _t4, const detail::import_operator_t & _t5 )
	{
		pybind::tuple args = pybind::make_tuple_t( _t0, _t1, _t2, _t3, _t4, _t5 );

		return pybind::ask_tuple( _obj, args );
	}
	//////////////////////////////////////////////////////////////////////////
	detail::extract_operator_t call_t( PyObject * _obj, const detail::import_operator_t & _t0, const detail::import_operator_t & _t1, const detail::import_operator_t & _t2, const detail::import_operator_t & _t3, const detail::import_operator_t & _t4, const detail::import_operator_t & _t5, const detail::import_operator_t & _t6 )
	{
		pybind::tuple args = pybind::make_tuple_t( _t0, _t1, _t2, _t3, _t4, _t5, _t6 );

		return pybind::ask_tuple( _obj, args );
	}
	//////////////////////////////////////////////////////////////////////////
	detail::extract_operator_t call_t( PyObject * _obj, const detail::import_operator_t & _t0, const detail::import_operator_t & _t1, const detail::import_operator_t & _t2, const detail::import_operator_t & _t3, const detail::import_operator_t & _t4, const detail::import_operator_t & _t5, const detail::import_operator_t & _t6, const detail::import_operator_t & _t7 )
	{
		pybind::tuple args = pybind::make_tuple_t( _t0, _t1, _t2, _t3, _t4, _t5, _t6, _t7 );

		return pybind::ask_tuple( _obj, args );
	}
	//////////////////////////////////////////////////////////////////////////
	detail::extract_operator_t extract_t( PyObject * _obj )
	{
		return detail::extract_operator_t( _obj );
	}
}