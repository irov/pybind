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
		class import_operator_t
		{
		public:
			import_operator_t( const import_operator_t  & _op )
				: m_obj(_op.m_obj)
			{
			}

			template<class T>
			import_operator_t( const T & _value )
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
		class borrowed_import_operator_t
			: public import_operator_t
		{
		public:
			borrowed_import_operator_t( const import_operator_t  & _op )
				: import_operator_t( _op )
			{
			}

			template<class T>
			borrowed_import_operator_t( const T & _value )
				: import_operator_t( _value )
			{
			}

			~borrowed_import_operator_t()
			{
				pybind::decref( m_obj );
			}
		};
		//////////////////////////////////////////////////////////////////////////
		class extract_operator_t
		{
		public:
			explicit extract_operator_t( PyObject * _obj )
				: m_obj( _obj )
			{
				pybind::incref( m_obj );
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
		//////////////////////////////////////////////////////////////////////////
		class extract_list_operator_t
		{
		public:
			extract_list_operator_t( PyObject * _list, PyObject * _value, uint32_t _index )
				: m_list( _list )
				, m_value( _value )
				, m_index( _index )
			{
			}

			extract_list_operator_t( const extract_list_operator_t & _r )
				: m_list( _r.m_list )
				, m_value( _r.m_value )
				, m_index( _r.m_index )
			{
				pybind::incref( m_list );
				pybind::incref( m_value );
			}

			~extract_list_operator_t()
			{
				pybind::decref( m_list );
				pybind::decref( m_value );
			}

			operator PyObject * ()
			{
				pybind::incref( m_value );

				return m_value;
			}

			template<class T>
			operator T ()
			{
				return pybind::extract_throw<T>( m_value );
			}

			template<class T>
			bool operator == (const T & _value)
			{
				return pybind::extract_throw<T>( m_value ) == _value;
			}

			template<class T>
			void operator = (const T & _value)
			{
				PyObject * py_value = pybind::ptr( _value );

				if( py_value == nullptr )
				{
					return;
				}

				pybind::tuple_setitem( m_list, m_index, py_value );

				pybind::decref( m_value );
				m_value = py_value;
				pybind::incref( m_value );
			}

		protected:
			PyObject * m_list;
			PyObject * m_value;
			uint32_t m_index;
		};
	}
	//////////////////////////////////////////////////////////////////////////
	PYBIND_API detail::extract_operator_t list_getitem_t( PyObject * _list, size_t _it );
	PYBIND_API bool list_setitem_t( PyObject * _list, size_t _it, const detail::import_operator_t & _item );
	PYBIND_API bool list_appenditem_t( PyObject * _obj, const detail::borrowed_import_operator_t & _item );
	PYBIND_API bool dict_setstring_t( PyObject * _dict, const char * _name, const detail::borrowed_import_operator_t & _value );
	PYBIND_API detail::extract_operator_t tuple_getitem_t( PyObject * _tuple, size_t _it );
	PYBIND_API bool tuple_setitem_t( PyObject * _tuple, size_t _it, const detail::import_operator_t & _item );
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