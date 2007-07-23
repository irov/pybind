#	include "pybind/module.hpp"

#	include "pybind/config.hpp"

namespace pybind
{
	module_::module_( const char * _name )
	{
		static PyMethodDef module_methods[] = {
			{NULL}  /* Sentinel */
		};

		m_module = Py_InitModule4( _name, module_methods, 0, 0, PYTHON_API_VERSION );
	}

	bool module_::addObject( const char * _name, PyObject * _object )
	{
		Py_INCREF( _object );
		return PyModule_AddObject( m_module, _name, _object ) == 1; 
	}
}