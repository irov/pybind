#	pragma once

#	include "class_core.hpp"

extern "C" 
{ 
	struct _object;
	typedef _object PyObject;
}

namespace pybind
{
	template<class T>
	T extract( PyObject * _object )
	{
		return from_py_to_cpp_parser<T>::value( _object );
	}

	template<class T>
	PyObject * ptr( T _value )
	{
		return from_py_to_cpp_parser<T>::wrapp( _value );
	}
}