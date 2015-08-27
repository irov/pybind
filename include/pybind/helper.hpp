#	pragma once

#	include "system.hpp"
#	include "extract.hpp"

#	include <stddef.h>

namespace pybind
{
	//////////////////////////////////////////////////////////////////////////
	namespace detail
	{
		//////////////////////////////////////////////////////////////////////////
		class return_operator_t
		{
		public:
			explicit return_operator_t( const return_operator_t & _op )
				: m_obj( _op.m_obj )
			{
			}

			explicit return_operator_t( PyObject * _value )
				: m_obj( _value )
			{
			}

			template<class T>
			explicit return_operator_t( const T & _value )
				: m_obj( ptr_throw_specialized<T>()(_value) )
			{
			}

		public:
			operator PyObject * () const
			{
				return m_obj;
			}

		protected:
			PyObject * m_obj;
		};
		//////////////////////////////////////////////////////////////////////////
		class import_operator_t
		{
		public:
			import_operator_t( const import_operator_t  & _op )
				: m_obj(_op.m_obj)
			{
				pybind::incref( m_obj );
			}
						
			import_operator_t( PyObject * _value )
				: m_obj( _value )
			{
				pybind::incref( m_obj );
			}

			template<class T>
			import_operator_t( const T & _value )
				: m_obj( ptr_throw_specialized<T>()(_value) )
			{				
			}

		public:
			~import_operator_t()
			{
				pybind::decref( m_obj );
			}

		public:
			operator PyObject * () const
			{
				return m_obj;
			}

		protected:
			PyObject * m_obj;
		};
		//////////////////////////////////////////////////////////////////////////
		class extract_operator_t
		{
		public:
			extract_operator_t( const extract_operator_t & _r )
				: m_obj( _r.m_obj )
			{
			}

			explicit extract_operator_t( PyObject * _obj )
				: m_obj( _obj )
			{
			}

		public:
			~extract_operator_t()
			{
			}

		public:
			operator PyObject * ()
			{
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
	PYBIND_API detail::extract_operator_t list_getitem_t( PyObject * _list, size_t _it );
	PYBIND_API bool list_setitem_t( PyObject * _list, size_t _it, const detail::import_operator_t & _item );
	PYBIND_API bool list_appenditem_t( PyObject * _obj, const detail::import_operator_t & _item );
	//////////////////////////////////////////////////////////////////////////
	template<class It>
	PYBIND_API bool list_appenditems_t( PyObject * _obj, It _begin, It _end )
	{
		for( It it = _begin; it != _end; ++it )
		{
			if( pybind::list_appenditem_t( _obj, *it ) == false )
			{
				return false;
			}
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////	
	PYBIND_API detail::extract_operator_t tuple_getitem_t( PyObject * _tuple, size_t _it );
	PYBIND_API bool tuple_setitem_t( PyObject * _tuple, size_t _it, const detail::import_operator_t & _item );	
	PYBIND_API bool dict_setstring_t( PyObject * _dict, const char * _name, const detail::import_operator_t & _value );
	PYBIND_API bool dict_set_t( PyObject * _dict, const detail::import_operator_t & _name, const detail::import_operator_t & _value );
	PYBIND_API detail::extract_operator_t dict_get_t( PyObject * _dict, const detail::import_operator_t & _key );
	PYBIND_API bool dict_remove_t( PyObject * _dict, const detail::import_operator_t & _key );
	template<class K, class V>
	PYBIND_API bool dict_next_t( PyObject * _dict, size_t & _pos, K & _key, V & _value )
	{
		PyObject * py_key;
		PyObject * py_value;

		if( pybind::dict_next( _dict, _pos, &py_key, &py_value ) == false )
		{
			return false;
		}

		_key = pybind::extract<K>( py_key );
		_value = pybind::extract<V>( py_value );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	PYBIND_API detail::extract_operator_t ask_tuple( PyObject * _obj, const pybind::tuple & _tuple );	
	PYBIND_API detail::extract_operator_t call_t( PyObject * _obj );
	PYBIND_API detail::extract_operator_t call_t( PyObject * _obj, const detail::import_operator_t & _t0 );
	PYBIND_API detail::extract_operator_t call_t( PyObject * _obj, const detail::import_operator_t & _t0, const detail::import_operator_t & _t1 );
	PYBIND_API detail::extract_operator_t call_t( PyObject * _obj, const detail::import_operator_t & _t0, const detail::import_operator_t & _t1, const detail::import_operator_t & _t2 );
	PYBIND_API detail::extract_operator_t call_t( PyObject * _obj, const detail::import_operator_t & _t0, const detail::import_operator_t & _t1, const detail::import_operator_t & _t2, const detail::import_operator_t & _t3 );
	PYBIND_API detail::extract_operator_t call_t( PyObject * _obj, const detail::import_operator_t & _t0, const detail::import_operator_t & _t1, const detail::import_operator_t & _t2, const detail::import_operator_t & _t3, const detail::import_operator_t & _t4 );
	PYBIND_API detail::extract_operator_t call_t( PyObject * _obj, const detail::import_operator_t & _t0, const detail::import_operator_t & _t1, const detail::import_operator_t & _t2, const detail::import_operator_t & _t3, const detail::import_operator_t & _t4, const detail::import_operator_t & _t5 );
	PYBIND_API detail::extract_operator_t call_t( PyObject * _obj, const detail::import_operator_t & _t0, const detail::import_operator_t & _t1, const detail::import_operator_t & _t2, const detail::import_operator_t & _t3, const detail::import_operator_t & _t4, const detail::import_operator_t & _t5, const detail::import_operator_t & _t6 );
	PYBIND_API detail::extract_operator_t call_t( PyObject * _obj, const detail::import_operator_t & _t0, const detail::import_operator_t & _t1, const detail::import_operator_t & _t2, const detail::import_operator_t & _t3, const detail::import_operator_t & _t4, const detail::import_operator_t & _t5, const detail::import_operator_t & _t6, const detail::import_operator_t & _t7 );
	PYBIND_API detail::extract_operator_t extract_t( PyObject * _obj );
}