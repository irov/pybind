#include "python_system.hpp"

#include "pybind/class_type_scope_interface.hpp"

#include "pybind/stl/stl_type_cast.hpp"

#include "config/python.hpp"

#include "python_kernel.hpp"

#include <stdexcept>
#include <stdio.h>

#if PYBIND_PYTHON_ERROR_FORMAT_FLAG
#if PYBIND_PYTHON_VERSION < 300
#ifdef __cplusplus
extern "C" {
#endif
    extern int Py_ErrFormatFlag;
#ifdef __cplusplus
}
#endif
#endif
#endif

namespace pybind
{
    //////////////////////////////////////////////////////////////////////////
#if PYBIND_PYTHON_VERSION < 300 && defined(WITH_THREAD)
    static PyThreadState * gtstate;
#endif
    //////////////////////////////////////////////////////////////////////////
    kernel_interface * initialize( const kernel_domain_allocator_t * _allocator, const wchar_t * _path, bool _debug, bool install_sigs, bool _nosite )
    {
        (void)_allocator;
        (void)_path;
        (void)_debug;

#if PYBIND_PYTHON_ERROR_FORMAT_FLAG
#if PYBIND_PYTHON_VERSION < 300
        if( _debug == true )
        {
            Py_ErrFormatFlag = 1;
        }
        else
        {
            Py_ErrFormatFlag = 0;
        }
#endif
#endif

#if PYBIND_PYTHON_VERSION >= 300
        if( _allocator != nullptr )
        {
            if( _allocator->raw != nullptr )
            {
                PyMemAllocatorEx pyalloc;
                pyalloc.ctx = _allocator->raw->ctx;
                pyalloc.malloc = _allocator->raw->malloc;
                pyalloc.calloc = _allocator->raw->calloc;
                pyalloc.realloc = _allocator->raw->realloc;
                pyalloc.free = _allocator->raw->free;

                PyMem_SetAllocator( PYMEM_DOMAIN_RAW, &pyalloc );
            }

            if( _allocator->mem != nullptr )
            {
                PyMemAllocatorEx pyalloc;
                pyalloc.ctx = _allocator->mem->ctx;
                pyalloc.malloc = _allocator->mem->malloc;
                pyalloc.calloc = _allocator->mem->calloc;
                pyalloc.realloc = _allocator->mem->realloc;
                pyalloc.free = _allocator->mem->free;

                PyMem_SetAllocator( PYMEM_DOMAIN_MEM, &pyalloc );
            }

            if( _allocator->obj != nullptr )
            {
                PyMemAllocatorEx pyalloc;
                pyalloc.ctx = _allocator->obj->ctx;
                pyalloc.malloc = _allocator->obj->malloc;
                pyalloc.calloc = _allocator->obj->calloc;
                pyalloc.realloc = _allocator->obj->realloc;
                pyalloc.free = _allocator->obj->free;

                PyMem_SetAllocator( PYMEM_DOMAIN_OBJ, &pyalloc );
            }
        }
#endif


#if PYBIND_PYTHON_VERSION >= 300
        if( _path != nullptr )
        {
            Py_SetPath( _path );

            check_error();
        }
#endif

        if( Py_IsInitialized() == 0 )
        {
            if( _debug == false )
            {
                Py_OptimizeFlag = 2;
            }

            if( _nosite == true )
            {
                Py_NoSiteFlag = 1;
            }

            Py_IgnoreEnvironmentFlag = 1;

#if PYBIND_PYTHON_VERSION < 300
            Py_HashRandomizationFlag = 0;
#endif

#if PYBIND_PYTHON_VERSION >= 330
            Py_DontWriteBytecodeFlag = 1;
#endif

#if PYBIND_PYTHON_VERSION >= 330
            wchar_t pyProgramName[] = L"pybind330";
            Py_SetProgramName( pyProgramName );

            wchar_t pySearchPath[] = L".";
            Py_SetPythonHome( pySearchPath );

            Py_SetStandardStreamEncoding( "utf-8", "utf-8" );

            Py_InitializeEx( install_sigs ? 1 : 0 );
#elif PYBIND_PYTHON_VERSION >= 320
            wchar_t pyProgramName[] = L"pybind320";
            Py_SetProgramName( pyProgramName );

            wchar_t pySearchPath[] = L".";
            Py_SetPythonHome( pySearchPath );

            Py_InitializeEx( install_sigs ? 1 : 0 );
#else
            char pyProgramName[] = "pybind";
            Py_SetProgramName( pyProgramName );

            char pySearchPath[] = ".";
            Py_SetPythonHome( pySearchPath );

            Py_InitializeEx( install_sigs ? 1 : 0 );
#endif

#if PYBIND_PYTHON_VERSION < 300 && defined(WITH_THREAD)
            PyEval_InitThreads();
            gtstate = PyEval_SaveThread();
#endif
        }

        python_kernel * kernel = new python_kernel();

        if( kernel->initialize() == false )
        {
            return nullptr;
        }

        pybind::set_kernel( kernel );

        return kernel;
    }
    //////////////////////////////////////////////////////////////////////////
    void check_error()
    {
        if( PyErr_Occurred() != nullptr )
        {
            PyErr_Print();
        }
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t get_python_version()
    {
        uint32_t version = PYBIND_PYTHON_VERSION;

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
    PyObject * getStdErrorHandle()
    {
        PyObject * sysModule = PyImport_AddModule( "sys" );

        PyObject * stderr_handle = PyObject_GetAttrString( sysModule, "stderr" );

        return stderr_handle;
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * getStdOutHandle()
    {
        PyObject * sysModule = PyImport_AddModule( "sys" );

        PyObject * stdout_handle = PyObject_GetAttrString( sysModule, "stdout" );

        return stdout_handle;
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * get_builtins()
    {
#   if PYBIND_PYTHON_VERSION < 300
        PyObject * builtins = PyImport_ImportModuleLevel( const_cast<char *>("__builtin__"),
            nullptr, nullptr, nullptr, 0 );
#   endif


#   if PYBIND_PYTHON_VERSION >= 300
        PyObject * builtins = PyImport_ImportModuleLevel( "builtins",
            nullptr, nullptr, nullptr, 0 );
#   endif

        return builtins;
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * module_import( const char * _name, bool & _exsist )
    {
        PyObject * module = PyImport_ImportModule( _name );

        if( PyErr_Occurred() )
        {
            if( PyErr_ExceptionMatches( PyExc_ImportError ) == 1 )
            {
                _exsist = false;

                PyErr_Print();
                PyErr_Clear();

                return nullptr;

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
        PyObject * module = PyImport_AddModule( _name );

        return module;
    }
    //////////////////////////////////////////////////////////////////////////
    void module_fini( PyObject * _module )
    {
        _PyModule_Clear( _module );

        pybind::decref( _module );
    }
    //////////////////////////////////////////////////////////////////////////
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
    //////////////////////////////////////////////////////////////////////////
    void module_fini( PyObject * _module )
    {
        _PyModule_Clear( _module );

        pybind::decref( _module );
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
    void module_removeobject( PyObject * _module, const char * _name )
    {
        PyModule_AddObject( _module, _name, Py_None );
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * module_execcode( const char * _name, PyObject * _code )
    {
        char * unconst_name = const_cast<char *>(_name);
        PyObject * module = PyImport_ExecCodeModule( unconst_name, _code );

        return module;
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * module_reload( PyObject * _module )
    {
        PyObject * reload_module = PyImport_ReloadModule( _module );

        check_error();

        return reload_module;
    }
    //////////////////////////////////////////////////////////////////////////
    bool code_check( PyObject * _code )
    {
        return PyCode_Check( _code );
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * code_compile_file( const void * _buf, const char * _module )
    {
        PyObject * code = Py_CompileString( (const char *)_buf, _module, Py_file_input );

        return code;
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * set_currentmodule( PyObject * _obj )
    {
        kernel_interface * kernel = pybind::get_kernel();

        PyObject * old_module = kernel->get_current_module();

        kernel->set_current_module( _obj );

        return old_module;
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * get_currentmodule()
    {
        kernel_interface * kernel = pybind::get_kernel();

        PyObject * module = kernel->get_current_module();

        return module;
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
        va_start( valist, _format );

        PyObject * result = ask_va( _obj, _format, valist );

        va_end( valist );

        return result;
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * ask_method( PyObject * _obj, const char * _method, const char * _format, ... )
    {
        va_list valist;
        va_start( valist, _format );

        PyObject * result = ask_method_va( _obj, _method, _format, valist );

        va_end( valist );

        return result;
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * ask_va( PyObject * _obj, const char * _format, va_list _va )
    {
        PyObject * value = Py_VaBuildValue( _format, _va );

        if( value == nullptr )
        {
            check_error();

            return nullptr;
        }

        PyObject * result = ask_native( _obj, value );

        Py_DECREF( value );

        return result;
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * ask_method_va( PyObject * _obj, const char * _method, const char * _format, va_list _va )
    {
        PyObject * method = PyObject_GetAttrString( _obj, _method );

        if( method == nullptr )
        {
            Py_RETURN_NONE;
        }

        PyObject * result = ask_va( method, _format, _va );

        Py_DECREF( method );

        return result;
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * ask_method_native( PyObject * _obj, const char * _method, PyObject * _args )
    {
        PyObject * method = PyObject_GetAttrString( _obj, _method );

        if( method == nullptr )
        {
            Py_RETURN_NONE;
        }

        PyObject * result = ask_native( method, _args );

        Py_DECREF( method );

        return result;
    }
    //////////////////////////////////////////////////////////////////////////
    void call_native( PyObject * _obj, PyObject * _args )
    {
        PyObject * res = ask_native( _obj, _args );

        if( res == nullptr )
        {
            return;
        }

        Py_DECREF( res );
    }
    //////////////////////////////////////////////////////////////////////////
    void call( PyObject * _obj, const char * _format, ... )
    {
        va_list valist;
        va_start( valist, _format );

        call_va( _obj, _format, valist );

        va_end( valist );
    }
    //////////////////////////////////////////////////////////////////////////
    void call_method( PyObject * _obj, const char * _method, const char * _format, ... )
    {
        va_list valist;
        va_start( valist, _format );

        call_method_va( _obj, _method, _format, valist );

        va_end( valist );
    }
    //////////////////////////////////////////////////////////////////////////
    void call_method_native( PyObject * _obj, const char * _method, PyObject * _args )
    {
        PyObject * res = ask_method_native( _obj, _method, _args );

        if( res == nullptr )
        {
            return;
        }

        Py_DECREF( res );
    }
    //////////////////////////////////////////////////////////////////////////
    void call_va( PyObject * _obj, const char * _format, va_list _va )
    {
        PyObject * res = ask_va( _obj, _format, _va );

        if( res == nullptr )
        {
            return;
        }

        Py_DECREF( res );
    }
    //////////////////////////////////////////////////////////////////////////
    void call_method_va( PyObject * _obj, const char * _method, const char * _format, va_list _va )
    {
        PyObject * res = ask_method_va( _obj, _method, _format, _va );

        if( res == NULL )
        {
            return;
        }

        Py_DECREF( res );
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * exec( const char * _code, PyObject * _global, PyObject * _local )
    {
        PyObject* result = PyRun_String( _code, Py_file_input, _global, _local );

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
    void set_path( const wchar_t * _value )
    {
        (void)_value;
#   if PYBIND_PYTHON_VERSION >= 300
        Py_SetPath( _value );
#   endif

        check_error();
    }
    //////////////////////////////////////////////////////////////////////////
    void set_syspath( PyObject * _value )
    {
        PySys_SetObject( const_cast<char *>("path"), _value );

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
        Py_XINCREF( _obj );
    }
    //////////////////////////////////////////////////////////////////////////
    void decref( PyObject * _obj )
    {
        Py_XDECREF( _obj );
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t refcount( PyObject * _obj )
    {
        return (uint32_t)_obj->ob_refcnt;
    }
    //////////////////////////////////////////////////////////////////////////
    bool is_callable( PyObject * _obj )
    {
        return PyCallable_Check( _obj ) == 1;
    }
    //////////////////////////////////////////////////////////////////////////
    bool is_instanceof( PyObject * _obj, PyTypeObject * _type )
    {
        int result = PyObject_TypeCheck( _obj, _type );

        return result == 1;
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
    bool has_attr( PyObject * _obj, PyObject * _attr )
    {
        int value = PyObject_HasAttr( _obj, _attr );

        return value == 1;
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * get_attr( PyObject * _obj, PyObject * _attr )
    {
        PyObject * obj = PyObject_GetAttr( _obj, _attr );

        return obj;
    }
    //////////////////////////////////////////////////////////////////////////
    bool has_attrstring( PyObject * _obj, const char * _attr )
    {
        int value = PyObject_HasAttrString( _obj, _attr );

        return value == 1;
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * get_attrstring( PyObject * _obj, const char * _attr )
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
        int result = PyType_Check( _obj );

        return result == 1;
    }
    //////////////////////////////////////////////////////////////////////////
    bool int_check( PyObject * _obj )
    {
#   if PYBIND_PYTHON_VERSION < 300
        int result = PyInt_Check( _obj );
#	else
        int result = PyLong_Check( _obj );
#	endif

        return result == 1;
    }
    //////////////////////////////////////////////////////////////////////////
    bool long_check( PyObject * _obj )
    {
        int result = PyLong_Check( _obj );

        return result == 1;
    }
    //////////////////////////////////////////////////////////////////////////
    bool float_check( PyObject * _obj )
    {
        int result = PyFloat_Check( _obj );

        return result == 1;
    }
    //////////////////////////////////////////////////////////////////////////
    bool extract_bool( PyObject * _obj, bool & _value )
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
        else if( PyFloat_Check( _obj ) )
        {
            _value = (PyFloat_AsDouble( _obj ) != 0.0);
        }
        else if( PyLong_Check( _obj ) )
        {
            _value = (PyLong_AsLong( _obj ) != 0);
        }
        else
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    template<class T>
    static bool extract_int_t( PyObject * _obj, T & _value )
    {
        if( _obj == nullptr )
        {
            return false;
        }

#   if PYBIND_PYTHON_VERSION < 300
        else if( PyInt_Check( _obj ) )
        {
            _value = (T)PyInt_AS_LONG( _obj );
        }
#	endif
        else if( PyLong_Check( _obj ) )
        {
            _value = (T)PyLong_AsLongLong( _obj );
        }
        else if( PyFloat_Check( _obj ) )
        {
            _value = (T)PyFloat_AsDouble( _obj );
        }
        else if( PyBool_Check( _obj ) )
        {
            _value = (T)(_obj == Py_True);
        }
        else
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool extract_int8( PyObject * _obj, int8_t & _value )
    {
        return extract_int_t<int8_t>( _obj, _value );
    }
    //////////////////////////////////////////////////////////////////////////
    bool extract_int16( PyObject * _obj, int16_t & _value )
    {
        return extract_int_t<int16_t>( _obj, _value );
    }
    //////////////////////////////////////////////////////////////////////////
    bool extract_int32( PyObject * _obj, int32_t & _value )
    {
        return extract_int_t<int32_t>( _obj, _value );
    }
    //////////////////////////////////////////////////////////////////////////
    bool extract_int64( PyObject * _obj, int64_t & _value )
    {
        return extract_int_t<int64_t>( _obj, _value );
    }
    //////////////////////////////////////////////////////////////////////////
    template<class T>
    static bool extract_unsigned_int_t( PyObject * _obj, T & _value )
    {
        if( _obj == nullptr )
        {
            return false;
        }

#   if PYBIND_PYTHON_VERSION < 300
        if( PyInt_Check( _obj ) )
        {
            _value = (T)PyInt_AS_LONG( _obj );
        }
        else
#	endif
            if( PyLong_Check( _obj ) )
            {
                _value = (T)PyLong_AsUnsignedLong( _obj );
            }
            else if( PyFloat_Check( _obj ) )
            {
                _value = (T)PyFloat_AsDouble( _obj );
            }
            else if( PyBool_Check( _obj ) )
            {
                _value = (T)(_obj == Py_True);
            }
            else
            {
                return false;
            }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool extract_uint8( PyObject * _obj, uint8_t & _value )
    {
        return extract_unsigned_int_t<uint8_t>( _obj, _value );
    }
    //////////////////////////////////////////////////////////////////////////
    bool extract_uint16( PyObject * _obj, uint16_t & _value )
    {
        return extract_unsigned_int_t<uint16_t>( _obj, _value );
    }
    //////////////////////////////////////////////////////////////////////////
    bool extract_uint32( PyObject * _obj, uint32_t & _value )
    {
        return extract_unsigned_int_t<uint32_t>( _obj, _value );
    }
    //////////////////////////////////////////////////////////////////////////
    bool extract_uint64( PyObject * _obj, uint64_t & _value )
    {
        return extract_unsigned_int_t<uint64_t>( _obj, _value );
    }
    //////////////////////////////////////////////////////////////////////////
    template<class T>
    static bool extract_float_t( PyObject * _obj, T & _value )
    {
        if( _obj == nullptr )
        {
            return false;
        }

        if( PyFloat_Check( _obj ) )
        {
            _value = (T)PyFloat_AsDouble( _obj );
        }
#   if PYBIND_PYTHON_VERSION < 300
        else if( PyInt_Check( _obj ) )
        {
            _value = (T)PyInt_AsLong( _obj );
        }
#	endif
        else if( PyLong_Check( _obj ) )
        {
            _value = (T)PyLong_AsDouble( _obj );
        }
        else if( PyBool_Check( _obj ) )
        {
            _value = (T)(_obj == Py_True);
        }
        else
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool extract_float( PyObject * _obj, float & _value )
    {
        return extract_float_t<float>( _obj, _value );
    }
    //////////////////////////////////////////////////////////////////////////
    bool extract_double( PyObject * _obj, double & _value )
    {
        return extract_float_t<double>( _obj, _value );
    }
    //////////////////////////////////////////////////////////////////////////
    bool extract_wchar( PyObject * _obj, wchar_t & _value )
    {
        if( _obj == nullptr )
        {
            return false;
        }

        if( PyUnicode_CheckExact( _obj ) == true )
        {
            const wchar_t * ch_buff = PyUnicode_AS_UNICODE( _obj );

            if( ch_buff == nullptr )
            {
                return false;
            }

            Py_ssize_t sz = PyUnicode_GET_SIZE( _obj );

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
    PyObject * ptr_bool( bool _value )
    {
        if( _value == true )
        {
            Py_RETURN_TRUE;
        }

        Py_RETURN_FALSE;
    }
    //////////////////////////////////////////////////////////////////////////
    template<class T>
    static PyObject * ptr_int_t( T _value )
    {
#   if PYBIND_PYTHON_VERSION < 300
        return PyInt_FromLong( _value );
#	else
        return PyLong_FromLong( _value );
#	endif
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * ptr_int8( int8_t _value )
    {
        return ptr_int_t<int8_t>( _value );
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * ptr_int16( int16_t _value )
    {
        return ptr_int_t<int16_t>( _value );
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * ptr_int32( int32_t _value )
    {
        return ptr_int_t<int32_t>( _value );
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * ptr_int64( int64_t _value )
    {
        return PyLong_FromLongLong( _value );
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * ptr_uint8( uint8_t _value )
    {
        return ptr_int_t<uint8_t>( _value );
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * ptr_uint16( uint16_t _value )
    {
        return ptr_int_t<uint16_t>( _value );
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * ptr_uint32( uint32_t _value )
    {
        return ptr_int_t<uint32_t>( _value );
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * ptr_uint64( uint64_t _value )
    {
        return PyLong_FromUnsignedLongLong( _value );
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * ptr_float( float _value )
    {
        return PyFloat_FromDouble( _value );
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * ptr_double( double _value )
    {
        return PyFloat_FromDouble( _value );
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * ptr_long( long _value )
    {
        return PyLong_FromLong( _value );
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * ptr_ulong( unsigned long _value )
    {
        return PyLong_FromUnsignedLong( _value );
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * list_new( uint32_t _size )
    {
        Py_ssize_t py_size = (Py_ssize_t)_size;
        PyObject * obj = PyList_New( py_size );

        return obj;
    }
    //////////////////////////////////////////////////////////////////////////
    bool list_check( PyObject * _obj )
    {
        int res = PyList_Check( _obj );

        return res == 1;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t list_size( PyObject * _obj )
    {
        Py_ssize_t size = PyList_Size( _obj );

        if( size == -1 )
        {
            check_error();

            return 0;
        }

        return (uint32_t)size;
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * list_getitem( PyObject * _obj, uint32_t _index )
    {
        Py_ssize_t py_index = (Py_ssize_t)_index;
        PyObject * obj = PyList_GetItem( _obj, py_index );

        if( obj == nullptr )
        {
            check_error();
        }

        return obj;
    }
    //////////////////////////////////////////////////////////////////////////
    bool list_insert( PyObject * _obj, uint32_t _index, PyObject * _item )
    {
        Py_ssize_t py_index = (Py_ssize_t)_index;
        int res = PyList_Insert( _obj, py_index, _item );

        if( res != 0 )
        {
            check_error();

            return false;
        }

        //Py_DECREF( _item );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool list_remove( PyObject * _obj, uint32_t _it )
    {
        int res = PyList_SetSlice( _obj, _it, _it + 1, nullptr );

        if( res != 0 )
        {
            check_error();

            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool list_setitem( PyObject * _obj, uint32_t _index, PyObject * _item )
    {
        Py_ssize_t py_index = (Py_ssize_t)_index;
        int res = PyList_SetItem( _obj, py_index, _item );

        if( res != 0 )
        {
            check_error();

            return false;
        }

        Py_INCREF( _item );

        return true;
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

        //Py_DECREF( _item );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * dict_new()
    {
        PyObject * obj = PyDict_New();

        return obj;
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * dict_new_presized( uint32_t _count )
    {
        PyObject * obj = _PyDict_NewPresized( _count );

        return obj;
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * dict_from( PyObject * _obj )
    {
        PyObject * py_dict = pybind::dict_new();

        PyObject * py_dir = PyObject_Dir( _obj );

        for( Py_ssize_t
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

            Py_DECREF( py_value );
        }

        Py_DECREF( py_dir );

        return py_dict;
    }
    //////////////////////////////////////////////////////////////////////////
    bool dict_check( PyObject * _obj )
    {
        int value = PyDict_Check( _obj );

        return value == 1;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t dict_size( PyObject * _obj )
    {
        Py_ssize_t size = PyDict_Size( _obj );

        return (uint32_t)size;
    }
    //////////////////////////////////////////////////////////////////////////
    bool dict_setstring( PyObject * _dict, const char * _name, PyObject * _value )
    {
        int res = PyDict_SetItemString( _dict, _name, _value );

        if( res == -1 )
        {
            check_error();

            return false;
        }

        //Py_DECREF( _value );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool dict_set( PyObject * _dict, PyObject * _name, PyObject * _value )
    {
        int res = PyDict_SetItem( _dict, _name, _value );

        if( res == -1 )
        {
            check_error();

            return false;
        }

        //Py_DECREF( _value );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool dict_removestring( PyObject * _dict, const char * _key )
    {
        int res = PyDict_DelItemString( _dict, _key );

        if( res == -1 )
        {
            check_error();

            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool dict_remove( PyObject * _dict, PyObject * _key )
    {
        int res = PyDict_DelItem( _dict, _key );

        if( res == -1 )
        {
            check_error();

            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * dict_getstring( PyObject * _dict, const char * _key )
    {
        PyObject * obj = PyDict_GetItemString( _dict, _key );

        return obj;
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * dict_get( PyObject * _dict, PyObject * _key )
    {
        PyObject * obj = PyDict_GetItem( _dict, _key );

        return obj;
    }
    //////////////////////////////////////////////////////////////////////////
    bool dict_exist( PyObject * _dict, PyObject * _key )
    {
        int contains = PyDict_Contains( _dict, _key );

        if( contains == 1 )
        {
            return true;
        }

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool dict_existstring( PyObject * _dict, const char * _key )
    {
        PyObject * kv = string_from_char( _key );

        bool result = dict_exist( _dict, kv );

        Py_DECREF( kv );

        return result;
    }
    //////////////////////////////////////////////////////////////////////////
    bool dict_next( PyObject * _dict, uint32_t & _pos, PyObject ** _key, PyObject ** _value )
    {
        Py_ssize_t ps = (Py_ssize_t)(_pos);

        int res = PyDict_Next( _dict, &ps, _key, _value );

        _pos = (uint32_t)ps;

        return res == 1;
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * dict_items( PyObject * _dict )
    {
        PyObject * obj = PyDict_Items( _dict );

        return obj;
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * tuple_new( uint32_t _size )
    {
        Py_ssize_t py_size = (Py_ssize_t)_size;
        PyObject * obj = PyTuple_New( py_size );

        return obj;
    }
    //////////////////////////////////////////////////////////////////////////
    bool tuple_setitem( PyObject * _tuple, uint32_t _index, PyObject * _value )
    {
        if( _value == nullptr )
        {
            return false;
        }

        Py_ssize_t py_index = (Py_ssize_t)_index;

        int res = PyTuple_SetItem( _tuple, py_index, _value );

        if( res != 0 )
        {
            check_error();

            return false;
        }

        Py_INCREF( _value );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool tuple_check( PyObject * _tuple )
    {
        int value = PyTuple_Check( _tuple );

        return value == 1;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t tuple_size( PyObject * _obj )
    {
        Py_ssize_t size = PyTuple_Size( _obj );

        return (uint32_t)size;
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * tuple_getitem( PyObject * _obj, uint32_t _index )
    {
        Py_ssize_t py_index = (Py_ssize_t)_index;
        PyObject * obj = PyTuple_GetItem( _obj, py_index );

        return obj;
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * tuple_slice( PyObject * _obj, uint32_t _low, uint32_t _high )
    {
        Py_ssize_t py_low = (Py_ssize_t)_low;
        Py_ssize_t py_high = (Py_ssize_t)_high;
        PyObject * obj_slice = PyTuple_GetSlice( _obj, py_low, py_high );

        return obj_slice;
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * tuple_slice_tail( PyObject * _obj, uint32_t _size )
    {
        uint32_t size = pybind::tuple_size( _obj );

        PyObject * obj_slice = pybind::tuple_slice( _obj, _size, size );

        return obj_slice;
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * object_dir( PyObject * _obj )
    {
        PyObject * py_dir = PyObject_Dir( _obj );

        return py_dir;
    }
    //////////////////////////////////////////////////////////////////////////
    PyTypeObject * object_type( PyObject * _obj )
    {
        PyTypeObject * type = Py_TYPE( _obj );

        return type;
    }
    //////////////////////////////////////////////////////////////////////////
    const char * object_repr_type( PyObject * _obj )
    {
        if( _obj == nullptr )
        {
            return "NULL";
        }

        PyTypeObject * type = pybind::object_type( _obj );

        PyObject * repr = PyObject_Repr( (PyObject *)type );

        if( repr == nullptr )
        {
            return "INVALID";
        }

        const char * str = pybind::string_to_char( repr );

        return str;
    }
    //////////////////////////////////////////////////////////////////////////
    const char * object_repr( PyObject * _obj )
    {
        if( _obj == nullptr )
        {
            return "NULL";
        }

        PyObject * py_repr = PyObject_Repr( _obj );

        if( py_repr == nullptr )
        {
            return "INVALID";
        }

        const char * str = pybind::string_to_char( py_repr );

        return str;
    }
    //////////////////////////////////////////////////////////////////////////
    const char * object_str( PyObject * _obj )
    {
        if( _obj == nullptr )
        {
            return "NULL";
        }

        PyObject * py_str = PyObject_Str( _obj );

        if( py_str == nullptr )
        {
            return "INVALID";
        }

        const char * str = pybind::string_to_char( py_str );

        return str;
    }
    //////////////////////////////////////////////////////////////////////////
    int64_t object_hash( PyObject * _obj )
    {
        int64_t py_hash = (int64_t)PyObject_Hash( _obj );

        return py_hash;
    }
    //////////////////////////////////////////////////////////////////////////
    bool get_traceback( char * _buffer, size_t _maxlen )
    {
        if( _maxlen == 0 )
        {
            return true;
        }

        PyObject * modtraceback = PyImport_ImportModule( "traceback" );        

        if( modtraceback == nullptr )
        {
            PyErr_Clear();

            _buffer[0] = '\0';

            return false;
        }

        PyObject * string_stackFunc = PyObject_GetAttrString( modtraceback, "string_stack" );
        PyObject * string_stackResult = PyObject_CallObject( string_stackFunc, NULL );
        char * sresult = PyBytes_AsString( string_stackResult );
        strncpy( _buffer, sresult, _maxlen );
        _buffer[_maxlen - 1] = '\0';
        Py_XDECREF( string_stackResult );
        Py_XDECREF( string_stackFunc );
        Py_XDECREF( modtraceback );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void error_traceback( const char * _format, ... )
    {
        va_list valist;
        va_start( valist, _format );

        error_traceback_va( _format, valist );

        va_end( valist );
    }
    //////////////////////////////////////////////////////////////////////////
    void error_traceback_va( const char * _format, va_list _va )
    {
        pybind::check_error();

        char buffer[4096];
        vsprintf( buffer, _format, _va );

        PyThreadState * tstate = PyThreadState_GET();

        if( tstate->frame == nullptr )
        {
            return;
        }

        PyObject * py_filename = tstate->frame->f_code->co_filename;
        int fileline = tstate->frame->f_lineno;

        const char * str_filename = pybind::string_to_char( py_filename );

        char trace_buffer[2048];
        sprintf( trace_buffer, "%s[%d]: %s"
            , str_filename
            , fileline
            , buffer
        );

        PyErr_SetString( PyExc_RuntimeError, trace_buffer );

        pybind::check_error();
    }
    //////////////////////////////////////////////////////////////////////////
    void error_message( const char * _format, ... )
    {
        va_list valist;
        va_start( valist, _format );
        error_message_va( _format, valist );
        va_end( valist );
    }
    //////////////////////////////////////////////////////////////////////////
    void error_message_va( const char * _format, va_list _va )
    {
        pybind::check_error();

        char buffer[2048];
        vsprintf( buffer, _format, _va );

        PyErr_SetString( PyExc_RuntimeError, buffer );
    }
    //////////////////////////////////////////////////////////////////////////
    void throw_message( const char * _format, ... )
    {
        va_list valist;
        va_start( valist, _format );
        throw_message_va( _format, valist );
        va_end( valist );
    }
    //////////////////////////////////////////////////////////////////////////
    void throw_message_va( const char * _format, va_list _va )
    {
        error_message_va( _format, _va );
        throw_exception_va( _format, _va );
    }
    //////////////////////////////////////////////////////////////////////////
    void warning_traceback( const char * _format, ... )
    {
        va_list valist;
        va_start( valist, _format );

        warning_traceback_va( _format, valist );
        va_end( valist );
    }
    //////////////////////////////////////////////////////////////////////////
    void warning_traceback_va( const char * _format, va_list _va )
    {
        pybind::check_error();

        char buffer[4096];
        vsprintf( buffer, _format, _va );

        PyThreadState * tstate = PyThreadState_GET();

        if( tstate->frame == nullptr )
        {
            return;
        }

        PyObject * py_filename = tstate->frame->f_code->co_filename;
        int fileline = tstate->frame->f_lineno;

        const char * str_filename = pybind::string_to_char( py_filename );

        char trace_buffer[2048];
        sprintf( trace_buffer, "%s[%d]: %s"
            , str_filename
            , fileline
            , buffer
        );

        PyErr_SetString( PyExc_RuntimeWarning, trace_buffer );
    }
    //////////////////////////////////////////////////////////////////////////
    void error_clear()
    {
        PyErr_Clear();
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * build_value( const char * _format, ... )
    {
        va_list valist;
        va_start( valist, _format );

        PyObject * value = build_value_va( _format, valist );

        va_end( valist );

        return value;
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * build_value_va( const char * _format, va_list _va )
    {
        PyObject * value = Py_VaBuildValue( _format, _va );

        if( value == nullptr )
        {
            pybind::check_error();

            return nullptr;
        }

        return value;
    }
    //////////////////////////////////////////////////////////////////////////
    void unwrap( PyObject * _value )
    {
        kernel_interface * kernel = pybind::get_kernel();

        kernel->unwrap( _value );
    }
    //////////////////////////////////////////////////////////////////////////
    bool is_wrap( PyObject * _value )
    {
        kernel_interface * kernel = pybind::get_kernel();

        bool result = kernel->is_wrap( _value );

        return result;
    }
    //////////////////////////////////////////////////////////////////////////
    bool is_class( PyObject * _obj )
    {
        kernel_interface * kernel = pybind::get_kernel();

        bool result = kernel->is_class( _obj );

        return result;
    }
    //////////////////////////////////////////////////////////////////////////
    bool is_type_class( PyTypeObject * _type )
    {
        kernel_interface * kernel = pybind::get_kernel();

        bool result = kernel->is_type_class( _type );

        return result;
    }
    //////////////////////////////////////////////////////////////////////////
    bool type_initialize( PyObject * _obj )
    {
        kernel_interface * kernel = pybind::get_kernel();

        PyTypeObject * type = (PyTypeObject *)_obj;

        const class_type_scope_interface_ptr & scope = kernel->get_class_scope( type );

        if( scope == nullptr )
        {
            return false;
        }

        scope->type_initialize( type );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool is_none( PyObject * _obj )
    {
        return _obj == Py_None;
    }
    //////////////////////////////////////////////////////////////////////////
    bool bool_check( PyObject * _obj )
    {
        bool result = PyBool_Check( _obj );

        return result;
    }
    //////////////////////////////////////////////////////////////////////////
    bool test_equal( PyObject * _left, PyObject * _right )
    {
        int value = PyObject_RichCompareBool( _left, _right, Py_EQ );

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
        if( PyBytes_Check( _string ) == 1 )
        {
            return true;
        }

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t string_size( PyObject * _string )
    {
        Py_ssize_t size = PyBytes_Size( _string );

        return (uint32_t)size;
    }
    //////////////////////////////////////////////////////////////////////////
    int64_t string_hash( PyObject * _string )
    {
#   if PYBIND_PYTHON_VERSION < 300
        int64_t hash = (int64_t)((PyStringObject *)_string)->ob_shash;
#	else
        int64_t hash = (int64_t)((PyUnicodeObject *)_string)->_base._base.hash;
#	endif                

        return hash;
    }
    //////////////////////////////////////////////////////////////////////////
    const char * string_to_char( PyObject * _string )
    {
        char * ch_buff_unconst = PyBytes_AsString( _string );

        const char * ch_buff = const_cast<const char *>(ch_buff_unconst);

        return ch_buff;
    }
    //////////////////////////////////////////////////////////////////////////
    const char * string_to_char_and_size( PyObject * _string, uint32_t & _size )
    {
        char * ch_buff_unconst;
        Py_ssize_t len;
        PyBytes_AsStringAndSize( _string, &ch_buff_unconst, &len );

        _size = (uint32_t)len;

        const char * ch_buff = const_cast<const char *>(ch_buff_unconst);

        return ch_buff;
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * string_from_char( const char * _str )
    {
        PyObject * py_str = PyBytes_FromString( _str );

        return py_str;
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * string_from_char_size( const char * _str, uint32_t _size )
    {
        Py_ssize_t py_size = (Py_ssize_t)_size;
        PyObject * py_str = PyBytes_FromStringAndSize( _str, py_size );

        return py_str;
    }
    //////////////////////////////////////////////////////////////////////////
    bool unicode_check( PyObject * _unicode )
    {
        bool value = PyUnicode_CheckExact( _unicode );

        return value;
    }
    //////////////////////////////////////////////////////////////////////////
    const wchar_t * unicode_to_wchar( PyObject * _unicode )
    {
        const wchar_t * wstr = PyUnicode_AS_UNICODE( _unicode );

        return wstr;
    }
    //////////////////////////////////////////////////////////////////////////
    const wchar_t * unicode_to_wchar_and_size( PyObject * _unicode, uint32_t & _size )
    {
        const wchar_t * wstr = PyUnicode_AS_UNICODE( _unicode );
        Py_ssize_t py_size = PyUnicode_GET_SIZE( _unicode );

        _size = (uint32_t)py_size;

        return wstr;
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * unicode_from_wchar( const wchar_t * _value )
    {
        size_t size = wcslen( _value );

        Py_ssize_t py_size = (Py_ssize_t)size;
        PyObject * py_unicode = PyUnicode_FromUnicode( _value, py_size );

        return py_unicode;
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * unicode_from_wchar_size( const wchar_t * _value, uint32_t _size )
    {
        Py_ssize_t py_size = (Py_ssize_t)_size;
        PyObject * py_unicode = PyUnicode_FromUnicode( _value, py_size );

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
    const char * unicode_to_utf8_and_size( PyObject * _unicode, uint32_t & _size )
    {
        PyObject* py_utf8 = PyUnicode_AsUTF8String( _unicode );

        uint32_t size;
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
    PyObject * unicode_from_utf8_size( const char * _utf8, uint32_t _size )
    {
        Py_ssize_t py_size = (Py_ssize_t)_size;
        PyObject * unicode = PyUnicode_FromStringAndSize( _utf8, py_size );

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
        return PyCapsule_CheckExact( _obj );
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * void_ptr_new( void * _impl )
    {
        PyObject * py_void = PyCapsule_New( _impl, nullptr, nullptr );

        return py_void;
    }
    //////////////////////////////////////////////////////////////////////////
    void * void_ptr_get( PyObject * _obj )
    {
        void * impl = PyCapsule_GetPointer( _obj, nullptr );

        return impl;
    }
    //////////////////////////////////////////////////////////////////////////
    long marshal_magic_number()
    {
        long py_magic = PyImport_GetMagicNumber();

        return py_magic;
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * marshal_get_object( const void * _buf, size_t _len )
    {
        Py_ssize_t py_len = (Py_ssize_t)_len;

        char * buf_unconst = const_cast<char *>(static_cast<const char *>(_buf));
        PyObject * obj = PyMarshal_ReadObjectFromString( buf_unconst, py_len );

        return obj;
    }
    //////////////////////////////////////////////////////////////////////////
    void set_module_finder( PyObject * _finder )
    {
#   if PYBIND_PYTHON_VERSION < 300
        PyObject * py_meta_path = PySys_GetObject( const_cast<char *>("meta_path") );

        pybind::list_insert( py_meta_path, 0, _finder );
#   endif

#   if PYBIND_PYTHON_VERSION >= 330
        PyThreadState * thread_state = PyThreadState_Get();
        PyInterpreterState * interp = thread_state->interp;
        interp->importlib = _finder;
#   endif
    }
    //////////////////////////////////////////////////////////////////////////
    void remove_module_finder()
    {
#   if PYBIND_PYTHON_VERSION < 300
        PyObject * py_meta_path = PySys_GetObject( const_cast<char *>("meta_path") );

        pybind::list_remove( py_meta_path, 0 );
#   endif

#   if PYBIND_PYTHON_VERSION >= 330
        PyThreadState * thread_state = PyThreadState_Get();
        PyInterpreterState * interp = thread_state->interp;
        interp->importlib = nullptr;
#   endif
    }
}

