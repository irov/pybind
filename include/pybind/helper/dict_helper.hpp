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
			set_dict_operator_t( kernel_interface * _kernel, PyObject * _dict, PyObject * _key )
				: m_kernel( _kernel )
				, m_dict( _dict )
				, m_key( _key )
			{
			}

			set_dict_operator_t( const set_dict_operator_t & _r )
				: m_kernel( _r.m_kernel )
				, m_dict( _r.m_dict )
				, m_key( _r.m_key )
			{
			}

			~set_dict_operator_t()
			{
			}

		public:
            set_dict_operator_t & operator = ( const import_operator_t & _value )
            {
                pybind::dict_set_t( m_kernel, m_dict, m_key, _value );

                return *this;
            }

            template<class T>
			set_dict_operator_t & operator = (const T & _value)
			{
				return this->operator = (import_operator_t( m_kernel, _value ));
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
				return pybind::dict_get_t( m_kernel, m_dict, m_key );
			}

			template<class T>
			bool operator == (const T & _value)
			{
				return pybind::dict_get_t( m_kernel, m_dict, m_key ) == _value;
			}

		protected:
			kernel_interface * m_kernel;

			PyObject * m_dict;
			PyObject * m_key;
		};
	}
}