#pragma once

#include "pybind/helper.hpp"

namespace pybind
{
	//////////////////////////////////////////////////////////////////////////
	namespace detail
	{
		//////////////////////////////////////////////////////////////////////////
		class set_list_operator_t
		{
		public:
			set_list_operator_t( kernel_interface * _kernel, PyObject * _list, uint32_t _index )
				: m_kernel( _kernel )
				, m_list( _list )
				, m_index( _index )
			{
			}

			set_list_operator_t( const set_list_operator_t & _r )
				: m_kernel( _r.m_kernel )
				, m_list( _r.m_list )
				, m_index( _r.m_index )
			{
			}

			~set_list_operator_t()
			{
			}
			
		public:
            void operator = ( const import_operator_t & _imp )
            {
                pybind::list_setitem_i( m_kernel, m_list, m_index, _imp );
            }

			template<class T>
			void operator = (const T & _value)
			{
				pybind::list_setitem_i( m_kernel, m_list, m_index, import_operator_t( m_kernel, _value ) );
			}

		public:
			operator PyObject * ()
			{
				PyObject * obj = pybind::list_getitem( m_list, m_index );

				return obj;
			}

			template<class T>
			operator T ()
			{
				return pybind::list_getitem_t( m_kernel, m_list, m_index );
			}

			template<class T>
			bool operator == (const T & _value)
			{
				return pybind::list_getitem_t( m_kernel, m_list, m_index ) == _value;
			}

		protected:
			kernel_interface * m_kernel;
			PyObject * m_list;
            uint32_t m_index;
		};
	}
}

