#	include "pybind/extract.hpp"

#	include "pybind/system.hpp"

#	include "config/python.hpp"

namespace pybind
{
	//////////////////////////////////////////////////////////////////////////
	bool extract_value( PyObject * _obj, bool & _value )
	{
		if( PyBool_Check( _obj ) )
		{
			_value = (_obj == Py_True);
		}
#	ifndef PYBIND_PYTHON_3
		else if( PyInt_Check( _obj ) )
		{
			_value = (PyInt_AsLong( _obj ) != 0);
		}
#	endif
		else if( PyLong_Check( _obj ) )
		{
			_value = ( PyLong_AsLong( _obj ) != 0 );				
		}
		else if( PyFloat_Check( _obj ) )
		{
			_value = ( int(PyFloat_AsDouble( _obj )) != 0 );
		}
		else
		{
			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool extract_value( PyObject * _obj, int & _value )
	{
		if( PyLong_Check( _obj ) )
		{
			_value = (int)PyLong_AsLong( _obj );
		}
#	ifndef PYBIND_PYTHON_3
		else if( PyInt_Check( _obj ) )
		{
			_value = PyInt_AsLong( _obj );
		}
#	endif
		else if( PyFloat_Check( _obj ) )
		{
			_value = (int)PyFloat_AsDouble( _obj );				
		}
		else
		{
			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool extract_value( PyObject * _obj, size_t & _value )
	{
		if( PyLong_Check( _obj ) )
		{				
			_value = (size_t)PyLong_AsUnsignedLong( _obj );
		}
#	ifndef PYBIND_PYTHON_3
		else if( PyInt_Check( _obj ) )
		{
			_value = (size_t)PyInt_AsUnsignedLongMask( _obj );
		}
#	endif
		else if( PyFloat_Check( _obj ) )
		{				
			_value = (size_t)PyFloat_AsDouble( _obj );
		}
		else
		{
			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool extract_value( PyObject * _obj, float & _value )
	{
		if( PyFloat_Check( _obj ) )
		{
			_value = (float)PyFloat_AsDouble( _obj );
		}
		else if( PyLong_Check( _obj ) )
		{
			_value = (float)PyLong_AsLong( _obj );
		}
#	ifndef PYBIND_PYTHON_3
		else if( PyInt_Check( _obj ) )
		{
			_value = (float)PyInt_AsLong( _obj );
		}
#	endif
		else
		{
			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool extract_value( PyObject * _obj, double & _value )
	{
		if( PyFloat_Check( _obj ) )
		{
			_value = (double)PyFloat_AsDouble( _obj );				
		}
		else if( PyLong_Check( _obj ) )
		{
			_value = (double)PyLong_AsLong( _obj );				
		}
#	ifndef PYBIND_PYTHON_3
		else if( PyInt_Check( _obj ) )
		{
			_value = (double)PyInt_AsLong( _obj );				
		}
#	endif
		else
		{
			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool extract_value( PyObject * _obj, const char *& _value )
	{
		if( pybind::string_check( _obj ) == false )
		{
			size_t size;
			_value = pybind::string_to_char( _obj, size );
		}
		else
		{
			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool extract_value( PyObject * _obj, std::string & _value )
	{
		if( pybind::string_check( _obj ) )
		{
			size_t ch_size;

			const char * ch_buff = pybind::string_to_char(_obj, ch_size );			

			if( ch_size == 0 )
			{
				_value.clear();

				return true;
			}

			_value.assign( ch_buff, ch_size );
		}
		else
		{
			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool extract_value( PyObject * _obj, std::wstring & _value )
	{
		if( pybind::unicode_check( _obj ) )
		{
			size_t ch_size;

			const wchar_t * ch_buff = pybind::unicode_to_wchar(_obj, ch_size );

			if( ch_size == 0 )
			{
				_value.clear();

				return true;
			}

			_value.assign( ch_buff, ch_size );
		}
		else
		{
			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool extract_value( PyObject * _obj, PyObject *& _value )
	{
		_value = _obj;

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	PyObject * ptr( bool _value )
	{
		if( _value == true )
		{
			Py_RETURN_TRUE;
		}
		
		Py_RETURN_FALSE;
	}
	//////////////////////////////////////////////////////////////////////////
	PyObject * ptr( int _value )
	{
#	ifndef PYBIND_PYTHON_3
		return PyInt_FromLong( _value );
#	else
		return PyLong_FromLong( _value );
#	endif
	}
	//////////////////////////////////////////////////////////////////////////
	PyObject * ptr( size_t _value )
	{
#	ifndef PYBIND_PYTHON_3
		return PyInt_FromLong( _value );
#	else
		return PyLong_FromLong( _value );
#	endif
	}
	//////////////////////////////////////////////////////////////////////////
	PyObject * ptr( float _value )
	{
		return PyFloat_FromDouble( _value );
	}
	//////////////////////////////////////////////////////////////////////////
	PyObject * ptr( double _value )
	{
		return PyFloat_FromDouble( _value );
	}
	//////////////////////////////////////////////////////////////////////////
	PyObject * ptr( const char * _value )
	{
		size_t size = ::strlen(_value);
		return pybind::string_from_char( _value, size );
	}
	//////////////////////////////////////////////////////////////////////////
	PyObject * ptr( const std::string & _value )
	{
		return pybind::string_from_char( _value.c_str(), _value.size() );
	}
	//////////////////////////////////////////////////////////////////////////
	PyObject * ptr( const std::wstring & _value )
	{
		return pybind::unicode_from_wchar( _value.c_str(), _value.size() );
	}
	//////////////////////////////////////////////////////////////////////////
	PyObject * ptr( PyObject * _value )
	{
		return _value;
	}
}