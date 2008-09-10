#	pragma once

#	include <cstddef>

using std::size_t;

#	include "exports.hpp"

#	include <stdarg.h>

extern "C" 
{ 
	struct _object;
	typedef _object PyObject;
}

namespace pybind
{
	PYBIND_API void initialize();
	PYBIND_API void initialize_ts();
	PYBIND_API void finalize();

	PYBIND_API bool is_initialized();

	PYBIND_API void check_error();

	PYBIND_API void setStdErrorHandle( PyObject * _handle );
	PYBIND_API void setStdOutHandle( PyObject * _handle );
	PYBIND_API PyObject * module_import( const char * _name );
	PYBIND_API PyObject * module_init( const char * _name );
	PYBIND_API PyObject * module_dict( PyObject * _module );
	PYBIND_API void module_addobject( PyObject * _module, const char * _name, PyObject * _obj );

	PYBIND_API void set_currentmodule( PyObject * _module );
	PYBIND_API PyObject * get_currentmodule();

	PYBIND_API PyObject * ask( PyObject * _obj, const char * _format, ... );
	PYBIND_API PyObject * ask_ne( PyObject * _obj, const char * _format, ... );
	PYBIND_API PyObject * ask_ts( PyObject * _obj, const char * _format, ... );

	PYBIND_API PyObject * ask_method( PyObject * _obj, const char * _method, const char * _format, ... );
	PYBIND_API PyObject * ask_method_ts( PyObject * _obj, const char * _method, const char * _format, ... );

	PYBIND_API PyObject * ask_va( PyObject * _obj, const char * _format, va_list );
	PYBIND_API PyObject * ask_method_va( PyObject * _obj, const char * _method, const char * _format, va_list );

	PYBIND_API void call( PyObject * _obj, const char * _format, ... );
	PYBIND_API void call_ne( PyObject * _obj, const char * _format, ... );
	PYBIND_API void call_ts( PyObject * _obj, const char * _format, ... );

	PYBIND_API void call_method( PyObject * _obj, const char * _method, const char * _format, ... );
	PYBIND_API void call_method_ts( PyObject * _obj, const char * _method, const char * _format, ... );
	
	PYBIND_API void call_va( PyObject * _obj, const char * _format, va_list );
	PYBIND_API void call_method_va( PyObject * _obj, const char * _method, const char * _format, va_list );
		
	PYBIND_API PyObject * compile_string( const char * _string, const char * _file );
	PYBIND_API PyObject * exec( const char * _code, PyObject * _global = 0, PyObject * _local = 0 );
	PYBIND_API PyObject * exec_file( const char * _filename, PyObject * _global = 0, PyObject * _local = 0 );

	PYBIND_API const char * get_syspath();
	PYBIND_API void set_syspath( const char * _path );

	PYBIND_API void incref( PyObject * _obj );
	PYBIND_API void decref( PyObject * _obj );
	PYBIND_API size_t refcount( PyObject * _obj );

	PYBIND_API PyObject * ret_none();
	PYBIND_API PyObject * ret_true();
	PYBIND_API PyObject * ret_false();

	PYBIND_API bool has_attr( PyObject * _obj, const char * _attr );
	PYBIND_API PyObject * get_attr( PyObject * _obj, const char * _attr );
	PYBIND_API bool set_attr( PyObject * _obj, const char * _attr , PyObject * _value);

	PYBIND_API bool check_type( PyObject * _obj );
	
	PYBIND_API bool list_check( PyObject * _obj );
	PYBIND_API size_t list_size( PyObject * _obj );
	PYBIND_API PyObject * list_getitem( PyObject * _obj, size_t _it );
	PYBIND_API int list_appenditem( PyObject * _obj, PyObject * _item );

	PYBIND_API bool dict_check( PyObject * _dict );
	PYBIND_API bool dict_set( PyObject * _dict, const char * _name, PyObject * _value );
	PYBIND_API bool dict_next( PyObject * _dict, size_t *_pos, PyObject ** _key, PyObject ** _value );

	PYBIND_API PyObject * tuple_new( size_t _it );
	PYBIND_API bool tuple_check( PyObject * _tuple );
	PYBIND_API size_t tuple_size( PyObject * _tuple );
	PYBIND_API PyObject * tuple_getitem( PyObject * _tuple, size_t _it );
	PYBIND_API bool tuple_setitem( PyObject * _tuple, size_t _it, PyObject * _value );
	PYBIND_API bool tuple_resize( PyObject ** _ptuple, size_t _it );

	PYBIND_API void error_message( const char * _message, ... );
	PYBIND_API PyObject * build_value( const char * _format, ... );

	PYBIND_API void exception_filter();

	namespace convert
	{
		PYBIND_API bool is_none( PyObject * _none );

		PYBIND_API bool is_bool( PyObject * _bool );
		PYBIND_API bool to_bool( PyObject * _bool );

		PYBIND_API bool is_string( PyObject * _string );
		PYBIND_API const char * to_string( PyObject * _string );
		PYBIND_API const wchar_t * to_unicode( PyObject * _unicode );
	}
}

