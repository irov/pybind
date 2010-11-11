#	include "pybind/system.hpp"
#	include "pybind/class_core.hpp"
#	include "pybind/method_type.hpp"
#	include "pybind/member_type.hpp"
#	include "pybind/def.hpp"
#	include "pybind/functor.hpp"

#	include "pybind/class_type.hpp"

#	include "config/python.hpp"

#	include <stdexcept>

namespace pybind
{
	void throw_exception()
	{
		throw pybind_exception();
	}

	void initialize()
	{
		Py_Initialize();


		initialize_methods();
		initialize_members();
		initialize_classes();
		initialize_def();
		initialize_functor();
	}

	void initialize_ts()
	{
		initialize();	

		PyEval_InitThreads();
		PyEval_SaveThread();
	}

	void finalize()
	{
		Py_Finalize();

		class_core::finalize();		
		
		finalize_def();
		finalize_functor();
	}

	bool is_initialized()
	{
		return (Py_IsInitialized() != 0);
	}

	void check_error()
	{
		if (PyErr_Occurred())
		{
			PyErr_Print();
		}
	}

	void setStdErrorHandle( PyObject * _handle )
	{
		PyObject * sysModule = PyImport_AddModule( "sys" );

		PyObject_SetAttrString( sysModule,
			"stderr", _handle );
	}

	void setStdOutHandle( PyObject * _handle )
	{
		PyObject * sysModule = PyImport_AddModule( "sys" );

		PyObject_SetAttrString( sysModule,
			"stdout", _handle );
	}

	PyObject * module_import( const char * _name )
	{
		PyObject * module = PyImport_ImportModule( _name );

		if( module == 0 )
		{
			check_error();
		}

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

	PyObject * ask( PyObject * _obj, const char * _format, ... )
	{
		va_list valist;
		va_start(valist, _format);

		PyObject * result = ask_va( _obj, _format, valist );

		check_error();

		va_end( valist ); 

		return result;
	}

	PyObject * ask_method( PyObject * _obj, const char * _method, const char * _format, ... )
	{

		va_list valist;
		va_start(valist, _format);

		PyObject * result = ask_method_va( _obj, _method, _format, valist );

		check_error();

		va_end( valist ); 

		return result;
	}

	PyObject * ask_ne( PyObject * _obj, const char * _format, ... )
	{
		PyErr_Clear();
		va_list valist;
		va_start(valist, _format);

		PyObject * result = ask_va( _obj, _format, valist );

		check_error();

		va_end( valist ); 

		return result;
	}

	PyObject * ask_ts( PyObject * _obj, const char * _format, ... )
	{
		va_list valist;
		va_start(valist, _format);

		PyGILState_STATE _state = PyGILState_Ensure();
		PyObject * result = ask_va( _obj, _format, valist );

		check_error();

		PyGILState_Release(_state);

		va_end( valist ); 

		return result;
	}

	PyObject * ask_method_ts( PyObject * _obj, const char * _method, const char * _format, ... )
	{
		va_list valist;
		va_start(valist, _format);

		PyGILState_STATE _state = PyGILState_Ensure();
		PyObject * result = ask_method_va( _obj, _method, _format, valist );
		PyGILState_Release(_state);

		va_end( valist );
		return result;
	}

	PyObject * ask_va( PyObject * _obj, const char * _format, va_list _va )
	{		
		PyObject * value = Py_VaBuildValue( _format, _va );
		PyObject * result = PyObject_CallObject( _obj, value );

		check_error();

		Py_DECREF( value );
		return result;
	}

	PyObject * ask_method_va( PyObject * _obj, const char * _method, const char * _format, va_list _va )
	{
		PyObject * method = PyObject_GetAttrString( _obj, _method );

		if( method == 0 )
		{
			Py_RETURN_NONE;
		}

		PyObject * result = ask_va( method, _format, _va );

		check_error();

		Py_DECREF( method );

		return result;		
	}

	void call( PyObject * _obj, const char * _format, ... )
	{
		va_list valist;
		va_start(valist, _format);

		call_va( _obj, _format, valist );

		check_error();

		va_end( valist ); 
	}

	void call_method( PyObject * _obj, const char * _method, const char * _format, ... )
	{

		va_list valist;
		va_start(valist, _format);

		call_method_va( _obj, _method, _format, valist );

		check_error();

		va_end( valist ); 
	}

	void call_ne( PyObject * _obj, const char * _format, ... )
	{
		PyErr_Clear();
		va_list valist;
		va_start(valist, _format);

		call_va( _obj, _format, valist );

		check_error();

		va_end( valist ); 
	}

	void call_ts( PyObject * _obj, const char * _format, ... )
	{
		va_list valist;
		va_start(valist, _format);

		PyGILState_STATE _state = PyGILState_Ensure();
		call_va( _obj, _format, valist );
		check_error();
		PyGILState_Release(_state);

		va_end( valist ); 
	}

	void call_method_ts( PyObject * _obj, const char * _method, const char * _format, ... )
	{
		va_list valist;
		va_start(valist, _format);

		PyGILState_STATE _state = PyGILState_Ensure();
		call_method_va( _obj, _method, _format, valist );
		PyGILState_Release(_state);

		va_end( valist );
	}

	void call_va( PyObject * _obj, const char * _format, va_list _va )
	{
		PyObject * res = ask_va( _obj, _format, _va );
		Py_XDECREF( res );
	}

	void call_method_va( PyObject * _obj, const char * _method, const char * _format, va_list _va )
	{
		PyObject * res = ask_method_va( _obj, _method, _format, _va );
		Py_XDECREF( res );
	}

	PyObject * exec( const char * _code, PyObject * _global, PyObject * _local )
	{
		PyObject* result = PyRun_String( _code, Py_file_input, _global, _local);

		check_error();

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

		check_error();

		return result;
	}

	PyObject * compile_string( const char * _string, const char * _file )
	{
		PyObject* result = Py_CompileString( _string, _file, Py_file_input );

		check_error();

		return result;
	}

	const char * get_syspath()
	{
		return Py_GetPath();
	}

	void set_syspath( const char * _path )
	{
		PySys_SetPath( const_cast< char * >( _path ) );

		check_error();
	}

	void incref( PyObject * _obj )
	{
		Py_XINCREF( _obj );
	}

	void decref( PyObject * _obj )
	{
		Py_XDECREF( _obj );
	}
	
	size_t refcount( PyObject * _obj )
	{
		return _obj->ob_refcnt;
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

	bool set_attr( PyObject * _obj, const char * _attr, PyObject * _value )
	{
		int res = PyObject_SetAttrString( _obj, _attr, _value );
		
		if( res == -1 )
		{
			check_error();
		}

		Py_XDECREF( _value );

		return res != -1;
	}

	bool check_type( PyObject * _obj )
	{
		return PyType_Check( _obj ) == 1;
	}

	PyObject * list_new( std::size_t _size )
	{
		return PyList_New( _size );
	}

	bool list_check( PyObject * _obj )
	{
		return PyList_Check( _obj ) == 1;
	}

	std::size_t list_size( PyObject * _obj )
	{
		return (std::size_t) PyList_Size( _obj );
	}

	PyObject * list_getitem( PyObject * _obj, std::size_t _it )
	{
		return PyList_GetItem( _obj, _it );
	}

	bool dict_check( PyObject * _obj )
	{
		return PyDict_Check( _obj ) == 1;
	}

	int list_appenditem( PyObject * _obj, PyObject * _item )
	{
		return PyList_Append( _obj, _item );
	}

	bool dict_set( PyObject * _dict, const char * _name, PyObject * _value )
	{
		return PyDict_SetItemString( _dict, _name, _value ) == 0;
	}

	bool dict_next( PyObject * _dict, std::size_t *_pos, PyObject ** _key, PyObject ** _value )
	{
		Py_ssize_t ps = (Py_ssize_t)(*_pos);
		int res = PyDict_Next( _dict, &ps, _key, _value );
		*_pos = (std::size_t)ps;
		return res == 1;
	}

	bool tuple_check( PyObject * _tuple )
	{
		return PyTuple_Check( _tuple ) == 1;
	}

	size_t tuple_size( PyObject * _tuple )
	{
		return PyTuple_Size( _tuple );
	}

	PyObject * tuple_getitem( PyObject * _tuple, std::size_t _it )
	{
		return PyTuple_GetItem( _tuple, _it );
	}

	const char * object_to_string( PyObject * _obj )
	{
		PyObject * repr = PyObject_Repr( _obj );
		
		if( repr == 0 )
		{
			return 0;
		}

		return PyString_AS_STRING( repr );
	}

	static void traceback_error( const char * _msg )
	{
		PyErr_SetString( PyExc_RuntimeError, _msg );
		PyObject *error = PyErr_Occurred();
		if( error )
		{
			PyErr_Print();

			PyObject *ptype, *pvalue, *ptraceback;
			PyErr_Fetch(&ptype, &pvalue, &ptraceback);

			PyObject * sysModule = PyImport_AddModule( "sys" );
			PyObject * handle = PyObject_GetAttrString( sysModule, "stderr" );

			PyTraceBack_Print( ptraceback, handle );			
		}
	}

	void error_message( const char * _message, ... )
	{
		va_list valist;
		va_start(valist, _message);
		char buffer[1024];
		vsprintf( buffer, _message, valist );

		traceback_error( buffer );

		va_end( valist ); 
	}

	bool tuple_setitem( PyObject * _tuple, std::size_t _it, PyObject * _value )
	{
		return PyTuple_SetItem( _tuple, _it, _value ) == 0;
	}

	bool tuple_resize( PyObject ** _ptuple, std::size_t _it )
	{
		return _PyTuple_Resize( _ptuple, _it ) == 0;
	}

	PyObject * tuple_new( std::size_t _it )
	{
		return PyTuple_New(_it);
	}

	PyObject * build_value( const char * _format, ... )
	{
		va_list valist;
		va_start(valist, _format);
		PyObject * value = Py_VaBuildValue( _format, valist );
		return value;
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
	//////////////////////////////////////////////////////////////////////////
	void unwrap( PyObject * _value )
	{
		detail::unwrap( _value );
	}
	//////////////////////////////////////////////////////////////////////////
	namespace convert
	{
		//////////////////////////////////////////////////////////////////////////
		bool is_none( PyObject * _none )
		{
			return _none == Py_None;
		}
		//////////////////////////////////////////////////////////////////////////
		bool is_bool( PyObject * _bool )
		{
			return PyBool_Check( _bool ) == 1;
		}
		//////////////////////////////////////////////////////////////////////////
		bool to_bool( PyObject * _bool )
		{
			if( PyBool_Check( _bool ) )
			{
				return _bool == Py_True;
			}

			return false;
		}
		//////////////////////////////////////////////////////////////////////////
		bool is_string( PyObject * _string )
		{
			if( PyString_Check( _string ) == 1 )
			{
				return true;
			}

			return false;
		}
		//////////////////////////////////////////////////////////////////////////
		const char * to_string( PyObject * _string )
		{
			return PyString_AsString( _string );
		}
		//////////////////////////////////////////////////////////////////////////
		// const wchar_t * to_unicode( PyObject * _unicode )
		// {
		// 	return PyUnicode_AsUnicode( _unicode );
		// }
	}
}

