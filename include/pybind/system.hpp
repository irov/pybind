#	pragma once

#	include "exports.hpp"
#	include "types.hpp"

#	include <stdarg.h>

namespace pybind
{
	PYBIND_API void visit_class_type_scope( visitor_class_type_scope * _visitor );

	PYBIND_API bool initialize( bool _debug, bool install_sigs, bool _nosite );
	PYBIND_API void finalize();

	PYBIND_API bool is_initialized();

    PYBIND_API uint32_t get_python_version();

	PYBIND_API void check_error();

	PYBIND_API void setStdErrorHandle( PyObject * _handle );
	PYBIND_API void setStdOutHandle( PyObject * _handle );
    PYBIND_API PyObject * get_builtins();

	PYBIND_API PyObject * module_import( const char * _name, bool & _exsist );
	PYBIND_API PyObject * module_init( const char * _name );
    PYBIND_API void module_fini( PyObject * _module );
	PYBIND_API PyObject * module_dict( PyObject * _module );
	PYBIND_API void module_addobject( PyObject * _module, const char * _name, PyObject * _obj );
    PYBIND_API PyObject * module_execcode( const char * _name, PyObject * _code );

    PYBIND_API bool code_check( PyObject * _code );
    PYBIND_API PyObject * code_compile_file( const char * _buf, const char * _module );

	PYBIND_API void set_currentmodule( PyObject * _module );
	PYBIND_API PyObject * get_currentmodule();

	PYBIND_API PyObject * ask( PyObject * _obj, const char * _format, ... );
	PYBIND_API PyObject * ask_va( PyObject * _obj, const char * _format, va_list );
	PYBIND_API PyObject * ask_native( PyObject * _obj, PyObject * _args );

	PYBIND_API PyObject * ask_method( PyObject * _obj, const char * _method, const char * _format, ... );	
	PYBIND_API PyObject * ask_method_va( PyObject * _obj, const char * _method, const char * _format, va_list );
	PYBIND_API PyObject * ask_method_native( PyObject * _obj, const char * _method, PyObject * _args );
		
	PYBIND_API void call( PyObject * _obj, const char * _format, ... );
	PYBIND_API void call_va( PyObject * _obj, const char * _format, va_list );
	PYBIND_API void call_native( PyObject * _obj, PyObject * _args );

	PYBIND_API void call_method( PyObject * _obj, const char * _method, const char * _format, ... );
	PYBIND_API void call_method_va( PyObject * _obj, const char * _method, const char * _format, va_list );
	PYBIND_API void call_method_native( PyObject * _obj, const char * _method, PyObject * _args );
		
	PYBIND_API PyObject * compile_string( const char * _string, const char * _file );
	PYBIND_API PyObject * exec( const char * _code, PyObject * _global = 0, PyObject * _local = 0 );
	//PYBIND_API PyObject * exec_file( const char * _filename, PyObject * _global = 0, PyObject * _local = 0 );

    PYBIND_API void set_path( const wchar_t * _value );
	PYBIND_API void set_syspath( PyObject * _value );

	PYBIND_API char get_sysdelim();

	PYBIND_API void incref( PyObject * _obj );
	PYBIND_API void decref( PyObject * _obj );
	PYBIND_API size_t refcount( PyObject * _obj );

	PYBIND_API bool is_callable( PyObject * _obj );
	PYBIND_API bool is_instanceof( PyObject * _obj, PyTypeObject * _type );

	PYBIND_API PyObject * get_none();
	PYBIND_API bool is_none( PyObject * _none );
	PYBIND_API PyObject * ret_none();

	PYBIND_API bool bool_check( PyObject * _bool );
	PYBIND_API bool is_true( PyObject * _bool );

	PYBIND_API PyObject * ret_true();
	PYBIND_API PyObject * ret_false();

	PYBIND_API PyObject * ret_bool( bool _value );
	PYBIND_API PyObject * get_bool( bool _value );
	
	PYBIND_API bool has_attr( PyObject * _obj, const char * _attr );
	PYBIND_API PyObject * get_attr( PyObject * _obj, const char * _attr );
	PYBIND_API bool set_attr( PyObject * _obj, const char * _attr , PyObject * _value);

	PYBIND_API bool check_type( PyObject * _obj );

	PYBIND_API bool int_check( PyObject * _obj );
	PYBIND_API bool long_check( PyObject * _obj );
	PYBIND_API bool float_check( PyObject * _obj );
		
	PYBIND_API PyObject * list_new( size_t _size );
	PYBIND_API bool list_check( PyObject * _list );
	PYBIND_API size_t list_size( PyObject * _list );
	PYBIND_API PyObject * list_getitem( PyObject * _list, size_t _it );
    PYBIND_API bool list_insert( PyObject * _list, size_t _it, PyObject * _item );
	PYBIND_API bool list_remove( PyObject * _list, size_t _it );
	PYBIND_API bool list_setitem( PyObject * _list, size_t _it, PyObject * _item );
	PYBIND_API bool list_appenditem( PyObject * _list, PyObject * _item );

	template<class T>
	inline T list_getitem_t( PyObject * _list, size_t _it )
	{
		PyObject * py_item = pybind::list_getitem( _list, _it );

		return pybind::extract<T>( py_item );
	}

	template<class T>
	inline bool list_setitem_t( PyObject * _list, size_t _it, const T & _item )
	{
		PyObject * py_item = pybind::ptr( _item );

		return pybind::list_setitem( _list, _it, py_item );
	}

	template<class T>
	inline bool list_appenditem_t( PyObject * _obj, const T & _item )
	{
		PyObject * py_item = pybind::ptr( _item );

		bool result = pybind::list_appenditem( _obj, py_item );

		pybind::decref( py_item );

		return result;
	}

	PYBIND_API PyObject * dict_new();
	PYBIND_API PyObject * dict_new_presized( size_t _count );
	PYBIND_API PyObject * dict_from( PyObject * _obj );

	PYBIND_API bool dict_check( PyObject * _dict );
	PYBIND_API size_t dict_size( PyObject * _obj );
	PYBIND_API bool dict_set( PyObject * _dict, PyObject * _name, PyObject * _value );
    PYBIND_API bool dict_setstring( PyObject * _dict, const char * _name, PyObject * _value );
    PYBIND_API bool dict_remove( PyObject * _dict, const char * _name );	
	PYBIND_API PyObject * dict_get( PyObject * _dict, const char * _name );
	PYBIND_API bool dict_contains( PyObject * _dict, const char * _name );	
	PYBIND_API bool dict_next( PyObject * _dict, size_t & _pos, PyObject ** _key, PyObject ** _value );

	PYBIND_API PyObject * dict_items( PyObject * _dict );

	template<class T>
	PYBIND_API bool dict_setstring_t( PyObject * _dict, const char * _name, const T & _value )
	{
		PyObject * py_value = pybind::ptr( _value );

		return pybind::dict_setstring( _dict, _name, py_value );
	}
	
	PYBIND_API PyObject * tuple_new( size_t _it );
	PYBIND_API bool tuple_check( PyObject * _tuple );
	PYBIND_API size_t tuple_size( PyObject * _tuple );
	PYBIND_API PyObject * tuple_getitem( PyObject * _tuple, size_t _it );
	PYBIND_API bool tuple_setitem( PyObject * _tuple, size_t _it, PyObject * _value );

	template<class T>
	inline T tuple_getitem_t( PyObject * _tuple, size_t _it )
	{
		PyObject * py_item = pybind::tuple_getitem( _tuple, _it );

		return pybind::extract<T>( py_item );
	}

	template<class T>
	inline bool tuple_setitem_t( PyObject * _tuple, size_t _it, const T & _item )
	{
		PyObject * py_item = pybind::ptr( _item );

		return pybind::tuple_setitem( _tuple, _it, py_item );
	}

	template<class T0>
	inline PyObject * make_tuple_t( const T0 & _t0 )
	{
		PyObject * py_tuple = pybind::tuple_new( 1 );

		pybind::tuple_setitem_t( py_tuple, 0, _t0 );

		return py_tuple;
	}

	template<class T0, class T1>
	inline PyObject * make_tuple_t( const T0 & _t0, const T1 & _t1 )
	{
		PyObject * py_tuple = pybind::tuple_new( 2 );

		pybind::tuple_setitem_t( py_tuple, 0, _t0 );
		pybind::tuple_setitem_t( py_tuple, 1, _t1 );

		return py_tuple;
	}

	template<class T0, class T1, class T2>
	inline PyObject * make_tuple_t( const T0 & _t0, const T1 & _t1, const T2 & _t2 )
	{
		PyObject * py_tuple = pybind::tuple_new( 3 );

		pybind::tuple_setitem_t( py_tuple, 0, _t0 );
		pybind::tuple_setitem_t( py_tuple, 1, _t1 );
		pybind::tuple_setitem_t( py_tuple, 2, _t2 );

		return py_tuple;
	}

	template<class T0, class T1, class T2, class T3>
	inline PyObject * make_tuple_t( const T0 & _t0, const T1 & _t1, const T2 & _t2, const T3 & _t3 )
	{
		PyObject * py_tuple = pybind::tuple_new( 4 );

		pybind::tuple_setitem_t( py_tuple, 0, _t0 );
		pybind::tuple_setitem_t( py_tuple, 1, _t1 );
		pybind::tuple_setitem_t( py_tuple, 2, _t2 );
		pybind::tuple_setitem_t( py_tuple, 3, _t3 );

		return py_tuple;
	}

	template<class T0>
	inline bool list_appendtuple_t( PyObject * _obj, const T0 & _t0 )
	{
		PyObject * py_tuple = pybind::make_tuple_t( _t0 );

		bool result = pybind::list_appenditem( _obj, py_tuple );

		pybind::decref( py_tuple );

		return result;
	}

	template<class T0, class T1>
	inline bool list_appendtuple_t( PyObject * _obj, const T0 & _t0, const T1 & _t1 )
	{
		PyObject * py_tuple = pybind::make_tuple_t( _t0, _t1 );

		bool result = pybind::list_appenditem( _obj, py_tuple );

		pybind::decref( py_tuple );

		return result;
	}

	template<class T0, class T1, class T2>
	inline bool list_appendtuple_t( PyObject * _obj, const T0 & _t0, const T1 & _t1, const T2 & _t2 )
	{
		PyObject * py_tuple = pybind::make_tuple_t( _t0, _t1, _t2 );

		bool result = pybind::list_appenditem( _obj, py_tuple );

		pybind::decref( py_tuple );

		return result;
	}

	template<class T0, class T1, class T2, class T3>
	inline bool list_appendtuple_t( PyObject * _obj, const T0 & _t0, const T1 & _t1, const T2 & _t2, const T3 & _t3 )
	{
		PyObject * py_tuple = pybind::make_tuple_t( _t0, _t1, _t2, _t3 );

		bool result = pybind::list_appenditem( _obj, py_tuple );

		pybind::decref( py_tuple );

		return result;
	}

	PYBIND_API PyObject * object_dir( PyObject * _obj );
    PYBIND_API PyTypeObject * object_type( PyObject * _obj );
    PYBIND_API const char * object_repr_type( PyObject * _obj );
	PYBIND_API const char * object_repr( PyObject * _obj );  
	PYBIND_API const char * object_str( PyObject * _obj );
	PYBIND_API long object_hash( PyObject * _obj );

    PYBIND_API void error_traceback( const char * _message, ... );
	PYBIND_API void error_message( const char * _message, ... );
	PYBIND_API PyObject * build_value( const char * _format, ... );
	PYBIND_API PyObject * build_value_va( const char * _format, va_list _va );


	PYBIND_API void unwrap( PyObject * _value );
	PYBIND_API bool is_wrap( PyObject * _value );

	PYBIND_API bool type_initialize( PyObject * _value );

	PYBIND_API void exception_filter();

	PYBIND_API bool test_equal( PyObject * _left, PyObject * _right );
	
	PYBIND_API bool string_check( PyObject * _string );	
	PYBIND_API size_t string_size( PyObject * _string );
	PYBIND_API const char * string_to_char( PyObject * _string );
    PYBIND_API const char * string_to_char_and_size( PyObject * _string, size_t & _size );
    PYBIND_API PyObject * string_from_char( const char * _str );
	PYBIND_API PyObject * string_from_char_size( const char * _str, size_t _size );

	PYBIND_API bool unicode_check( PyObject * _unicode );	
	//PYBIND_API pybind_unicode_t * unicode_to_wchar( PyObject * _unicode );
    PYBIND_API const wchar_t * unicode_to_wchar( PyObject * _unicode );
    PYBIND_API const wchar_t * unicode_to_wchar_and_size( PyObject * _unicode, size_t & _size );
    PYBIND_API PyObject * unicode_from_wchar( const pybind_unicode_t * _value );
	PYBIND_API PyObject * unicode_from_wchar_size( const pybind_unicode_t * _value, size_t _size );
	
	PYBIND_API const char * unicode_to_utf8( PyObject * _unicode );
    PYBIND_API const char * unicode_to_utf8_and_size( PyObject * _unicode, size_t & _size );
    PYBIND_API PyObject * unicode_from_utf8( const char * _utf8 );
	PYBIND_API PyObject * unicode_from_utf8_size( const char * _utf8, size_t _size );

    PYBIND_API PyObject * _unicode_to_utf8_obj( PyObject * _unicode );

    PYBIND_API bool void_ptr_check( PyObject * _obj );
    PYBIND_API PyObject * void_ptr_new( void * _impl );
    PYBIND_API void * void_ptr_get( PyObject * _obj );

    PYBIND_API long marshal_magic_number();
    PYBIND_API PyObject * marshal_get_object( const char * _buf, size_t _len );

    PYBIND_API void _set_module_finder( PyObject * _finder );
	PYBIND_API void _remove_module_finder();
}

