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
#   if PYBIND_PYTHON_VERSION < 300
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
#   if PYBIND_PYTHON_VERSION < 300
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
	bool extract_value( PyObject * _obj, unsigned int & _value )
	{
		if( PyLong_Check( _obj ) )
		{				
			_value = (size_t)PyLong_AsUnsignedLong( _obj );
		}
#   if PYBIND_PYTHON_VERSION < 300
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
	bool extract_value( PyObject * _obj, unsigned long & _value )
	{
		if( PyLong_Check( _obj ) )
		{				
			_value = (size_t)PyLong_AsUnsignedLong( _obj );
		}
#   if PYBIND_PYTHON_VERSION < 300
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
#   if PYBIND_PYTHON_VERSION < 300
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
#   if PYBIND_PYTHON_VERSION < 300
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
		if( pybind::string_check( _obj ) == true )
		{
            const char * ch_buff = pybind::string_to_char( _obj );

            if( ch_buff == 0 )
            {
                return false;
            }

			_value = ch_buff;
		}
		else
        {
			return false;
		}

		return true;
	}
    //////////////////////////////////////////////////////////////////////////
    bool extract_value( PyObject * _obj, const pybind_unicode_t *& _value )
    {
        if( pybind::unicode_check( _obj ) == true )
        {
            const pybind_unicode_t * ch_buff = pybind::unicode_to_wchar( _obj );

            if( ch_buff == 0 )
            {
                return false;
            }

            _value = ch_buff;
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
#   if PYBIND_PYTHON_VERSION < 300
		return PyInt_FromLong( _value );
#	else
		return PyLong_FromLong( _value );
#	endif
	}
	//////////////////////////////////////////////////////////////////////////
	PyObject * ptr( unsigned int _value )
	{
#   if PYBIND_PYTHON_VERSION < 300
		return PyInt_FromLong( _value );
#	else
		return PyLong_FromLong( _value );
#	endif
	}
    //////////////////////////////////////////////////////////////////////////
	PyObject * ptr( unsigned long _value )
	{
#   if PYBIND_PYTHON_VERSION < 300
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
		return pybind::string_from_char( _value );
	}
    //////////////////////////////////////////////////////////////////////////
    PyObject * ptr( const pybind_unicode_t * _value )
    {
        return pybind::unicode_from_wchar( _value );
    }
	//////////////////////////////////////////////////////////////////////////
	PyObject * ptr( PyObject * _value )
	{
		return _value;
	}
}