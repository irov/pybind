#	pragma once

#	include "system.hpp"
#	include "extract.hpp"

namespace pybind
{

	template<class T>
	inline T list_getitem_t( PyObject * _list, size_t _it )
	{
		PyObject * py_item = pybind::list_getitem( _list, _it );

		if( py_item == nullptr )
		{
			pybind::check_error();

			return T();
		}

		return pybind::extract<T>( py_item );
	}

	template<class T>
	inline bool list_setitem_t( PyObject * _list, size_t _it, const T & _item )
	{
		PyObject * py_item = pybind::ptr( _item );

		if( py_item == nullptr )
		{
			pybind::check_error();

			return false;
		}

		return pybind::list_setitem( _list, _it, py_item );
	}

	template<class T>
	inline bool list_appenditem_t( PyObject * _obj, const T & _item )
	{
		PyObject * py_item = pybind::ptr( _item );

		if( py_item == nullptr )
		{
			pybind::check_error();

			return false;
		}

		bool result = pybind::list_appenditem( _obj, py_item );

		pybind::decref( py_item );

		return result;
	}

	template<class T>
	PYBIND_API bool dict_setstring_t( PyObject * _dict, const char * _name, const T & _value )
	{
		PyObject * py_value = pybind::ptr( _value );

		if( py_value == nullptr )
		{
			pybind::check_error();

			return false;
		}

		bool result = pybind::dict_setstring( _dict, _name, py_value );

		pybind::decref( py_value );

		return result;
	}

	template<class T>
	inline T tuple_getitem_t( PyObject * _tuple, size_t _it )
	{
		PyObject * py_item = pybind::tuple_getitem( _tuple, _it );

		if( py_item == nullptr )
		{
			pybind::check_error();

			return T();
		}

		return pybind::extract<T>( py_item );
	}

	template<class T>
	inline bool tuple_setitem_t( PyObject * _tuple, size_t _it, const T & _item )
	{
		PyObject * py_item = pybind::ptr( _item );

		if( py_item == nullptr )
		{
			pybind::check_error();

			return false;
		}

		return pybind::tuple_setitem( _tuple, _it, py_item );
	}

	template<class T0>
	inline PyObject * make_tuple_t( const T0 & _t0 )
	{
		PyObject * py_tuple = pybind::tuple_new( 1 );

		if( py_tuple == nullptr )
		{
			pybind::check_error();

			return nullptr;
		}

		pybind::tuple_setitem_t( py_tuple, 0, _t0 );

		return py_tuple;
	}

	template<class T0, class T1>
	inline PyObject * make_tuple_t( const T0 & _t0, const T1 & _t1 )
	{
		PyObject * py_tuple = pybind::tuple_new( 2 );

		if( py_tuple == nullptr )
		{
			pybind::check_error();

			return nullptr;
		}

		pybind::tuple_setitem_t( py_tuple, 0, _t0 );
		pybind::tuple_setitem_t( py_tuple, 1, _t1 );

		return py_tuple;
	}

	template<class T0, class T1, class T2>
	inline PyObject * make_tuple_t( const T0 & _t0, const T1 & _t1, const T2 & _t2 )
	{
		PyObject * py_tuple = pybind::tuple_new( 3 );

		if( py_tuple == nullptr )
		{
			pybind::check_error();

			return nullptr;
		}

		pybind::tuple_setitem_t( py_tuple, 0, _t0 );
		pybind::tuple_setitem_t( py_tuple, 1, _t1 );
		pybind::tuple_setitem_t( py_tuple, 2, _t2 );

		return py_tuple;
	}

	template<class T0, class T1, class T2, class T3>
	inline PyObject * make_tuple_t( const T0 & _t0, const T1 & _t1, const T2 & _t2, const T3 & _t3 )
	{
		PyObject * py_tuple = pybind::tuple_new( 4 );

		if( py_tuple == nullptr )
		{
			pybind::check_error();

			return nullptr;
		}

		pybind::tuple_setitem_t( py_tuple, 0, _t0 );
		pybind::tuple_setitem_t( py_tuple, 1, _t1 );
		pybind::tuple_setitem_t( py_tuple, 2, _t2 );
		pybind::tuple_setitem_t( py_tuple, 3, _t3 );

		return py_tuple;
	}

	template<class T0>
	inline bool list_appendtuple_t( PyObject * _obj, const T0 & _t0 )
	{
		PyObject * py_tuple = pybind::make_tuple_t( _t0 );

		if( py_tuple == nullptr )
		{
			pybind::check_error();

			return false;
		}

		bool result = pybind::list_appenditem( _obj, py_tuple );

		pybind::decref( py_tuple );

		return result;
	}

	template<class T0, class T1>
	inline bool list_appendtuple_t( PyObject * _obj, const T0 & _t0, const T1 & _t1 )
	{
		PyObject * py_tuple = pybind::make_tuple_t( _t0, _t1 );

		if( py_tuple == nullptr )
		{
			pybind::check_error();

			return false;
		}

		bool result = pybind::list_appenditem( _obj, py_tuple );

		pybind::decref( py_tuple );

		return result;
	}

	template<class T0, class T1, class T2>
	inline bool list_appendtuple_t( PyObject * _obj, const T0 & _t0, const T1 & _t1, const T2 & _t2 )
	{
		PyObject * py_tuple = pybind::make_tuple_t( _t0, _t1, _t2 );

		if( py_tuple == nullptr )
		{
			pybind::check_error();

			return false;
		}

		bool result = pybind::list_appenditem( _obj, py_tuple );

		pybind::decref( py_tuple );

		return result;
	}

	template<class T0, class T1, class T2, class T3>
	inline bool list_appendtuple_t( PyObject * _obj, const T0 & _t0, const T1 & _t1, const T2 & _t2, const T3 & _t3 )
	{
		PyObject * py_tuple = pybind::make_tuple_t( _t0, _t1, _t2, _t3 );

		if( py_tuple == nullptr )
		{
			pybind::check_error();

			return false;
		}

		bool result = pybind::list_appenditem( _obj, py_tuple );

		pybind::decref( py_tuple );

		return result;
	}
}