#	include "pybind/type_cast.hpp"
#	include "pybind/class_core.hpp"
#	include "pybind/class_type.hpp"

#	include "pybind/convert_adapter.hpp"

#	include "config/python.hpp"
#	include "pybind/system.hpp"

#	include <map>
#	include <string>

namespace pybind
{
	namespace detail
	{
		typedef std::map<const char *, type_cast *> TMapExtractTypes;

		static TMapExtractTypes & mapExtractTypesInstance()
		{
			static TMapExtractTypes s_mapExtractTypes;
			return s_mapExtractTypes;
		}

		void register_type_info_extract( const std::type_info & _info, type_cast * _type )
		{
			const char * name = _info.name();
			TMapExtractTypes & types = mapExtractTypesInstance();
			types[ name ] = _type;
		}

		type_cast * find_type_info_extract( const std::type_info & _info )
		{
			const char * name = _info.name();
			TMapExtractTypes & types = mapExtractTypesInstance();
			TMapExtractTypes::iterator it_find = types.find( name );

			if( it_find == types.end() )
			{
				return 0;
			}

			return it_find->second;
		}

		void error_invalid_extract( PyObject * _obj, const std::type_info & _tinfo )
		{
			if( const char * repr = pybind::object_to_string( _obj ) )
			{
				pybind::error_message( "invalid extract %s from %.256s"
					, _tinfo.name()
					, repr
					);
			}
			else
			{
				pybind::error_message( "invalid extract %s from unknown object type %s"
					, _tinfo.name()
					, _obj->ob_type->tp_name
					);
			}
		}

		bool convert_object( PyObject * _obj, const std::type_info & _tinfo, void * _place )
		{
			class_type_scope * scope = detail::get_class_type_scope( _tinfo );

			convert_adapter_interface * convert = scope->get_convert();

			if( convert == 0 )
			{
				return false;
			}

			bool result = convert->convert( _obj, _place );
			
			return result;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool type_cast::type_info_cast( PyObject * _obj, const std::type_info & _tinfo, const std::type_info & _tptrinfo, void ** _impl )
	{
		m_valid = false;

		if( detail::is_class( _obj ) == false )
		{
			return false;
		}

		void * impl = detail::get_class_impl( _obj );

		if( impl == 0 )
		{
			error_message( "type_info_cast: unbind object" );
			return false;
		}

		class_type_scope * scope = detail::get_class_scope( _obj->ob_type );
		class_type_scope * cur_scope = detail::get_class_type_scope( _tinfo );

		void * result = 0;

		if( cur_scope != scope )
		{
			const std::type_info & tinfo = _tptrinfo;
			const char * name = tinfo.name();
			impl = class_core::meta_cast( impl, scope, name );
		}

		if( impl == 0 )
		{
			return false;
		}

		m_valid = true;

		*_impl = impl;

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
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

		PyObject * wrap( bool _value ) override
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

		PyObject * wrap( int _value ) override
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

		PyObject * wrap( unsigned int _value ) override
		{
			return PyInt_FromLong( _value );
		}
	}s_extract_unsigned_int_type;


	static struct extract_size_t_type
		: public type_cast_result<std::size_t>
	{
		size_t apply( PyObject * _obj ) override
		{
			m_valid = false;
			if( PyInt_Check( _obj ) )
			{
				m_valid = true;
				return (std::size_t)PyInt_AsUnsignedLongMask( _obj );
			}
			else if( PyLong_Check( _obj ) )
			{				
				m_valid = true;
				return (std::size_t)PyLong_AsUnsignedLong( _obj );
			}
			else if( PyFloat_Check( _obj ) )
			{				
				m_valid = true;
				return (std::size_t)PyFloat_AsDouble( _obj );
			}

			return 0;
		}

		PyObject * wrap( std::size_t _value ) override
		{
			return PyInt_FromLong( (long)_value );
		}
	}s_extract_size_t_type;


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

		PyObject * wrap( float _value ) override
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
		PyObject * wrap( double _value ) override
		{
			return PyFloat_FromDouble( _value );
		}
	}s_extract_double_type;

	static struct extract_cchar_type
		: public type_cast_result<const char *>
	{
		const char * apply( PyObject * _obj ) override
		{
			m_valid = false;

			if( PyString_Check( _obj ) )
			{
				m_valid = true;
				const char * str = PyString_AS_STRING( _obj );

				return str;
			}
			else if( _obj == Py_None )
			{
				m_valid = true;

				return 0;
			}

			return 0;
		}
		PyObject * wrap( const char * _value ) override
		{
			return PyString_FromString( _value );
		}
	}s_extract_cchar_type;

	static struct extract_string_type
		: public type_cast_result<std::string>
	{
		std::string apply( PyObject * _obj ) override
		{
			m_valid = false;

			if( PyString_Check( _obj ) )
			{
				m_valid = true;
				
				const char * ch_buff = PyString_AsString(_obj);
				Py_ssize_t ch_size = PyString_Size(_obj);
				
				return std::string( ch_buff, ch_size );
			}
			else if( PyUnicode_Check( _obj ) )
			{
				m_valid = true;

				PyObject* strObj = PyUnicode_AsUTF8String( _obj );

				const char * ch_buff = PyString_AsString(strObj);
				Py_ssize_t ch_size = PyString_Size(strObj);

				return std::string( ch_buff, ch_size );
			}
			else if( _obj == Py_None )
			{
				m_valid = true;

				return std::string();
			}

			return std::string();
		}

		PyObject * wrap( std::string _value ) override
		{
			return PyString_FromStringAndSize( _value.c_str(), _value.size() );
		}


	}s_extract_string_type;

	//static struct extract_wstring_type
	//	: public type_cast_result<std::wstring>
	//{
	//	std::wstring apply( PyObject * _obj ) override
	//	{
	//		m_valid = false;

	//		if( PyString_Check( _obj ) )
	//		{
	//			m_valid = true;

	//			char * str = PyString_AsString( _obj );

	//			if( str == 0 )
	//			{
	//				return std::wstring();
	//			}

	//			Py_ssize_t size = PyString_Size( _obj );

	//			const char * encoding = PyUnicode_GetDefaultEncoding();

	//			PyObject * unicode = PyUnicode_Decode( str, size, "mbcs", 0 );

	//			Py_UNICODE * unicode_str = PyUnicode_AsUnicode( unicode );

	//			if( unicode_str == 0 )
	//			{
	//				throw_exception();
	//			}

	//			return std::wstring(unicode_str);
	//		}
	//		else if ( PyUnicode_Check( _obj ) )
	//		{
	//			m_valid = true;

	//			wchar_t * unicode_str = PyUnicode_AsUnicode( _obj );

	//			if( unicode_str == 0 )
	//			{
	//				throw_exception();
	//			}

	//			return std::wstring(unicode_str);
	//		}

	//		return std::wstring();
	//	}

	//	PyObject * wrap( std::wstring _value ) override
	//	{
	//		return PyUnicode_FromWideChar( _value.c_str(), _value.size() );
	//	}
	//}s_extract_wstring_type;

	static struct extract_pyobject_type
		: public type_cast_result<PyObject *>
	{
		PyObject * apply( PyObject * _obj ) override
		{
			m_valid = true;
			return _obj;
		}
		PyObject * wrap( PyObject * _value ) override
		{
			return _value;
		}
	}s_extract_pyobject_type;
}
