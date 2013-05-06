#	include "pybind/system.hpp"
#	include "pybind/class_core.hpp"

#	include "config/python.hpp"

#	include "method_type.hpp"
#	include "member_type.hpp"
#	include "function_type.hpp"
#	include "functor_type.hpp"

#	include "class_type.hpp"

#   include "static_var.hpp"

#	include <stdexcept>
#	include <stdio.h>

extern int Py_OptimizeFlag;
extern int Py_NoSiteFlag;

namespace pybind
{
    //////////////////////////////////////////////////////////////////////////
#	ifdef PYBIND_VISIT_OBJECTS
    //////////////////////////////////////////////////////////////////////////
	void visit_objects( pybind_visit_objects * _visitor )
	{
		detail::TVectorTypeScope types;
		detail::get_types_scope(types);

		for( detail::TVectorTypeScope::iterator
			it = types.begin(),
			it_end = types.end();
		it != it_end;
		++it )
		{
			(*it)->visit_objects(_visitor);
		}
	}
#	endif
    //////////////////////////////////////////////////////////////////////////
	bool initialize( bool _debug, bool install_sigs )
	{
		if( _debug == false )
		{
			Py_OptimizeFlag = 2;
			Py_NoSiteFlag = 1;
		}

        Py_IgnoreEnvironmentFlag = 1;

#   if PYBIND_PYTHON_VERSION > 330
		_Py_InitializeEx_Private( install_sigs?1:0, 0 );
#   else
        Py_InitializeEx( install_sigs?1:0 );
#   endif

		if( initialize_methods() == false )
        {
            return false;
        }
		
        if( initialize_members() == false )
        {
            return false;
        }
		
        if( initialize_classes() == false )
        {
            return false;
        }

		if( initialize_function() == false )
        {
            return false;
        }

		if( initialize_functor() == false )
        {
            return false;
        }

		//initialize_default_type_cast();

        return true;
	}
    //////////////////////////////////////////////////////////////////////////
	void finalize()
	{	
		class_core::finalize();
		
		finalize_methods();
		finalize_classes();
		finalize_function();
		finalize_functor();
        finialize_type_cast();

        Py_Finalize();

		//finalize_default_type_cast();
	}
    //////////////////////////////////////////////////////////////////////////
	bool is_initialized()
	{
		return (Py_IsInitialized() != 0);
	}
    //////////////////////////////////////////////////////////////////////////
	void check_error()
	{
		if (PyErr_Occurred())
		{
			PyErr_Print();
		}
	}
    //////////////////////////////////////////////////////////////////////////
    size_t get_python_version()
    {
        size_t version = PYBIND_PYTHON_VERSION;

        return version;
    }
    //////////////////////////////////////////////////////////////////////////
	void setStdErrorHandle( PyObject * _handle )
	{
		PyObject * sysModule = PyImport_AddModule( "sys" );

		PyObject_SetAttrString( sysModule, "stderr", _handle );
	}
    //////////////////////////////////////////////////////////////////////////
	void setStdOutHandle( PyObject * _handle )
	{
		PyObject * sysModule = PyImport_AddModule( "sys" );

		PyObject_SetAttrString( sysModule, "stdout", _handle );
	}
    //////////////////////////////////////////////////////////////////////////
    PyObject * get_builtins()
    {
#   if PYBIND_PYTHON_VERSION < 300
        PyObject * builtins = PyImport_ImportModuleLevel("__builtin__",
            NULL, NULL, NULL, 0);
#   endif


#   if PYBIND_PYTHON_VERSION >= 300
        PyObject * builtins = PyImport_ImportModuleLevel("builtins",
            NULL, NULL, NULL, 0);
#   endif

        return builtins;
    }
    //////////////////////////////////////////////////////////////////////////
	PyObject * module_import( const char * _name, bool & _exsist )
	{
		PyObject * module = PyImport_ImportModule( _name );

		if (PyErr_Occurred())
		{
			if( PyErr_ExceptionMatches(PyExc_ImportError) == 1 )
			{
				_exsist = false;

				PyErr_Print();
				PyErr_Clear();

				return NULL;
			
			}

			PyErr_Print();
		}

		_exsist = true;

		return module;
	}
    //////////////////////////////////////////////////////////////////////////
#   if PYBIND_PYTHON_VERSION < 300
    //////////////////////////////////////////////////////////////////////////
	PyObject * module_init( const char * _name )
	{
		//static PyMethodDef module_methods[] = {
		//	{NULL}  /* Sentinel */
		//};
		//return Py_InitModule4( _name, module_methods, 0, 0, PYTHON_API_VERSION );
        PyObject * module = PyImport_AddModule( _name );

        return module;
	}
#	else
    //   //////////////////////////////////////////////////////////////////////////
	//static PyObject* initfunc(void)
	//{
	//	static PyMethodDef module_methods[] = {
	//		{NULL}  /* Sentinel */
	//	};

	//	static struct PyModuleDef module_definition = {
	//		PyModuleDef_HEAD_INIT,
	//		"pybind_module",
	//		NULL,
	//		-1,
	//		module_methods,
	//		NULL,
	//		NULL,
	//		NULL,
	//		NULL
	//	};

	//	PyObject * obj = PyModule_Create( &module_definition );

	//	return obj;
	//}
    //////////////////////////////////////////////////////////////////////////
	PyObject * module_init( const char * _name )
	{
		//PyImport_AppendInittab( _name, &initfunc );
        PyObject * module = PyImport_AddModule( _name );

		return module;
	}
#	endif
    //////////////////////////////////////////////////////////////////////////
	PyObject * module_dict( PyObject * _module )
	{
		return PyModule_GetDict( _module );
	}
    //////////////////////////////////////////////////////////////////////////
	void module_addobject( PyObject * _module, const char * _name, PyObject * _obj )
	{
		PyModule_AddObject( _module, _name, _obj );
	}
    //////////////////////////////////////////////////////////////////////////
    PyObject * module_execcode( const char * _name, PyObject * _code )
    {
        char * unconst_name = const_cast<char *>(_name);
        PyObject * module = PyImport_ExecCodeModule( unconst_name, _code );

        return module;
    }
    //////////////////////////////////////////////////////////////////////////
    bool code_check( PyObject * _code )
    {
        return PyCode_Check( _code );
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * code_compile_file( const char * _buf, const char * _module )
    {
        PyObject * code = Py_CompileString( _buf, _module, Py_file_input );

        return code;
    }
    //////////////////////////////////////////////////////////////////////////
    STATIC_DECLARE_VALUE(PyObject *, s_current_module, 0);
    //////////////////////////////////////////////////////////////////////////
	void set_currentmodule( PyObject * _module )
	{
		STATIC_VAR(s_current_module) = _module;
	}
    //////////////////////////////////////////////////////////////////////////
	PyObject * get_currentmodule()
	{
		return STATIC_VAR(s_current_module);
	}
    //////////////////////////////////////////////////////////////////////////
	PyObject * ask_native( PyObject * _obj, PyObject * _args )
	{
		PyObject * result = PyObject_CallObject( _obj, _args );

       	check_error();

		return result;
	}
    //////////////////////////////////////////////////////////////////////////
	PyObject * ask( PyObject * _obj, const char * _format, ... )
	{
		va_list valist;
		va_start(valist, _format);

		PyObject * result = ask_va( _obj, _format, valist );

		va_end( valist ); 

		return result;
	}
    //////////////////////////////////////////////////////////////////////////
	PyObject * ask_method( PyObject * _obj, const char * _method, const char * _format, ... )
	{
		va_list valist;
		va_start(valist, _format);

		PyObject * result = ask_method_va( _obj, _method, _format, valist );

		va_end( valist ); 

		return result;
	}
    //////////////////////////////////////////////////////////////////////////
	PyObject * ask_va( PyObject * _obj, const char * _format, va_list _va )
	{		
		PyObject * value = Py_VaBuildValue( _format, _va );
		PyObject * result = ask_native( _obj, value );

		Py_DecRef( value );

		return result;
	}
    //////////////////////////////////////////////////////////////////////////
	PyObject * ask_method_va( PyObject * _obj, const char * _method, const char * _format, va_list _va )
	{
		PyObject * method = PyObject_GetAttrString( _obj, _method );

		if( method == 0 )
		{
			Py_RETURN_NONE;
		}

		PyObject * result = ask_va( method, _format, _va );

		Py_DecRef( method );

		return result;		
	}
    //////////////////////////////////////////////////////////////////////////
	void call_native( PyObject * _obj, PyObject * _args )
	{
		PyObject * res = ask_native( _obj, _args );
        
        if( res == NULL )
        {
            return;
        }

		Py_DecRef( res );
	}
    //////////////////////////////////////////////////////////////////////////
	void call( PyObject * _obj, const char * _format, ... )
	{
		va_list valist;
		va_start(valist, _format);

		call_va( _obj, _format, valist );

		va_end( valist ); 
	}
    //////////////////////////////////////////////////////////////////////////
	void call_method( PyObject * _obj, const char * _method, const char * _format, ... )
	{

		va_list valist;
		va_start(valist, _format);

		call_method_va( _obj, _method, _format, valist );

		va_end( valist ); 
	}
    //////////////////////////////////////////////////////////////////////////
	void call_va( PyObject * _obj, const char * _format, va_list _va )
	{
		PyObject * res = ask_va( _obj, _format, _va );

        if( res == NULL )
        {
            return;
        }

		Py_DecRef( res );
	}
    //////////////////////////////////////////////////////////////////////////
	void call_method_va( PyObject * _obj, const char * _method, const char * _format, va_list _va )
	{
		PyObject * res = ask_method_va( _obj, _method, _format, _va );

        if( res == NULL )
        {
            return;
        }

		Py_DecRef( res );
	}
    //////////////////////////////////////////////////////////////////////////
	PyObject * exec( const char * _code, PyObject * _global, PyObject * _local )
	{
		PyObject* result = PyRun_String( _code, Py_file_input, _global, _local);

		check_error();

		return result; 
	}
    //////////////////////////////////////////////////////////////////////////
	PyObject * compile_string( const char * _string, const char * _file )
	{
		PyObject* result = Py_CompileString( _string, _file, Py_file_input );

		check_error();

		return result;
	}
    //////////////////////////////////////////////////////////////////////////
	void set_syspath( PyObject * _value )
	{
		PySys_SetObject( "path", _value );

		check_error();
	}
    //////////////////////////////////////////////////////////////////////////
	char get_sysdelim()
	{
		return DELIM;
	}
    //////////////////////////////////////////////////////////////////////////
	void incref( PyObject * _obj )
	{
        if( _obj == NULL )
        {
            return;
        }

		Py_IncRef( _obj );
	}
    //////////////////////////////////////////////////////////////////////////
	void decref( PyObject * _obj )
	{
        if( _obj == NULL )
        {
            return;
        }

		Py_DecRef( _obj );
	}
	//////////////////////////////////////////////////////////////////////////
	size_t refcount( PyObject * _obj )
	{
		return _obj->ob_refcnt;
	}
    //////////////////////////////////////////////////////////////////////////
	PyObject * ret_none()
	{
		Py_RETURN_NONE;
	}
	//////////////////////////////////////////////////////////////////////////
	PyObject * ret_true()
	{
		Py_RETURN_TRUE;
	}
    //////////////////////////////////////////////////////////////////////////
	PyObject * ret_false()
	{
		Py_RETURN_FALSE;
	}
    //////////////////////////////////////////////////////////////////////////
	PyObject * ret_bool( bool _value )
	{
        if( _value == true )
        {
            PyObject * py_true = ret_true();

            return py_true;
        }
        
        PyObject * py_false = ret_false();

		return py_false;
	}
    //////////////////////////////////////////////////////////////////////////
	PyObject * get_none()
	{
		return Py_None;
	}
    //////////////////////////////////////////////////////////////////////////
	PyObject * get_bool( bool _value )
	{
        if( _value == true )
        {
            return Py_True;
        }

		return Py_False;
	}
    //////////////////////////////////////////////////////////////////////////
	bool has_attr( PyObject * _obj, const char * _attr )
	{
        int value = PyObject_HasAttrString( _obj, _attr );
		
        return value == 1;
	}
    //////////////////////////////////////////////////////////////////////////
	PyObject * get_attr( PyObject * _obj, const char * _attr )
	{
        PyObject * obj = PyObject_GetAttrString( _obj, _attr );

		return obj;
	}
    //////////////////////////////////////////////////////////////////////////
	bool set_attr( PyObject * _obj, const char * _attr, PyObject * _value )
	{
		int res = PyObject_SetAttrString( _obj, _attr, _value );
		
		if( res != 0 )
		{
			check_error();

            return false;
		}

		//Py_XDECREF( _value );

		return true;
	}
    //////////////////////////////////////////////////////////////////////////
	bool check_type( PyObject * _obj )
	{
        int value = PyType_Check( _obj );

		return value == 1;
	}
    //////////////////////////////////////////////////////////////////////////
	PyObject * list_new( size_t _size )
	{
        PyObject * obj = PyList_New( _size );

		return obj;
	}
    //////////////////////////////////////////////////////////////////////////
	bool list_check( PyObject * _obj )
	{
        int res = PyList_Check( _obj );

		return res == 1;
	}
    //////////////////////////////////////////////////////////////////////////
	size_t list_size( PyObject * _obj )
	{
        Py_ssize_t size = PyList_Size( _obj );

		return (size_t)size;
	}
    //////////////////////////////////////////////////////////////////////////
	PyObject * list_getitem( PyObject * _obj, size_t _it )
	{
        PyObject * obj = PyList_GetItem( _obj, _it );

		return obj;
	}
    //////////////////////////////////////////////////////////////////////////
    bool list_insert( PyObject * _obj, size_t _it, PyObject * _item )
    {
        int res = PyList_Insert( _obj, _it, _item );

        if( res != 0 )
        {
            check_error();

            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
	bool list_setitem( PyObject * _obj, size_t _it, PyObject * _item )
	{
        int res = PyList_SetItem( _obj, _it, _item );

        if( res != 0 )
        {
            check_error();

            return false;
        }

		return true;
	}
    //////////////////////////////////////////////////////////////////////////
	PyObject * dict_new()
	{
        PyObject * obj = PyDict_New();

		return obj;
	}
    //////////////////////////////////////////////////////////////////////////
	PyObject * dict_from( PyObject * _obj )
	{
		PyObject * py_dict = pybind::dict_new();

		PyObject * py_dir = PyObject_Dir( _obj );

		for( size_t 
			it = 0, 
			it_end = PyList_Size( py_dir );
		it != it_end;
		++it )
		{
			PyObject * py_key = PyList_GetItem( py_dir, it );
			PyObject * py_value = PyObject_GetAttr( _obj, py_key );

			if( PyDict_SetItem( py_dict, py_key, py_value ) < 0 )
			{
				continue;
			}

			Py_DecRef( py_value );
		}

		Py_DecRef( py_dir );

		return py_dict;
	}
    //////////////////////////////////////////////////////////////////////////
	bool dict_check( PyObject * _obj )
	{
        int value = PyDict_Check( _obj );

		return value == 1;
	}
    //////////////////////////////////////////////////////////////////////////
	bool list_appenditem( PyObject * _obj, PyObject * _item )
	{
        int res = PyList_Append( _obj, _item );

        if( res == -1 )
        {
            check_error();

            return false;
        }

		return true;
	}
    //////////////////////////////////////////////////////////////////////////
	bool dict_set( PyObject * _dict, const char * _name, PyObject * _value )
	{
        int res = PyDict_SetItemString( _dict, _name, _value );

        if( res == -1 )
        {
            check_error();

            return false;
        }

		return true;
	}
    //////////////////////////////////////////////////////////////////////////
	PyObject * dict_get( PyObject * _dict, const char * _name )
	{
        PyObject * obj = PyDict_GetItemString( _dict, _name );

		return obj;
	}
    //////////////////////////////////////////////////////////////////////////
	bool dict_contains( PyObject * _dict, const char * _name )
	{
#   if PYBIND_PYTHON_VERSION < 300
		PyObject * kv = PyString_FromString( _name );
#	else
		PyObject * kv = PyUnicode_FromString( _name );
#	endif

		int contains = PyDict_Contains( _dict, kv );
		Py_DecRef( kv );

		return contains == 1;
	}
    //////////////////////////////////////////////////////////////////////////
	bool dict_next( PyObject * _dict, size_t *_pos, PyObject ** _key, PyObject ** _value )
	{
		Py_ssize_t ps = (Py_ssize_t)(*_pos);
		
        int res = PyDict_Next( _dict, &ps, _key, _value );

		*_pos = (size_t)ps;

		return res == 1;
	}
    //////////////////////////////////////////////////////////////////////////
	PyObject * dict_items( PyObject * _dict )
	{
        PyObject * obj = PyDict_Items( _dict );

		return obj;
	}
    //////////////////////////////////////////////////////////////////////////
	bool tuple_check( PyObject * _tuple )
	{
        int value = PyTuple_Check( _tuple );

		return value == 1;
	}
    //////////////////////////////////////////////////////////////////////////
	size_t tuple_size( PyObject * _tuple )
	{
        Py_ssize_t size = PyTuple_Size( _tuple );

		return (size_t)size;
	}
    //////////////////////////////////////////////////////////////////////////
	PyObject * tuple_getitem( PyObject * _tuple, size_t _it )
	{
        PyObject * obj = PyTuple_GetItem( _tuple, _it );

		return obj;
	}
    //////////////////////////////////////////////////////////////////////////
	PyObject * object_dir( PyObject * _obj )
	{
		PyObject * py_dir = PyObject_Dir( _obj );

		return py_dir;
	}
    //////////////////////////////////////////////////////////////////////////
	const char * object_repr( PyObject * _obj )
	{
		PyObject * repr = PyObject_Repr( _obj );
		
		if( repr == 0 )
		{
			return 0;
		}

        const char * str = pybind::string_to_char( repr );

		return str;
	}
    //////////////////////////////////////////////////////////////////////////
	static void traceback_error( const char * _msg )
	{
		PyErr_SetString( PyExc_RuntimeError, _msg );
		//PyObject *error = PyErr_Occurred();
		//if( error )
		//{
		//	PyErr_Print();

		//	PyObject *ptype, *pvalue, *ptraceback;
		//	PyErr_Fetch(&ptype, &pvalue, &ptraceback);

		//	PyObject * sysModule = PyImport_AddModule( "sys" );
		//	PyObject * handle = PyObject_GetAttrString( sysModule, "stderr" );

		//	PyTraceBack_Print( ptraceback, handle );			
		//}
	}
    //////////////////////////////////////////////////////////////////////////
	void error_message( const char * _message, ... )
	{
		va_list valist;
		va_start(valist, _message);
		char buffer[1024];
		vsprintf( buffer, _message, valist );

		traceback_error( buffer );

		va_end( valist ); 
	}
    //////////////////////////////////////////////////////////////////////////
	bool tuple_setitem( PyObject * _tuple, size_t _it, PyObject * _value )
	{
        int res = PyTuple_SetItem( _tuple, _it, _value );

        if( res != 0 )
        {
            check_error();

            return false;
        }

		return true;
	}
    //////////////////////////////////////////////////////////////////////////
	PyObject * tuple_new( size_t _it )
	{
        PyObject * obj = PyTuple_New( _it );

		return obj;
	}
    //////////////////////////////////////////////////////////////////////////
	PyObject * build_value( const char * _format, ... )
	{
		va_list valist;
		va_start(valist, _format);
		
        PyObject * value = build_value_va( _format, valist );

        va_end(valist);

		return value;
	}
    //////////////////////////////////////////////////////////////////////////
	PyObject * build_value_va( const char * _format, va_list _va )
	{
		PyObject * value = Py_VaBuildValue( _format, _va );

		return value;
	}
    //////////////////////////////////////////////////////////////////////////
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
	bool is_wrap( PyObject * _value )
	{
		return detail::is_wrap( _value );
	}
	//////////////////////////////////////////////////////////////////////////
	bool type_initialize( PyObject * _value )
	{
		PyTypeObject * type = (PyTypeObject *)_value;
		class_type_scope * scope = detail::get_class_scope( type );

		if( scope == 0 )
		{
			return false;
		}

		scope->type_initialize( type );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool is_none( PyObject * _none )
	{
		return _none == Py_None;
	}
	//////////////////////////////////////////////////////////////////////////
	bool bool_check( PyObject * _bool )
	{
        int value = PyBool_Check( _bool );

		return value == 1;
	}
	//////////////////////////////////////////////////////////////////////////
	bool is_true( PyObject * _value )
	{
		return _value == Py_True;
	}

	//////////////////////////////////////////////////////////////////////////
	bool string_check( PyObject * _string )
	{
#   if PYBIND_PYTHON_VERSION < 300
		if( PyString_Check( _string ) == 1 )
		{
			return true;
		}
#   else
        if( PyBytes_Check( _string ) == 1 )
        {
            return true;
        }
#   endif
                
		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	size_t string_size( PyObject * _string )
	{
#   if PYBIND_PYTHON_VERSION < 300
		Py_ssize_t size = PyString_Size( _string );
#   else
        Py_ssize_t size = PyBytes_Size( _string );
#   endif

		return (size_t)size;
	}
	//////////////////////////////////////////////////////////////////////////
	const char * string_to_char( PyObject * _string )
	{
#   if PYBIND_PYTHON_VERSION < 300
        char * ch_buff_unconst = PyString_AsString( _string );
#   else
		char * ch_buff_unconst = PyBytes_AsString( _string );        
#   endif

        const char * ch_buff = const_cast<const char *>(ch_buff_unconst);

		return ch_buff;
	}
    //////////////////////////////////////////////////////////////////////////
    const char * string_to_char_and_size( PyObject * _string, size_t & _size )
    {
#   if PYBIND_PYTHON_VERSION < 300
        char * ch_buff_unconst;
        Py_ssize_t len;
        PyString_AsStringAndSize( _string, &ch_buff_unconst, &len );
#   else
        char * ch_buff_unconst = PyBytes_AsString( _string );
        Py_ssize_t len = PyBytes_Size( _string );
#   endif

        _size = (size_t)len;

        const char * ch_buff = const_cast<const char *>(ch_buff_unconst);

        return ch_buff;
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * string_from_char( const char * _str )
    {
#   if PYBIND_PYTHON_VERSION < 300
        PyObject * py_str = PyString_FromString( _str );
#   else
        PyObject * py_str = PyBytes_FromString( _str );
#   endif

        return py_str;
    }
	//////////////////////////////////////////////////////////////////////////
	PyObject * string_from_char_size( const char * _str, size_t _size )
	{
#   if PYBIND_PYTHON_VERSION < 300
		PyObject * py_str = PyString_FromStringAndSize( _str, _size );
#   else
        PyObject * py_str = PyBytes_FromStringAndSize( _str, _size );
#   endif

		return py_str;
	}
	//////////////////////////////////////////////////////////////////////////
	bool unicode_check( PyObject * _unicode )
	{
        bool value = PyUnicode_CheckExact( _unicode );

		return value;
	}
	//////////////////////////////////////////////////////////////////////////
	pybind_unicode_t * unicode_to_wchar( PyObject * _unicode )
	{
        pybind_unicode_t * unicode = PyUnicode_AsUnicode( _unicode );

		return unicode;
	}
    //////////////////////////////////////////////////////////////////////////
    PyObject * unicode_from_wchar( const pybind_unicode_t * _value )
    {
        size_t size = wcslen( _value );
        PyObject * py_unicode = PyUnicode_FromUnicode( _value, size );

        return py_unicode;
    }
	//////////////////////////////////////////////////////////////////////////
	PyObject * unicode_from_wchar_size( const pybind_unicode_t * _value, size_t _size )
	{
		PyObject * py_unicode = PyUnicode_FromUnicode( _value, _size );

		return py_unicode;
	}
	//////////////////////////////////////////////////////////////////////////
	const char * unicode_to_utf8( PyObject * _unicode )
	{
		PyObject * py_utf8 = _unicode_to_utf8_obj( _unicode );

		const char * ch_buff = pybind::string_to_char( py_utf8 );
		
		return ch_buff;
	}
    //////////////////////////////////////////////////////////////////////////
    const char * unicode_to_utf8_and_size( PyObject * _unicode, size_t & _size )
    {
        PyObject* py_utf8 = PyUnicode_AsUTF8String( _unicode );

        size_t size;
        const char * ch_buff = pybind::string_to_char_and_size( py_utf8, size );

        _size = size;

        return ch_buff;
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * unicode_from_utf8( const char * _utf8 )
    {
        PyObject * unicode = PyUnicode_FromString( _utf8 );

        return unicode;
    }
	//////////////////////////////////////////////////////////////////////////
	PyObject * unicode_from_utf8_size( const char * _utf8, size_t _size )
	{
		PyObject * unicode = PyUnicode_FromStringAndSize( _utf8, _size );

		return unicode;
	}
    //////////////////////////////////////////////////////////////////////////
    PyObject * _unicode_to_utf8_obj( PyObject * _unicode )
    {
        PyObject * py_utf8 = PyUnicode_AsUTF8String( _unicode );

        return py_utf8;
    }
    //////////////////////////////////////////////////////////////////////////
    bool void_ptr_check( PyObject * _obj )
    {
#   if PYBIND_PYTHON_VERSION > 300
        return PyCapsule_CheckExact( _obj );
#   else
        return PyCObject_Check( _obj );
#   endif		
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * void_ptr_new( void * _impl )
    {
#   if PYBIND_PYTHON_VERSION > 300
        PyObject * py_void = PyCapsule_New( _impl, NULL, NULL );
#	else
        PyObject * py_void = PyCObject_FromVoidPtr( _impl, NULL );    
#	endif

        return py_void;
    }
    //////////////////////////////////////////////////////////////////////////
    void * void_ptr_get( PyObject * _obj )
    {
#   if PYBIND_PYTHON_VERSION > 300
        void * impl = PyCapsule_GetPointer( _obj, NULL );
#   else
        void * impl = PyCObject_AsVoidPtr( _obj );
#   endif	

        return impl;
    }
    //////////////////////////////////////////////////////////////////////////
    long marshal_magic_number()
    {
        long py_magic = PyImport_GetMagicNumber();

        return py_magic;
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * marshal_get_object( const char * _buf, size_t _len )
    {
        char * buf_unconst = const_cast<char *>(_buf);
        PyObject * obj = PyMarshal_ReadObjectFromString( buf_unconst, _len );

        return obj;
    }
    //////////////////////////////////////////////////////////////////////////
    void _set_module_finder( PyObject * _finder )
    {
#   if PYBIND_PYTHON_VERSION < 300
        PyObject * py_meta_path = PySys_GetObject( "meta_path" );

        pybind::list_insert( py_meta_path, 0, _finder );
        pybind::decref( _finder );
#   endif

#   if PYBIND_PYTHON_VERSION >= 330
        PyThreadState * thread_state = PyThreadState_Get();
        PyInterpreterState * interp = thread_state->interp;
        interp->importlib = _finder;
#   endif
    }
}

