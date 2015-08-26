#	pragma once

#	include "pybind/object.hpp"

namespace pybind
{
	//////////////////////////////////////////////////////////////////////////
	namespace detail
	{
		//////////////////////////////////////////////////////////////////////////
		class set_dict_operator_t
		{
		public:
			set_dict_operator_t( PyObject * _dict, PyObject * _key )
				: m_dict( _dict )
				, m_key( _key )
			{
			}

			set_dict_operator_t( const set_dict_operator_t & _r )
				: m_dict( _r.m_dict )
				, m_key( _r.m_key )
			{
			}

			~set_dict_operator_t()
			{
			}

		public:
			void operator = (const import_operator_t & _value)
			{				
				pybind::dict_set_t( m_dict, m_key, _value );
			}

		public:
			operator PyObject * ()
			{
				PyObject * py_object = pybind::dict_get( m_dict, m_key );

				return py_object;
			}

			template<class T>
			operator T ()
			{
				return pybind::dict_get_t( m_dict, m_key );
			}

			template<class T>
			bool operator == (const T & _value)
			{
				return pybind::dict_get_t( m_dict, m_key ) == _value;
			}

		protected:
			PyObject * m_dict;
			PyObject * m_key;
		};
	}
}