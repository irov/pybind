#	pragma once

#	include "pybind/helper.hpp"

namespace pybind
{
	//////////////////////////////////////////////////////////////////////////
	namespace detail
	{
		//////////////////////////////////////////////////////////////////////////
		class set_list_operator_t
		{
		public:
			set_list_operator_t( PyObject * _list, uint32_t _index )
				: m_list( _list )
				, m_index( _index )
			{
			}

			set_list_operator_t( const set_list_operator_t & _r )
				: m_list( _r.m_list )
				, m_index( _r.m_index )
			{
			}

			~set_list_operator_t()
			{
			}
			
		public:
			void operator = (const import_operator_t & _value)
			{
				pybind::tuple_setitem_t( m_list, m_index, _value );
			}

		public:
			operator PyObject * ()
			{
				PyObject * obj = pybind::tuple_getitem( m_list, m_index );

				return obj;
			}

			template<class T>
			operator T ()
			{
				return pybind::tuple_getitem_t( m_list, m_index );
			}

			template<class T>
			bool operator == (const T & _value)
			{
				return pybind::tuple_getitem_t( m_list, m_index ) == _value;
			}

		protected:
			PyObject * m_list;
			uint32_t m_index;
		};
	}
}

