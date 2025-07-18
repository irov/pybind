#pragma once

#include "pybind/exports.hpp"
#include "pybind/types.hpp"
#include "pybind/mutex.hpp"
#include "pybind/string_view.hpp"

#include "pybind/allocator_interface.hpp"
#include "pybind/function_interface.hpp"
#include "pybind/functor_interface.hpp"
#include "pybind/member_interface.hpp"
#include "pybind/method_interface.hpp"

#include <typeinfo>
#include <cstdarg>

namespace pybind
{
    typedef intrusive_ptr<class type_cast> type_cast_ptr;
    typedef intrusive_ptr<class class_type_scope_interface> class_type_scope_interface_ptr;

    typedef intrusive_ptr<class new_adapter_interface> new_adapter_interface_ptr;
    typedef intrusive_ptr<class destroy_adapter_interface> destroy_adapter_interface_ptr;

    PYBIND_API kernel_interface * initialize( allocator_interface * _allocator, const wchar_t * _path, bool _debug, bool install_sigs, bool _nosite );
    PYBIND_API class kernel_interface * get_kernel();
    PYBIND_API void set_kernel( class kernel_interface * _kernel );

    PYBIND_API void update_main_thread();

    typedef void (*pybind_excepthook_handler_f)(void * _ud, PyTypeObject * _exctype, PyObject * _value, PyObject * _traceback);

    class kernel_interface
    {
    public:
        kernel_interface()
        {
        };
        virtual ~kernel_interface()
        {
        };

    public:
        virtual allocator_interface * get_allocator() const = 0;

    public:
        virtual void collect() = 0;
        virtual void destroy() = 0;

    public:
        virtual void remove_from_module( const char * _name, PyObject * _module ) = 0;

    public:
        virtual void def_function_adapter( const function_adapter_interface_ptr & _adapter, bool _native, PyObject * _module ) = 0;
        virtual void def_functor_adapter( const functor_adapter_interface_ptr & _adapter, bool _native, PyObject * _module ) = 0;

        virtual PyObject * create_function_adapter( const function_adapter_interface_ptr & _adapter, bool _native ) = 0;
        virtual PyObject * create_functor_adapter( const functor_adapter_interface_ptr & _adapter, bool _native ) = 0;

        virtual PyObject * create_member_adapter( const member_adapter_interface_ptr & _iadapter ) = 0;
        virtual PyObject * create_method_adapter( const method_adapter_interface_ptr & _ifunc, PyTypeObject * _type ) = 0;
        virtual method_adapter_interface * get_method_adapter( PyObject * _obj ) = 0;

        virtual PyTypeObject * get_pod_type( size_t _pod, bool _hash ) = 0;
        virtual bool is_object_bindable( PyObject * _obj ) = 0;

    public:
        virtual void set_current_module( PyObject * _module ) = 0;
        virtual PyObject * get_current_module() const = 0;

        virtual observer_bind_call * set_observer_bind_call( observer_bind_call * _observer ) = 0;
        virtual observer_bind_call * get_observer_bind_call() = 0;

    public:
        virtual void * unwrap( PyObject * _obj ) = 0;
        virtual bool is_wrap( PyObject * _obj ) = 0;

        virtual bool is_class( PyObject * _obj ) = 0;
        virtual bool is_type_class( PyTypeObject * _type ) = 0;

        virtual typeid_t get_class_type_id( const std::type_info & _info ) = 0;
        virtual const char * get_class_type_info( typeid_t _id ) = 0;

        template<class T>
        const char * get_class_type_info_t()
        {
            typeid_t scope_id = this->class_info<T>();

            const char * type_info = this->get_class_type_info( scope_id );

            return type_info;
        }

        template<class T>
        typeid_t class_info()
        {
            static typeid_t type_id = 0;

            if( type_id == 0 )
            {
                const std::type_info & ti = typeid(T);

                typeid_t id = this->get_class_type_id( ti );

                type_id = id;
            }

            return type_id;
        }

        virtual const char * object_type_name( PyObject * _type ) = 0;
        virtual const char * type_name( PyTypeObject * _type ) = 0;

        virtual typeid_t find_class_info_desc_name( const char * _name ) = 0;

        virtual void register_type_info_extract( typeid_t _info, const type_cast_ptr & _cast ) = 0;

        template<class T>
        void register_type_info_extract_t( const type_cast_ptr & _cast )
        {
            typeid_t id = this->class_info<T>();

            this->register_type_info_extract( id, _cast );
        }

        virtual void unregister_type_info_extract( typeid_t _info ) = 0;

        template<class T>
        void unregister_type_info_extract_t()
        {
            typeid_t id = this->class_info<T>();

            this->unregister_type_info_extract( id );
        }

        virtual type_cast * find_type_info_extract( typeid_t _info ) = 0;

        virtual class_type_scope_interface_ptr create_new_type_scope( typeid_t _info, const char * _name, void * _user, const new_adapter_interface_ptr & _pynew, const destroy_adapter_interface_ptr & _pydestructor, size_t _pod, bool _hash ) = 0;
        virtual void remove_type_scope( typeid_t _info ) = 0;

        virtual bool has_class_type_scope( typeid_t _info ) = 0;
        virtual const class_type_scope_interface_ptr & get_class_type_scope( typeid_t _info ) = 0;

        virtual void visit_types_scope( visitor_class_type_scope * _getter ) = 0;

        virtual PyTypeObject * get_object_type( PyObject * _type ) = 0;
        virtual typeid_t get_object_type_id( PyObject * _type ) = 0;

        virtual const class_type_scope_interface_ptr & get_class_scope( PyTypeObject * _type ) = 0;
        virtual void cache_class_scope_type( const class_type_scope_interface_ptr & _scope ) = 0;
        virtual void remove_class_scope_type( const class_type_scope_interface_ptr & _scope ) = 0;

        template<class T>
        class_type_scope_interface_ptr create_new_scope( const char * _name, void * _user, const new_adapter_interface_ptr & _pynew, const destroy_adapter_interface_ptr & _pydestructor, size_t _pod, bool _hash )
        {
            typeid_t ti = this->class_info<T>();

            class_type_scope_interface_ptr scope = this->create_new_type_scope( ti, _name, _user, _pynew, _pydestructor, _pod, _hash );

            return scope;
        }

        template<class T>
        const class_type_scope_interface_ptr & get_class_type_scope_t()
        {
            typeid_t ti = this->class_info<T>();

            const class_type_scope_interface_ptr & scope = this->get_class_type_scope( ti );

            return scope;
        }

        template<class T>
        void remove_scope()
        {
            typeid_t ti = this->class_info<T>();
            this->remove_type_scope( ti );
            this->unregister_type_info_extract( ti );
        }

        virtual PyObject * scope_create_holder( const class_type_scope_interface_ptr & _scope, void * _ptr ) = 0;
        virtual PyObject * scope_create_weak( const class_type_scope_interface_ptr & _scope, void * _ptr ) = 0;

        template<class T>
        PyObject * scope_create_holder_t( T * _ptr )
        {
            const class_type_scope_interface_ptr & scope = this->get_class_type_scope_t<T>();

            PyObject * py_obj = this->scope_create_holder( scope, _ptr );

            return py_obj;
        }

        template<class T>
        PyObject * scope_create_weak_t( T * _ptr )
        {
            const class_type_scope_interface_ptr & scope = this->get_class_type_scope_t<T>();

            PyObject * py_obj = this->scope_create_weak( scope, _ptr );

            return py_obj;
        }

        virtual void * meta_cast_scope( void * _self, typeid_t _scope_name, typeid_t _class_name, const class_type_scope_interface_ptr & _scope ) = 0;

        template<class T>
        inline T meta_cast_scope_t( void * _self, typeid_t _scope_name, typeid_t _class_name, const class_type_scope_interface_ptr & _scope )
        {
            void * impl = this->meta_cast_scope( _self, _scope_name, _class_name, _scope );

            T obj = static_cast<T>(impl);

            return obj;
        }

        template<class C>
        inline C * meta_cast_class_t( void * _self, const class_type_scope_interface_ptr & _scope )
        {
            typeid_t scope_name = this->class_info<C>();
            typeid_t class_name = this->class_info<C *>();

            void * impl = this->meta_cast_scope_t<C *>( _self, scope_name, class_name, _scope );

            C * obj = static_cast<C *>(impl);

            return obj;
        }

        virtual void * check_registred_class( PyObject * _obj, typeid_t _info ) = 0;

        virtual void error_invalid_extract( PyObject * _obj, typeid_t _tinfo ) = 0;
        virtual bool instance_of_type( PyObject * _obj, typeid_t _tinfo ) = 0;

        template<class C>
        C * get_class_impl_t( PyObject * _obj )
        {
            void * impl = this->get_class_impl( _obj );

            C * obj = static_cast<C *>(impl);

            return obj;
        }

        virtual void * get_class_impl( PyObject * _obj ) = 0;
        virtual bool is_class_weak( PyObject * _obj ) = 0;
        virtual bool type_initialize( PyObject * _obj ) = 0;

    public:
        virtual uint32_t get_python_version() = 0;

        virtual PyObject * get_builtins() = 0;

        virtual void set_syspath( PyObject * _path ) = 0;

        virtual PyObject * code_compile_file( const void * _buf, const char * _module ) = 0;
        virtual bool code_check( PyObject * _code ) = 0;
        virtual long marshal_magic_number() = 0;
        virtual PyObject * marshal_get_object( const void * _buf, size_t _len ) = 0;

        virtual void set_module_finder( PyObject * _finder ) = 0;
        virtual void remove_module_finder() = 0;

        virtual void set_sys_excepthook( pybind_excepthook_handler_f _excepthook, void * _ud ) = 0;

        virtual void call_native( PyObject * _obj, PyObject * _args ) = 0;
        virtual void call_method( PyObject * _obj, const char * _method, const char * _format, ... ) = 0;
        virtual void call_method_native( PyObject * _obj, const char * _method, PyObject * _args ) = 0;

        virtual PyObject * ask_native( PyObject * _obj, PyObject * _args ) = 0;
        virtual PyObject * ask_method( PyObject * _obj, const char * _method, const char * _format, ... ) = 0;
        virtual PyObject * ask_method_native( PyObject * _obj, const char * _method, PyObject * _args ) = 0;
        virtual PyObject * ask_adapter( void * _self, const class_type_scope_interface_ptr & _scope, const char * _name, PyObject * _args ) = 0;

        virtual PyObject * compile_string( const char * _string, const char * _file ) = 0;
        virtual PyObject * eval_string( const char * _string, PyObject * _globals, PyObject * _locals ) = 0;
        virtual PyObject * exec_file( const char * _code, PyObject * _globals, PyObject * _locals ) = 0;

        virtual void setStdOutHandle( PyObject * _obj ) = 0;
        virtual void setStdErrorHandle( PyObject * _obj ) = 0;
        virtual PyObject * getStdOutHandle() = 0;
        virtual PyObject * getStdErrorHandle() = 0;

        virtual bool get_statetrace_top( char * const _filename, size_t _maxlenfilename, char * const _function, size_t _maxlenfunction, uint32_t * _lineno ) = 0;
        virtual bool get_statetrace( char * _buffer, size_t _maxlen, bool _append ) = 0;
        virtual void error_traceback( const char * _format, ... ) = 0;
        virtual void error_traceback_va( const char * _format, va_list _va ) = 0;
        virtual void error_message( const char * _format, ... ) = 0;
        virtual void error_message_va( const char * _format, va_list _va ) = 0;
        virtual void throw_message( const char * _format, ... ) = 0;
        virtual void throw_message_va( const char * _format, va_list _va ) = 0;
        virtual void warning_traceback( const char * _format, ... ) = 0;
        virtual void warning_traceback_va( const char * _format, va_list _va ) = 0;
        virtual PyObject * build_value( const char * _format, ... ) = 0;
        virtual PyObject * build_value_va( const char * _format, va_list _va ) = 0;

        virtual bool traceback_check( PyObject * _traceback ) = 0;
        virtual string_view traceback_filename( PyObject * _traceback ) = 0;
        virtual uint32_t traceback_lineno( PyObject * _traceback ) = 0;
        virtual string_view traceback_function( PyObject * _traceback ) = 0;
        virtual PyObject * traceback_next( PyObject * _traceback ) = 0;

        virtual PyObject * module_import( const char * _name, bool & _exsist ) = 0;
        virtual PyObject * module_init( const char * _name ) = 0;
        virtual void module_fini( PyObject * _module ) = 0;
        virtual PyObject * module_dict( PyObject * _module ) = 0;
        virtual void module_addobject( PyObject * _module, const char * _name, PyObject * _obj ) = 0;
        virtual PyObject * module_execcode( const char * _name, PyObject * _code ) = 0;
        virtual PyObject * module_reload( PyObject * _module ) = 0;

        virtual void incref( PyObject * _obj ) = 0;
        virtual void decref( PyObject * _obj ) = 0;
        virtual uint32_t refcount( PyObject * _obj ) = 0;

        virtual bool is_none( PyObject * _obj ) const = 0;
        virtual PyObject * get_none() = 0;
        virtual PyObject * ret_none() = 0;

        virtual PyObject * ret_not_implemented() = 0;

        virtual bool is_callable( PyObject * _obj ) const = 0;
        virtual bool is_instanceof( PyObject * _obj, PyTypeObject * _type ) const = 0;
        virtual bool test_equal( PyObject * _lhs, PyObject * _rhs ) = 0;
        virtual bool has_attr( PyObject * _obj, PyObject * _attr ) = 0;
        virtual PyObject * get_attr( PyObject * _obj, PyObject * _attr ) = 0;
        virtual bool set_attr( PyObject * _obj, PyObject * _attr, PyObject * _value ) = 0;
        virtual bool set_attrstring( PyObject * _obj, const char * _attr, PyObject * _value ) = 0;
        virtual bool has_attrstring( PyObject * _obj, const char * _attr ) = 0;
        virtual PyObject * get_attrstring( PyObject * _obj, const char * _attr ) = 0;

        virtual string_view object_str( PyObject * _obj ) = 0;
        virtual string_view object_repr( PyObject * _obj ) = 0;
        virtual string_view object_repr_type( PyObject * _obj ) = 0;

        virtual bool extract_bool( PyObject * _obj, bool & _value ) = 0;
        virtual bool extract_int8( PyObject * _obj, int8_t & _value ) = 0;
        virtual bool extract_int16( PyObject * _obj, int16_t & _value ) = 0;
        virtual bool extract_int32( PyObject * _obj, int32_t & _value ) = 0;
        virtual bool extract_int64( PyObject * _obj, int64_t & _value ) = 0;
        virtual bool extract_uint8( PyObject * _obj, uint8_t & _value ) = 0;
        virtual bool extract_uint16( PyObject * _obj, uint16_t & _value ) = 0;
        virtual bool extract_uint32( PyObject * _obj, uint32_t & _value ) = 0;
        virtual bool extract_uint64( PyObject * _obj, uint64_t & _value ) = 0;
        virtual bool extract_float( PyObject * _obj, float & _value ) = 0;
        virtual bool extract_double( PyObject * _obj, double & _value ) = 0;
        virtual bool extract_wchar( PyObject * _obj, wchar_t & _value ) = 0;

        virtual PyObject * ptr_bool( bool _value ) = 0;
        virtual PyObject * ptr_int8( int8_t _value ) = 0;
        virtual PyObject * ptr_int16( int16_t _value ) = 0;
        virtual PyObject * ptr_int32( int32_t _value ) = 0;
        virtual PyObject * ptr_int64( int64_t _value ) = 0;
        virtual PyObject * ptr_uint8( uint8_t _value ) = 0;
        virtual PyObject * ptr_uint16( uint16_t _value ) = 0;
        virtual PyObject * ptr_uint32( uint32_t _value ) = 0;
        virtual PyObject * ptr_uint64( uint64_t _value ) = 0;
        virtual PyObject * ptr_float( float _value ) = 0;
        virtual PyObject * ptr_double( double _value ) = 0;
        virtual PyObject * ptr_long( long _value ) = 0;
        virtual PyObject * ptr_ulong( unsigned long _value ) = 0;
        virtual PyObject * ptr_string( const char * _str ) = 0;
        virtual PyObject * ptr_unicode( const wchar_t * _value ) = 0;

        virtual bool bool_check( PyObject * _obj ) const = 0;
        virtual bool is_true( PyObject * _obj ) const = 0;
        virtual bool is_false( PyObject * _obj ) const = 0;
        virtual PyObject * ret_bool( bool _value ) = 0;
        virtual PyObject * ret_true() = 0;
        virtual PyObject * ret_false() = 0;
        virtual PyObject * get_true() = 0;
        virtual PyObject * get_false() = 0;

        virtual bool int_check( PyObject * _obj ) const = 0;
        virtual bool long_check( PyObject * _obj ) const = 0;
        virtual bool float_check( PyObject * _obj ) const = 0;

        virtual bool string_check( PyObject * _string ) const = 0;
        virtual size_t string_size( PyObject * _string ) const = 0;
        virtual int64_t string_hash( PyObject * _string ) const = 0;
        virtual const char * string_to_char( PyObject * _string ) = 0;
        virtual const char * string_to_char_and_size( PyObject * _string, size_t * _size ) = 0;
        virtual PyObject * string_from_char( const char * _str ) = 0;
        virtual PyObject * string_from_char_size( const char * _str, size_t _size ) = 0;

        virtual bool bytearray_check( PyObject * _bytearray ) const = 0;
        virtual size_t bytearray_size( PyObject * _bytearray ) const = 0;
        virtual const uint8_t * bytearray_to_data( PyObject * _bytearray ) const = 0;
        virtual PyObject * bytearray_from_data( const uint8_t * _data, size_t _size ) = 0;

        virtual bool unicode_check( PyObject * _unicode ) const = 0;
        virtual const wchar_t * unicode_to_wchar( PyObject * _unicode ) = 0;
        virtual const wchar_t * unicode_to_wchar_and_size( PyObject * _unicode, size_t * _size ) = 0;
        virtual PyObject * unicode_from_wchar( const wchar_t * _value ) = 0;
        virtual PyObject * unicode_from_wchar_size( const wchar_t * _value, size_t _size ) = 0;
        virtual const char * unicode_to_utf8( PyObject * _unicode ) = 0;
        virtual const char * unicode_to_utf8_and_size( PyObject * _unicode, size_t * _size ) = 0;
        virtual PyObject * unicode_from_utf8( const char * _utf8 ) = 0;
        virtual PyObject * unicode_from_utf8_size( const char * _utf8, size_t _size ) = 0;

        virtual PyObject * list_new( size_t _size ) = 0;
        virtual bool list_check( PyObject * _list ) const = 0;
        virtual size_t list_size( PyObject * _list ) const = 0;
        virtual PyObject * list_getitem( PyObject * _list, size_t _it ) = 0;
        virtual bool list_insert( PyObject * _list, size_t _it, PyObject * _item ) = 0;
        virtual bool list_remove( PyObject * _list, size_t _it ) = 0;
        virtual bool list_setitem( PyObject * _list, size_t _it, PyObject * _item ) = 0;
        virtual bool list_appenditem( PyObject * _list, PyObject * _item ) = 0;

        virtual PyObject * dict_new() = 0;
        virtual PyObject * dict_new_presized( size_t _count ) = 0;
        virtual PyObject * dict_from( PyObject * _obj ) = 0;
        virtual bool dict_check( PyObject * _dict ) const = 0;
        virtual size_t dict_size( PyObject * _obj ) const = 0;
        virtual bool dict_set( PyObject * _dict, PyObject * _name, PyObject * _value ) = 0;
        virtual bool dict_setstring( PyObject * _dict, const char * _name, PyObject * _value ) = 0;
        virtual bool dict_remove( PyObject * _dict, PyObject * _name ) = 0;
        virtual bool dict_removestring( PyObject * _dict, const char * _name ) = 0;
        virtual PyObject * dict_getstring( PyObject * _dict, const char * _key ) = 0;
        virtual PyObject * dict_get( PyObject * _dict, PyObject * _name ) = 0;
        virtual bool dict_exist( PyObject * _dict, PyObject * _name ) = 0;
        virtual bool dict_existstring( PyObject * _dict, const char * _name ) = 0;
        virtual bool dict_next( PyObject * _dict, size_t * const _pos, PyObject ** _key, PyObject ** _value ) = 0;

        virtual PyObject * set_new() = 0;
        virtual bool set_check( PyObject * _dict ) const = 0;
        virtual size_t set_size( PyObject * _obj ) const = 0;
        virtual bool set_set( PyObject * _dict, PyObject * _value ) = 0;
        virtual bool set_remove( PyObject * _dict, PyObject * _value ) = 0;
        virtual bool set_exist( PyObject * _dict, PyObject * _value ) = 0;

        virtual bool iterator_get( PyObject * _collections, PyObject ** _iterator ) = 0;
        virtual bool iterator_next( PyObject * _iterator, PyObject ** _value ) = 0;
        virtual void iterator_end( PyObject * _iterator ) = 0;

        virtual PyObject * tuple_new( size_t _it ) = 0;
        virtual bool tuple_check( PyObject * _tuple ) const = 0;
        virtual size_t tuple_size( PyObject * _tuple ) const = 0;
        virtual PyObject * tuple_getitem( PyObject * _tuple, size_t _it ) = 0;
        virtual PyObject * tuple_slice( PyObject * _tuple, size_t _low, size_t _high ) = 0;
        virtual PyObject * tuple_slice_tail( PyObject * _tuple, size_t _size ) = 0;
        virtual bool tuple_setitem( PyObject * _tuple, size_t _it, PyObject * _value ) = 0;

        virtual PyObject * exception_new( const char * _msg ) = 0;
    };
}
