#	include "pybind/type_cast.hpp"
#	include "pybind/config.hpp"

#	include <map>
#	include <string>

namespace pybind
{
	namespace detail
	{
		typedef std::map<std::string, type_cast *> TMapExtractTypes;
		static TMapExtractTypes s_mapExtractTypes;

		void register_type_info_extract( const type_info & _info, type_cast * _type )
		{
			const char * name = _info.name();
			s_mapExtractTypes[ name ] = _type;
		}

		type_cast * find_type_info_extract( const type_info & _info )
		{
			const char * name = _info.name();
			TMapExtractTypes::iterator it_find = s_mapExtractTypes.find( name );

			if( it_find == s_mapExtractTypes.end() )
			{
				return 0;
			}

			return it_find->second;
		}
	}

	static struct extract_bool_type
		: public type_cast_result<bool>
	{
		void apply( PyObject * _obj ) override
		{
			m_valid = false;
			if( PyBool_Check( _obj ) )
			{
				m_result = (_obj == Py_True);
				m_valid = true;
			}
		}

		PyObject * wrapp( bool _value ) override
		{
			return PyBool_FromLong( _value );
		}
	}s_extract_bool_type;

	static struct extract_int_type
		: public type_cast_result<int>
	{
		void apply( PyObject * _obj ) override
		{
			m_valid = false;
			if( PyInt_Check( _obj ) )
			{
				m_result = PyInt_AsLong( _obj );
				m_valid = true;
			}
		}

		PyObject * wrapp( int _value ) override
		{
			return PyInt_FromLong( _value );
		}
	}s_extract_int_type;

	static struct extract_float_type
		: public type_cast_result<float>
	{
		void apply( PyObject * _obj ) override
		{
			m_valid = false;
			if( PyFloat_Check( _obj ) )
			{
				m_result = (float)PyFloat_AsDouble( _obj );
				m_valid = true;
			}
		}

		PyObject * wrapp( float _value ) override
		{
			return PyFloat_FromDouble( _value );
		}
	}s_extract_float_type;

	static struct extract_double_type
		: public type_cast_result<double>
	{
		void apply( PyObject * _obj ) override
		{
			m_valid = false;
			if( PyFloat_Check( _obj ) )
			{
				m_result = PyFloat_AsDouble( _obj );
				m_valid = true;
			}
		}
		PyObject * wrapp( double _value ) override
		{
			return PyFloat_FromDouble( _value );
		}
	}s_extract_double_type;

	static struct extract_string_type
		: public type_cast_result<std::string>
	{
		void apply( PyObject * _obj ) override
		{
			m_valid = false;
			if( PyString_Check( _obj ) )
			{
				char * str = PyString_AsString( _obj );
				if( str )
				{
					m_result = str;
				}
				else
				{
					m_result = "";
				}
				m_valid = true;
			}
		}
		PyObject * wrapp( std::string _value ) override
		{
			return PyString_FromStringAndSize( _value.c_str(), _value.size() );
		}
	}s_extract_string_type;
}