#	pragma once

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
	PYBIND_API void finalize();

	PYBIND_API void check_error();

	PYBIND_API PyObject * module_import( const char * _name );
	PYBIND_API PyObject * module_init( const char * _name );
	PYBIND_API PyObject * module_dict( PyObject * _module );
	PYBIND_API void module_addobject( PyObject * _module, const char * _name, PyObject * _obj );

	PYBIND_API void set_currentmodule( PyObject * _module );
	PYBIND_API PyObject * get_currentmodule();

	PYBIND_API PyObject * call( PyObject * _obj, const char * _format, ... );
	PYBIND_API PyObject * call_method( PyObject * _obj, const char * _method, const char * _format, ... );
	
	PYBIND_API PyObject * call_va( PyObject * _obj, const char * _format, va_list );
	PYBIND_API PyObject * call_method_va( PyObject * _obj, const char * _method, const char * _format, va_list );
	
	PYBIND_API PyObject * exec( const char * _code, PyObject * _global = 0, PyObject * _local = 0 );
	PYBIND_API PyObject * exec_file( const char * _filename, PyObject * _global = 0, PyObject * _local = 0 );

	PYBIND_API void set_syspath( const char * _path );

	PYBIND_API void incref( PyObject * _obj );
	PYBIND_API void decref( PyObject * _obj );
	PYBIND_API size_t refcount( PyObject * _obj );

	PYBIND_API PyObject * ret_none();
	PYBIND_API PyObject * ret_true();
	PYBIND_API PyObject * ret_false();

	PYBIND_API bool has_attr( PyObject * _obj, const char * _attr );
	PYBIND_API PyObject * get_attr( PyObject * _obj, const char * _attr );

	PYBIND_API bool check_type( PyObject * _obj );
	
	PYBIND_API bool list_check( PyObject * _obj );
	PYBIND_API size_t list_size( PyObject * _obj );
	PYBIND_API PyObject * list_getitem( PyObject * _obj, size_t _it );

	PYBIND_API bool dict_check( PyObject * _dict );
	PYBIND_API bool dict_next( PyObject * _dict, size_t *_pos, PyObject ** _key, PyObject ** _value );

	PYBIND_API bool tuple_check( PyObject * _tuple );
	PYBIND_API size_t tuple_size( PyObject * _tuple );
	PYBIND_API PyObject * tuple_getitem( PyObject * _tuple, size_t _it );

	PYBIND_API void exception_filter();

	namespace convert
	{
		PYBIND_API bool to_bool( PyObject * _bool );

		PYBIND_API bool is_string( PyObject * _string );
		PYBIND_API const char * to_string( PyObject * _string );
	}
}