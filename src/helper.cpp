#	include "pybind/helper.hpp"

#	include "pybind/tuple.hpp"

namespace pybind
{
	namespace detail
	{
		//////////////////////////////////////////////////////////////////////////
		size_t args_operator_t::size() const
		{
			size_t args_size = pybind::tuple_size( m_args );

			return args_size;
		}
		//////////////////////////////////////////////////////////////////////////
		detail::extract_operator_t args_operator_t::operator[]( size_t _index ) const
		{
			return pybind::tuple_getitem_t( m_args, _index );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	detail::extract_operator_t list_getitem_t( PyObject * _list, size_t _it )
	{
		PyObject * py_item = pybind::list_getitem( _list, _it );

		return detail::extract_operator_t( py_item );
	}
	//////////////////////////////////////////////////////////////////////////
	bool list_setitem_t( PyObject * _list, size_t _it, const detail::import_operator_t & _item )
	{
		bool result = pybind::list_setitem( _list, _it, _item );

		return result;
	}
	//////////////////////////////////////////////////////////////////////////
	bool list_appenditem_t( PyObject * _obj, const detail::import_operator_t & _item )
	{
		bool result = pybind::list_appenditem( _obj, _item );

		return result;
	}
	//////////////////////////////////////////////////////////////////////////
	bool dict_setstring_t( PyObject * _dict, const char * _name, const detail::import_operator_t & _value )
	{
 		bool result = pybind::dict_setstring( _dict, _name, _value );

		return result;
	}
	//////////////////////////////////////////////////////////////////////////
	detail::args_operator_t make_args_t( PyObject * _tuple, size_t _size )
	{
		PyObject * py_args = pybind::tuple_slice_tail( _tuple, _size );

		return detail::args_operator_t( py_args );
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
		bool result = pybind::tuple_setitem( _tuple, _it, _item );

		return result;
	}
	//////////////////////////////////////////////////////////////////////////
	bool dict_set_t( PyObject * _dict, const detail::import_operator_t & _name, const detail::import_operator_t & _value )
	{
		bool result = dict_set( _dict, _name, _value );

		return result;
	}
	//////////////////////////////////////////////////////////////////////////
	detail::extract_operator_t dict_get_t( PyObject * _dict, const detail::import_operator_t & _key )
	{
		PyObject * py_item = pybind::dict_get( _dict, _key );

		return detail::extract_operator_t( py_item );
	}
	//////////////////////////////////////////////////////////////////////////
	bool dict_remove_t( PyObject * _dict, const detail::import_operator_t & _key )
	{
		return dict_remove( _dict, _key );
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
	detail::extract_operator_t call_args_t( PyObject * _obj, const detail::args_operator_t & _args )
	{ 
		pybind::tuple args = pybind::make_tuple_args_t( _args );

		return pybind::ask_tuple( _obj, args );
	}
	//////////////////////////////////////////////////////////////////////////	
	detail::extract_operator_t call_args_t( PyObject * _obj, const detail::import_operator_t & _t0, const detail::args_operator_t & _args )
	{
		pybind::tuple args = pybind::make_tuple_args_t( _t0, _args );

		return pybind::ask_tuple( _obj, args );
	}
	//////////////////////////////////////////////////////////////////////////
	detail::extract_operator_t call_args_t( PyObject * _obj, const detail::import_operator_t & _t0, const detail::import_operator_t & _t1, const detail::args_operator_t & _args )
	{
		pybind::tuple args = pybind::make_tuple_args_t( _t0, _t1, _args );

		return pybind::ask_tuple( _obj, args );
	}
	//////////////////////////////////////////////////////////////////////////
	detail::extract_operator_t call_args_t( PyObject * _obj, const detail::import_operator_t & _t0, const detail::import_operator_t & _t1, const detail::import_operator_t & _t2, const detail::args_operator_t & _args )
	{
		pybind::tuple args = pybind::make_tuple_args_t( _t0, _t1, _t2, _args );

		return pybind::ask_tuple( _obj, args );
	}
	//////////////////////////////////////////////////////////////////////////
	detail::extract_operator_t call_args_t( PyObject * _obj, const detail::import_operator_t & _t0, const detail::import_operator_t & _t1, const detail::import_operator_t & _t2, const detail::import_operator_t & _t3, const detail::args_operator_t & _args )
	{
		pybind::tuple args = pybind::make_tuple_args_t( _t0, _t1, _t2, _t3, _args );

		return pybind::ask_tuple( _obj, args );
	}
	//////////////////////////////////////////////////////////////////////////
	detail::extract_operator_t call_args_t( PyObject * _obj, const detail::import_operator_t & _t0, const detail::import_operator_t & _t1, const detail::import_operator_t & _t2, const detail::import_operator_t & _t3, const detail::import_operator_t & _t4, const detail::args_operator_t & _args )
	{
		pybind::tuple args = pybind::make_tuple_args_t( _t0, _t1, _t2, _t3, _t4, _args );

		return pybind::ask_tuple( _obj, args );
	}
	//////////////////////////////////////////////////////////////////////////
	detail::extract_operator_t call_args_t( PyObject * _obj, const detail::import_operator_t & _t0, const detail::import_operator_t & _t1, const detail::import_operator_t & _t2, const detail::import_operator_t & _t3, const detail::import_operator_t & _t4, const detail::import_operator_t & _t5, const detail::args_operator_t & _args )
	{
		pybind::tuple args = pybind::make_tuple_args_t( _t0, _t1, _t2, _t3, _t4, _t5, _args );

		return pybind::ask_tuple( _obj, args );
	}
	//////////////////////////////////////////////////////////////////////////
	detail::extract_operator_t call_args_t( PyObject * _obj, const detail::import_operator_t & _t0, const detail::import_operator_t & _t1, const detail::import_operator_t & _t2, const detail::import_operator_t & _t3, const detail::import_operator_t & _t4, const detail::import_operator_t & _t5, const detail::import_operator_t & _t6, const detail::args_operator_t & _args )
	{
		pybind::tuple args = pybind::make_tuple_args_t( _t0, _t1, _t2, _t3, _t4, _t5, _t6, _args );

		return pybind::ask_tuple( _obj, args );
	}
	//////////////////////////////////////////////////////////////////////////
	detail::extract_operator_t call_args_t( PyObject * _obj, const detail::import_operator_t & _t0, const detail::import_operator_t & _t1, const detail::import_operator_t & _t2, const detail::import_operator_t & _t3, const detail::import_operator_t & _t4, const detail::import_operator_t & _t5, const detail::import_operator_t & _t6, const detail::import_operator_t & _t7, const detail::args_operator_t & _args )
	{
		pybind::tuple args = pybind::make_tuple_args_t( _t0, _t1, _t2, _t3, _t4, _t5, _t6, _t7, _args );

		return pybind::ask_tuple( _obj, args );
	}
	//////////////////////////////////////////////////////////////////////////
	detail::extract_operator_t extract_t( PyObject * _obj )
	{
		return detail::extract_operator_t( _obj );
	}
}