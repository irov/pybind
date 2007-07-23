#	pragma once

#	include "pybind/exports.hpp"

#	include <string>


extern "C" 
{ 
	struct _object;
	typedef _object PyObject;
}

namespace pybind
{
	class class_type_scope;

	namespace detail
	{
		PYBIND_API void * py_class_type_value( PyObject * _arg );
		
		PYBIND_API bool bool_value( PyObject * _arg );
		PYBIND_API int int_value( PyObject * _arg );
		PYBIND_API float float_value( PyObject * _arg );
		PYBIND_API const char * string_value( PyObject * _arg );
		PYBIND_API std::string stl_string_value( PyObject * _arg );

		PYBIND_API PyObject * bool_wrapp( bool _value );
		PYBIND_API PyObject * int_wrapp( int _value );
		PYBIND_API PyObject * float_wrapp( float _value );
		PYBIND_API PyObject * string_wrapp( const char * _value );
		PYBIND_API PyObject * stl_string_wrapp( const std::string & _value );
	}

	template<class T>
	struct from_py_to_cpp_parser
	{
		static T value( PyObject * _arg )
		{
			return (T)detail::py_class_type_value( _arg );

			return 0;
		}

		static PyObject * wrapp( T _value )
		{
			return 0;
		}
	};

	template<>
	struct from_py_to_cpp_parser<bool>
	{
		static bool value( PyObject * _arg )
		{
			return detail::bool_value( _arg );
		}

		static PyObject * wrapp( bool _value )
		{
			return detail::bool_wrapp( _value );
		}
	};

	template<>
	struct from_py_to_cpp_parser<int>
	{
		static int value( PyObject * _arg )
		{
			return detail::int_value( _arg );
		}

		static PyObject * wrapp( int _value )
		{
			return detail::int_wrapp( _value );
		}
	};

	template<>
	struct from_py_to_cpp_parser<float>
	{
		static float value( PyObject * _arg )
		{
			return detail::float_value( _arg );
		}

		static PyObject * wrapp( float _value )
		{
			return detail::float_wrapp( _value );
		}
	};

	template<>
	struct from_py_to_cpp_parser<const char *>
	{
		static const char * value( PyObject * _arg )
		{
			return detail::string_value( _arg );
		}

		static PyObject * wrapp( const char * _value )
		{
			return detail::string_wrapp( _value );
		}
	};

	template<>
	struct from_py_to_cpp_parser<std::string>
	{
		static std::string value( PyObject * _arg )
		{
			return detail::stl_string_value( _arg );
		}

		static PyObject * wrapp( const std::string & _value )
		{
			return detail::stl_string_wrapp( _value );
		}
	};
}