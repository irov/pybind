#include "python_system.hpp"

#include "pybind/class_type_scope_interface.hpp"
#include "pybind/stl/stl_type_cast.hpp"

#include "config/python.hpp"

#include "python_kernel.hpp"

#include <stdexcept>
#include <cstdio>

#ifndef NDEBUG
#include <thread>
#endif

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
#ifndef NDEBUG
    std::thread::id g_main_thread_id;

    static void check_main_thread()
    {
        if( g_main_thread_id != std::this_thread::get_id() )
        {
            throw std::runtime_error( "pybind: not main thread" );
        }
    }

#   define PYBIND_CHECK_MAIN_THREAD() pybind::check_main_thread()
#else
#   define PYBIND_CHECK_MAIN_THREAD()
#endif
    //////////////////////////////////////////////////////////////////////////
    kernel_interface * initialize( allocator_interface * _allocator, const wchar_t * _path, bool _debug, bool install_sigs, bool _nosite )
    {
        (void)_allocator;
        (void)_path;
        (void)_debug;

#ifndef NDEBUG
        std::thread::id main_thread_id = std::this_thread::get_id();

        g_main_thread_id = main_thread_id;
#endif

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

#if PYBIND_PYTHON_VERSION >= 380
        if( _allocator != nullptr )
        {
            //PyObjectArenaAllocator arenaalloc;
            //arenaalloc.ctx = static_cast<void *>(_allocator);
            //arenaalloc.alloc = &detail::s_pybind_malloc;
            //arenaalloc.free = &detail::s_pybind_free_n;

            //PyObject_SetArenaAllocator( &arenaalloc );

            //PyMemAllocatorEx pyalloc;
            //pyalloc.ctx = static_cast<void *>(_allocator);
            //pyalloc.malloc = &detail::s_pybind_malloc;
            //pyalloc.calloc = &detail::s_pybind_calloc;
            //pyalloc.realloc = &detail::s_pybind_realloc;
            //pyalloc.free = &detail::s_pybind_free;

            //PyMem_SetAllocator( PYMEM_DOMAIN_RAW, &pyalloc );
            //PyMem_SetAllocator( PYMEM_DOMAIN_MEM, &pyalloc );
            //PyMem_SetAllocator( PYMEM_DOMAIN_OBJ, &pyalloc );
        }
#elif PYBIND_PYTHON_VERSION >= 300
        //if( _allocator != nullptr )
        //{
        //    PyMemAllocatorEx pyalloc;
        //    pyalloc.ctx = static_cast<void *>(_allocator);
        //    pyalloc.malloc = &detail::s_pybind_malloc;
        //    pyalloc.calloc = &detail::s_pybind_calloc;
        //    pyalloc.realloc = &detail::s_pybind_realloc;
        //    pyalloc.free = &detail::s_pybind_free;

        //    PyMem_SetAllocator( PYMEM_DOMAIN_RAW, &pyalloc );
        //    PyMem_SetAllocator( PYMEM_DOMAIN_MEM, &pyalloc );
        //    PyMem_SetAllocator( PYMEM_DOMAIN_OBJ, &pyalloc );
        //}
#endif


#if PYBIND_PYTHON_VERSION >= 300
        if( _path != nullptr )
        {
            Py_SetPath( _path );

            pybind::check_error();
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

#if PYBIND_PYTHON_VERSION >= 380
            wchar_t pyProgramName[] = L"pybind380";
            Py_SetProgramName( pyProgramName );

            wchar_t pyPythonHome[] = L".";
            Py_SetPythonHome( pyPythonHome );

            Py_SetStandardStreamEncoding( "utf-8", "utf-8" );

            Py_InitializeEx( install_sigs ? 1 : 0 );
#elif PYBIND_PYTHON_VERSION >= 330
            wchar_t pyProgramName[] = L"pybind330";
            Py_SetProgramName( pyProgramName );

            wchar_t pyPythonHome[] = L".";
            Py_SetPythonHome( pyPythonHome );

            Py_SetStandardStreamEncoding( "utf-8", "utf-8" );

            Py_InitializeEx( install_sigs ? 1 : 0 );
#elif PYBIND_PYTHON_VERSION >= 320
            wchar_t pyProgramName[] = L"pybind320";
            Py_SetProgramName( pyProgramName );

            wchar_t pyPythonHome[] = L".";
            Py_SetPythonHome( pyPythonHome );

            Py_InitializeEx( install_sigs ? 1 : 0 );
#else
            char pyProgramName[] = "pybind";
            Py_SetProgramName( pyProgramName );

            char pyPythonHome[] = ".";
            Py_SetPythonHome( pyPythonHome );

            Py_InitializeEx( install_sigs ? 1 : 0 );
#endif

#if PYBIND_PYTHON_VERSION < 300 && defined(WITH_THREAD)
            PyEval_InitThreads();
            gtstate = PyEval_SaveThread();
#endif
        }

        python_kernel * kernel = _allocator->newP<python_kernel>();

        if( kernel->initialize( _allocator ) == false )
        {
            kernel->destroy();

            return nullptr;
        }

        pybind::set_kernel( kernel );

        return kernel;
    }
    //////////////////////////////////////////////////////////////////////////
    void update_main_thread()
    {        
#ifndef NDEBUG
        g_main_thread_id = std::this_thread::get_id();
#endif
    }
    //////////////////////////////////////////////////////////////////////////
    void check_error()
    {
        PYBIND_CHECK_MAIN_THREAD();

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
        PYBIND_CHECK_MAIN_THREAD();

        PySys_SetObject( "stderr", _handle );
    }
    //////////////////////////////////////////////////////////////////////////
    void setStdOutHandle( PyObject * _handle )
    {
        PYBIND_CHECK_MAIN_THREAD();

        PySys_SetObject( "stdout", _handle );
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * getStdErrorHandle()
    {
        PYBIND_CHECK_MAIN_THREAD();

        PyObject * stderr_handle = PySys_GetObject( "stderr" );

        return stderr_handle;
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * getStdOutHandle()
    {
        PYBIND_CHECK_MAIN_THREAD();

        PyObject * stdout_handle = PySys_GetObject( "stdout" );

        return stdout_handle;
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * get_builtins()
    {
        PYBIND_CHECK_MAIN_THREAD();

#if PYBIND_PYTHON_VERSION < 300
        PyObject * builtins = PyImport_ImportModuleLevel( const_cast<char *>("__builtin__"),
            nullptr, nullptr, nullptr, 0 );
#endif


#if PYBIND_PYTHON_VERSION >= 300
        PyObject * builtins = PyImport_ImportModuleLevel( "builtins",
            nullptr, nullptr, nullptr, 0 );
#endif

        return builtins;
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * module_import( const char * _name, bool & _exsist )
    {
        PYBIND_CHECK_MAIN_THREAD();

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
#if PYBIND_PYTHON_VERSION < 300
    //////////////////////////////////////////////////////////////////////////
    PyObject * module_init( const char * _name )
    {
        PYBIND_CHECK_MAIN_THREAD();

        PyObject * module = PyImport_AddModule( _name );

        return module;
    }
    //////////////////////////////////////////////////////////////////////////
    void module_fini( PyObject * _module )
    {
        PYBIND_CHECK_MAIN_THREAD();

        _PyModule_Clear( _module );
    }
    //////////////////////////////////////////////////////////////////////////
#else
    //////////////////////////////////////////////////////////////////////////
    PyObject * module_init( const char * _name )
    {
        PYBIND_CHECK_MAIN_THREAD();

        //PyImport_AppendInittab( _name, &initfunc );
        PyObject * module = PyImport_AddModule( _name );

        return module;
    }
    //////////////////////////////////////////////////////////////////////////
    void module_fini( PyObject * _module )
    {
        PYBIND_CHECK_MAIN_THREAD();

        _PyModule_Clear( _module );

        pybind::decref( _module );
    }
    //////////////////////////////////////////////////////////////////////////
#endif
    //////////////////////////////////////////////////////////////////////////
    PyObject * module_dict( PyObject * _module )
    {
        PYBIND_CHECK_MAIN_THREAD();

        return PyModule_GetDict( _module );
    }
    //////////////////////////////////////////////////////////////////////////
    void module_addobject( PyObject * _module, const char * _name, PyObject * _obj )
    {
        PYBIND_CHECK_MAIN_THREAD();

        PyModule_AddObject( _module, _name, _obj );
    }
    //////////////////////////////////////////////////////////////////////////
    void module_removeobject( PyObject * _module, const char * _name )
    {
        PYBIND_CHECK_MAIN_THREAD();

        PyObject * py_none = pybind::ret_none();

        PyModule_AddObject( _module, _name, py_none );
    }
    //////////////////////////////////////////////////////////////////////////
    bool module_hasobject( PyObject * _module, const char * _name )
    {
        PYBIND_CHECK_MAIN_THREAD();

        PyObject * dict = pybind::module_dict( _module );

        bool result = pybind::dict_existstring( dict, _name );

        return result;
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * module_execcode( const char * _name, PyObject * _code )
    {
        PYBIND_CHECK_MAIN_THREAD();

        char * unconst_name = const_cast<char *>(_name);
        PyObject * module = PyImport_ExecCodeModule( unconst_name, _code );

        pybind::check_error();

        return module;
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * module_reload( PyObject * _module )
    {
        PYBIND_CHECK_MAIN_THREAD();

        PyObject * reload_module = PyImport_ReloadModule( _module );

        pybind::check_error();

        return reload_module;
    }
    //////////////////////////////////////////////////////////////////////////
    bool code_check( PyObject * _code )
    {
        PYBIND_CHECK_MAIN_THREAD();

        return PyCode_Check( _code );
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * code_compile_file( const void * _buf, const char * _module )
    {
        PYBIND_CHECK_MAIN_THREAD();

        PyObject * code = Py_CompileString( (const char *)_buf, _module, Py_file_input );

        return code;
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * ask_native( PyObject * _obj, PyObject * _args )
    {
        PYBIND_CHECK_MAIN_THREAD();

        PyObject * result = PyObject_CallObject( _obj, _args );

        pybind::check_error();

        return result;
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * ask( PyObject * _obj, const char * _format, ... )
    {
        va_list valist;
        va_start( valist, _format );

        PyObject * result = pybind::ask_va( _obj, _format, valist );

        va_end( valist );

        return result;
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * ask_method( PyObject * _obj, const char * _method, const char * _format, ... )
    {
        va_list valist;
        va_start( valist, _format );

        PyObject * result = pybind::ask_method_va( _obj, _method, _format, valist );

        va_end( valist );

        return result;
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * ask_va( PyObject * _obj, const char * _format, va_list _va )
    {
        PyObject * value = Py_VaBuildValue( _format, _va );

        if( value == nullptr )
        {
            pybind::check_error();

            return nullptr;
        }

        PyObject * result = pybind::ask_native( _obj, value );

        pybind::decref( value );

        return result;
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * ask_method_va( PyObject * _obj, const char * _method, const char * _format, va_list _va )
    {
        PyObject * method = PyObject_GetAttrString( _obj, _method );

        if( method == nullptr )
        {
            return pybind::ret_none();
        }

        PyObject * result = pybind::ask_va( method, _format, _va );

        pybind::decref( method );

        return result;
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * ask_method_native( PyObject * _obj, const char * _method, PyObject * _args )
    {
        PyObject * method = PyObject_GetAttrString( _obj, _method );

        if( method == nullptr )
        {
            return pybind::ret_none();
        }

        PyObject * result = pybind::ask_native( method, _args );

        pybind::decref( method );

        return result;
    }
    //////////////////////////////////////////////////////////////////////////
    void call_native( PyObject * _obj, PyObject * _args )
    {
        PyObject * res = pybind::ask_native( _obj, _args );

        if( res == nullptr )
        {
            return;
        }

        pybind::decref( res );
    }
    //////////////////////////////////////////////////////////////////////////
    void call( PyObject * _obj, const char * _format, ... )
    {
        va_list valist;
        va_start( valist, _format );

        pybind::call_va( _obj, _format, valist );

        va_end( valist );
    }
    //////////////////////////////////////////////////////////////////////////
    void call_method( PyObject * _obj, const char * _method, const char * _format, ... )
    {
        va_list valist;
        va_start( valist, _format );

        pybind::call_method_va( _obj, _method, _format, valist );

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

        pybind::decref( res );
    }
    //////////////////////////////////////////////////////////////////////////
    void call_va( PyObject * _obj, const char * _format, va_list _va )
    {
        PyObject * res = pybind::ask_va( _obj, _format, _va );

        if( res == nullptr )
        {
            return;
        }

        pybind::decref( res );
    }
    //////////////////////////////////////////////////////////////////////////
    void call_method_va( PyObject * _obj, const char * _method, const char * _format, va_list _va )
    {
        PyObject * res = pybind::ask_method_va( _obj, _method, _format, _va );

        if( res == nullptr )
        {
            return;
        }

        pybind::decref( res );
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * exec_file( const char * _code, PyObject * _globals, PyObject * _locals )
    {
        PYBIND_CHECK_MAIN_THREAD();

        PyObject * result = PyRun_String( _code, Py_file_input, _globals, _locals );

        pybind::check_error();

        return result;
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * compile_string( const char * _string, const char * _file )
    {
        PYBIND_CHECK_MAIN_THREAD();

        PyObject * result = Py_CompileString( _string, _file, Py_file_input );

        pybind::check_error();

        return result;
    }
    //////////////////////////////////////////////////////////////////////////
    PYBIND_API PyObject * eval_string( const char * _string, PyObject * _globals, PyObject * _locals )
    {
        PYBIND_CHECK_MAIN_THREAD();

        PyObject * obj = PyRun_String( _string, Py_eval_input, _globals, _locals );

        pybind::check_error();

        return obj;
    }
    //////////////////////////////////////////////////////////////////////////
    void set_path( const wchar_t * _value )
    {
        PYBIND_CHECK_MAIN_THREAD();

        (void)_value;
#   if PYBIND_PYTHON_VERSION >= 300
        Py_SetPath( _value );
#   endif

        pybind::check_error();
    }
    //////////////////////////////////////////////////////////////////////////
    void set_syspath( PyObject * _value )
    {
        PYBIND_CHECK_MAIN_THREAD();

        PySys_SetObject( const_cast<char *>("path"), _value );

        pybind::check_error();
    }
    //////////////////////////////////////////////////////////////////////////
    char get_sysdelim()
    {
        return DELIM;
    }
    //////////////////////////////////////////////////////////////////////////
    void incref( PyObject * _obj )
    {
        PYBIND_CHECK_MAIN_THREAD();

        Py_INCREF( _obj );
    }
    //////////////////////////////////////////////////////////////////////////
    void decref( PyObject * _obj )
    {
        PYBIND_CHECK_MAIN_THREAD();

        Py_DECREF( _obj );
    }
    //////////////////////////////////////////////////////////////////////////
    void xincref( PyObject * _obj )
    {
        PYBIND_CHECK_MAIN_THREAD();

        Py_XINCREF( _obj );
    }
    //////////////////////////////////////////////////////////////////////////
    void xdecref( PyObject * _obj )
    {
        PYBIND_CHECK_MAIN_THREAD();

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
    bool is_none( PyObject * _obj )
    {
        return _obj == Py_None;
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * get_none()
    {
        PYBIND_CHECK_MAIN_THREAD();

        return Py_None;
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * ret_none()
    {
        PYBIND_CHECK_MAIN_THREAD();

        Py_RETURN_NONE;
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * get_true()
    {
        PYBIND_CHECK_MAIN_THREAD();

        return Py_True;
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * get_false()
    {
        PYBIND_CHECK_MAIN_THREAD();

        return Py_False;
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * ret_true()
    {
        PYBIND_CHECK_MAIN_THREAD();

        Py_RETURN_TRUE;
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * ret_false()
    {
        PYBIND_CHECK_MAIN_THREAD();

        Py_RETURN_FALSE;
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * ret_bool( bool _value )
    {
        PYBIND_CHECK_MAIN_THREAD();

        if( _value == true )
        {
            PyObject * py_true = pybind::ret_true();

            return py_true;
        }

        PyObject * py_false = pybind::ret_false();

        return py_false;
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * get_bool( bool _value )
    {
        PYBIND_CHECK_MAIN_THREAD();

        if( _value == true )
        {
            return pybind::get_true();
        }

        return pybind::get_false();
    }
    //////////////////////////////////////////////////////////////////////////
    bool has_attr( PyObject * _obj, PyObject * _attr )
    {
        PYBIND_CHECK_MAIN_THREAD();

        int value = PyObject_HasAttr( _obj, _attr );

        return value == 1;
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * get_attr( PyObject * _obj, PyObject * _attr )
    {
        PYBIND_CHECK_MAIN_THREAD();

        PyObject * obj = PyObject_GetAttr( _obj, _attr );

        return obj;
    }
    //////////////////////////////////////////////////////////////////////////
    bool has_attrstring( PyObject * _obj, const char * _attr )
    {
        PYBIND_CHECK_MAIN_THREAD();

        int value = PyObject_HasAttrString( _obj, _attr );

        return value == 1;
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * get_attrstring( PyObject * _obj, const char * _attr )
    {
        PYBIND_CHECK_MAIN_THREAD();

        PyObject * obj = PyObject_GetAttrString( _obj, _attr );

        return obj;
    }
    //////////////////////////////////////////////////////////////////////////
    bool set_attr( PyObject * _obj, PyObject * _attr, PyObject * _value )
    {
        PYBIND_CHECK_MAIN_THREAD();

        int res = PyObject_SetAttr( _obj, _attr, _value );

        if( res != 0 )
        {
            pybind::check_error();

            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool set_attrstring( PyObject * _obj, const char * _attr, PyObject * _value )
    {
        PYBIND_CHECK_MAIN_THREAD();

        int res = PyObject_SetAttrString( _obj, _attr, _value );

        if( res != 0 )
        {
            pybind::check_error();

            return false;
        }

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
#if PYBIND_PYTHON_VERSION < 300
        int result = PyInt_Check( _obj );
#else
        int result = PyLong_Check( _obj );
#endif

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
        PYBIND_CHECK_MAIN_THREAD();

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
            _value = (PyInt_AS_LONG( _obj ) != 0);
        }
#	endif
        else if( PyFloat_Check( _obj ) )
        {
            _value = (PyFloat_AS_DOUBLE( _obj ) != 0.0);
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
        PYBIND_CHECK_MAIN_THREAD();

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
            _value = (T)PyFloat_AS_DOUBLE( _obj );
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
        return pybind::extract_int_t<int8_t>( _obj, _value );
    }
    //////////////////////////////////////////////////////////////////////////
    bool extract_int16( PyObject * _obj, int16_t & _value )
    {
        return pybind::extract_int_t<int16_t>( _obj, _value );
    }
    //////////////////////////////////////////////////////////////////////////
    bool extract_int32( PyObject * _obj, int32_t & _value )
    {
        return pybind::extract_int_t<int32_t>( _obj, _value );
    }
    //////////////////////////////////////////////////////////////////////////
    bool extract_int64( PyObject * _obj, int64_t & _value )
    {
        return pybind::extract_int_t<int64_t>( _obj, _value );
    }
    //////////////////////////////////////////////////////////////////////////
    template<class T>
    static bool extract_unsigned_int_t( PyObject * _obj, T & _value )
    {
        PYBIND_CHECK_MAIN_THREAD();

        if( _obj == nullptr )
        {
            return false;
        }

#   if PYBIND_PYTHON_VERSION < 300
        if( PyInt_Check( _obj ) )
        {
            long value = PyInt_AS_LONG( _obj );

            _value = (T)value;
        }
        else
#	endif
            if( PyLong_Check( _obj ) )
            {
                unsigned PY_LONG_LONG value = PyLong_AsUnsignedLongLong( _obj );

                _value = (T)value;
            }
            else if( PyFloat_Check( _obj ) )
            {
                double value = PyFloat_AS_DOUBLE( _obj );

                _value = (T)value;
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
        return pybind::extract_unsigned_int_t<uint8_t>( _obj, _value );
    }
    //////////////////////////////////////////////////////////////////////////
    bool extract_uint16( PyObject * _obj, uint16_t & _value )
    {
        return pybind::extract_unsigned_int_t<uint16_t>( _obj, _value );
    }
    //////////////////////////////////////////////////////////////////////////
    bool extract_uint32( PyObject * _obj, uint32_t & _value )
    {
        return pybind::extract_unsigned_int_t<uint32_t>( _obj, _value );
    }
    //////////////////////////////////////////////////////////////////////////
    bool extract_uint64( PyObject * _obj, uint64_t & _value )
    {
        return pybind::extract_unsigned_int_t<uint64_t>( _obj, _value );
    }
    //////////////////////////////////////////////////////////////////////////
    template<class T>
    static bool extract_float_t( PyObject * _obj, T & _value )
    {
        PYBIND_CHECK_MAIN_THREAD();

        if( _obj == nullptr )
        {
            return false;
        }

        if( PyFloat_Check( _obj ) )
        {
            _value = (T)PyFloat_AS_DOUBLE( _obj );
        }
#   if PYBIND_PYTHON_VERSION < 300
        else if( PyInt_Check( _obj ) )
        {
            _value = (T)PyInt_AS_LONG( _obj );
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
        return pybind::extract_float_t<float>( _obj, _value );
    }
    //////////////////////////////////////////////////////////////////////////
    bool extract_double( PyObject * _obj, double & _value )
    {
        return pybind::extract_float_t<double>( _obj, _value );
    }
    //////////////////////////////////////////////////////////////////////////
    bool extract_wchar( PyObject * _obj, wchar_t & _value )
    {
        PYBIND_CHECK_MAIN_THREAD();

        if( _obj == nullptr )
        {
            return false;
        }

        if( PyUnicode_CheckExact( _obj ) == 1 )
        {
            const wchar_t * ch_buff = PyUnicode_AsUnicode( _obj );

            if( ch_buff == nullptr )
            {
                return false;
            }

#if PYBIND_PYTHON_VERSION < 300
            Py_ssize_t sz = PyUnicode_GET_SIZE( _obj );
#else
            Py_ssize_t sz = PyUnicode_GET_LENGTH( _obj );
#endif

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
        PYBIND_CHECK_MAIN_THREAD();

#   if PYBIND_PYTHON_VERSION < 300
        return PyInt_FromLong( _value );
#	else
        return PyLong_FromLong( _value );
#	endif
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * ptr_int8( int8_t _value )
    {
        return pybind::ptr_int_t<int8_t>( _value );
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * ptr_int16( int16_t _value )
    {
        return pybind::ptr_int_t<int16_t>( _value );
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * ptr_int32( int32_t _value )
    {
        return pybind::ptr_int_t<int32_t>( _value );
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * ptr_int64( int64_t _value )
    {
        PYBIND_CHECK_MAIN_THREAD();

        return PyLong_FromLongLong( _value );
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * ptr_uint8( uint8_t _value )
    {
        return pybind::ptr_int_t<uint8_t>( _value );
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * ptr_uint16( uint16_t _value )
    {
        return pybind::ptr_int_t<uint16_t>( _value );
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * ptr_uint32( uint32_t _value )
    {
        return pybind::ptr_int_t<uint32_t>( _value );
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * ptr_uint64( uint64_t _value )
    {
        PYBIND_CHECK_MAIN_THREAD();

        return PyLong_FromUnsignedLongLong( _value );
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * ptr_float( float _value )
    {
        PYBIND_CHECK_MAIN_THREAD();

        return PyFloat_FromDouble( _value );
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * ptr_double( double _value )
    {
        PYBIND_CHECK_MAIN_THREAD();

        return PyFloat_FromDouble( _value );
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * ptr_long( long _value )
    {
        PYBIND_CHECK_MAIN_THREAD();

        return PyLong_FromLong( _value );
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * ptr_ulong( unsigned long _value )
    {
        PYBIND_CHECK_MAIN_THREAD();

        return PyLong_FromUnsignedLong( _value );
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * list_new( uint32_t _size )
    {
        PYBIND_CHECK_MAIN_THREAD();

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
        PYBIND_CHECK_MAIN_THREAD();

        Py_ssize_t size = PyList_Size( _obj );

        if( size == -1 )
        {
            pybind::check_error();

            return 0;
        }

        return (uint32_t)size;
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * list_getitem( PyObject * _obj, uint32_t _index )
    {
        PYBIND_CHECK_MAIN_THREAD();

        Py_ssize_t py_index = (Py_ssize_t)_index;

        PyObject * obj = PyList_GetItem( _obj, py_index );

        if( obj == nullptr )
        {
            pybind::check_error();
        }

        return obj;
    }
    //////////////////////////////////////////////////////////////////////////
    bool list_insert( PyObject * _obj, uint32_t _index, PyObject * _item )
    {
        PYBIND_CHECK_MAIN_THREAD();

        Py_ssize_t py_index = (Py_ssize_t)_index;

        int res = PyList_Insert( _obj, py_index, _item );

        if( res != 0 )
        {
            pybind::check_error();

            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool list_remove( PyObject * _obj, uint32_t _it )
    {
        PYBIND_CHECK_MAIN_THREAD();

        int res = PyList_SetSlice( _obj, _it, _it + 1, nullptr );

        if( res != 0 )
        {
            pybind::check_error();

            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool list_setitem( PyObject * _obj, uint32_t _index, PyObject * _item )
    {
        PYBIND_CHECK_MAIN_THREAD();

        pybind::incref( _item );

        Py_ssize_t py_index = (Py_ssize_t)_index;

        int res = PyList_SetItem( _obj, py_index, _item );

        if( res != 0 )
        {
            pybind::check_error();

            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool list_appenditem( PyObject * _obj, PyObject * _item )
    {
        PYBIND_CHECK_MAIN_THREAD();

        int res = PyList_Append( _obj, _item );

        if( res == -1 )
        {
            pybind::check_error();

            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * dict_new()
    {
        PYBIND_CHECK_MAIN_THREAD();

        PyObject * obj = PyDict_New();

        return obj;
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * dict_new_presized( uint32_t _count )
    {
        PYBIND_CHECK_MAIN_THREAD();

        PyObject * obj = _PyDict_NewPresized( _count );

        return obj;
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * dict_from( PyObject * _obj )
    {
        PYBIND_CHECK_MAIN_THREAD();

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

            pybind::decref( py_value );
        }

        pybind::decref( py_dir );

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
        PYBIND_CHECK_MAIN_THREAD();

        Py_ssize_t size = PyDict_Size( _obj );

        return (uint32_t)size;
    }
    //////////////////////////////////////////////////////////////////////////
    bool dict_setstring( PyObject * _dict, const char * _name, PyObject * _value )
    {
        PYBIND_CHECK_MAIN_THREAD();

        int res = PyDict_SetItemString( _dict, _name, _value );

        if( res == -1 )
        {
            pybind::check_error();

            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool dict_set( PyObject * _dict, PyObject * _name, PyObject * _value )
    {
        PYBIND_CHECK_MAIN_THREAD();

        int res = PyDict_SetItem( _dict, _name, _value );

        if( res == -1 )
        {
            pybind::check_error();

            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool dict_removestring( PyObject * _dict, const char * _key )
    {
        PYBIND_CHECK_MAIN_THREAD();

        int res = PyDict_DelItemString( _dict, _key );

        if( res == -1 )
        {
            pybind::check_error();

            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool dict_remove( PyObject * _dict, PyObject * _key )
    {
        PYBIND_CHECK_MAIN_THREAD();

        int res = PyDict_DelItem( _dict, _key );

        if( res == -1 )
        {
            pybind::check_error();

            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * dict_getstring( PyObject * _dict, const char * _key )
    {
        PYBIND_CHECK_MAIN_THREAD();

        PyObject * obj = PyDict_GetItemString( _dict, _key );

        return obj;
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * dict_get( PyObject * _dict, PyObject * _key )
    {
        PYBIND_CHECK_MAIN_THREAD();

        PyObject * obj = PyDict_GetItem( _dict, _key );

        return obj;
    }
    //////////////////////////////////////////////////////////////////////////
    bool dict_exist( PyObject * _dict, PyObject * _key )
    {
        PYBIND_CHECK_MAIN_THREAD();

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
        PYBIND_CHECK_MAIN_THREAD();

        PyObject * kv = pybind::string_from_char( _key );

        bool result = pybind::dict_exist( _dict, kv );

        pybind::decref( kv );

        return result;
    }
    //////////////////////////////////////////////////////////////////////////
    bool dict_next( PyObject * _dict, uint32_t & _pos, PyObject ** _key, PyObject ** _value )
    {
        PYBIND_CHECK_MAIN_THREAD();

        Py_ssize_t ps = (Py_ssize_t)(_pos);

        int res = PyDict_Next( _dict, &ps, _key, _value );

        _pos = (uint32_t)ps;

        return res == 1;
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * dict_items( PyObject * _dict )
    {
        PYBIND_CHECK_MAIN_THREAD();

        PyObject * obj = PyDict_Items( _dict );

        return obj;
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * tuple_new( uint32_t _size )
    {
        PYBIND_CHECK_MAIN_THREAD();

        Py_ssize_t py_size = (Py_ssize_t)_size;
        PyObject * obj = PyTuple_New( py_size );

        return obj;
    }
    //////////////////////////////////////////////////////////////////////////
    bool tuple_setitem( PyObject * _tuple, uint32_t _index, PyObject * _value )
    {
        PYBIND_CHECK_MAIN_THREAD();

        if( _value == nullptr )
        {
            return false;
        }

        pybind::incref( _value );

        Py_ssize_t py_index = (Py_ssize_t)_index;

        int res = PyTuple_SetItem( _tuple, py_index, _value );

        if( res != 0 )
        {
            pybind::check_error();

            return false;
        }

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
        PYBIND_CHECK_MAIN_THREAD();

        Py_ssize_t size = PyTuple_Size( _obj );

        return (uint32_t)size;
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * tuple_getitem( PyObject * _obj, uint32_t _index )
    {
        PYBIND_CHECK_MAIN_THREAD();

        Py_ssize_t py_index = (Py_ssize_t)_index;
        PyObject * obj = PyTuple_GetItem( _obj, py_index );

        return obj;
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * tuple_slice( PyObject * _obj, uint32_t _low, uint32_t _high )
    {
        PYBIND_CHECK_MAIN_THREAD();

        Py_ssize_t py_low = (Py_ssize_t)_low;
        Py_ssize_t py_high = (Py_ssize_t)_high;
        PyObject * obj_slice = PyTuple_GetSlice( _obj, py_low, py_high );

        return obj_slice;
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * tuple_slice_tail( PyObject * _obj, uint32_t _size )
    {
        PYBIND_CHECK_MAIN_THREAD();

        uint32_t size = pybind::tuple_size( _obj );

        PyObject * obj_slice = pybind::tuple_slice( _obj, _size, size );

        return obj_slice;
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * object_dir( PyObject * _obj )
    {
        PYBIND_CHECK_MAIN_THREAD();

        PyObject * py_dir = PyObject_Dir( _obj );

        return py_dir;
    }
    //////////////////////////////////////////////////////////////////////////
    PyTypeObject * object_type( PyObject * _obj )
    {
        PYBIND_CHECK_MAIN_THREAD();

        PyTypeObject * type = Py_TYPE( _obj );

        return type;
    }
    //////////////////////////////////////////////////////////////////////////
    const char * object_type_name( PyObject * _obj )
    {
        PYBIND_CHECK_MAIN_THREAD();

        PyTypeObject * type = Py_TYPE( _obj );

        const char * name = type->tp_name;

        return name;
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * object_repr_type( PyObject * _obj )
    {
        PYBIND_CHECK_MAIN_THREAD();

        if( _obj == nullptr )
        {
            return nullptr;
        }

        PyTypeObject * py_type = pybind::object_type( _obj );

        PyObject * py_repr = PyObject_Repr( (PyObject *)py_type );

        if( py_repr == nullptr )
        {
            return nullptr;
        }

        return py_repr;
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * object_repr( PyObject * _obj )
    {
        PYBIND_CHECK_MAIN_THREAD();

        if( _obj == nullptr )
        {
            return nullptr;
        }

        PyObject * py_repr = PyObject_Repr( _obj );

        if( py_repr == nullptr )
        {
            return nullptr;
        }

        return py_repr;
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * object_str( PyObject * _obj )
    {
        PYBIND_CHECK_MAIN_THREAD();

        if( _obj == nullptr )
        {
            return nullptr;
        }

        PyObject * py_str = PyObject_Str( _obj );

        if( py_str == nullptr )
        {
            return nullptr;
        }

        return py_str;
    }
    //////////////////////////////////////////////////////////////////////////
    int64_t object_hash( PyObject * _obj )
    {
        PYBIND_CHECK_MAIN_THREAD();

        int64_t py_hash = (int64_t)PyObject_Hash( _obj );

        return py_hash;
    }
    //////////////////////////////////////////////////////////////////////////
    bool get_statetrace_top( char * const _filename, size_t _maxlenfilename, char * const _function, size_t _maxlenfunction, uint32_t * _lineno )
    {
        PYBIND_CHECK_MAIN_THREAD();

        PyThreadState * thread_state = PyThreadState_GET();

        if( thread_state == nullptr )
        {
            return false;
        }

        PyFrameObject * frame = thread_state->frame;

        if( frame == nullptr )
        {
            return false;
        }

        while( frame->f_back != nullptr )
        {
            frame = frame->f_back;
        }

        const char * co_filename = pybind::string_to_char( frame->f_code->co_filename );
        const char * co_name = pybind::string_to_char( frame->f_code->co_name );

        strncpy( _filename, co_filename, _maxlenfilename );
        strncpy( _function, co_name, _maxlenfunction );

        *_lineno = frame->f_lineno;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool get_statetrace( char * const _buffer, size_t _maxlen )
    {
        PYBIND_CHECK_MAIN_THREAD();

        PyThreadState * thread_state = PyThreadState_GET();

        if( thread_state == nullptr )
        {
            return false;
        }

        PyFrameObject * frame = thread_state->frame;

        if( frame == nullptr )
        {
            return false;
        }

        strncpy( _buffer, "", _maxlen );

        if( _maxlen == 0 )
        {
            return true;
        }

        PyFrameObject * frames[1024] = {nullptr};
        uint32_t frame_count = 0;

        while( frame != nullptr && frame_count < 1024 )
        {
            frames[frame_count++] = frame;
            frame = frame->f_back;
        }

        for( uint32_t index = frame_count; index != 0; index-- )
        {
            PyFrameObject * f = frames[index - 1];
            
            const char * co_filename = pybind::string_to_char( f->f_code->co_filename );
            const char * co_name = pybind::string_to_char( f->f_code->co_name );
            int lineno = f->f_lineno;

            char line[1024 + 1] = {'\0'};
            snprintf( line, 1024, "File \"%s\", line %d, in %s\n", co_filename, lineno, co_name );

            strncat( _buffer, line, _maxlen );
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void error_traceback( const char * _format, ... )
    {
        PYBIND_CHECK_MAIN_THREAD();

        va_list valist;
        va_start( valist, _format );

        pybind::error_traceback_va( _format, valist );

        va_end( valist );
    }
    //////////////////////////////////////////////////////////////////////////
    void error_traceback_va( const char * _format, va_list _va )
    {
        PYBIND_CHECK_MAIN_THREAD();

        pybind::check_error();

        char buffer[4096] = {'\0'};
        vsnprintf( buffer, 4096, _format, _va );

        PyThreadState * thread_state = PyThreadState_GET();

        if( thread_state->frame == nullptr )
        {
            return;
        }

        PyObject * py_filename = thread_state->frame->f_code->co_filename;
        int fileline = thread_state->frame->f_lineno;

        const char * str_filename = pybind::string_to_char( py_filename );

        char trace_buffer[4096 + 256 + 1] = {'\0'};
        snprintf( trace_buffer, 4096 + 256, "%s[%d]: %s"
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
        PYBIND_CHECK_MAIN_THREAD();

        va_list valist;
        va_start( valist, _format );
        pybind::error_message_va( _format, valist );
        va_end( valist );
    }
    //////////////////////////////////////////////////////////////////////////
    void error_message_va( const char * _format, va_list _va )
    {
        PYBIND_CHECK_MAIN_THREAD();

        pybind::check_error();

        char buffer[2048] = {'\0'};
        vsprintf( buffer, _format, _va );

        PyErr_SetString( PyExc_RuntimeError, buffer );
    }
    //////////////////////////////////////////////////////////////////////////
    void throw_message( const char * _format, ... )
    {
        PYBIND_CHECK_MAIN_THREAD();

        va_list valist;
        va_start( valist, _format );
        pybind::throw_message_va( _format, valist );
        va_end( valist );
    }
    //////////////////////////////////////////////////////////////////////////
    void throw_message_va( const char * _format, va_list _va )
    {
        PYBIND_CHECK_MAIN_THREAD();

        pybind::error_message_va( _format, _va );
        pybind::throw_exception_va( _format, _va );
    }
    //////////////////////////////////////////////////////////////////////////
    void warning_traceback( const char * _format, ... )
    {
        PYBIND_CHECK_MAIN_THREAD();

        va_list valist;
        va_start( valist, _format );

        pybind::warning_traceback_va( _format, valist );
        va_end( valist );
    }
    //////////////////////////////////////////////////////////////////////////
    void warning_traceback_va( const char * _format, va_list _va )
    {
        PYBIND_CHECK_MAIN_THREAD();

        pybind::check_error();

        char buffer[4096] = {'\0'};
        vsprintf( buffer, _format, _va );

        PyThreadState * thread_state = PyThreadState_GET();

        if( thread_state->frame == nullptr )
        {
            return;
        }

        PyObject * py_filename = thread_state->frame->f_code->co_filename;
        int fileline = thread_state->frame->f_lineno;

        const char * str_filename = pybind::string_to_char( py_filename );

        char trace_buffer[4096 + 256] = {'\0'};
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
        PYBIND_CHECK_MAIN_THREAD();

        PyErr_Clear();
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * build_value( const char * _format, ... )
    {
        PYBIND_CHECK_MAIN_THREAD();

        va_list valist;
        va_start( valist, _format );

        PyObject * value = build_value_va( _format, valist );

        va_end( valist );

        return value;
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * build_value_va( const char * _format, va_list _va )
    {
        PYBIND_CHECK_MAIN_THREAD();

        PyObject * value = Py_VaBuildValue( _format, _va );

        if( value == nullptr )
        {
            pybind::check_error();

            return nullptr;
        }

        return value;
    }
    //////////////////////////////////////////////////////////////////////////
    bool traceback_check( PyObject * _traceback )
    {
        int result = PyTraceBack_Check( _traceback );

        return result == 1;
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * traceback_filename( PyObject * _traceback )
    {
        PYBIND_CHECK_MAIN_THREAD();

        PyTracebackObject * tb = (PyTracebackObject *)_traceback;

        PyObject * filename = tb->tb_frame->f_code->co_filename;

        return filename;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t traceback_lineno( PyObject * _traceback )
    {
        PYBIND_CHECK_MAIN_THREAD();
        
        PyTracebackObject * tb = (PyTracebackObject *)_traceback;

        uint32_t lineno = tb->tb_lineno;

        return lineno;
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * traceback_function( PyObject * _traceback )
    {
        PYBIND_CHECK_MAIN_THREAD();

        PyTracebackObject * tb = (PyTracebackObject *)_traceback;

        PyObject * function = tb->tb_frame->f_code->co_name;

        return function;
    }
    //////////////////////////////////////////////////////////////////////////
    PYBIND_API PyObject * traceback_next( PyObject * _traceback )
    {
        PYBIND_CHECK_MAIN_THREAD();

        PyTracebackObject * tb = (PyTracebackObject *)_traceback;

        PyTracebackObject * next = tb->tb_next;

        return (PyObject *)next;
    }
    //////////////////////////////////////////////////////////////////////////
    int64_t type_hash( PyTypeObject * _obj )
    {
        PYBIND_CHECK_MAIN_THREAD();

        int64_t py_hash = (int64_t)PyObject_Hash( (PyObject *)_obj );

        return py_hash;
    }
    //////////////////////////////////////////////////////////////////////////
    const char * type_name( PyTypeObject * _obj )
    {
        PYBIND_CHECK_MAIN_THREAD();

        const char * name = _obj->tp_name;

        return name;
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
        PYBIND_CHECK_MAIN_THREAD();

        int value = PyObject_RichCompareBool( _left, _right, Py_EQ );

        return value == 1;
    }
    //////////////////////////////////////////////////////////////////////////
    bool is_true( PyObject * _obj )
    {
        return _obj == Py_True;
    }
    //////////////////////////////////////////////////////////////////////////
    bool is_false( PyObject * _obj )
    {
        return _obj == Py_False;
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
        PYBIND_CHECK_MAIN_THREAD();

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
        PYBIND_CHECK_MAIN_THREAD();

        char * ch_buff_unconst = PyBytes_AsString( _string );

        const char * ch_buff = const_cast<const char *>(ch_buff_unconst);

        return ch_buff;
    }
    //////////////////////////////////////////////////////////////////////////
    const char * string_to_char_and_size( PyObject * _string, size_t * _size )
    {
        PYBIND_CHECK_MAIN_THREAD();

        char * ch_buff_unconst;
        Py_ssize_t len;
        if( PyBytes_AsStringAndSize( _string, &ch_buff_unconst, &len ) == -1 )
        {
            *_size = 0;

            return nullptr;
        }

        *_size = (size_t)len;

        const char * ch_buff = const_cast<const char *>(ch_buff_unconst);

        return ch_buff;
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * string_from_char( const char * _str )
    {
        PYBIND_CHECK_MAIN_THREAD();

        PyObject * py_str = PyBytes_FromString( _str );

        return py_str;
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * string_from_char_size( const char * _str, size_t _size )
    {
        PYBIND_CHECK_MAIN_THREAD();

        Py_ssize_t py_size = (Py_ssize_t)_size;
        PyObject * py_str = PyBytes_FromStringAndSize( _str, py_size );

        return py_str;
    }
    //////////////////////////////////////////////////////////////////////////
    bool unicode_check( PyObject * _unicode )
    {
        PYBIND_CHECK_MAIN_THREAD();

        bool value = PyUnicode_CheckExact( _unicode );

        return value;
    }
    //////////////////////////////////////////////////////////////////////////
    const wchar_t * unicode_to_wchar( PyObject * _unicode )
    {
        PYBIND_CHECK_MAIN_THREAD();

        const wchar_t * wstr = PyUnicode_AsUnicode( _unicode );

        return wstr;
    }
    //////////////////////////////////////////////////////////////////////////
    const wchar_t * unicode_to_wchar_and_size( PyObject * _unicode, size_t * _size )
    {
        PYBIND_CHECK_MAIN_THREAD();

        const wchar_t * wstr = PyUnicode_AsUnicode( _unicode );

#if PYBIND_PYTHON_VERSION < 300
        Py_ssize_t py_size = PyUnicode_GET_SIZE( _unicode );
#else
        Py_ssize_t py_size = PyUnicode_GET_LENGTH( _unicode );
#endif

        * _size = (size_t)py_size;

        return wstr;
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * unicode_from_wchar( const wchar_t * _value )
    {
        PYBIND_CHECK_MAIN_THREAD();

        size_t size = wcslen( _value );

        Py_ssize_t py_size = (Py_ssize_t)size;
        PyObject * py_unicode = PyUnicode_FromWideChar( _value, py_size );

        return py_unicode;
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * unicode_from_wchar_size( const wchar_t * _value, size_t _size )
    {
        PYBIND_CHECK_MAIN_THREAD();

        Py_ssize_t py_size = (Py_ssize_t)_size;
        PyObject * py_unicode = PyUnicode_FromWideChar( _value, py_size );

        return py_unicode;
    }
    //////////////////////////////////////////////////////////////////////////
    const char * unicode_to_utf8( PyObject * _unicode )
    {
        PYBIND_CHECK_MAIN_THREAD();

        PyObject * py_utf8 = _unicode_to_utf8_obj( _unicode );

        const char * ch_buff = pybind::string_to_char( py_utf8 );

        return ch_buff;
    }
    //////////////////////////////////////////////////////////////////////////
    const char * unicode_to_utf8_and_size( PyObject * _unicode, size_t * _size )
    {
        PYBIND_CHECK_MAIN_THREAD();

        PyObject * py_utf8 = PyUnicode_AsUTF8String( _unicode );

        const char * ch_buff = pybind::string_to_char_and_size( py_utf8, _size );

        return ch_buff;
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * unicode_from_utf8( const char * _utf8 )
    {
        PYBIND_CHECK_MAIN_THREAD();

        PyObject * unicode = PyUnicode_FromString( _utf8 );

        return unicode;
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * unicode_from_utf8_size( const char * _utf8, size_t _size )
    {
        PYBIND_CHECK_MAIN_THREAD();

        Py_ssize_t py_size = (Py_ssize_t)_size;
        PyObject * unicode = PyUnicode_FromStringAndSize( _utf8, py_size );

        return unicode;
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * _unicode_to_utf8_obj( PyObject * _unicode )
    {
        PYBIND_CHECK_MAIN_THREAD();

        PyObject * py_utf8 = PyUnicode_AsUTF8String( _unicode );

        return py_utf8;
    }
    //////////////////////////////////////////////////////////////////////////
    bool void_ptr_check( PyObject * _obj )
    {
        PYBIND_CHECK_MAIN_THREAD();

        return PyCapsule_CheckExact( _obj );
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * void_ptr_new( void * _impl )
    {
        PYBIND_CHECK_MAIN_THREAD();

        PyObject * py_void = PyCapsule_New( _impl, nullptr, nullptr );

        return py_void;
    }
    //////////////////////////////////////////////////////////////////////////
    void * void_ptr_get( PyObject * _obj )
    {
        PYBIND_CHECK_MAIN_THREAD();

        void * impl = PyCapsule_GetPointer( _obj, nullptr );

        return impl;
    }
    //////////////////////////////////////////////////////////////////////////
    long marshal_magic_number()
    {
        PYBIND_CHECK_MAIN_THREAD();

        long py_magic = PyImport_GetMagicNumber();

        return py_magic;
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * marshal_get_object( const void * _buf, size_t _len )
    {
        PYBIND_CHECK_MAIN_THREAD();

        Py_ssize_t py_len = (Py_ssize_t)_len;

        char * buf_unconst = const_cast<char *>(static_cast<const char *>(_buf));
        PyObject * obj = PyMarshal_ReadObjectFromString( buf_unconst, py_len );

        return obj;
    }
    //////////////////////////////////////////////////////////////////////////
    void set_module_finder( PyObject * _finder )
    {
        (void)_finder;

        PYBIND_CHECK_MAIN_THREAD();

#if PYBIND_PYTHON_VERSION < 300
        PyObject * py_meta_path = PySys_GetObject( const_cast<char *>("meta_path") );

        pybind::list_insert( py_meta_path, 0, _finder );
#endif

#if PYBIND_PYTHON_VERSION >= 330 && PYBIND_PYTHON_VERSION < 380
        PyThreadState * thread_state = PyThreadState_Get();
        PyInterpreterState * interp = thread_state->interp;
        interp->importlib = _finder;
#endif
    }
    //////////////////////////////////////////////////////////////////////////
    void remove_module_finder()
    {
        PYBIND_CHECK_MAIN_THREAD();

#if PYBIND_PYTHON_VERSION < 300
        PyObject * py_meta_path = PySys_GetObject( const_cast<char *>("meta_path") );

        pybind::list_remove( py_meta_path, 0 );
#endif

#if PYBIND_PYTHON_VERSION >= 330 && PYBIND_PYTHON_VERSION < 380
        PyThreadState * thread_state = PyThreadState_Get();
        PyInterpreterState * interp = thread_state->interp;
        interp->importlib = nullptr;
#endif
    }
    //////////////////////////////////////////////////////////////////////////
}

