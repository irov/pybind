#	pragma once

#	include "system.hpp"
#	include "extract.hpp"

namespace pybind
{
	//////////////////////////////////////////////////////////////////////////
	namespace detail
	{
		//////////////////////////////////////////////////////////////////////////
		class extract_operator_t
		{
		public:
			explicit extract_operator_t( PyObject * _obj )
				: m_obj( _obj )
			{
			}

			extract_operator_t( const extract_operator_t & _r )
				: m_obj( _r.m_obj )
			{
				pybind::incref( m_obj );
			}

			~extract_operator_t()
			{
				pybind::decref( m_obj );
			}

			operator PyObject * ()
			{
				pybind::incref( m_obj );

				return m_obj;
			}

			template<class T>
			operator T ()
			{
				return pybind::extract<T>( m_obj );
			}

			template<class T>
			bool operator == (const T & _value)
			{
				return pybind::extract<T>( m_obj ) == _value;
			}

		protected:
			PyObject * m_obj;
		};
	}
	//////////////////////////////////////////////////////////////////////////
	inline detail::extract_operator_t list_getitem_t( PyObject * _list, size_t _it )
	{
		PyObject * py_item = pybind::list_getitem( _list, _it );

		if( py_item == nullptr )
		{
			pybind::throw_exception("list_getitem_t %d"
				, _it
				);
		}

		pybind::incref( py_item );

		return detail::extract_operator_t( py_item );
	}
	//////////////////////////////////////////////////////////////////////////
	template<class T>
	inline bool list_setitem_t( PyObject * _list, size_t _it, const T & _item )
	{
		PyObject * py_item = pybind::ptr( _item );

		if( py_item == nullptr )
		{
			return false;
		}

		return pybind::list_setitem( _list, _it, py_item );
	}
	//////////////////////////////////////////////////////////////////////////
	template<class T>
	inline bool list_appenditem_t( PyObject * _obj, const T & _item )
	{
		PyObject * py_item = pybind::ptr( _item );

		if( py_item == nullptr )
		{
			return false;
		}

		bool result = pybind::list_appenditem( _obj, py_item );

		pybind::decref( py_item );

		return result;
	}
	//////////////////////////////////////////////////////////////////////////
	template<class T>
	PYBIND_API bool dict_setstring_t( PyObject * _dict, const char * _name, const T & _value )
	{
		PyObject * py_value = pybind::ptr( _value );

		if( py_value == nullptr )
		{
			return false;
		}

		bool result = pybind::dict_setstring( _dict, _name, py_value );

		pybind::decref( py_value );

		return result;
	}
	//////////////////////////////////////////////////////////////////////////
	inline detail::extract_operator_t tuple_getitem_t( PyObject * _tuple, size_t _it )
	{
		PyObject * py_item = pybind::tuple_getitem( _tuple, _it );

		if( py_item == nullptr )
		{
			pybind::throw_exception( "tuple_getitem_t %d"
				, _it
				);
		}

		pybind::incref( py_item );

		return detail::extract_operator_t( py_item );
	}
	//////////////////////////////////////////////////////////////////////////
	template<class T>
	inline bool tuple_setitem_t( PyObject * _tuple, size_t _it, const T & _item )
	{
		PyObject * py_item = pybind::ptr( _item );

		if( py_item == nullptr )
		{
			return false;
		}

		return pybind::tuple_setitem( _tuple, _it, py_item );
	}
	//////////////////////////////////////////////////////////////////////////
	inline detail::extract_operator_t ask_t( PyObject * _obj )
	{
		PyObject * py_args = pybind::tuple_new( 0 );

		PyObject * py_result = pybind::ask_native( _obj, py_args );

		pybind::decref( py_args );
		
		return detail::extract_operator_t( py_result );
	}
	//////////////////////////////////////////////////////////////////////////
	template<class T0>
	inline detail::extract_operator_t ask_t( PyObject * _obj, const T0 & _t0 )
	{
		PyObject * py_args = pybind::tuple_new( 1 );

		pybind::tuple_setitem_t( py_args, 0, _t0 );

		PyObject * py_result = pybind::ask_native( _obj, py_args );

		pybind::decref( py_args );

		return detail::extract_operator_t( py_result );
	}
	//////////////////////////////////////////////////////////////////////////
	template<class T0, class T1>
	inline detail::extract_operator_t ask_t( PyObject * _obj, const T0 & _t0, const T1 & _t1 )
	{
		PyObject * py_args = pybind::tuple_new( 2 );

		pybind::tuple_setitem_t( py_args, 0, _t0 );
		pybind::tuple_setitem_t( py_args, 1, _t1 );

		PyObject * py_result = pybind::ask_native( _obj, py_args );

		pybind::decref( py_args );

		return detail::extract_operator_t( py_result );
	}
	//////////////////////////////////////////////////////////////////////////
	template<class T0, class T1, class T2>
	inline detail::extract_operator_t ask_t( PyObject * _obj, const T0 & _t0, const T1 & _t1, const T2 & _t2 )
	{
		PyObject * py_args = pybind::tuple_new( 3 );

		pybind::tuple_setitem_t( py_args, 0, _t0 );
		pybind::tuple_setitem_t( py_args, 1, _t1 );
		pybind::tuple_setitem_t( py_args, 2, _t2 );

		PyObject * py_result = pybind::ask_native( _obj, py_args );

		pybind::decref( py_args );

		return detail::extract_operator_t( py_result );
	}
	//////////////////////////////////////////////////////////////////////////
	template<class T0, class T1, class T2, class T3>
	inline detail::extract_operator_t ask_t( PyObject * _obj, const T0 & _t0, const T1 & _t1, const T2 & _t2, const T3 & _t3 )
	{
		PyObject * py_args = pybind::tuple_new( 4 );

		pybind::tuple_setitem_t( py_args, 0, _t0 );
		pybind::tuple_setitem_t( py_args, 1, _t1 );
		pybind::tuple_setitem_t( py_args, 2, _t2 );
		pybind::tuple_setitem_t( py_args, 3, _t3 );

		PyObject * py_result = pybind::ask_native( _obj, py_args );

		pybind::decref( py_args );

		return detail::extract_operator_t( py_result );
	}
	//////////////////////////////////////////////////////////////////////////
	template<class T0, class T1, class T2, class T3, class T4>
	inline detail::extract_operator_t ask_t( PyObject * _obj, const T0 & _t0, const T1 & _t1, const T2 & _t2, const T3 & _t3, const T4 & _t4 )
	{
		PyObject * py_args = pybind::tuple_new( 5 );

		pybind::tuple_setitem_t( py_args, 0, _t0 );
		pybind::tuple_setitem_t( py_args, 1, _t1 );
		pybind::tuple_setitem_t( py_args, 2, _t2 );
		pybind::tuple_setitem_t( py_args, 3, _t3 );
		pybind::tuple_setitem_t( py_args, 4, _t4 );

		PyObject * py_result = pybind::ask_native( _obj, py_args );

		pybind::decref( py_args );

		return detail::extract_operator_t( py_result );
	}
	//////////////////////////////////////////////////////////////////////////
	template<class T0, class T1, class T2, class T3, class T4, class T5>
	inline detail::extract_operator_t ask_t( PyObject * _obj, const T0 & _t0, const T1 & _t1, const T2 & _t2, const T3 & _t3, const T4 & _t4, const T5 & _t5 )
	{
		PyObject * py_args = pybind::tuple_new( 6 );

		pybind::tuple_setitem_t( py_args, 0, _t0 );
		pybind::tuple_setitem_t( py_args, 1, _t1 );
		pybind::tuple_setitem_t( py_args, 2, _t2 );
		pybind::tuple_setitem_t( py_args, 3, _t3 );
		pybind::tuple_setitem_t( py_args, 4, _t4 );
		pybind::tuple_setitem_t( py_args, 5, _t5 );

		PyObject * py_result = pybind::ask_native( _obj, py_args );

		pybind::decref( py_args );

		return detail::extract_operator_t( py_result );
	}
	//////////////////////////////////////////////////////////////////////////
	template<class T0, class T1, class T2, class T3, class T4, class T5, class T6>
	inline detail::extract_operator_t ask_t( PyObject * _obj, const T0 & _t0, const T1 & _t1, const T2 & _t2, const T3 & _t3, const T4 & _t4, const T5 & _t5, const T6 & _t6 )
	{
		PyObject * py_args = pybind::tuple_new( 7 );

		pybind::tuple_setitem_t( py_args, 0, _t0 );
		pybind::tuple_setitem_t( py_args, 1, _t1 );
		pybind::tuple_setitem_t( py_args, 2, _t2 );
		pybind::tuple_setitem_t( py_args, 3, _t3 );
		pybind::tuple_setitem_t( py_args, 4, _t4 );
		pybind::tuple_setitem_t( py_args, 5, _t5 );
		pybind::tuple_setitem_t( py_args, 6, _t6 );

		PyObject * py_result = pybind::ask_native( _obj, py_args );

		pybind::decref( py_args );

		return detail::extract_operator_t( py_result );
	}
	//////////////////////////////////////////////////////////////////////////
	template<class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7>
	inline detail::extract_operator_t ask_t( PyObject * _obj, const T0 & _t0, const T1 & _t1, const T2 & _t2, const T3 & _t3, const T4 & _t4, const T5 & _t5, const T6 & _t6, const T7 & _t7 )
	{
		PyObject * py_args = pybind::tuple_new( 8 );

		pybind::tuple_setitem_t( py_args, 0, _t0 );
		pybind::tuple_setitem_t( py_args, 1, _t1 );
		pybind::tuple_setitem_t( py_args, 2, _t2 );
		pybind::tuple_setitem_t( py_args, 3, _t3 );
		pybind::tuple_setitem_t( py_args, 4, _t4 );
		pybind::tuple_setitem_t( py_args, 5, _t5 );
		pybind::tuple_setitem_t( py_args, 6, _t6 );
		pybind::tuple_setitem_t( py_args, 7, _t7 );

		PyObject * py_result = pybind::ask_native( _obj, py_args );

		pybind::decref( py_args );

		return detail::extract_operator_t( py_result );
	}
	//////////////////////////////////////////////////////////////////////////
	inline detail::extract_operator_t extract_t( PyObject * _obj )
	{
		pybind::incref( _obj );

		return detail::extract_operator_t( _obj );
	}
	//////////////////////////////////////////////////////////////////////////
	inline void call_t( PyObject * _obj )
	{
		PyObject * py_args = pybind::tuple_new( 0 );

		pybind::call_native( _obj, py_args );

		pybind::decref( py_args );
	}
	//////////////////////////////////////////////////////////////////////////
	template<class T0>
	inline void call_t( PyObject * _obj, const T0 & _t0 )
	{
		PyObject * py_args = pybind::tuple_new( 1 );

		pybind::tuple_setitem_t( py_args, 0, _t0 );

		pybind::call_native( _obj, py_args );

		pybind::decref( py_args );
	}
	//////////////////////////////////////////////////////////////////////////
	template<class T0, class T1>
	inline void call_t( PyObject * _obj, const T0 & _t0, const T1 & _t1 )
	{
		PyObject * py_args = pybind::tuple_new( 2 );

		pybind::tuple_setitem_t( py_args, 0, _t0 );
		pybind::tuple_setitem_t( py_args, 1, _t1 );

		pybind::call_native( _obj, py_args );

		pybind::decref( py_args );
	}
	//////////////////////////////////////////////////////////////////////////
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
	//////////////////////////////////////////////////////////////////////////
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
	//////////////////////////////////////////////////////////////////////////
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
	//////////////////////////////////////////////////////////////////////////
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
	//////////////////////////////////////////////////////////////////////////
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
	//////////////////////////////////////////////////////////////////////////
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