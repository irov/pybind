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

	inline PyObject * ask_t( PyObject * _obj )
	{
		PyObject * py_args = pybind::tuple_new( 0 );

		PyObject * py_result = pybind::ask_native( _obj, py_args );

		pybind::decref( py_args );

		return py_result;
	}

	template<class T0>
	inline PyObject * ask_t( PyObject * _obj, const T0 & _t0 )
	{
		PyObject * py_args = pybind::tuple_new( 1 );

		pybind::tuple_setitem_t( py_args, 0, _t0 );

		PyObject * py_result = pybind::ask_native( _obj, py_args );

		pybind::decref( py_args );

		return py_result;
	}

	template<class T0, class T1>
	inline PyObject * ask_t( PyObject * _obj, const T0 & _t0, const T1 & _t1 )
	{
		PyObject * py_args = pybind::tuple_new( 2 );

		pybind::tuple_setitem_t( py_args, 0, _t0 );
		pybind::tuple_setitem_t( py_args, 1, _t1 );

		PyObject * py_result = pybind::ask_native( _obj, py_args );

		pybind::decref( py_args );

		return py_result;
	}

	template<class T0, class T1, class T2>
	inline PyObject * ask_t( PyObject * _obj, const T0 & _t0, const T1 & _t1, const T2 & _t2 )
	{
		PyObject * py_args = pybind::tuple_new( 3 );

		pybind::tuple_setitem_t( py_args, 0, _t0 );
		pybind::tuple_setitem_t( py_args, 1, _t1 );
		pybind::tuple_setitem_t( py_args, 2, _t2 );

		PyObject * py_result = pybind::ask_native( _obj, py_args );

		pybind::decref( py_args );

		return py_result;
	}

	template<class T0, class T1, class T2, class T3>
	inline PyObject * ask_t( PyObject * _obj, const T0 & _t0, const T1 & _t1, const T2 & _t2, const T3 & _t3 )
	{
		PyObject * py_args = pybind::tuple_new( 4 );

		pybind::tuple_setitem_t( py_args, 0, _t0 );
		pybind::tuple_setitem_t( py_args, 1, _t1 );
		pybind::tuple_setitem_t( py_args, 2, _t2 );
		pybind::tuple_setitem_t( py_args, 3, _t3 );

		PyObject * py_result = pybind::ask_native( _obj, py_args );

		pybind::decref( py_args );

		return py_result;
	}

	template<class T0, class T1, class T2, class T3, class T4>
	inline PyObject * ask_t( PyObject * _obj, const T0 & _t0, const T1 & _t1, const T2 & _t2, const T3 & _t3, const T4 & _t4 )
	{
		PyObject * py_args = pybind::tuple_new( 5 );

		pybind::tuple_setitem_t( py_args, 0, _t0 );
		pybind::tuple_setitem_t( py_args, 1, _t1 );
		pybind::tuple_setitem_t( py_args, 2, _t2 );
		pybind::tuple_setitem_t( py_args, 3, _t3 );
		pybind::tuple_setitem_t( py_args, 4, _t4 );

		PyObject * py_result = pybind::ask_native( _obj, py_args );

		pybind::decref( py_args );

		return py_result;
	}

	inline void call_t( PyObject * _obj )
	{
		PyObject * py_args = pybind::tuple_new( 0 );

		pybind::call_native( _obj, py_args );

		pybind::decref( py_args );
	}

	template<class T0>
	inline void call_t( PyObject * _obj, const T0 & _t0 )
	{
		PyObject * py_args = pybind::tuple_new( 1 );

		pybind::tuple_setitem_t( py_args, 0, _t0 );

		pybind::call_native( _obj, py_args );

		pybind::decref( py_args );
	}

	template<class T0, class T1>
	inline void call_t( PyObject * _obj, const T0 & _t0, const T1 & _t1 )
	{
		PyObject * py_args = pybind::tuple_new( 2 );

		pybind::tuple_setitem_t( py_args, 0, _t0 );
		pybind::tuple_setitem_t( py_args, 1, _t1 );

		pybind::call_native( _obj, py_args );

		pybind::decref( py_args );
	}

	template<class T0, class T1, class T2>
	inline void call_t( PyObject * _obj, const T0 & _t0, const T1 & _t1, const T2 & _t2 )
	{
		PyObject * py_args = pybind::tuple_new( 3 );

		pybind::tuple_setitem_t( py_args, 0, _t0 );
		pybind::tuple_setitem_t( py_args, 1, _t1 );
		pybind::tuple_setitem_t( py_args, 2, _t2 );

		pybind::call_native( _obj, py_args );

		pybind::decref( py_args );
	}

	template<class T0, class T1, class T2, class T3>
	inline void call_t( PyObject * _obj, const T0 & _t0, const T1 & _t1, const T2 & _t2, const T3 & _t3 )
	{
		PyObject * py_args = pybind::tuple_new( 4 );

		pybind::tuple_setitem_t( py_args, 0, _t0 );
		pybind::tuple_setitem_t( py_args, 1, _t1 );
		pybind::tuple_setitem_t( py_args, 2, _t2 );
		pybind::tuple_setitem_t( py_args, 3, _t3 );

		pybind::call_native( _obj, py_args );

		pybind::decref( py_args );
	}

	template<class T0, class T1, class T2, class T3, class T4>
	inline void call_t( PyObject * _obj, const T0 & _t0, const T1 & _t1, const T2 & _t2, const T3 & _t3, const T4 & _t4 )
	{
		PyObject * py_args = pybind::tuple_new( 5 );

		pybind::tuple_setitem_t( py_args, 0, _t0 );
		pybind::tuple_setitem_t( py_args, 1, _t1 );
		pybind::tuple_setitem_t( py_args, 2, _t2 );
		pybind::tuple_setitem_t( py_args, 3, _t3 );
		pybind::tuple_setitem_t( py_args, 4, _t4 );

		pybind::call_native( _obj, py_args );

		pybind::decref( py_args );
	}

	template<class T0, class T1, class T2, class T3, class T4, class T5>
	inline void call_t( PyObject * _obj, const T0 & _t0, const T1 & _t1, const T2 & _t2, const T3 & _t3, const T4 & _t4, const T5 & _t5 )
	{
		PyObject * py_args = pybind::tuple_new( 5 );

		pybind::tuple_setitem_t( py_args, 0, _t0 );
		pybind::tuple_setitem_t( py_args, 1, _t1 );
		pybind::tuple_setitem_t( py_args, 2, _t2 );
		pybind::tuple_setitem_t( py_args, 3, _t3 );
		pybind::tuple_setitem_t( py_args, 4, _t4 );
		pybind::tuple_setitem_t( py_args, 5, _t5 );

		pybind::call_native( _obj, py_args );

		pybind::decref( py_args );
	}

	template<class T0, class T1, class T2, class T3, class T4, class T5, class T6>
	inline void call_t( PyObject * _obj, const T0 & _t0, const T1 & _t1, const T2 & _t2, const T3 & _t3, const T4 & _t4, const T5 & _t5, const T6 & _t6 )
	{
		PyObject * py_args = pybind::tuple_new( 5 );

		pybind::tuple_setitem_t( py_args, 0, _t0 );
		pybind::tuple_setitem_t( py_args, 1, _t1 );
		pybind::tuple_setitem_t( py_args, 2, _t2 );
		pybind::tuple_setitem_t( py_args, 3, _t3 );
		pybind::tuple_setitem_t( py_args, 4, _t4 );
		pybind::tuple_setitem_t( py_args, 5, _t5 );
		pybind::tuple_setitem_t( py_args, 6, _t6 );

		pybind::call_native( _obj, py_args );

		pybind::decref( py_args );
	}

	template<class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7>
	inline void call_t( PyObject * _obj, const T0 & _t0, const T1 & _t1, const T2 & _t2, const T3 & _t3, const T4 & _t4, const T5 & _t5, const T6 & _t6, const T7 & _t7 )
	{
		PyObject * py_args = pybind::tuple_new( 5 );

		pybind::tuple_setitem_t( py_args, 0, _t0 );
		pybind::tuple_setitem_t( py_args, 1, _t1 );
		pybind::tuple_setitem_t( py_args, 2, _t2 );
		pybind::tuple_setitem_t( py_args, 3, _t3 );
		pybind::tuple_setitem_t( py_args, 4, _t4 );
		pybind::tuple_setitem_t( py_args, 5, _t5 );
		pybind::tuple_setitem_t( py_args, 6, _t6 );
		pybind::tuple_setitem_t( py_args, 7, _t7 );

		pybind::call_native( _obj, py_args );

		pybind::decref( py_args );
	}
}