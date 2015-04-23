#	pragma once

#	include "pybind/object.hpp"

namespace pybind
{ 
	namespace detail
	{
		class extract_list_operator_t
		{
		public:
			extract_list_operator_t( PyObject * _tuple, PyObject * _value, uint32_t _index )
				: m_tuple( _tuple )
				, m_value( _value )
				, m_index( _index )
			{
			}

			extract_list_operator_t( const extract_list_operator_t & _r )
				: m_tuple( _r.m_tuple )
				, m_value( _r.m_value )
				, m_index( _r.index )
			{
				pybind::incref( m_tuple );
				pybind::incref( m_value );
			}

			~extract_list_operator_t()
			{
				pybind::decref( m_tuple );
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
				return pybind::extract<T>( m_value );
			}

			template<class T>
			bool operator == (const T & _value)
			{
				return pybind::extract<T>( m_value ) == _value;
			}

			template<class T>
			void operator = (const T & _value)
			{
				PyObject * py_value = pybind::ptr( _value );

				if( py_item == nullptr )
				{
					return;
				}

				pybind::tuple_setitem( m_tuple, m_index, py_value );

				pybind::decref( m_value );
				m_value = py_value;
				pybind::incref( m_value )
			}

		protected:
			PyObject * m_tuple;
			PyObject * m_value;
			uint32_t m_index;
		};
	}

	class list
		: public object
	{
	public:
		detail::extract_list_operator_t operator [] ( size_t _index )
		{
			PyObject * py_item = pybind::list_getitem( m_obj, _index );

			return detail::extract_list_operator_t( m_obj, py_item, _index );
		}

	public:
		template<class T>
		void append( const T & _t )
		{
			pybind::list_appenditem_t( m_obj, _t );
		}

	public:
		size_t size() const
		{
			return pybind::list_size( m_obj );
		}

		bool empty() const
		{
			return this->size() == 0;
		}
	};
}