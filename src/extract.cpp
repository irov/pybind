#	include "pybind/extract.hpp"

#	include "pybind/system.hpp"
#	include "pybind/bindable.hpp"

#	include "config/python.hpp"

namespace pybind
{
	//////////////////////////////////////////////////////////////////////////
	bool extract_value( PyObject * _obj, bool & _value )
	{
		if( _obj == nullptr )
		{
			return false;
		}

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
			_value = (PyLong_AsLong( _obj ) != 0);				
		}
		else if( PyFloat_Check( _obj ) )
		{
			_value = (PyFloat_AsDouble( _obj ) != 0.0);
		}
		else
		{
			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool extract_value( PyObject * _obj, int8_t & _value )
	{
		if( _obj == nullptr )
		{
			return false;
		}

#   if PYBIND_PYTHON_VERSION < 300
		if( PyInt_Check( _obj ) )
		{
			_value = (int8_t)PyInt_AsLong( _obj );
		} else 
#	endif

			if( PyLong_Check( _obj ) )
			{
				_value = (int8_t)PyLong_AsLong( _obj );
			}
			else if( PyFloat_Check( _obj ) )
			{
				_value = (int8_t)PyFloat_AsDouble( _obj );				
			}
			else
			{
				return false;
			}

			return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool extract_value( PyObject * _obj, int16_t & _value )
	{
		if( _obj == nullptr )
		{
			return false;
		}

#   if PYBIND_PYTHON_VERSION < 300
		if( PyInt_Check( _obj ) )
		{
			_value = (int16_t)PyInt_AsLong( _obj );
		} else 
#	endif

			if( PyLong_Check( _obj ) )
			{
				_value = (int16_t)PyLong_AsLong( _obj );
			}
			else if( PyFloat_Check( _obj ) )
			{
				_value = (int16_t)PyFloat_AsDouble( _obj );				
			}
			else
			{
				return false;
			}

			return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool extract_value( PyObject * _obj, int32_t & _value )
	{
		if( _obj == nullptr )
		{
			return false;
		}

#   if PYBIND_PYTHON_VERSION < 300
		if( PyInt_Check( _obj ) )
		{
			_value = (int32_t)PyInt_AsLong( _obj );
		} else
#	endif

		if( PyLong_Check( _obj ) )
		{
			_value = (int32_t)PyLong_AsLong( _obj );
		}
		else if( PyFloat_Check( _obj ) )
		{
			_value = (int32_t)PyFloat_AsDouble( _obj );				
		}
		else
		{
			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool extract_value( PyObject * _obj, int64_t & _value )
	{
		if( _obj == nullptr )
		{
			return false;
		}

#   if PYBIND_PYTHON_VERSION < 300
		if( PyInt_Check( _obj ) )
		{
			_value = (int64_t)PyInt_AsLong( _obj );
		} else
#	endif

		if( PyLong_Check( _obj ) )
		{				
			_value = (int64_t)PyLong_AsLong( _obj );
		}
		else if( PyFloat_Check( _obj ) )
		{				
			_value = (int64_t)PyFloat_AsDouble( _obj );
		}
		else
		{
			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool extract_value( PyObject * _obj, uint8_t & _value )
	{
		if( _obj == nullptr )
		{
			return false;
		}

#   if PYBIND_PYTHON_VERSION < 300
		if( PyInt_Check( _obj ) )
		{
			_value = (uint8_t)PyInt_AsUnsignedLongMask( _obj );
		} else
#	endif

			if( PyLong_Check( _obj ) )
			{				
				_value = (uint8_t)PyLong_AsUnsignedLong( _obj );
			}
			else if( PyFloat_Check( _obj ) )
			{				
				_value = (uint8_t)PyFloat_AsDouble( _obj );
			}
			else
			{
				return false;
			}

			return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool extract_value( PyObject * _obj, uint16_t & _value )
	{
		if( _obj == nullptr )
		{
			return false;
		}

#   if PYBIND_PYTHON_VERSION < 300
		if( PyInt_Check( _obj ) )
		{
			_value = (uint16_t)PyInt_AsUnsignedLongMask( _obj );
		} else
#	endif

			if( PyLong_Check( _obj ) )
			{				
				_value = (uint16_t)PyLong_AsUnsignedLong( _obj );
			}
			else if( PyFloat_Check( _obj ) )
			{				
				_value = (uint16_t)PyFloat_AsDouble( _obj );
			}
			else
			{
				return false;
			}

			return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool extract_value( PyObject * _obj, uint32_t & _value )
	{
		if( _obj == nullptr )
		{
			return false;
		}

#   if PYBIND_PYTHON_VERSION < 300
		if( PyInt_Check( _obj ) )
		{
			_value = (uint32_t)PyInt_AsUnsignedLongMask( _obj );
		} else
#	endif

		if( PyLong_Check( _obj ) )
		{				
			_value = (uint32_t)PyLong_AsUnsignedLong( _obj );
		}
		else if( PyFloat_Check( _obj ) )
		{				
			_value = (uint32_t)PyFloat_AsDouble( _obj );
		}
		else
		{
			return false;
		}

		return true;
	}
    //////////////////////////////////////////////////////////////////////////
	bool extract_value( PyObject * _obj, uint64_t & _value )
	{
		if( _obj == nullptr )
		{
			return false;
		}

#   if PYBIND_PYTHON_VERSION < 300
		if( PyInt_Check( _obj ) )
		{
			_value = (uint64_t)PyInt_AsUnsignedLongMask( _obj );
		} else
#	endif

		if( PyLong_Check( _obj ) )
		{				
			_value = (uint64_t)PyLong_AsUnsignedLong( _obj );
		}
		else if( PyFloat_Check( _obj ) )
		{				
			_value = (uint64_t)PyFloat_AsDouble( _obj );
		}
		else
		{
			return false;
		}
        
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool extract_value( PyObject * _obj, wchar_t & _value )
	{
		if( _obj == nullptr )
		{
			return false;
		}

		if( pybind::unicode_check( _obj ) == true )
		{
			size_t sz;
			const wchar_t * ch_buff = pybind::unicode_to_wchar_and_size( _obj, sz );

			if( ch_buff == nullptr )
			{
				return false;
			}

			if( sz != 1 )
			{
				return false;
			}

			_value = ch_buff[0];
		}
		else
		{
			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool extract_value( PyObject * _obj, long & _value )
	{
		if( _obj == nullptr )
		{
			return false;
		}

#   if PYBIND_PYTHON_VERSION < 300
		if( PyInt_Check( _obj ) )
		{
			_value = (long)PyInt_AsLong( _obj );
		} else
#	endif

			if( PyLong_Check( _obj ) )
			{				
				_value = (long)PyLong_AsLong( _obj );
			}
			else if( PyFloat_Check( _obj ) )
			{				
				_value = (long)PyFloat_AsDouble( _obj );
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
		if( _obj == nullptr )
		{
			return false;
		}

#   if PYBIND_PYTHON_VERSION < 300
		if( PyInt_Check( _obj ) )
		{
			_value = (unsigned long)PyInt_AsUnsignedLongMask( _obj );
		} else
#	endif

			if( PyLong_Check( _obj ) )
			{				
				_value = (unsigned long)PyLong_AsUnsignedLong( _obj );
			}
			else if( PyFloat_Check( _obj ) )
			{				
				_value = (unsigned long)PyFloat_AsDouble( _obj );
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
		if( _obj == nullptr )
		{
			return false;
		}

		if( PyFloat_Check( _obj ) )
		{
			_value = (float)PyFloat_AsDouble( _obj );
		}
#   if PYBIND_PYTHON_VERSION < 300
		else if( PyInt_Check( _obj ) )
		{
			_value = (float)PyInt_AsLong( _obj );
		}
#	endif
		else if( PyLong_Check( _obj ) )
		{
			_value = (float)PyLong_AsLong( _obj );
		}
		else
		{
			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool extract_value( PyObject * _obj, double & _value )
	{
		if( _obj == nullptr )
		{
			return false;
		}

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
		if( _obj == nullptr )
		{
			return false;
		}

		if( pybind::string_check( _obj ) == true )
		{
            const char * ch_buff = pybind::string_to_char( _obj );

            if( ch_buff == nullptr )
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
	bool extract_value( PyObject * _obj, const wchar_t *& _value )
	{
		if( _obj == nullptr )
		{
			return false;
		}

		if( pybind::unicode_check( _obj ) == true )
		{
			const wchar_t * ch_buff = pybind::unicode_to_wchar( _obj );

			if( ch_buff == nullptr )
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
		if( _obj == nullptr )
		{
			return false;
		}

		_value = _obj;

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	PyObject * ptr_throw( bool _value )
	{
		if( _value == true )
		{
			Py_RETURN_TRUE;
		}
		
		Py_RETURN_FALSE;
	}
	//////////////////////////////////////////////////////////////////////////
	PyObject * ptr_throw( int8_t _value )
	{
#   if PYBIND_PYTHON_VERSION < 300
		return PyInt_FromLong( _value );
#	else
		return PyLong_FromLong( _value );
#	endif
	}
	//////////////////////////////////////////////////////////////////////////
	PyObject * ptr_throw( int16_t _value )
	{
#   if PYBIND_PYTHON_VERSION < 300
		return PyInt_FromLong( _value );
#	else
		return PyLong_FromLong( _value );
#	endif
	}
	//////////////////////////////////////////////////////////////////////////
	PyObject * ptr_throw( int32_t _value )
	{
#   if PYBIND_PYTHON_VERSION < 300
		return PyInt_FromLong( _value );
#	else
		return PyLong_FromLong( _value );
#	endif
	}
	//////////////////////////////////////////////////////////////////////////
	PyObject * ptr_throw( int64_t _value )
	{
		return PyLong_FromLong( (long)_value );
	}
	//////////////////////////////////////////////////////////////////////////
	PyObject * ptr_throw( uint8_t _value )
	{
#   if PYBIND_PYTHON_VERSION < 300
		return PyInt_FromLong( _value );
#	else
		return PyLong_FromLong( _value );
#	endif
	}
	//////////////////////////////////////////////////////////////////////////
	PyObject * ptr_throw( uint16_t _value )
	{
#   if PYBIND_PYTHON_VERSION < 300
		return PyInt_FromLong( _value );
#	else
		return PyLong_FromLong( _value );
#	endif
	}
	//////////////////////////////////////////////////////////////////////////
	PyObject * ptr_throw( uint32_t _value )
	{
#   if PYBIND_PYTHON_VERSION < 300
		return PyInt_FromLong( _value );
#	else
		return PyLong_FromLong( _value );
#	endif
	}
    //////////////////////////////////////////////////////////////////////////
	PyObject * ptr_throw( uint64_t _value )
	{
		return PyLong_FromUnsignedLong( (unsigned long)_value );
	}
	//////////////////////////////////////////////////////////////////////////
	PyObject * ptr_throw( wchar_t _value )
	{
		wchar_t tmp[2];
		tmp[0] = _value;
		tmp[1] = 0;

		return pybind::unicode_from_wchar( tmp );
	}
	//////////////////////////////////////////////////////////////////////////
	PyObject * ptr_throw( long _value )
	{
		return PyLong_FromLong( _value );
	}
	//////////////////////////////////////////////////////////////////////////
	PyObject * ptr_throw( unsigned long _value )
	{
		return PyLong_FromUnsignedLong( _value );
	}
	//////////////////////////////////////////////////////////////////////////
	PyObject * ptr_throw( float _value )
	{
		return PyFloat_FromDouble( _value );
	}
	//////////////////////////////////////////////////////////////////////////
	PyObject * ptr_throw( double _value )
	{
		return PyFloat_FromDouble( _value );
	}
	//////////////////////////////////////////////////////////////////////////
	PyObject * ptr_throw( char * _value )
	{
		return pybind::string_from_char( _value );
	}
	//////////////////////////////////////////////////////////////////////////
	PyObject * ptr_throw( wchar_t * _value )
	{
		return pybind::unicode_from_wchar( _value );
	}
	//////////////////////////////////////////////////////////////////////////
	PyObject * ptr_throw( const char * _value )
	{
		return pybind::string_from_char( _value );
	}
	//////////////////////////////////////////////////////////////////////////
	PyObject * ptr_throw( const wchar_t * _value )
	{
		return pybind::unicode_from_wchar( _value );
	}
	//////////////////////////////////////////////////////////////////////////
	PyObject * ptr_throw( PyObject * _value )
	{		
		return _value;
	}
	//////////////////////////////////////////////////////////////////////////
	PyObject * ptr_throw( pybind::bindable * _value )
	{
		pybind::object py_obj = _value->getEmbed();

		PyObject * py_obj_ptr = py_obj.ptr();
		
		pybind::incref( py_obj_ptr );

		return py_obj_ptr;
	}
}