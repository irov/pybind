#	include "pybind/system.hpp"

#	include "pybind/config.hpp"

namespace pybind
{
	void initialize()
	{
		Py_Initialize();
	}

	void finalize()
	{
		Py_Finalize();
	}

	PyObject * call( PyObject * _obj, const char * _format, ... )
	{
		va_list valist;
		va_start(valist, _format);

		PyObject * result = PyObject_CallObject( _obj, Py_VaBuildValue( _format, valist ) );

		va_end( valist ); 

		return result;
	}
}