#	include "pybind/system.hpp"

#	include "pybind/config.hpp"

#	include <stdexcept>

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

	void check_error()
	{
		if (PyErr_Occurred())
		{
			PyErr_Print();
		}
	}

	PyObject * module_import( const char * _name )
	{
		char *n = const_cast<char *>(_name); 
		PyObject * module = PyImport_ImportModule( n );
		return module;
	}

	PyObject * module_init( const char * _name )
	{
		static PyMethodDef module_methods[] = {
			{NULL}  /* Sentinel */
		};

		return Py_InitModule4( _name, module_methods, 0, 0, PYTHON_API_VERSION );
	}

	PyObject * module_dict( PyObject * _module )
	{
		return PyModule_GetDict( _module );
	}

	void module_addobject( PyObject * _module, const char * _name, PyObject * _obj )
	{
		PyModule_AddObject( _module, _name, _obj );
	}

	static PyObject * s_current_module = 0;

	void set_currentmodule( PyObject * _module )
	{
		s_current_module = _module;
	}

	PyObject * get_currentmodule()
	{
		return s_current_module;
	}


	PyObject * call( PyObject * _obj, const char * _format, ... )
	{
		va_list valist;
		va_start(valist, _format);

		PyObject * result = call_va( _obj, _format, valist );

		va_end( valist ); 

		return result;
	}

	PyObject * call_method( PyObject * _obj, const char * _method, const char * _format, ... )
	{
		va_list valist;
		va_start(valist, _format);

		PyObject * result = call_method_va( _obj, _method, _format, valist );

		va_end( valist ); 

		return result;
	}

	PyObject * call_va( PyObject * _obj, const char * _format, va_list _va )
	{
		PyObject * result = PyObject_CallObject( _obj, Py_VaBuildValue( _format, _va ) );
		return result;
	}

	PyObject * call_method_va( PyObject * _obj, const char * _method, const char * _format, va_list _va )
	{
		if( PyObject_HasAttrString( _obj, _method ) != 1 )
		{
			return 0;
		}

		PyObject * method = PyObject_GetAttrString( _obj, _method );

		PyObject * result = call_va( method, _format, _va );

		return result;
	}

	PyObject * exec( const char * _code, PyObject * _global, PyObject * _local )
	{
		PyObject* result = PyRun_String( _code, Py_file_input, _global, _local);
		return result; 
	}

	PyObject * exec_file( const char * _filename, PyObject * _global, PyObject * _local )
	{
		PyObject * pyfile = PyFile_FromString( const_cast<char*>(_filename), "r" );
		FILE * f = PyFile_AsFile( pyfile );

		PyObject* result = PyRun_File( f,
			_filename,
			Py_file_input,
			_global, _local );
		return result;
	}

	void set_syspath( const char * _path )
	{
		PySys_SetPath( const_cast< char * >( _path ) );
	}

	void incref( PyObject * _obj )
	{
		Py_INCREF( _obj );
	}

	void decref( PyObject * _obj )
	{
		Py_DECREF( _obj );
	}

	PyObject * ret_none()
	{
		Py_RETURN_NONE;
	}

	PyObject * ret_true()
	{
		Py_RETURN_TRUE;
	}

	PyObject * ret_false()
	{
		Py_RETURN_FALSE;
	}

	bool has_attr( PyObject * _obj, const char * _attr )
	{
		return PyObject_HasAttrString( _obj, _attr ) == 1;
	}

	PyObject * get_attr( PyObject * _obj, const char * _attr )
	{
		return PyObject_GetAttrString( _obj, _attr );
	}

	bool check_type( PyObject * _obj )
	{
		return PyType_Check( _obj ) == 1;
	}

	bool list_check( PyObject * _obj )
	{
		return PyList_Check( _obj ) == 1;
	}

	size_t list_size( PyObject * _obj )
	{
		return (size_t) PyList_Size( _obj );
	}

	PyObject * list_getitem( PyObject * _obj, size_t _it )
	{
		return PyList_GetItem( _obj, _it );
	}

	bool dict_check( PyObject * _obj )
	{
		return PyDict_Check( _obj ) == 1;
	}

	bool dict_next( PyObject * _dict, size_t *_pos, PyObject ** _key, PyObject ** _value )
	{
		Py_ssize_t ps;
		int res = PyDict_Next( _dict, &ps, _key, _value );
		*_pos = (size_t)ps;
		return res == 1;
	}

	void exception_filter()
	{
		try
		{
			throw;
		}
		catch(const std::bad_alloc&)
		{
			PyErr_NoMemory();
		}
		catch(const std::out_of_range& x)
		{
			PyErr_SetString(PyExc_IndexError, x.what());
		}
		catch(const std::exception& x)
		{
			PyErr_SetString(PyExc_RuntimeError, x.what());
		}
		catch(...)
		{
			PyErr_SetString(PyExc_RuntimeError, "unidentifiable C++ exception");
		}

		if (PyErr_Occurred())
		{
			PyErr_Print();
		}
	}
}