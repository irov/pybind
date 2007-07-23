#	pragma once

#	include <stdarg.h>

extern "C" 
{ 
	struct _object;
	typedef _object PyObject;
}

namespace pybind
{
	void initialize();
	void finalize();

	void check_error();

	PyObject * module_import( const char * _name );
	PyObject * module_init( const char * _name );
	PyObject * module_dict( PyObject * _module );
	void module_addobject( PyObject * _module, const char * _name, PyObject * _obj );

	void set_currentmodule( PyObject * _module );
	PyObject * get_currentmodule();

	PyObject * call( PyObject * _obj, const char * _format, ... );
	PyObject * call_method( PyObject * _obj, const char * _method, const char * _format, ... );
	
	PyObject * call_va( PyObject * _obj, const char * _format, va_list );
	PyObject * call_method_va( PyObject * _obj, const char * _method, const char * _format, va_list );
	
	

	PyObject * exec( const char * _code, PyObject * _global, PyObject * _local );
	PyObject * exec_file( const char * _filename, PyObject * _global, PyObject * _local );

	void set_syspath( const char * _path );

	void incref( PyObject * _obj );
	void decref( PyObject * _obj );

	bool has_attr( PyObject * _obj, const char * _attr );
	PyObject * get_attr( PyObject * _obj, const char * _attr );

	bool check_type( PyObject * _obj );
	
	
	bool list_check( PyObject * _obj );
	size_t list_size( PyObject * _obj );
	PyObject * list_getitem( PyObject * _obj, size_t _it );

	bool dict_check( PyObject * _obj );
	bool dict_next( PyObject * _dict, size_t *_pos, PyObject ** _key, PyObject ** _value );

	void exception_filter();
}