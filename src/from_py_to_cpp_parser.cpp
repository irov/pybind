#	include "pybind/from_py_to_cpp_parser.hpp"

#	include "pybind/class_scope.hpp"
#	include "pybind/class_type.hpp"

#	include "pybind/config.hpp"

namespace pybind
{
	namespace detail
	{
		void * py_class_type_value( const type_info & _info, PyObject * _arg )
		{
			PyTypeObject * type = (PyTypeObject *)PyObject_Type( _arg );

			PyTypeObject * subtype = class_scope::find_sub_type( type );

			if( subtype )
			{
				py_class_type * self = (py_class_type *)(_arg);
				return self->impl;
			}

			return 0;			
		}

		bool bool_value( PyObject * _arg )
		{
			if( PyBool_Check( _arg ) )
			{
				return _arg == Py_True;
			}

			return false;
		}

		PyObject * bool_wrapp( bool _value )
		{
			if( _value )
			{
				Py_RETURN_TRUE;
			}
			else
			{
				Py_RETURN_FALSE;
			}
		}

		int int_value( PyObject * _arg )
		{
			if( PyInt_Check( _arg ) )
			{
				return PyInt_AsLong( _arg );
			}

			return 0;
		}

		PyObject * int_wrapp( int _value )
		{
			PyObject * pyValue = PyInt_FromLong( _value );
			Py_INCREF( pyValue );
			return pyValue;
		}

		float float_value( PyObject * _arg )
		{
			if( PyFloat_Check( _arg ) )
			{
				return (float)PyFloat_AsDouble( _arg );
			}

			return 0.f;
		}

		PyObject * float_wrapp( float _value )
		{
			PyObject * pyValue = PyFloat_FromDouble( (double)_value );
			Py_INCREF( pyValue );
			return pyValue;
		}

		const char * string_value( PyObject * _arg )
		{
			if( PyString_Check( _arg ) )
			{
				return const_cast<const char *>(PyString_AsString( _arg ));
			}

			return 0;
		}

		PyObject * string_wrapp( const char * _value )
		{
			PyObject * pyValue = PyString_FromString( _value );
			Py_INCREF( pyValue );
			return pyValue;
		}

		std::string stl_string_value( PyObject * _arg )
		{
			if( PyString_Check( _arg ) )
			{
				return std::string(PyString_AsString( _arg ));
			}

			return std::string();
		}

		PyObject * stl_string_wrapp( const std::string & _value )
		{
			PyObject * pyValue = PyString_FromString( _value.c_str() );
			Py_INCREF( pyValue );
			return pyValue;
		}
	}
}