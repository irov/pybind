#	include "pybind/type_cast.hpp"
#	include "pybind/config.hpp"

#	include <map>
#	include <string>

namespace pybind
{
	namespace detail
	{
		typedef std::map<std::string, type_cast *> TMapExtractTypes;
		static TMapExtractTypes & mapExtractTypesInstance()
		{
			static TMapExtractTypes s_mapExtractTypes;
			return s_mapExtractTypes;
		}

		void register_type_info_extract( const type_info & _info, type_cast * _type )
		{
			const char * name = _info.name();
			mapExtractTypesInstance()[ name ] = _type;
		}

		type_cast * find_type_info_extract( const type_info & _info )
		{
			const char * name = _info.name();
			TMapExtractTypes::iterator it_find = mapExtractTypesInstance().find( name );

			if( it_find == mapExtractTypesInstance().end() )
			{
				return 0;
			}

			return it_find->second;
		}
	}

	static struct extract_bool_type
		: public type_cast_result<bool>
	{
		bool apply( PyObject * _obj ) override
		{
			m_valid = false;

			if( PyBool_Check( _obj ) )
			{
				m_valid = true;
				return (_obj == Py_True);				
			}
			else if( PyInt_Check( _obj ) )
			{
				m_valid = true;
				return (PyInt_AsLong( _obj ) != 0);
			}
			else if( PyLong_Check( _obj ) )
			{
				m_valid = true;
				return ( PyLong_AsLong( _obj ) != 0 );				
			}
			else if( PyFloat_Check( _obj ) )
			{
				m_valid = true;
				return ( int(PyFloat_AsDouble( _obj )) != 0 );
			}

			return false;
		}

		PyObject * wrapp( bool _value ) override
		{
			return PyBool_FromLong( _value );
		}
	}s_extract_bool_type;

	static struct extract_int_type
		: public type_cast_result<int>
	{
		int apply( PyObject * _obj ) override
		{
			m_valid = false;

			if( PyInt_Check( _obj ) )
			{
				m_valid = true;
				return PyInt_AsLong( _obj );
			}
			else if( PyLong_Check( _obj ) )
			{
				m_valid = true;
				return (int)PyLong_AsLong( _obj );
			}
			else if( PyFloat_Check( _obj ) )
			{
				m_valid = true;
				return (int)PyFloat_AsDouble( _obj );				
			}

			return 0;
		}

		PyObject * wrapp( int _value ) override
		{
			return PyInt_FromLong( _value );
		}
	}s_extract_int_type;

	static struct extract_unsigned_int_type
		: public type_cast_result<unsigned int>
	{
		unsigned int apply( PyObject * _obj ) override
		{
			m_valid = false;
			if( PyInt_Check( _obj ) )
			{
				m_valid = true;
				return PyInt_AsUnsignedLongMask( _obj );
			}
			else if( PyLong_Check( _obj ) )
			{				
				m_valid = true;
				return (unsigned int)PyLong_AsUnsignedLong( _obj );
			}
			else if( PyFloat_Check( _obj ) )
			{				
				m_valid = true;
				return (unsigned int)PyFloat_AsDouble( _obj );
			}

			return 0;
		}

		PyObject * wrapp( unsigned int _value ) override
		{
			return PyInt_FromLong( _value );
		}
	}s_extract_unsigned_int_type;

	static struct extract_float_type
		: public type_cast_result<float>
	{
		float apply( PyObject * _obj ) override
		{
			m_valid = false;
			if( PyFloat_Check( _obj ) )
			{
				m_valid = true;
				return (float)PyFloat_AsDouble( _obj );
			}
			else if( PyLong_Check( _obj ) )
			{
				m_valid = true;
				return (float)PyLong_AsLong( _obj );
			}
			else if( PyInt_Check( _obj ) )
			{
				m_valid = true;
				return (float)PyInt_AsLong( _obj );
			}

			return 0.f;
		}

		PyObject * wrapp( float _value ) override
		{
			return PyFloat_FromDouble( _value );
		}
	}s_extract_float_type;

	static struct extract_double_type
		: public type_cast_result<double>
	{
		double apply( PyObject * _obj ) override
		{
			m_valid = false;
			if( PyFloat_Check( _obj ) )
			{
				m_valid = true;
				return PyFloat_AsDouble( _obj );				
			}
			else if( PyLong_Check( _obj ) )
			{
				m_valid = true;
				return (float)PyLong_AsLong( _obj );				
			}
			else if( PyInt_Check( _obj ) )
			{
				m_valid = true;
				return (float)PyInt_AsLong( _obj );				
			}

			return 0.0;
		}
		PyObject * wrapp( double _value ) override
		{
			return PyFloat_FromDouble( _value );
		}
	}s_extract_double_type;

	static struct extract_string_type
		: public type_cast_result<std::string>
	{
		std::string apply( PyObject * _obj ) override
		{
			m_valid = false;

			if( PyString_Check( _obj ) )
			{
				m_valid = true;
				char * str = PyString_AsString( _obj );
				if( str )
				{
					return std::string(str);
				}
				else
				{
					return std::string();
				}				
			}

			return std::string();
		}
		PyObject * wrapp( std::string _value ) override
		{
			return PyString_FromStringAndSize( _value.c_str(), _value.size() );
		}
	}s_extract_string_type;

	static struct extract_pyobject_type
		: public type_cast_result<PyObject *>
	{
		PyObject * apply( PyObject * _obj ) override
		{
			m_valid = true;
			return _obj;
		}
		PyObject * wrapp( PyObject * _value ) override
		{
			return _value;
		}
	}s_extract_pyobject_type;
}