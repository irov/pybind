#pragma once

#include "pybind/exports.hpp"
#include "pybind/types.hpp"

#include <stdarg.h>

namespace pybind
{
    PYBIND_API uint32_t get_python_version();

    PYBIND_API void check_error();

    PYBIND_API void setStdErrorHandle( PyObject * _handle );    
    PYBIND_API void setStdOutHandle( PyObject * _handle );
    PYBIND_API PyObject * getStdErrorHandle();
    PYBIND_API PyObject * getStdOutHandle();

    PYBIND_API PyObject * get_builtins();

    PYBIND_API PyObject * module_import( const char * _name, bool & _exsist );
    PYBIND_API PyObject * module_init( const char * _name );
    PYBIND_API void module_fini( PyObject * _module );
    PYBIND_API PyObject * module_dict( PyObject * _module );
    PYBIND_API void module_addobject( PyObject * _module, const char * _name, PyObject * _obj );
    PYBIND_API void module_removeobject( PyObject * _module, const char * _name );
    PYBIND_API bool module_hasobject( PyObject * _module, const char * _name );
    PYBIND_API PyObject * module_execcode( const char * _name, PyObject * _code );
    PYBIND_API PyObject * module_reload( PyObject * _module );

    PYBIND_API bool code_check( PyObject * _code );
    PYBIND_API PyObject * code_compile_file( const void * _buf, const char * _module );

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
    PYBIND_API PyObject * eval_string( const char * _string, PyObject * _global, PyObject * _local );
    PYBIND_API PyObject * exec_file( const char * _code, PyObject * _global, PyObject * _local );

    PYBIND_API void set_path( const wchar_t * _value );
    PYBIND_API void set_syspath( PyObject * _value );

    PYBIND_API char get_sysdelim();

    PYBIND_API void incref( PyObject * _obj );
    PYBIND_API void decref( PyObject * _obj );
    PYBIND_API void xincref( PyObject * _obj );
    PYBIND_API void xdecref( PyObject * _obj );
    PYBIND_API uint32_t refcount( PyObject * _obj );

    PYBIND_API bool is_callable( PyObject * _obj );
    PYBIND_API bool is_instanceof( PyObject * _obj, PyTypeObject * _type );

    PYBIND_API PyObject * get_none();
    PYBIND_API bool is_none( PyObject * _obj );
    PYBIND_API PyObject * ret_none();

    PYBIND_API PyObject * ret_not_implemented();

    PYBIND_API bool bool_check( PyObject * _obj );
    PYBIND_API bool is_true( PyObject * _obj );
    PYBIND_API bool is_false( PyObject * _obj );

    PYBIND_API PyObject * get_true();
    PYBIND_API PyObject * get_false();

    PYBIND_API PyObject * ret_true();
    PYBIND_API PyObject * ret_false();

    PYBIND_API PyObject * ret_bool( bool _value );
    PYBIND_API PyObject * get_bool( bool _value );

    PYBIND_API bool has_attr( PyObject * _obj, PyObject * _attr );
    PYBIND_API PyObject * get_attr( PyObject * _obj, PyObject * _attr );

    PYBIND_API bool has_attrstring( PyObject * _obj, const char * _attr );
    PYBIND_API PyObject * get_attrstring( PyObject * _obj, const char * _attr );
    PYBIND_API bool set_attr( PyObject* _obj, PyObject* _attr, PyObject* _value );
    PYBIND_API bool set_attrstring( PyObject * _obj, const char * _attr, PyObject * _value );

    PYBIND_API bool check_type( PyObject * _obj );
    PYBIND_API bool int_check( PyObject * _obj );
    PYBIND_API bool long_check( PyObject * _obj );
    PYBIND_API bool float_check( PyObject * _obj );

    PYBIND_API bool extract_bool( PyObject * _obj, bool & _value );
    PYBIND_API bool extract_int8( PyObject * _obj, int8_t & _value );
    PYBIND_API bool extract_int16( PyObject * _obj, int16_t & _value );
    PYBIND_API bool extract_int32( PyObject * _obj, int32_t & _value );
    PYBIND_API bool extract_int64( PyObject * _obj, int64_t & _value );
    PYBIND_API bool extract_uint8( PyObject * _obj, uint8_t & _value );
    PYBIND_API bool extract_uint16( PyObject * _obj, uint16_t & _value );
    PYBIND_API bool extract_uint32( PyObject * _obj, uint32_t & _value );
    PYBIND_API bool extract_uint64( PyObject * _obj, uint64_t & _value );
    PYBIND_API bool extract_float( PyObject * _obj, float & _value );
    PYBIND_API bool extract_double( PyObject * _obj, double & _value );
    PYBIND_API bool extract_wchar( PyObject * _obj, wchar_t & _value );

    PYBIND_API PyObject * ptr_bool( bool _value );
    PYBIND_API PyObject * ptr_int8( int8_t _value );
    PYBIND_API PyObject * ptr_int16( int16_t _value );
    PYBIND_API PyObject * ptr_int32( int32_t _value );
    PYBIND_API PyObject * ptr_int64( int64_t _value );
    PYBIND_API PyObject * ptr_uint8( uint8_t _value );
    PYBIND_API PyObject * ptr_uint16( uint16_t _value );
    PYBIND_API PyObject * ptr_uint32( uint32_t _value );
    PYBIND_API PyObject * ptr_uint64( uint64_t _value );
    PYBIND_API PyObject * ptr_float( float _value );
    PYBIND_API PyObject * ptr_double( double _value );
    PYBIND_API PyObject * ptr_long( long _value );
    PYBIND_API PyObject * ptr_ulong( unsigned long _value );

    PYBIND_API PyObject * list_new( size_t _size );
    PYBIND_API bool list_check( PyObject * _list );
    PYBIND_API size_t list_size( PyObject * _list );
    PYBIND_API PyObject * list_getitem( PyObject * _list, size_t _it );
    PYBIND_API bool list_insert( PyObject * _list, size_t _it, PyObject * _item );
    PYBIND_API bool list_remove( PyObject * _list, size_t _it );
    PYBIND_API bool list_setitem( PyObject * _list, size_t _it, PyObject * _item );
    PYBIND_API bool list_appenditem( PyObject * _list, PyObject * _item );

    PYBIND_API PyObject * dict_new();
    PYBIND_API PyObject * dict_new_presized( size_t _count );
    PYBIND_API PyObject * dict_from( PyObject * _obj );
    PYBIND_API bool dict_check( PyObject * _dict );
    PYBIND_API size_t dict_size( PyObject * _dict );
    PYBIND_API bool dict_set( PyObject * _dict, PyObject * _name, PyObject * _value );
    PYBIND_API bool dict_setstring( PyObject * _dict, const char * _name, PyObject * _value );
    PYBIND_API bool dict_remove( PyObject * _dict, PyObject * _name );
    PYBIND_API bool dict_removestring( PyObject * _dict, const char * _name );
    PYBIND_API PyObject * dict_getstring( PyObject * _dict, const char * _key );
    PYBIND_API PyObject * dict_get( PyObject * _dict, PyObject * _name );
    PYBIND_API bool dict_exist( PyObject * _dict, PyObject * _name );
    PYBIND_API bool dict_existstring( PyObject * _dict, const char * _name );
    PYBIND_API bool dict_next( PyObject * _dict, size_t * const _pos, PyObject ** _key, PyObject ** _value );
    PYBIND_API PyObject * dict_items( PyObject * _dict );

    PYBIND_API PyObject * set_new();
    PYBIND_API bool set_check( PyObject * _set );
    PYBIND_API size_t set_size( PyObject * _set );
    PYBIND_API bool set_set( PyObject * _set, PyObject * _value );
    PYBIND_API bool set_remove( PyObject * _set, PyObject * _value );
    PYBIND_API bool set_exist( PyObject * _set, PyObject * _value );
    
    PYBIND_API bool iterator_get( PyObject * _collections, PyObject ** _iterator );
    PYBIND_API bool iterator_next( PyObject * _iterator, PyObject ** _value );
    PYBIND_API void iterator_end( PyObject * _iterator );

    PYBIND_API PyObject * tuple_new( size_t _it );
    PYBIND_API bool tuple_check( PyObject * _obj );
    PYBIND_API size_t tuple_size( PyObject * _obj );
    PYBIND_API PyObject * tuple_getitem( PyObject * _obj, size_t _it );
    PYBIND_API PyObject * tuple_slice( PyObject * _obj, size_t _low, size_t _high );
    PYBIND_API PyObject * tuple_slice_tail( PyObject * _obj, size_t _size );
    PYBIND_API bool tuple_setitem( PyObject * _obj, size_t _it, PyObject * _value );

    PYBIND_API PyObject * object_dir( PyObject * _obj );
    PYBIND_API PyTypeObject * object_type( PyObject * _obj );
    PYBIND_API const char * object_type_name( PyObject * _obj );
    PYBIND_API PyObject * object_repr_type( PyObject * _obj );
    PYBIND_API PyObject * object_repr( PyObject * _obj );
    PYBIND_API PyObject * object_str( PyObject * _obj );
    PYBIND_API int64_t object_hash( PyObject * _obj );

    PYBIND_API bool get_statetrace_top( char * const _filename, size_t _maxlenfilename, char * const _function, size_t _maxlenfunction, uint32_t * const _lineno );
    PYBIND_API bool get_statetrace( char * const _buffer, size_t _maxlen, bool _append );
    PYBIND_API void error_traceback( const char * _format, ... );
    PYBIND_API void error_traceback_va( const char * _format, va_list _va );
    PYBIND_API void error_message( const char * _format, ... );
    PYBIND_API void error_message_va( const char * _format, va_list _va );
    PYBIND_API void throw_message( const char * _format, ... );
    PYBIND_API void throw_message_va( const char * _format, va_list _va );
    PYBIND_API void warning_traceback( const char * _format, ... );
    PYBIND_API void warning_traceback_va( const char * _format, va_list _va );
    PYBIND_API void error_clear();
    PYBIND_API PyObject * build_value( const char * _format, ... );
    PYBIND_API PyObject * build_value_va( const char * _format, va_list _va );

    PYBIND_API bool traceback_check( PyObject * _traceback );
    PYBIND_API PyObject * traceback_filename( PyObject * _traceback );
    PYBIND_API uint32_t traceback_lineno( PyObject * _traceback );
    PYBIND_API PyObject * traceback_function( PyObject * _traceback );
    PYBIND_API PyObject * traceback_next( PyObject * _traceback );

    PYBIND_API int64_t type_hash( PyTypeObject * _obj );
    PYBIND_API const char * type_name( PyTypeObject * _obj );

    PYBIND_API bool test_equal( PyObject * _left, PyObject * _right );

    PYBIND_API bool string_check( PyObject * _string );
    PYBIND_API size_t string_size( PyObject * _string );
    PYBIND_API int64_t string_hash( PyObject * _string );
    PYBIND_API const char * string_to_char( PyObject * _string );
    PYBIND_API const char * string_to_char_and_size( PyObject * _string, size_t * _size );
    PYBIND_API PyObject * string_from_char( const char * _str );
    PYBIND_API PyObject * string_from_char_size( const char * _str, size_t _size );

    PYBIND_API bool bytearray_check( PyObject * _bytearray );
    PYBIND_API size_t bytearray_size( PyObject * _bytearray );
    PYBIND_API const uint8_t * bytearray_to_data( PyObject * _bytearray );
    PYBIND_API PyObject * bytearray_from_data( const uint8_t * _data, size_t _size );

    PYBIND_API bool unicode_check( PyObject * _unicode );
    PYBIND_API const wchar_t * unicode_to_wchar( PyObject * _unicode );
    PYBIND_API const wchar_t * unicode_to_wchar_and_size( PyObject * _unicode, size_t * _size );
    PYBIND_API PyObject * unicode_from_wchar( const wchar_t * _value );
    PYBIND_API PyObject * unicode_from_wchar_size( const wchar_t * _value, size_t _size );

    PYBIND_API const char * unicode_to_utf8( PyObject * _unicode );
    PYBIND_API const char * unicode_to_utf8_and_size( PyObject * _unicode, size_t * _size );
    PYBIND_API PyObject * unicode_from_utf8( const char * _utf8 );
    PYBIND_API PyObject * unicode_from_utf8_size( const char * _utf8, size_t _size );

    PYBIND_API PyObject * _unicode_to_utf8_obj( PyObject * _unicode );

    PYBIND_API bool void_ptr_check( PyObject * _obj );
    PYBIND_API PyObject * void_ptr_new( void * _impl );
    PYBIND_API void * void_ptr_get( PyObject * _obj );

    PYBIND_API PyObject * exception_new( const char * _msg );

    PYBIND_API long marshal_magic_number();
    PYBIND_API PyObject * marshal_get_object( const void * _buf, size_t _len );

    PYBIND_API void set_module_finder( PyObject * _finder );
    PYBIND_API void remove_module_finder();
}

