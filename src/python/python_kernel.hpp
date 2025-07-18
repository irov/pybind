#pragma once

#include "pybind/kernel_interface.hpp"

#include "config/python.hpp"

#include "python_function.hpp"
#include "python_functor.hpp"
#include "python_member.hpp"
#include "python_method.hpp"
#include "python_pod.hpp"

namespace pybind
{
    class python_kernel
        : public kernel_interface
    {
    public:
        python_kernel();
        ~python_kernel() override;

    public:
        bool initialize( allocator_interface * _allocator );
        void finalize();

    public:
        allocator_interface * get_allocator() const override;

    public:
        void collect() override;
        void destroy() override;

    public:
        void remove_from_module( const char * _name, PyObject * _module ) override;

    public:
        void def_function_adapter( const function_adapter_interface_ptr & _adapter, bool _native, PyObject * _module ) override;
        void def_functor_adapter( const functor_adapter_interface_ptr & _adapter, bool _native, PyObject * _module ) override;

        PyObject * create_function_adapter( const function_adapter_interface_ptr & _adapter, bool _native ) override;
        PyObject * create_functor_adapter( const functor_adapter_interface_ptr & _adapter, bool _native ) override;

        PyObject * create_member_adapter( const member_adapter_interface_ptr & _iadapter ) override;
        PyObject * create_method_adapter( const method_adapter_interface_ptr & _iadapter, PyTypeObject * _type ) override;
        method_adapter_interface * get_method_adapter( PyObject * _obj ) override;

    public:
        PyTypeObject * get_pod_type( size_t _pod, bool _hash ) override;
        bool is_object_bindable( PyObject * _obj ) override;

    public:
        void set_current_module( PyObject * _module ) override;
        PyObject * get_current_module() const override;

        observer_bind_call * set_observer_bind_call( observer_bind_call * _observer ) override;
        observer_bind_call * get_observer_bind_call() override;

    public:
        void * unwrap( PyObject * _obj ) override;
        bool is_wrap( PyObject * _obj ) override;

        bool is_class( PyObject * _obj ) override;
        bool is_type_class( PyTypeObject * _type ) override;

        typeid_t get_class_type_id( const std::type_info & _info ) override;
        const char * get_class_type_info( typeid_t _id ) override;

        typeid_t find_class_info_desc_name( const char * _name ) override;

        void register_type_info_extract( typeid_t _info, const type_cast_ptr & _cast ) override;
        void unregister_type_info_extract( typeid_t _info ) override;
        type_cast * find_type_info_extract( typeid_t _info ) override;

        class_type_scope_interface_ptr create_new_type_scope( typeid_t _info, const char * _name, void * _user, const new_adapter_interface_ptr & _pynew, const destroy_adapter_interface_ptr & _pydestructor, size_t _pod, bool _hash ) override;
        void remove_type_scope( typeid_t _info ) override;

        bool has_class_type_scope( typeid_t _info ) override;
        const class_type_scope_interface_ptr & get_class_type_scope( typeid_t _info ) override;

        void visit_types_scope( visitor_class_type_scope * _getter ) override;

        PyTypeObject * get_object_type( PyObject * _type ) override;
        typeid_t get_object_type_id( PyObject * _type ) override;

        const char * object_type_name( PyObject * _type ) override;
        const char * type_name( PyTypeObject * _type ) override;

        const class_type_scope_interface_ptr & get_class_scope( PyTypeObject * _type ) override;
        void cache_class_scope_type( const class_type_scope_interface_ptr & _scope ) override;
        void remove_class_scope_type( const class_type_scope_interface_ptr & _scope ) override;

        PyObject * scope_create_holder( const class_type_scope_interface_ptr & _scope, void * _ptr ) override;
        PyObject * scope_create_weak( const class_type_scope_interface_ptr & _scope, void * _ptr ) override;

        void * meta_cast_scope( void * _self, typeid_t _scope_name, typeid_t _class_name, const class_type_scope_interface_ptr & _scope ) override;

        void * check_registred_class( PyObject * _obj, typeid_t _info ) override;

        void error_invalid_extract( PyObject * _obj, typeid_t _tinfo ) override;
        bool instance_of_type( PyObject * _obj, typeid_t _tinfo ) override;

        void * get_class_impl( PyObject * _obj ) override;
        bool is_class_weak( PyObject * _obj ) override;

        bool type_initialize( PyObject * _obj ) override;

    public:
        uint32_t get_python_version() override;

        PyObject * get_builtins() override;

        void set_syspath( PyObject * _path ) override;

        PyObject * code_compile_file( const void * _buf, const char * _module ) override;
        bool code_check( PyObject * _code ) override;
        long marshal_magic_number() override;
        PyObject * marshal_get_object( const void * _buf, size_t _len ) override;

        void set_module_finder( PyObject * _finder ) override;
        void remove_module_finder() override;

        void set_sys_excepthook( pybind_excepthook_handler_f _excepthook, void * _ud ) override;

        void call_native( PyObject * _obj, PyObject * _args ) override;
        void call_method( PyObject * _obj, const char * _method, const char * _format, ... ) override;
        void call_method_native( PyObject * _obj, const char * _method, PyObject * _args ) override;

        PyObject * ask_native( PyObject * _obj, PyObject * _args ) override;
        PyObject * ask_method( PyObject * _obj, const char * _method, const char * _format, ... ) override;
        PyObject * ask_method_native( PyObject * _obj, const char * _method, PyObject * _args ) override;
        PyObject * ask_adapter( void * _self, const class_type_scope_interface_ptr & _scope, const char * _name, PyObject * _args ) override;

        PyObject * compile_string( const char * _string, const char * _file ) override;
        PyObject * eval_string( const char * _string, PyObject * _globals, PyObject * _locals ) override;
        PyObject * exec_file( const char * _code, PyObject * _globals, PyObject * _locals ) override;

        void setStdOutHandle( PyObject * _obj ) override;
        void setStdErrorHandle( PyObject * _obj ) override;
        PyObject * getStdOutHandle() override;
        PyObject * getStdErrorHandle() override;

    public:
        bool get_statetrace_top( char * const _filename, size_t _maxlenfilename, char * const _function, size_t _maxlenfunction, uint32_t * _lineno ) override;
        bool get_statetrace( char * _buffer, size_t _maxlen, bool _append ) override;
        void error_traceback( const char * _format, ... ) override;
        void error_traceback_va( const char * _format, va_list _va ) override;
        void error_message( const char * _format, ... ) override;
        void error_message_va( const char * _format, va_list _va ) override;
        void throw_message( const char * _format, ... ) override;
        void throw_message_va( const char * _format, va_list _va ) override;
        void warning_traceback( const char * _format, ... ) override;
        void warning_traceback_va( const char * _format, va_list _va ) override;
        PyObject * build_value( const char * _format, ... ) override;
        PyObject * build_value_va( const char * _format, va_list _va ) override;

        bool traceback_check( PyObject * _traceback ) override;
        string_view traceback_filename( PyObject * _traceback ) override;
        uint32_t traceback_lineno( PyObject * _traceback ) override;
        string_view traceback_function( PyObject * _traceback ) override;
        PyObject * traceback_next( PyObject * _traceback ) override;

        PyObject * module_import( const char * _name, bool & _exsist ) override;
        PyObject * module_init( const char * _name ) override;
        void module_fini( PyObject * _module ) override;
        PyObject * module_dict( PyObject * _module ) override;
        void module_addobject( PyObject * _module, const char * _name, PyObject * _obj ) override;
        PyObject * module_execcode( const char * _name, PyObject * _code ) override;
        PyObject * module_reload( PyObject * _module ) override;

        void incref( PyObject * _obj ) override;
        void decref( PyObject * _obj ) override;
        uint32_t refcount( PyObject * _obj ) override;

        bool is_none( PyObject * _obj ) const override;
        PyObject * get_none() override;
        PyObject * ret_none() override;

        PyObject * ret_not_implemented() override;

        bool is_callable( PyObject * _obj ) const override;
        bool is_instanceof( PyObject * _obj, PyTypeObject * _type ) const override;
        bool test_equal( PyObject * _lhs, PyObject * _rhs ) override;
        bool has_attr( PyObject * _obj, PyObject * _attr ) override;
        PyObject * get_attr( PyObject * _obj, PyObject * _attr ) override;
        bool set_attr( PyObject* _obj, PyObject* _attr, PyObject* _value ) override;
        bool set_attrstring( PyObject * _obj, const char * _attr, PyObject * _value ) override;
        bool has_attrstring( PyObject * _obj, const char * _attr ) override;
        PyObject * get_attrstring( PyObject * _obj, const char * _attr ) override;

        string_view object_str( PyObject * _obj ) override;
        string_view object_repr( PyObject * _obj ) override;
        string_view object_repr_type( PyObject * _obj ) override;

        bool extract_bool( PyObject * _obj, bool & _value ) override;
        bool extract_int8( PyObject * _obj, int8_t & _value ) override;
        bool extract_int16( PyObject * _obj, int16_t & _value ) override;
        bool extract_int32( PyObject * _obj, int32_t & _value ) override;
        bool extract_int64( PyObject * _obj, int64_t & _value ) override;
        bool extract_uint8( PyObject * _obj, uint8_t & _value ) override;
        bool extract_uint16( PyObject * _obj, uint16_t & _value ) override;
        bool extract_uint32( PyObject * _obj, uint32_t & _value ) override;
        bool extract_uint64( PyObject * _obj, uint64_t & _value ) override;
        bool extract_float( PyObject * _obj, float & _value ) override;
        bool extract_double( PyObject * _obj, double & _value ) override;
        bool extract_wchar( PyObject * _obj, wchar_t & _value ) override;

        PyObject * ptr_bool( bool _value ) override;
        PyObject * ptr_int8( int8_t _value ) override;
        PyObject * ptr_int16( int16_t _value ) override;
        PyObject * ptr_int32( int32_t _value ) override;
        PyObject * ptr_int64( int64_t _value ) override;
        PyObject * ptr_uint8( uint8_t _value ) override;
        PyObject * ptr_uint16( uint16_t _value ) override;
        PyObject * ptr_uint32( uint32_t _value ) override;
        PyObject * ptr_uint64( uint64_t _value ) override;
        PyObject * ptr_float( float _value ) override;
        PyObject * ptr_double( double _value ) override;
        PyObject * ptr_long( long _value ) override;
        PyObject * ptr_ulong( unsigned long _value ) override;
        PyObject * ptr_string( const char * _str ) override;
        PyObject * ptr_unicode( const wchar_t * _value ) override;

        bool bool_check( PyObject * _obj ) const override;
        bool is_true( PyObject * _obj ) const override;
        bool is_false( PyObject * _obj ) const override;
        PyObject * ret_bool( bool _value ) override;
        PyObject * ret_true() override;
        PyObject * ret_false() override;
        PyObject * get_true() override;
        PyObject * get_false() override;

        bool int_check( PyObject * _obj ) const override;
        bool long_check( PyObject * _obj ) const override;
        bool float_check( PyObject * _obj ) const override;        

        bool string_check( PyObject * _obj ) const override;
        size_t string_size( PyObject * _obj ) const override;
        int64_t string_hash( PyObject * _obj ) const override;
        const char * string_to_char( PyObject * _obj ) override;
        const char * string_to_char_and_size( PyObject * _obj, size_t * _size ) override;
        PyObject * string_from_char( const char * _str ) override;
        PyObject * string_from_char_size( const char * _str, size_t _size ) override;

        bool bytearray_check( PyObject * _bytearray ) const override;
        size_t bytearray_size( PyObject * _bytearray ) const override;
        const uint8_t * bytearray_to_data( PyObject * _bytearray ) const override;
        PyObject * bytearray_from_data( const uint8_t * _data, size_t _size ) override;

        bool unicode_check( PyObject * _obj ) const override;
        const wchar_t * unicode_to_wchar( PyObject * _obj ) override;
        const wchar_t * unicode_to_wchar_and_size( PyObject * _obj, size_t * _size ) override;
        PyObject * unicode_from_wchar( const wchar_t * _value ) override;
        PyObject * unicode_from_wchar_size( const wchar_t * _value, size_t _size ) override;
        const char * unicode_to_utf8( PyObject * _obj ) override;
        const char * unicode_to_utf8_and_size( PyObject * _obj, size_t * _size ) override;
        PyObject * unicode_from_utf8( const char * _utf8 ) override;
        PyObject * unicode_from_utf8_size( const char * _utf8, size_t _size ) override;

        PyObject * list_new( size_t _size ) override;
        bool list_check( PyObject * _obj ) const override;
        size_t list_size( PyObject * _obj ) const override;
        PyObject * list_getitem( PyObject * _obj, size_t _it ) override;
        bool list_insert( PyObject * _obj, size_t _it, PyObject * _item ) override;
        bool list_remove( PyObject * _obj, size_t _it ) override;
        bool list_setitem( PyObject * _obj, size_t _it, PyObject * _item ) override;
        bool list_appenditem( PyObject * _obj, PyObject * _item ) override;

        PyObject * dict_new() override;
        PyObject * dict_new_presized( size_t _count ) override;
        PyObject * dict_from( PyObject * _obj ) override;
        bool dict_check( PyObject * _obj ) const override;
        size_t dict_size( PyObject * _obj ) const override;
        bool dict_set( PyObject * _obj, PyObject * _name, PyObject * _value ) override;
        bool dict_setstring( PyObject * _obj, const char * _name, PyObject * _value ) override;
        bool dict_remove( PyObject * _obj, PyObject * _name ) override;
        bool dict_removestring( PyObject * _obj, const char * _name ) override;
        PyObject * dict_getstring( PyObject * _obj, const char * _key ) override;
        PyObject * dict_get( PyObject * _obj, PyObject * _name ) override;
        bool dict_exist( PyObject * _obj, PyObject * _name ) override;
        bool dict_existstring( PyObject * _obj, const char * _name ) override;
        bool dict_next( PyObject * _obj, size_t * const _pos, PyObject ** _key, PyObject ** _value ) override;

        PyObject * set_new() override;
        bool set_check( PyObject * _dict ) const override;
        size_t set_size( PyObject * _obj ) const override;
        bool set_set( PyObject * _dict, PyObject * _value ) override;
        bool set_remove( PyObject * _dict, PyObject * _value ) override;
        bool set_exist( PyObject * _dict, PyObject * _value ) override;

        bool iterator_get( PyObject * _collections, PyObject ** _iterator ) override;
        bool iterator_next( PyObject * _iterator, PyObject ** _value ) override;
        void iterator_end( PyObject * _iterator ) override;

        PyObject * tuple_new( size_t _it ) override;
        bool tuple_check( PyObject * _obj ) const override;
        size_t tuple_size( PyObject * _obj ) const override;
        PyObject * tuple_getitem( PyObject * _obj, size_t _it ) override;
        PyObject * tuple_slice( PyObject * _obj, size_t _low, size_t _high ) override;
        PyObject * tuple_slice_tail( PyObject * _obj, size_t _size ) override;
        bool tuple_setitem( PyObject * _obj, size_t _it, PyObject * _value ) override;
        
        PyObject * exception_new( const char * _msg ) override;

    protected:
        allocator_interface * m_allocator;

        function_python m_functions;
        functor_python m_functors;
        member_python m_members;
        method_python m_methods;
        pod_python m_pods;

        PyObject * m_current_module;

        struct class_info_desc_t
        {
            const char * name;
        };

        class_info_desc_t m_class_info_desc[PYBIND_TYPE_COUNT];
        type_cast_ptr m_type_cast[PYBIND_TYPE_COUNT];
        class_type_scope_interface_ptr m_class_type_scopes[PYBIND_TYPE_COUNT];

        class_type_scope_interface_ptr m_class_type_dummy;
        class_type_scope_interface_ptr m_class_type_hashes[PYBIND_TYPE_COUNT_HASH];

        typeid_t m_enumerator;

    protected:
        bool set_class_info_desc( typeid_t _typeId, const char * _info );
        typeid_t get_next_id();
    };
    //////////////////////////////////////////////////////////////////////////
    typedef stdex::intrusive_ptr<python_kernel> python_kernel_ptr;
    //////////////////////////////////////////////////////////////////////////
}
