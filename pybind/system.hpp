#	pragma once

extern "C" 
{ 
	struct _object;
	typedef _object PyObject;
}

namespace pybind
{
	void initialize();
	void finalize();

	PyObject * call( PyObject * _obj, const char * _format, ... );	
}