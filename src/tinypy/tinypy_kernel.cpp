#include "tinypy_kernel.hpp"

#include "tinypy_class_type_scope.hpp"

#include "pybind/debug.hpp"
#include "pybind/exception.hpp"
#include "pybind/function.hpp"
#include "pybind/adapter/new_adapter.hpp"
#include "pybind/adapter/destroy_adapter.hpp"

#include <cassert>
#include <climits>
#include <cstdio>
#include <cstring>
#include <cwchar>

namespace pybind
{
    //////////////////////////////////////////////////////////////////////////
    namespace detail
    {
        //////////////////////////////////////////////////////////////////////////
        static tinypy_value_t * value_cast( PyObject * _object )
        {
            return reinterpret_cast<tinypy_value_t *>(_object);
        }
        //////////////////////////////////////////////////////////////////////////
        static const tinypy_value_t * value_cast( const PyObject * _object )
        {
            return reinterpret_cast<const tinypy_value_t *>(_object);
        }
        //////////////////////////////////////////////////////////////////////////
        static PyObject * object_cast( tinypy_value_t * _value )
        {
            return reinterpret_cast<PyObject *>(_value);
        }
        //////////////////////////////////////////////////////////////////////////
        static tinypy_type_t * tinypy_type_cast( PyTypeObject * _type )
        {
            return reinterpret_cast<tinypy_type_t *>(_type);
        }
        //////////////////////////////////////////////////////////////////////////
        static const tinypy_type_t * tinypy_type_cast( const PyTypeObject * _type )
        {
            return reinterpret_cast<const tinypy_type_t *>(_type);
        }
        //////////////////////////////////////////////////////////////////////////
        static PyTypeObject * tinypy_type_cast( const tinypy_type_t * _type )
        {
            return reinterpret_cast<PyTypeObject *>(const_cast<tinypy_type_t *>(_type));
        }

        enum callable_kind_e
        {
            CallableFunction,
            CallableFunctor
        };

        struct callable_holder_t
        {
            tinypy_kernel * kernel;
            callable_kind_e kind;
            function_adapter_interface_ptr function;
            functor_adapter_interface_ptr functor;
        };

        struct excepthook_holder_t
        {
            tinypy_kernel * kernel;
            pybind_excepthook_handler_f handler;
            void * userData;
        };
        //////////////////////////////////////////////////////////////////////////
        static void * allocator_allocate( void * _userData, size_t _size, size_t _alignment, uint32_t _tag )
        {
            (void)_alignment;
            (void)_tag;

            allocator_interface * allocator = static_cast<allocator_interface *>(_userData);
            return allocator->malloc( _size );
        }
        //////////////////////////////////////////////////////////////////////////
        static void * allocator_reallocate( void * _userData, void * _memory, size_t _oldSize, size_t _newSize, size_t _alignment, uint32_t _tag )
        {
            (void)_oldSize;
            (void)_alignment;
            (void)_tag;

            allocator_interface * allocator = static_cast<allocator_interface *>(_userData);
            return allocator->realloc( _memory, _newSize );
        }
        //////////////////////////////////////////////////////////////////////////
        static void allocator_deallocate( void * _userData, void * _memory, size_t _size, size_t _alignment, uint32_t _tag )
        {
            (void)_size;
            (void)_alignment;
            (void)_tag;

            allocator_interface * allocator = static_cast<allocator_interface *>(_userData);
            allocator->free( _memory );
        }
        //////////////////////////////////////////////////////////////////////////
        static void finalize_callable( void * _userData )
        {
            callable_holder_t * holder = static_cast<callable_holder_t *>(_userData);
            allocator_interface * allocator = holder->kernel->get_allocator();
            allocator->deleteT( holder );
        }
        //////////////////////////////////////////////////////////////////////////
        static void finalize_excepthook( void * _userData )
        {
            excepthook_holder_t * holder = static_cast<excepthook_holder_t *>(_userData);
            allocator_interface * allocator = holder->kernel->get_allocator();
            allocator->deleteT( holder );
        }
        //////////////////////////////////////////////////////////////////////////
        static tinypy_value_t * call_excepthook( tinypy_value_t * _function, tinypy_value_t * _args, tinypy_value_t * _kwargs, void * _userData, tinypy_error_t ** _outError )
        {
            (void)_function;
            (void)_kwargs;
            (void)_outError;
            excepthook_holder_t * holder = static_cast<excepthook_holder_t *>(_userData);
            assert( tinypy_tuple_size( _args ) == 3 );
            PyObject * type = object_cast( tinypy_tuple_get( _args, 0 ) );
            PyObject * value = object_cast( tinypy_tuple_get( _args, 1 ) );
            PyObject * traceback = object_cast( tinypy_tuple_get( _args, 2 ) );
            holder->handler( holder->userData, reinterpret_cast<PyTypeObject *>(type), value, traceback );
            return tinypy_none_get( holder->kernel->vm() );
        }
        //////////////////////////////////////////////////////////////////////////
        static tinypy_value_t * call_adapter( tinypy_value_t * _function, tinypy_value_t * _args, tinypy_value_t * _kwargs, void * _userData, tinypy_error_t ** _outError )
        {
            (void)_function;
            callable_holder_t * holder = static_cast<callable_holder_t *>(_userData);

            try
            {
                PyObject * result = holder->kind == CallableFunction
                    ? holder->function->call( holder->kernel, object_cast( _args ), object_cast( _kwargs ) )
                    : holder->functor->call( holder->kernel, object_cast( _args ), object_cast( _kwargs ) );
                return value_cast( result );
            }
            catch( const pybind_exception & exception )
            {
                tinypy_vm_raise_error( holder->kernel->vm(), TINYPY_ERROR_RUNTIME, exception.what() );
                if( _outError != nullptr )
                {
                    *_outError = nullptr;
                }

                return nullptr;
            }
        }
        //////////////////////////////////////////////////////////////////////////
        static bool long_to_u64( const tinypy_value_t * _value, uint64_t * const _result )
        {
            int sign;
            size_t count;
            const uint16_t * digits = tinypy_long_base15_view( _value, &sign, &count );
            if( sign < 0 || count > 5 )
            {
                return false;
            }

            uint64_t value = 0;
            for( size_t index = count; index != 0; --index )
            {
                if( value > ( UINT64_MAX >> 15 ) )
                {
                    return false;
                }

                value = ( value << 15 ) | digits[index - 1];
            }
            *_result = value;
            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        static tinypy_value_t * u64_to_long( tinypy_vm_t * _vm, uint64_t _value )
        {
            uint16_t digits[5];
            size_t count = 0;
            while( _value != 0 )
            {
                digits[count++] = static_cast<uint16_t>(_value & UINT64_C( 0x7fff));
                _value >>= 15;
            }

            return tinypy_long_from_base15_digits( _vm, count == 0 ? 0 : 1, digits, count );
        }
        //////////////////////////////////////////////////////////////////////////
        static bool decode_utf8( const char * _bytes, size_t _size, wchar_t * const _result, size_t _capacity, size_t * const _resultSize )
        {
            assert( _capacity >= _size + 1 );

            size_t resultSize = 0;

            for( size_t index = 0; index != _size; )
            {
                uint32_t code;
                unsigned char first = static_cast<unsigned char>(_bytes[index++]);
                size_t continuation;
                if( first < 0x80 )
                {
                    code = first;
                    continuation = 0;
                }
                else if( ( first & 0xe0 ) == 0xc0 )
                {
                    code = first & 0x1f;
                    continuation = 1;
                }
                else if( ( first & 0xf0 ) == 0xe0 )
                {
                    code = first & 0x0f;
                    continuation = 2;
                }
                else if( ( first & 0xf8 ) == 0xf0 )
                {
                    code = first & 0x07;
                    continuation = 3;
                }
                else
                {
                    return false;
                }

                if( continuation > _size - index )
                {
                    return false;
                }

                for( size_t part = 0; part != continuation; ++part )
                {
                    unsigned char byte = static_cast<unsigned char>(_bytes[index++]);
                    if( ( byte & 0xc0 ) != 0x80 )
                    {
                        return false;
                    }

                    code = ( code << 6 ) | ( byte & 0x3f );
                }
#if WCHAR_MAX <= 0xffff
                if( code > 0xffff )
                {
                    code -= 0x10000;
                    _result[resultSize++] = static_cast<wchar_t>(0xd800 + ( code >> 10));
                    _result[resultSize++] = static_cast<wchar_t>(0xdc00 + ( code & 0x3ff));
                }
                else
#endif
                {
                    _result[resultSize++] = static_cast<wchar_t>(code);
                }
            }

            _result[resultSize] = L'\0';
            *_resultSize = resultSize;

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        static size_t encode_utf8( const wchar_t * _value, size_t _size, char * const _result, size_t _capacity )
        {
            assert( _capacity >= _size * 4 + 1 );

            size_t resultSize = 0;

            for( size_t index = 0; index != _size; ++index )
            {
                uint32_t code = static_cast<uint32_t>(_value[index]);
#if WCHAR_MAX <= 0xffff
                if( code >= 0xd800 && code <= 0xdbff && index + 1 < _size )
                {
                    uint32_t low = static_cast<uint32_t>(_value[index + 1]);
                    if( low >= 0xdc00 && low <= 0xdfff )
                    {
                        code = 0x10000 + ( ( code - 0xd800 ) << 10 ) + ( low - 0xdc00 );
                        ++index;
                    }
                }
#endif
                if( code < 0x80 )
                {
                    _result[resultSize++] = static_cast<char>(code);
                }
                else if( code < 0x800 )
                {
                    _result[resultSize++] = static_cast<char>(0xc0 | ( code >> 6));
                    _result[resultSize++] = static_cast<char>(0x80 | ( code & 0x3f));
                }
                else if( code < 0x10000 )
                {
                    _result[resultSize++] = static_cast<char>(0xe0 | ( code >> 12));
                    _result[resultSize++] = static_cast<char>(0x80 | ( ( code >> 6 ) & 0x3f));
                    _result[resultSize++] = static_cast<char>(0x80 | ( code & 0x3f));
                }
                else
                {
                    _result[resultSize++] = static_cast<char>(0xf0 | ( code >> 18));
                    _result[resultSize++] = static_cast<char>(0x80 | ( ( code >> 12 ) & 0x3f));
                    _result[resultSize++] = static_cast<char>(0x80 | ( ( code >> 6 ) & 0x3f));
                    _result[resultSize++] = static_cast<char>(0x80 | ( code & 0x3f));
                }
            }

            _result[resultSize] = '\0';

            return resultSize;
        }
        //////////////////////////////////////////////////////////////////////////
    }
    //////////////////////////////////////////////////////////////////////////
    tinypy_kernel::tinypy_kernel()
        : m_allocator( nullptr )
        , m_optimize_level( 0 )
        , m_vm( nullptr )
        , m_current_module( nullptr )
        , m_excepthook( nullptr )
        , m_stdout( nullptr )
        , m_stderr( nullptr )
        , m_observer( nullptr )
        , m_enumerator( 4 )
        , m_retired_scope_count( 0 )
        , m_unicode_cache( nullptr )
        , m_unicode_cache_size( 0 )
        , m_unicode_cache_capacity( 0 )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool tinypy_kernel::initialize( allocator_interface * _allocator, const kernel_config_t & _config )
    {
        m_allocator = _allocator;
        m_optimize_level = _config.optimize_level;

        tinypy_allocator_t allocator{};
        allocator.abi_version = TINYPY_ABI_VERSION;
        allocator.struct_size = sizeof( allocator );
        allocator.user_data = _allocator;
        allocator.allocate = &detail::allocator_allocate;
        allocator.reallocate = &detail::allocator_reallocate;
        allocator.deallocate = &detail::allocator_deallocate;

        tinypy_vm_config_t config{};
        config.abi_version = TINYPY_ABI_VERSION;
        config.struct_size = sizeof( config );
        config.allocator = &allocator;
        config.host = nullptr;
        config.max_heap_bytes = _config.max_heap_bytes;
        config.feature_flags = _config.feature_flags;
        config.optimize_level = _config.optimize_level;
        m_vm = tinypy_vm_create( &config );

        if( m_vm == nullptr )
        {
            return false;
        }

        for( typeid_t id = 0; id != PYBIND_TYPE_COUNT; ++id )
        {
            m_class_names[id][0] = '\0';
            m_type_casts[id] = nullptr;
            m_scopes[id] = nullptr;
            m_retired_scopes[id] = nullptr;
        }

        m_dummy_scope = m_allocator->newT<tinypy_class_type_scope>( this, "__dummy__", 0, nullptr, nullptr, nullptr, 0, false );
        tinypy_value_t * none = tinypy_none_get( m_vm );
        m_excepthook = detail::object_cast( none );
        bool sysExists;
        PyObject * sys = this->module_import( "sys", sysExists );
        assert( sys != nullptr );
        m_stdout = this->get_attrstring( sys, "stdout" );
        m_stderr = this->get_attrstring( sys, "stderr" );
        this->decref( sys );
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    tinypy_vm_t * tinypy_kernel::vm() const
    {
        return m_vm;
    }
    //////////////////////////////////////////////////////////////////////////
    allocator_interface * tinypy_kernel::get_allocator() const
    {
        return m_allocator;
    }
    //////////////////////////////////////////////////////////////////////////
    void tinypy_kernel::collect()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void tinypy_kernel::update_main_thread()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void tinypy_kernel::destroy()
    {
        for( class_type_scope_interface_ptr & scope : m_scopes )
        {
            if( scope != nullptr )
            {
                scope->finalize();
            }
        }

        if( m_current_module != nullptr )
        {
            this->decref( m_current_module );
        }

        if( m_excepthook != nullptr )
        {
            this->decref( m_excepthook );
        }

        if( m_stdout != nullptr )
        {
            this->decref( m_stdout );
        }

        if( m_stderr != nullptr )
        {
            this->decref( m_stderr );
        }

        tinypy_vm_destroy( m_vm );
        m_vm = nullptr;

        kernel_interface * currentKernel = pybind::get_kernel();

        if( currentKernel == this )
        {
            pybind::set_kernel( nullptr );
        }

        for( typeid_t id = 0; id != PYBIND_TYPE_COUNT; ++id )
        {
            m_scopes[id] = nullptr;
            m_retired_scopes[id] = nullptr;
        }

        m_dummy_scope = nullptr;

        if( m_unicode_cache != nullptr )
        {
            m_allocator->free( m_unicode_cache );
            m_unicode_cache = nullptr;
            m_unicode_cache_size = 0;
            m_unicode_cache_capacity = 0;
        }

        m_allocator->deleteT( this );
    }
    //////////////////////////////////////////////////////////////////////////
    void tinypy_kernel::remove_from_module( const char * _name, PyObject * _module )
    {
        PyObject * module = _module != nullptr ? _module : m_current_module;
        if( module == nullptr )
        {
            return;
        }

        tinypy_value_t * key = tinypy_string_from_bytes( m_vm, _name, std::strlen( _name ) );
        tinypy_value_t * dict = tinypy_module_dict( detail::value_cast( module ) );

        if( tinypy_dict_contains( dict, key ) != 0 )
        {
            tinypy_dict_delete( dict, key );
        }

        tinypy_release( key );
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * tinypy_kernel::create_function_adapter( const function_adapter_interface_ptr & _adapter, bool _native )
    {
        (void)_native;
        detail::callable_holder_t * holder = m_allocator->newP<detail::callable_holder_t>( detail::callable_holder_t{ this, detail::CallableFunction, _adapter, nullptr } );
        return detail::object_cast( tinypy_native_function_new( m_vm, _adapter->getName(), std::strlen( _adapter->getName() ), &detail::call_adapter, holder, &detail::finalize_callable ) );
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * tinypy_kernel::create_functor_adapter( const functor_adapter_interface_ptr & _adapter, bool _native )
    {
        (void)_native;
        detail::callable_holder_t * holder = m_allocator->newP<detail::callable_holder_t>( detail::callable_holder_t{ this, detail::CallableFunctor, nullptr, _adapter } );
        return detail::object_cast( tinypy_native_function_new( m_vm, _adapter->getName(), std::strlen( _adapter->getName() ), &detail::call_adapter, holder, &detail::finalize_callable ) );
    }
    //////////////////////////////////////////////////////////////////////////
    void tinypy_kernel::def_function_adapter( const function_adapter_interface_ptr & _adapter, bool _native, PyObject * _module )
    {
        PyObject * function = this->create_function_adapter( _adapter, _native );
        PyObject * module = _module != nullptr ? _module : m_current_module;
        const char * name = _adapter->getName();
        this->module_addobject( module, name, function );
    }
    //////////////////////////////////////////////////////////////////////////
    void tinypy_kernel::def_functor_adapter( const functor_adapter_interface_ptr & _adapter, bool _native, PyObject * _module )
    {
        PyObject * function = this->create_functor_adapter( _adapter, _native );
        PyObject * module = _module != nullptr ? _module : m_current_module;
        const char * name = _adapter->getName();
        this->module_addobject( module, name, function );
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * tinypy_kernel::create_member_adapter( const member_adapter_interface_ptr & _adapter )
    {
        (void)_adapter;
        this->set_error( error_type_e::Runtime, "standalone member descriptors require a class scope" );
        return nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * tinypy_kernel::create_method_adapter( const method_adapter_interface_ptr & _adapter, PyTypeObject * _type )
    {
        (void)_adapter;
        (void)_type;
        this->set_error( error_type_e::Runtime, "standalone method descriptors require a class scope" );
        return nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    method_adapter_interface * tinypy_kernel::get_method_adapter( PyObject * _object )
    {
        (void)_object;
        return nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    PyTypeObject * tinypy_kernel::get_pod_type( size_t _pod, bool _hash )
    {
        (void)_pod;
        (void)_hash;
        return nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    bool tinypy_kernel::is_object_bindable( PyObject * _object )
    {
        const class_type_scope_interface_ptr & scope = this->get_class_scope( this->get_object_type( _object ) );
        return scope != nullptr && scope->get_bindable() != nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    void tinypy_kernel::set_current_module( PyObject * _module )
    {
        if( _module != nullptr )
        {
            this->incref( _module );
        }

        if( m_current_module != nullptr )
        {
            this->decref( m_current_module );
        }

        m_current_module = _module;
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * tinypy_kernel::get_current_module() const
    {
        return m_current_module;
    }
    //////////////////////////////////////////////////////////////////////////
    observer_bind_call * tinypy_kernel::set_observer_bind_call( observer_bind_call * _observer )
    {
        observer_bind_call * old = m_observer;
        m_observer = _observer;
        return old;
    }
    //////////////////////////////////////////////////////////////////////////
    observer_bind_call * tinypy_kernel::get_observer_bind_call()
    {
        return m_observer;
    }
    //////////////////////////////////////////////////////////////////////////
    void * tinypy_kernel::unwrap( PyObject * _object )
    {
        if( this->is_wrap( _object ) == false )
        {
            return nullptr;
        }

        tinypy_class_type_scope::payload_t * payload = static_cast<tinypy_class_type_scope *>(this->get_class_scope( this->get_object_type( _object ) ).get())->payload( _object );
        payload->flags |= tinypy_class_type_scope::PayloadUnwrapped;
        void * impl = payload->impl;
        payload->impl = nullptr;
        return impl;
    }
    //////////////////////////////////////////////////////////////////////////
    bool tinypy_kernel::is_wrap( PyObject * _object )
    {
        return tinypy_typeof( detail::value_cast( _object ) ) == TINYPY_VALUE_NATIVE_INSTANCE;
    }
    //////////////////////////////////////////////////////////////////////////
    bool tinypy_kernel::is_class( PyObject * _object )
    {
        return this->is_wrap( _object );
    }
    //////////////////////////////////////////////////////////////////////////
    bool tinypy_kernel::is_type_class( PyTypeObject * _type )
    {
        return this->get_class_scope( _type )->get_type_id() != 0;
    }
    //////////////////////////////////////////////////////////////////////////
    bool tinypy_kernel::set_class_info_desc_( typeid_t _id, const char * _name )
    {
        if( _id >= PYBIND_TYPE_COUNT )
        {
            return false;
        }
        ::strncpy( m_class_names[_id], _name, PYBIND_CLASS_TYPE_MAX_NAME );
        m_class_names[_id][PYBIND_CLASS_TYPE_MAX_NAME] = '\0';
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    typeid_t tinypy_kernel::next_type_id_()
    {
        assert( m_enumerator < PYBIND_TYPE_COUNT );
        return m_enumerator++;
    }
    //////////////////////////////////////////////////////////////////////////
    typeid_t tinypy_kernel::get_class_type_id( const std::type_info & _info )
    {
        const char * name = _info.name();
        typeid_t found = this->find_class_info_desc_name( name );
        if( found != 0 )
        {
            return found;
        }

        typeid_t id = this->next_type_id_();
        this->set_class_info_desc_( id, name );
        return id;
    }
    //////////////////////////////////////////////////////////////////////////
    const char * tinypy_kernel::get_class_type_info( typeid_t _id )
    {
        return _id < PYBIND_TYPE_COUNT ? m_class_names[_id] : "";
    }
    //////////////////////////////////////////////////////////////////////////
    const char * tinypy_kernel::object_type_name( PyObject * _object )
    {
        return this->type_name( this->get_object_type( _object ) );
    }
    //////////////////////////////////////////////////////////////////////////
    const char * tinypy_kernel::type_name( PyTypeObject * _type )
    {
        size_t size;
        return tinypy_type_name( detail::tinypy_type_cast( _type ), &size );
    }
    //////////////////////////////////////////////////////////////////////////
    typeid_t tinypy_kernel::find_class_info_desc_name( const char * _name )
    {
        for( typeid_t id = 1; id != m_enumerator; ++id )
        {
            if( ::strcmp( m_class_names[id], _name ) == 0 )
            {
                return id;
            }
        }

        return 0;
    }
    //////////////////////////////////////////////////////////////////////////
    void tinypy_kernel::register_type_info_extract( typeid_t _id, const type_cast_ptr & _cast )
    {
        assert( _id < PYBIND_TYPE_COUNT );
        m_type_casts[_id] = _cast;
    }
    //////////////////////////////////////////////////////////////////////////
    void tinypy_kernel::unregister_type_info_extract( typeid_t _id )
    {
        assert( _id < PYBIND_TYPE_COUNT );
        m_type_casts[_id] = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    type_cast * tinypy_kernel::find_type_info_extract( typeid_t _id )
    {
        return _id < PYBIND_TYPE_COUNT ? m_type_casts[_id].get() : nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    class_type_scope_interface_ptr tinypy_kernel::create_new_type_scope( typeid_t _info, const char * _name, void * _user, const new_adapter_interface_ptr & _new, const destroy_adapter_interface_ptr & _destroy, size_t _pod, bool _hash )
    {
        class_type_scope_interface_ptr scope = m_allocator->newT<tinypy_class_type_scope>( this, _name, _info, _user, _new, _destroy, _pod, _hash );
        m_scopes[_info] = scope;
        return scope;
    }
    //////////////////////////////////////////////////////////////////////////
    void tinypy_kernel::remove_type_scope( typeid_t _info )
    {
        if( _info >= PYBIND_TYPE_COUNT || m_scopes[_info] == nullptr )
        {
            return;
        }

        class_type_scope_interface_ptr scope = m_scopes[_info];
        scope->finalize();
        m_scopes[_info] = nullptr;
        assert( m_retired_scope_count != PYBIND_TYPE_COUNT );
        m_retired_scopes[m_retired_scope_count++] = scope;
    }
    //////////////////////////////////////////////////////////////////////////
    bool tinypy_kernel::has_class_type_scope( typeid_t _info )
    {
        return _info < PYBIND_TYPE_COUNT && m_scopes[_info] != nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    const class_type_scope_interface_ptr & tinypy_kernel::get_class_type_scope( typeid_t _info )
    {
        return _info < PYBIND_TYPE_COUNT ? m_scopes[_info] : m_dummy_scope;
    }
    //////////////////////////////////////////////////////////////////////////
    void tinypy_kernel::visit_types_scope( visitor_class_type_scope * _visitor )
    {
        for( class_type_scope_interface_ptr & scope : m_scopes )
        {
            if( scope != nullptr )
            {
                _visitor->visit_scope( scope.get() );
            }
        }
    }
    //////////////////////////////////////////////////////////////////////////
    PyTypeObject * tinypy_kernel::get_object_type( PyObject * _object )
    {
        return detail::tinypy_type_cast( tinypy_object_type( detail::value_cast( _object ) ) );
    }
    //////////////////////////////////////////////////////////////////////////
    typeid_t tinypy_kernel::get_object_type_id( PyObject * _object )
    {
        PyTypeObject * type = this->get_object_type( _object );
        const class_type_scope_interface_ptr & scope = this->get_class_scope( type );
        return scope != nullptr ? scope->get_type_id() : 0;
    }
    //////////////////////////////////////////////////////////////////////////
    const class_type_scope_interface_ptr & tinypy_kernel::get_class_scope( PyTypeObject * _type )
    {
        const tinypy_type_t * type = detail::tinypy_type_cast( _type );
        size_t mroCount = tinypy_type_mro_size( type );

        for( size_t mroIndex = 0; mroIndex != mroCount; ++mroIndex )
        {
            const tinypy_type_t * mroType = tinypy_type_mro_at( type, mroIndex );

            for( class_type_scope_interface_ptr & scope : m_scopes )
            {
                if( scope != nullptr && reinterpret_cast<const tinypy_type_t *>(scope->get_typeobject()) == mroType )
                {
                    return scope;
                }
            }
        }

        return m_dummy_scope;
    }
    //////////////////////////////////////////////////////////////////////////
    void tinypy_kernel::cache_class_scope_type( const class_type_scope_interface_ptr & _scope )
    {
        (void)_scope;
    }
    //////////////////////////////////////////////////////////////////////////
    void tinypy_kernel::remove_class_scope_type( const class_type_scope_interface_ptr & _scope )
    {
        (void)_scope;
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * tinypy_kernel::scope_create_holder( const class_type_scope_interface_ptr & _scope, void * _ptr )
    {
        return _scope->create_holder( _ptr );
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * tinypy_kernel::scope_create_weak( const class_type_scope_interface_ptr & _scope, void * _ptr )
    {
        return _scope->create_weak( _ptr );
    }
    //////////////////////////////////////////////////////////////////////////
    void * tinypy_kernel::meta_cast_scope( void * _self, typeid_t _scopeName, typeid_t _className, const class_type_scope_interface_ptr & _scope )
    {
        (void)_className;
        return _scope->meta_cast( _scopeName, _self );
    }
    //////////////////////////////////////////////////////////////////////////
    void * tinypy_kernel::check_registred_class( PyObject * _object, typeid_t _info )
    {
        if( this->instance_of_type( _object, _info ) == false )
        {
            return nullptr;
        }

        const class_type_scope_interface_ptr & scope = this->get_class_scope( this->get_object_type( _object ) );
        return scope->meta_cast( _info, this->get_class_impl( _object ) );
    }
    //////////////////////////////////////////////////////////////////////////
    void tinypy_kernel::error_invalid_extract( PyObject * _object, typeid_t _info )
    {
        char message[PYBIND_EXCEPTION_MESSAGE_SIZE + 1];
        ::snprintf( message, sizeof( message ), "invalid extract from %s to %s", this->object_type_name( _object ), this->get_class_type_info( _info ) );
        this->set_error( error_type_e::Type, message );
    }
    //////////////////////////////////////////////////////////////////////////
    bool tinypy_kernel::instance_of_type( PyObject * _object, typeid_t _info )
    {
        return _info < PYBIND_TYPE_COUNT && m_scopes[_info] != nullptr && m_scopes[_info]->is_instance( this->get_object_type( _object ) );
    }
    //////////////////////////////////////////////////////////////////////////
    void * tinypy_kernel::get_class_impl( PyObject * _object )
    {
        if( this->is_wrap( _object ) == false )
        {
            return nullptr;
        }

        const class_type_scope_interface_ptr & scope = this->get_class_scope( this->get_object_type( _object ) );
        if( scope == nullptr || scope->get_type_id() == 0 )
        {
            return nullptr;
        }

        return static_cast<tinypy_class_type_scope *>(scope.get())->payload( _object )->impl;
    }
    //////////////////////////////////////////////////////////////////////////
    bool tinypy_kernel::is_class_weak( PyObject * _object )
    {
        if( this->is_wrap( _object ) == false )
        {
            return false;
        }

        const class_type_scope_interface_ptr & scope = this->get_class_scope( this->get_object_type( _object ) );
        return scope != nullptr && scope->get_type_id() != 0 && ( static_cast<tinypy_class_type_scope *>(scope.get())->payload( _object )->flags & tinypy_class_type_scope::PayloadWeak ) != 0;
    }
    //////////////////////////////////////////////////////////////////////////
    bool tinypy_kernel::type_initialize( PyObject * _object )
    {
        const class_type_scope_interface_ptr & scope = this->get_class_scope( this->get_object_type( _object ) );
        if( scope == nullptr )
        {
            return false;
        }

        scope->type_initialize( this->get_object_type( _object ) );
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t tinypy_kernel::get_python_version()
    {
        return 278;
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * tinypy_kernel::get_builtins()
    {
        tinypy_value_t * name = tinypy_string_from_bytes( m_vm, "__builtin__", 11 );
        tinypy_value_t * modules = tinypy_vm_modules( m_vm );
        tinypy_value_t * module = tinypy_dict_get( modules, name );
        tinypy_release( name );
        return detail::object_cast( module );
    }
    //////////////////////////////////////////////////////////////////////////
    void tinypy_kernel::set_syspath( PyObject * _path )
    {
        bool exists;
        PyObject * sys = this->module_import( "sys", exists );
        if( sys != nullptr )
        {
            this->set_attrstring( sys, "path", _path );
            this->decref( sys );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * tinypy_kernel::code_compile_file( const void * _buffer, const char * _module )
    {
        return this->compile_string( static_cast<const char *>(_buffer), _module );
    }
    //////////////////////////////////////////////////////////////////////////
    bool tinypy_kernel::code_check( PyObject * _code )
    {
        return _code != nullptr && tinypy_typeof( detail::value_cast( _code ) ) == TINYPY_VALUE_CODE;
    }
    //////////////////////////////////////////////////////////////////////////
    long tinypy_kernel::marshal_magic_number()
    {
        return 62211L | ( static_cast<long>('\r') << 16 ) | ( static_cast<long>('\n') << 24 );
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * tinypy_kernel::marshal_get_object( const void * _buffer, size_t _size )
    {
        tinypy_value_t * code = nullptr;
        tinypy_marshal_error_t error{};
        if( tinypy_marshal_load_code_v2( m_vm, _buffer, _size, nullptr, &code, &error ) != TINYPY_MARSHAL_OK )
        {
            this->set_error( error_type_e::Value, "invalid marshal v2 code" );
            return nullptr;
        }

        return detail::object_cast( code );
    }
    //////////////////////////////////////////////////////////////////////////
    void tinypy_kernel::set_module_finder( PyObject * _finder )
    {
        tinypy_vm_set_module_finder( m_vm, detail::value_cast( _finder ) );
    }
    //////////////////////////////////////////////////////////////////////////
    void tinypy_kernel::remove_module_finder()
    {
        tinypy_vm_set_module_finder( m_vm, nullptr );
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * tinypy_kernel::get_sys_excepthook()
    {
        return m_excepthook;
    }
    //////////////////////////////////////////////////////////////////////////
    void tinypy_kernel::set_sys_excepthook( PyObject * _hook )
    {
        if( _hook != nullptr )
        {
            this->incref( _hook );
        }

        if( m_excepthook != nullptr )
        {
            this->decref( m_excepthook );
        }

        m_excepthook = _hook;
        bool exists;
        PyObject * sys = this->module_import( "sys", exists );
        if( sys != nullptr )
        {
            this->set_attrstring( sys, "excepthook", _hook );
            this->decref( sys );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void tinypy_kernel::set_sys_excepthook_f( pybind_excepthook_handler_f _handler, void * _ud )
    {
        detail::excepthook_holder_t * holder = m_allocator->newP<detail::excepthook_holder_t>( detail::excepthook_holder_t{ this, _handler, _ud } );
        PyObject * hook = detail::object_cast( tinypy_native_function_new( m_vm, "excepthook", 10, &detail::call_excepthook, holder, &detail::finalize_excepthook ) );
        this->set_sys_excepthook( hook );
        this->decref( hook );
    }
    //////////////////////////////////////////////////////////////////////////
    void tinypy_kernel::call_native( PyObject * _object, PyObject * _args )
    {
        PyObject * result = this->ask_native( _object, _args );
        if( result != nullptr )
        {
            this->decref( result );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void tinypy_kernel::call_method_native( PyObject * _object, const char * _method, PyObject * _args )
    {
        PyObject * result = this->ask_method_native( _object, _method, _args );
        if( result != nullptr )
        {
            this->decref( result );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * tinypy_kernel::ask_native( PyObject * _object, PyObject * _args )
    {
        tinypy_error_t * error = nullptr;
        tinypy_value_t * result = tinypy_call( detail::value_cast( _object ), detail::value_cast( _args ), nullptr, &error );
        if( result == nullptr )
        {
            this->report_error_( error );
        }

        return detail::object_cast( result );
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * tinypy_kernel::ask_method_native( PyObject * _object, const char * _method, PyObject * _args )
    {
        PyObject * method = this->get_attrstring( _object, _method );
        if( method == nullptr )
        {
            return nullptr;
        }

        PyObject * result = this->ask_native( method, _args );
        this->decref( method );
        return result;
    }
    //////////////////////////////////////////////////////////////////////////
    void tinypy_kernel::call_method( PyObject * _object, const char * _method, const char * _format, ... )
    {
        va_list args;
        va_start( args, _format );
        PyObject * values = this->build_value_va( _format, args );
        va_end( args );
        if( values != nullptr )
        {
            this->call_method_native( _object, _method, values );
            this->decref( values );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * tinypy_kernel::ask_method( PyObject * _object, const char * _method, const char * _format, ... )
    {
        va_list args;
        va_start( args, _format );
        PyObject * values = this->build_value_va( _format, args );
        va_end( args );
        if( values == nullptr )
        {
            return nullptr;
        }

        PyObject * result = this->ask_method_native( _object, _method, values );
        this->decref( values );
        return result;
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * tinypy_kernel::ask_adapter( void * _self, const class_type_scope_interface_ptr & _scope, const char * _name, PyObject * _args )
    {
        const method_adapter_interface_ptr & adapter = _scope->find_method( _name );
        return adapter != nullptr ? adapter->call( this, _self, _scope, _args, nullptr ) : nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * tinypy_kernel::compile_string( const char * _source, const char * _file )
    {
        size_t source_size = std::strlen( _source );
        tinypy_compile_options_t options;
        tinypy_compile_options_init( &options, TINYPY_COMPILE_EXEC );
        options.optimize_level = m_optimize_level;
        tinypy_error_t * error = nullptr;
        tinypy_value_t * code = tinypy_compile_source( m_vm, _source, source_size, _file, std::strlen( _file ), &options, &error );
        if( code == nullptr )
        {
            this->report_error_( error );
        }

        return detail::object_cast( code );
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * tinypy_kernel::eval_string( const char * _source, PyObject * _globals, PyObject * _locals )
    {
        size_t source_size = std::strlen( _source );
        tinypy_compile_options_t options;
        tinypy_compile_options_init( &options, TINYPY_COMPILE_EVAL );
        options.optimize_level = m_optimize_level;
        tinypy_error_t * error = nullptr;
        tinypy_value_t * result = tinypy_eval_source( m_vm, _source, source_size, "<eval>", 6, detail::value_cast( _globals ), detail::value_cast( _locals ), &options, &error );
        if( result == nullptr )
        {
            this->report_error_( error );
        }

        return detail::object_cast( result );
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * tinypy_kernel::exec_file( const char * _source, PyObject * _globals, PyObject * _locals )
    {
        size_t source_size = std::strlen( _source );
        tinypy_compile_options_t options;
        tinypy_compile_options_init( &options, TINYPY_COMPILE_EXEC );
        options.optimize_level = m_optimize_level;
        tinypy_error_t * error = nullptr;
        tinypy_value_t * result = tinypy_exec_source( m_vm, _source, source_size, "<exec>", 6, detail::value_cast( _globals ), detail::value_cast( _locals ), &options, &error );
        if( result == nullptr )
        {
            this->report_error_( error );
        }

        return detail::object_cast( result );
    }
    //////////////////////////////////////////////////////////////////////////
    void tinypy_kernel::setStdOutHandle( PyObject * _object )
    {
        if( _object != nullptr )
        {
            this->incref( _object );
        }

        if( m_stdout != nullptr )
        {
            this->decref( m_stdout );
        }

        m_stdout = _object;
        bool exists;
        PyObject * sys = this->module_import( "sys", exists );
        if( sys != nullptr )
        {
            this->set_attrstring( sys, "stdout", _object );
            this->decref( sys );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void tinypy_kernel::setStdErrorHandle( PyObject * _object )
    {
        if( _object != nullptr )
        {
            this->incref( _object );
        }

        if( m_stderr != nullptr )
        {
            this->decref( m_stderr );
        }

        m_stderr = _object;
        bool exists;
        PyObject * sys = this->module_import( "sys", exists );
        if( sys != nullptr )
        {
            this->set_attrstring( sys, "stderr", _object );
            this->decref( sys );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * tinypy_kernel::getStdOutHandle()
    {
        return m_stdout;
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * tinypy_kernel::getStdErrorHandle()
    {
        return m_stderr;
    }
    //////////////////////////////////////////////////////////////////////////
    bool tinypy_kernel::get_traceback_top( char * const _filename, size_t _filenameSize, char * const _function, size_t _functionSize, uint32_t * _line )
    {
        tinypy_value_t * traceback = tinypy_vm_raised_traceback( m_vm );
        if( traceback == nullptr )
        {
            return false;
        }

        string_view filename = this->traceback_filename( detail::object_cast( traceback ) );
        string_view function = this->traceback_function( detail::object_cast( traceback ) );
        std::snprintf( _filename, _filenameSize, "%s", filename.c_str() );
        std::snprintf( _function, _functionSize, "%s", function.c_str() );
        *_line = this->traceback_lineno( detail::object_cast( traceback ) );
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool tinypy_kernel::get_traceback( char * _buffer, size_t _size, bool _append )
    {
        char filename[512];
        char function[256];
        uint32_t line;
        if( this->get_traceback_top( filename, sizeof( filename ), function, sizeof( function ), &line ) == false )
        {
            return false;
        }

        size_t offset = _append == true ? std::strlen( _buffer ) : 0;
        std::snprintf( _buffer + offset, _size > offset ? _size - offset : 0, "%s:%u in %s", filename, line, function );
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void tinypy_kernel::error_traceback( const char * _format, ... )
    {
        va_list args;
        va_start( args, _format );
        this->error_traceback_va( _format, args );
        va_end( args );
    }
    //////////////////////////////////////////////////////////////////////////
    void tinypy_kernel::error_traceback_va( const char * _format, va_list _args )
    {
        this->error_message_va( _format, _args );
    }
    //////////////////////////////////////////////////////////////////////////
    void tinypy_kernel::error_message( const char * _format, ... )
    {
        va_list args;
        va_start( args, _format );
        this->error_message_va( _format, args );
        va_end( args );
    }
    //////////////////////////////////////////////////////////////////////////
    void tinypy_kernel::error_message_va( const char * _format, va_list _args )
    {
        char message[PYBIND_EXCEPTION_MESSAGE_SIZE + 1];
        std::vsnprintf( message, sizeof( message ), _format, _args );
        this->set_error( error_type_e::Runtime, message );
    }
    //////////////////////////////////////////////////////////////////////////
    void tinypy_kernel::set_error( error_type_e _type, const char * _message )
    {
        if( tinypy_vm_has_error( m_vm ) != 0 )
        {
            return;
        }

        tinypy_error_kind_e kind = TINYPY_ERROR_RUNTIME;
        if( _type == error_type_e::Type )
        {
            kind = TINYPY_ERROR_TYPE;
        }
        else if( _type == error_type_e::Value )
        {
            kind = TINYPY_ERROR_VALUE;
        }
        else if( _type == error_type_e::Overflow )
        {
            kind = TINYPY_ERROR_OVERFLOW;
        }

        tinypy_vm_raise_error( m_vm, kind, _message );
    }
    //////////////////////////////////////////////////////////////////////////
    void tinypy_kernel::throw_message( const char * _format, ... )
    {
        va_list args;
        va_start( args, _format );
        this->throw_message_va( _format, args );
        va_end( args );
    }
    //////////////////////////////////////////////////////////////////////////
    void tinypy_kernel::throw_message_va( const char * _format, va_list _args )
    {
        pybind::throw_exception_va( _format, _args );
    }
    //////////////////////////////////////////////////////////////////////////
    void tinypy_kernel::warning_traceback( const char * _format, ... )
    {
        va_list args;
        va_start( args, _format );
        this->warning_traceback_va( _format, args );
        va_end( args );
    }
    //////////////////////////////////////////////////////////////////////////
    void tinypy_kernel::warning_traceback_va( const char * _format, va_list _args )
    {
        char message[PYBIND_EXCEPTION_MESSAGE_SIZE + 1];
        std::vsnprintf( message, sizeof( message ), _format, _args );
        this->set_error( error_type_e::Runtime, message );
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * tinypy_kernel::build_value( const char * _format, ... )
    {
        va_list args;
        va_start( args, _format );
        PyObject * result = this->build_value_va( _format, args );
        va_end( args );
        return result;
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * tinypy_kernel::build_value_va( const char * _format, va_list _args )
    {
        if( _format == nullptr || _format[0] == '\0' )
        {
            return this->tuple_new( 0 );
        }

        size_t count = 0;
        for( const char * cursor = _format; *cursor != '\0'; ++cursor )
        {
            if( *cursor != '(' && *cursor != ')' && *cursor != ',' && *cursor != ' ' )
            {
                count += 1;
            }
        }

        PyObject * result = this->tuple_new( count );
        size_t index = 0;
        for( const char * cursor = _format; *cursor != '\0'; ++cursor )
        {
            if( *cursor == '(' || *cursor == ')' || *cursor == ',' || *cursor == ' ' )
            {
                continue;
            }

            PyObject * value = nullptr;
            switch( *cursor )
            {
            case 'O':
                value = va_arg( _args, PyObject * );
                this->incref( value );
                break;
            case 's':
                value = this->ptr_string( va_arg( _args, const char * ) );
                break;
            case 'i':
                value = this->ptr_int32( va_arg( _args, int ) );
                break;
            case 'l':
                value = this->ptr_long( va_arg( _args, long ) );
                break;
            case 'L':
                value = this->ptr_int64( va_arg( _args, int64_t ) );
                break;
            case 'd':
                value = this->ptr_double( va_arg( _args, double ) );
                break;
            default:
                this->set_error( error_type_e::Type, "unsupported build_value format" );
                break;
            }

            if( value == nullptr )
            {
                this->decref( result );
                return nullptr;
            }

            this->tuple_setitem( result, index++, value );
            this->decref( value );
        }

        return result;
    }
    //////////////////////////////////////////////////////////////////////////
    bool tinypy_kernel::traceback_check( PyObject * _traceback )
    {
        return _traceback != nullptr && tinypy_typeof( detail::value_cast( _traceback ) ) == TINYPY_VALUE_TRACEBACK;
    }
    //////////////////////////////////////////////////////////////////////////
    string_view tinypy_kernel::traceback_filename( PyObject * _traceback )
    {
        tinypy_value_t * frame = tinypy_traceback_frame( detail::value_cast( _traceback ) );
        return string_view( this, detail::object_cast( tinypy_code_filename( tinypy_frame_code( frame ) ) ) );
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t tinypy_kernel::traceback_lineno( PyObject * _traceback )
    {
        return static_cast<uint32_t>(tinypy_traceback_line_number( detail::value_cast( _traceback )));
    }
    //////////////////////////////////////////////////////////////////////////
    string_view tinypy_kernel::traceback_function( PyObject * _traceback )
    {
        tinypy_value_t * frame = tinypy_traceback_frame( detail::value_cast( _traceback ) );
        return string_view( this, detail::object_cast( tinypy_code_name( tinypy_frame_code( frame ) ) ) );
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * tinypy_kernel::traceback_next( PyObject * _traceback )
    {
        tinypy_value_t * next = tinypy_traceback_next( detail::value_cast( _traceback ) );
        if( next != nullptr )
        {
            tinypy_retain( next );
        }

        return detail::object_cast( next );
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * tinypy_kernel::module_import( const char * _name, bool & _exists )
    {
        tinypy_error_t * error = nullptr;
        tinypy_value_t * module = tinypy_import_module( m_vm, _name, std::strlen( _name ), nullptr, nullptr, 0, &error );
        _exists = module != nullptr;
        if( module == nullptr )
        {
            this->report_error_( error );
        }

        return detail::object_cast( module );
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * tinypy_kernel::module_init( const char * _name )
    {
        tinypy_value_t * module = tinypy_module_new( m_vm, _name, std::strlen( _name ) );
        tinypy_value_t * key = tinypy_string_from_bytes( m_vm, _name, std::strlen( _name ) );
        tinypy_module_add_value( module, "__name__", 8, key );
        tinypy_module_add_value( module, "__builtins__", 12, tinypy_vm_builtins( m_vm ) );
        tinypy_dict_set( tinypy_vm_modules( m_vm ), key, module );
        tinypy_release( key );
        return detail::object_cast( module );
    }
    //////////////////////////////////////////////////////////////////////////
    void tinypy_kernel::module_fini( PyObject * _module )
    {
        (void)_module;
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * tinypy_kernel::module_dict( PyObject * _module )
    {
        return detail::object_cast( tinypy_module_dict( detail::value_cast( _module ) ) );
    }
    //////////////////////////////////////////////////////////////////////////
    void tinypy_kernel::module_addobject( PyObject * _module, const char * _name, PyObject * _object )
    {
        tinypy_module_add_value( detail::value_cast( _module ), _name, std::strlen( _name ), detail::value_cast( _object ) );
        tinypy_release( detail::value_cast( _object ) );
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * tinypy_kernel::module_execcode( const char * _name, PyObject * _code )
    {
        tinypy_value_t * key = tinypy_string_from_bytes( m_vm, _name, std::strlen( _name ) );
        tinypy_value_t * module = tinypy_dict_get( tinypy_vm_modules( m_vm ), key );
        tinypy_release( key );
        if( module == nullptr )
        {
            module = detail::value_cast( this->module_init( _name ) );
        }
        else
        {
            tinypy_retain( module );
        }

        tinypy_error_t * error = nullptr;
        tinypy_value_t * none = tinypy_exec_code( detail::value_cast( _code ), tinypy_module_dict( module ), tinypy_module_dict( module ), &error );
        if( none == nullptr )
        {
            tinypy_release( module );
            this->report_error_( error );
            return nullptr;
        }

        tinypy_release( none );
        return detail::object_cast( module );
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * tinypy_kernel::module_reload( PyObject * _module )
    {
        tinypy_error_t * error = nullptr;
        tinypy_value_t * module = detail::value_cast( _module );
        tinypy_value_t * name = tinypy_object_get_attr( module, "__name__", 8, &error );

        if( name == nullptr )
        {
            this->report_error_( error );
            return nullptr;
        }

        size_t nameSize;
        const char * nameString = static_cast<const char *>(tinypy_string_view( name, &nameSize));
        tinypy_value_t * modules = tinypy_vm_modules( m_vm );

        tinypy_retain( module );
        tinypy_dict_delete( modules, name );

        tinypy_value_t * reloaded = tinypy_import_module( m_vm, nameString, nameSize, nullptr, nullptr, 0, &error );

        if( reloaded == nullptr )
        {
            tinypy_dict_set( modules, name, module );
            tinypy_release( module );
            tinypy_release( name );
            this->report_error_( error );
            return nullptr;
        }

        tinypy_value_t * destination = tinypy_module_dict( module );
        tinypy_value_t * source = tinypy_module_dict( reloaded );
        size_t position = 0;
        tinypy_value_t * key;
        tinypy_value_t * value;

        tinypy_dict_clear( destination );

        while( tinypy_dict_next( source, &position, &key, &value ) != 0 )
        {
            tinypy_value_t * destinationValue = value == reloaded ? module : value;
            tinypy_dict_set( destination, key, destinationValue );
        }

        tinypy_dict_set( modules, name, module );
        tinypy_release( reloaded );
        tinypy_release( name );

        return _module;
    }
    //////////////////////////////////////////////////////////////////////////
    void tinypy_kernel::incref( PyObject * _object )
    {
        if( _object != nullptr )
        {
            tinypy_retain( detail::value_cast( _object ) );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void tinypy_kernel::decref( PyObject * _object )
    {
        if( _object != nullptr )
        {
            tinypy_release( detail::value_cast( _object ) );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t tinypy_kernel::refcount( PyObject * _object )
    {
        tinypy_ref_t ref = tinypy_refcount( detail::value_cast( _object ) );
        assert( ref >= 0 && static_cast<uint64_t>(ref) <= UINT32_MAX );
        return static_cast<uint32_t>(ref);
    }
    //////////////////////////////////////////////////////////////////////////
    bool tinypy_kernel::is_none( PyObject * _object ) const
    {
        return tinypy_typeof( detail::value_cast( _object ) ) == TINYPY_VALUE_NONE;
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * tinypy_kernel::get_none()
    {
        tinypy_value_t * value = tinypy_none_get( m_vm );
        tinypy_release( value );
        return detail::object_cast( value );
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * tinypy_kernel::ret_none()
    {
        return detail::object_cast( tinypy_none_get( m_vm ) );
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * tinypy_kernel::ret_not_implemented()
    {
        return detail::object_cast( tinypy_not_implemented_get( m_vm ) );
    }
    //////////////////////////////////////////////////////////////////////////
    bool tinypy_kernel::is_callable( PyObject * _object ) const
    {
        return tinypy_is_callable( detail::value_cast( _object ) ) != 0;
    }
    //////////////////////////////////////////////////////////////////////////
    bool tinypy_kernel::is_instanceof( PyObject * _object, PyTypeObject * _type ) const
    {
        return tinypy_type_is_subtype( tinypy_object_type( detail::value_cast( _object ) ), detail::tinypy_type_cast( _type ) ) != 0;
    }
    //////////////////////////////////////////////////////////////////////////
    bool tinypy_kernel::test_equal( PyObject * _left, PyObject * _right )
    {
        return tinypy_equal( detail::value_cast( _left ), detail::value_cast( _right ) ) != 0;
    }
    //////////////////////////////////////////////////////////////////////////
    bool tinypy_kernel::has_attr( PyObject * _object, PyObject * _attribute )
    {
        size_t size;
        const char * name = static_cast<const char *>(tinypy_string_view( detail::value_cast( _attribute ), &size));
        tinypy_error_t * error = nullptr;
        tinypy_value_t * value = tinypy_object_get_attr( detail::value_cast( _object ), name, size, &error );
        if( value == nullptr )
        {
            if( error != nullptr )
            {
                tinypy_error_release( error );
            }

            tinypy_vm_clear_error( m_vm );
            return false;
        }

        tinypy_release( value );
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * tinypy_kernel::get_attr( PyObject * _object, PyObject * _attribute )
    {
        size_t size;
        const char * name = static_cast<const char *>(tinypy_string_view( detail::value_cast( _attribute ), &size));
        tinypy_error_t * error = nullptr;
        tinypy_value_t * result = tinypy_object_get_attr( detail::value_cast( _object ), name, size, &error );
        if( result == nullptr )
        {
            this->report_error_( error );
        }

        return detail::object_cast( result );
    }
    //////////////////////////////////////////////////////////////////////////
    bool tinypy_kernel::set_attr( PyObject * _object, PyObject * _attribute, PyObject * _value )
    {
        size_t size;
        const char * name = static_cast<const char *>(tinypy_string_view( detail::value_cast( _attribute ), &size));
        tinypy_error_t * error = nullptr;
        int32_t result = tinypy_object_set_attr( detail::value_cast( _object ), name, size, detail::value_cast( _value ), &error );
        if( result == 0 )
        {
            this->report_error_( error );
        }

        return result != 0;
    }
    //////////////////////////////////////////////////////////////////////////
    bool tinypy_kernel::set_attrstring( PyObject * _object, const char * _attribute, PyObject * _value )
    {
        tinypy_error_t * error = nullptr;
        int32_t result = tinypy_object_set_attr( detail::value_cast( _object ), _attribute, std::strlen( _attribute ), detail::value_cast( _value ), &error );
        if( result == 0 )
        {
            this->report_error_( error );
        }

        return result != 0;
    }
    //////////////////////////////////////////////////////////////////////////
    bool tinypy_kernel::has_attrstring( PyObject * _object, const char * _attribute )
    {
        tinypy_error_t * error = nullptr;
        tinypy_value_t * value = tinypy_object_get_attr( detail::value_cast( _object ), _attribute, std::strlen( _attribute ), &error );
        if( value == nullptr )
        {
            if( error != nullptr )
            {
                tinypy_error_release( error );
            }

            tinypy_vm_clear_error( m_vm );
            return false;
        }

        tinypy_release( value );
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * tinypy_kernel::get_attrstring( PyObject * _object, const char * _attribute )
    {
        tinypy_error_t * error = nullptr;
        tinypy_value_t * result = tinypy_object_get_attr( detail::value_cast( _object ), _attribute, std::strlen( _attribute ), &error );
        if( result == nullptr )
        {
            this->report_error_( error );
        }

        return detail::object_cast( result );
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * tinypy_kernel::object_dir( PyObject * _object )
    {
        tinypy_value_t * key = tinypy_string_from_bytes( m_vm, "dir", 3 );
        tinypy_value_t * function = tinypy_dict_get( tinypy_vm_builtins( m_vm ), key );
        tinypy_release( key );
        if( function == nullptr )
        {
            return nullptr;
        }

        tinypy_value_t * args = tinypy_tuple_new( m_vm, 1 );
        tinypy_tuple_set( args, 0, detail::value_cast( _object ) );
        tinypy_error_t * error = nullptr;
        tinypy_value_t * result = tinypy_call( function, args, nullptr, &error );
        tinypy_release( args );
        if( result == nullptr )
        {
            this->report_error_( error );
        }

        return detail::object_cast( result );
    }
    //////////////////////////////////////////////////////////////////////////
    string_view tinypy_kernel::object_str( PyObject * _object )
    {
        if( _object == nullptr )
        {
            return string_view( this, nullptr, pybind::borrowed );
        }

        tinypy_error_t * error = nullptr;
        tinypy_value_t * result = tinypy_object_str( detail::value_cast( _object ), &error );
        if( result == nullptr )
        {
            this->report_error_( error );
        }

        return string_view( this, detail::object_cast( result ), pybind::borrowed );
    }
    //////////////////////////////////////////////////////////////////////////
    string_view tinypy_kernel::object_repr( PyObject * _object )
    {
        if( _object == nullptr )
        {
            return string_view( this, nullptr, pybind::borrowed );
        }

        tinypy_error_t * error = nullptr;
        tinypy_value_t * result = tinypy_object_repr( detail::value_cast( _object ), &error );
        if( result == nullptr )
        {
            this->report_error_( error );
        }

        return string_view( this, detail::object_cast( result ), pybind::borrowed );
    }
    //////////////////////////////////////////////////////////////////////////
    string_view tinypy_kernel::object_repr_type( PyObject * _object )
    {
        if( _object == nullptr )
        {
            return string_view( this, nullptr, pybind::borrowed );
        }

        return this->object_repr( detail::object_cast( tinypy_type_as_value( detail::tinypy_type_cast( this->get_object_type( _object ) ) ) ) );
    }
    //////////////////////////////////////////////////////////////////////////
    bool tinypy_kernel::extract_bool( PyObject * _object, bool & _value )
    {
        tinypy_error_t * error = nullptr;
        int32_t result = tinypy_truth( detail::value_cast( _object ), &error );
        if( result < 0 )
        {
            this->report_error_( error );
            return false;
        }

        _value = result != 0;
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool tinypy_kernel::extract_int64( PyObject * _object, int64_t & _value )
    {
        tinypy_value_type_e kind = tinypy_typeof( detail::value_cast( _object ) );
        if( kind == TINYPY_VALUE_BOOL || kind == TINYPY_VALUE_INTEGER )
        {
            _value = tinypy_integer_as_i64( detail::value_cast( _object ) );
            return true;
        }

        if( kind == TINYPY_VALUE_LONG )
        {
            uint64_t raw;
            int sign;
            size_t count;
            const uint16_t * digits = tinypy_long_base15_view( detail::value_cast( _object ), &sign, &count );
            if( count > 5 )
            {
                return false;
            }

            raw = 0;
            for( size_t i = count; i != 0; --i )
            {
                raw = ( raw << 15 ) | digits[i - 1];
            }

            if( sign >= 0 && raw <= INT64_MAX )
            {
                _value = static_cast<int64_t>(raw);
            }
            else if( sign < 0 && raw <= UINT64_C( 0x8000000000000000 ) )
            {
                _value = raw == UINT64_C( 0x8000000000000000 ) ? INT64_MIN : -static_cast<int64_t>(raw);
            }
            else
            {
                return false;
            }

            return true;
        }

        this->set_error( error_type_e::Type, "value is not an integer" );
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool tinypy_kernel::extract_int8( PyObject * _object, int8_t & _value )
    {
        int64_t value;
        if( this->extract_int64( _object, value ) == false || value < INT8_MIN || value > INT8_MAX )
        {
            return false;
        }

        _value = static_cast<int8_t>(value);
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool tinypy_kernel::extract_int16( PyObject * _object, int16_t & _value )
    {
        int64_t value;
        if( this->extract_int64( _object, value ) == false || value < INT16_MIN || value > INT16_MAX )
        {
            return false;
        }

        _value = static_cast<int16_t>(value);
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool tinypy_kernel::extract_int32( PyObject * _object, int32_t & _value )
    {
        int64_t value;
        if( this->extract_int64( _object, value ) == false || value < INT32_MIN || value > INT32_MAX )
        {
            return false;
        }

        _value = static_cast<int32_t>(value);
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool tinypy_kernel::extract_uint64( PyObject * _object, uint64_t & _value )
    {
        tinypy_value_type_e kind = tinypy_typeof( detail::value_cast( _object ) );
        if( kind == TINYPY_VALUE_LONG )
        {
            return detail::long_to_u64( detail::value_cast( _object ), &_value );
        }

        int64_t value;
        if( this->extract_int64( _object, value ) == false || value < 0 )
        {
            return false;
        }

        _value = static_cast<uint64_t>(value);
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool tinypy_kernel::extract_uint8( PyObject * _object, uint8_t & _value )
    {
        uint64_t value;
        if( this->extract_uint64( _object, value ) == false || value > UINT8_MAX )
        {
            return false;
        }

        _value = static_cast<uint8_t>(value);
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool tinypy_kernel::extract_uint16( PyObject * _object, uint16_t & _value )
    {
        uint64_t value;
        if( this->extract_uint64( _object, value ) == false || value > UINT16_MAX )
        {
            return false;
        }

        _value = static_cast<uint16_t>(value);
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool tinypy_kernel::extract_uint32( PyObject * _object, uint32_t & _value )
    {
        uint64_t value;
        if( this->extract_uint64( _object, value ) == false || value > UINT32_MAX )
        {
            return false;
        }

        _value = static_cast<uint32_t>(value);
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool tinypy_kernel::extract_double( PyObject * _object, double & _value )
    {
        tinypy_value_type_e kind = tinypy_typeof( detail::value_cast( _object ) );
        if( kind == TINYPY_VALUE_FLOAT )
        {
            _value = tinypy_float_as_double( detail::value_cast( _object ) );
            return true;
        }

        int64_t integer;
        if( this->extract_int64( _object, integer ) == false )
        {
            return false;
        }

        _value = static_cast<double>(integer);
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool tinypy_kernel::extract_float( PyObject * _object, float & _value )
    {
        double value;
        if( this->extract_double( _object, value ) == false )
        {
            return false;
        }

        _value = static_cast<float>(value);
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool tinypy_kernel::extract_wchar( PyObject * _object, wchar_t & _value )
    {
        size_t size;
        const wchar_t * value = this->unicode_to_wchar_and_size( _object, &size );
        if( value == nullptr || size != 1 )
        {
            return false;
        }

        _value = value[0];
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * tinypy_kernel::ptr_bool( bool _value )
    {
        return detail::object_cast( tinypy_bool_from_i32( m_vm, _value ? 1 : 0 ) );
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * tinypy_kernel::ptr_int8( int8_t _value )
    {
        return this->ptr_int64( _value );
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * tinypy_kernel::ptr_int16( int16_t _value )
    {
        return this->ptr_int64( _value );
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * tinypy_kernel::ptr_int32( int32_t _value )
    {
        return this->ptr_int64( _value );
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * tinypy_kernel::ptr_int64( int64_t _value )
    {
        return detail::object_cast( tinypy_integer_from_i64( m_vm, _value ) );
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * tinypy_kernel::ptr_uint8( uint8_t _value )
    {
        return this->ptr_uint64( _value );
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * tinypy_kernel::ptr_uint16( uint16_t _value )
    {
        return this->ptr_uint64( _value );
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * tinypy_kernel::ptr_uint32( uint32_t _value )
    {
        return this->ptr_uint64( _value );
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * tinypy_kernel::ptr_uint64( uint64_t _value )
    {
        return _value <= INT64_MAX ? this->ptr_int64( static_cast<int64_t>(_value) ) : detail::object_cast( detail::u64_to_long( m_vm, _value ) );
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * tinypy_kernel::ptr_float( float _value )
    {
        return this->ptr_double( _value );
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * tinypy_kernel::ptr_double( double _value )
    {
        return detail::object_cast( tinypy_float_from_double( m_vm, _value ) );
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * tinypy_kernel::ptr_integer( int64_t _value )
    {
        return this->ptr_int64( _value );
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * tinypy_kernel::ptr_long( long _value )
    {
        return detail::object_cast( tinypy_long_from_i64( m_vm, _value ) );
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * tinypy_kernel::ptr_ulong( unsigned long _value )
    {
        return detail::object_cast( detail::u64_to_long( m_vm, _value ) );
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * tinypy_kernel::ptr_string( const char * _value )
    {
        return this->string_from_char( _value );
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * tinypy_kernel::ptr_unicode( const wchar_t * _value )
    {
        return this->unicode_from_wchar( _value );
    }
    //////////////////////////////////////////////////////////////////////////
    bool tinypy_kernel::bool_check( PyObject * _object ) const
    {
        return tinypy_typeof( detail::value_cast( _object ) ) == TINYPY_VALUE_BOOL;
    }
    //////////////////////////////////////////////////////////////////////////
    bool tinypy_kernel::is_true( PyObject * _object ) const
    {
        return this->bool_check( _object ) && tinypy_bool_as_i32( detail::value_cast( _object ) ) != 0;
    }
    //////////////////////////////////////////////////////////////////////////
    bool tinypy_kernel::is_false( PyObject * _object ) const
    {
        return this->bool_check( _object ) && tinypy_bool_as_i32( detail::value_cast( _object ) ) == 0;
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * tinypy_kernel::ret_bool( bool _value )
    {
        return this->ptr_bool( _value );
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * tinypy_kernel::ret_true()
    {
        return this->ptr_bool( true );
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * tinypy_kernel::ret_false()
    {
        return this->ptr_bool( false );
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * tinypy_kernel::get_true()
    {
        tinypy_value_t * value = tinypy_bool_from_i32( m_vm, 1 );
        tinypy_release( value );
        return detail::object_cast( value );
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * tinypy_kernel::get_false()
    {
        tinypy_value_t * value = tinypy_bool_from_i32( m_vm, 0 );
        tinypy_release( value );
        return detail::object_cast( value );
    }
    //////////////////////////////////////////////////////////////////////////
    bool tinypy_kernel::int_check( PyObject * _object ) const
    {
        tinypy_value_type_e kind = tinypy_typeof( detail::value_cast( _object ) );
        return kind == TINYPY_VALUE_INTEGER || kind == TINYPY_VALUE_BOOL;
    }
    //////////////////////////////////////////////////////////////////////////
    bool tinypy_kernel::long_check( PyObject * _object ) const
    {
        return tinypy_typeof( detail::value_cast( _object ) ) == TINYPY_VALUE_LONG;
    }
    //////////////////////////////////////////////////////////////////////////
    bool tinypy_kernel::float_check( PyObject * _object ) const
    {
        return tinypy_typeof( detail::value_cast( _object ) ) == TINYPY_VALUE_FLOAT;
    }
    //////////////////////////////////////////////////////////////////////////
    bool tinypy_kernel::string_check( PyObject * _object ) const
    {
        return tinypy_typeof( detail::value_cast( _object ) ) == TINYPY_VALUE_STRING;
    }
    //////////////////////////////////////////////////////////////////////////
    size_t tinypy_kernel::string_size( PyObject * _object ) const
    {
        size_t size;
        tinypy_string_view( detail::value_cast( _object ), &size );
        return size;
    }
    //////////////////////////////////////////////////////////////////////////
    int64_t tinypy_kernel::string_hash( PyObject * _object ) const
    {
        return tinypy_hash( detail::value_cast( _object ) );
    }
    //////////////////////////////////////////////////////////////////////////
    const char * tinypy_kernel::string_to_char( PyObject * _object )
    {
        size_t size;
        return static_cast<const char *>(tinypy_string_view( detail::value_cast( _object ), &size));
    }
    //////////////////////////////////////////////////////////////////////////
    const char * tinypy_kernel::string_to_char_and_size( PyObject * _object, size_t * _size )
    {
        return static_cast<const char *>(tinypy_string_view( detail::value_cast( _object ), _size));
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * tinypy_kernel::string_from_char( const char * _value )
    {
        return this->string_from_char_size( _value, std::strlen( _value ) );
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * tinypy_kernel::string_from_char_size( const char * _value, size_t _size )
    {
        return detail::object_cast( tinypy_string_from_bytes( m_vm, _value, _size ) );
    }
    //////////////////////////////////////////////////////////////////////////
    bool tinypy_kernel::bytearray_check( PyObject * _object ) const
    {
        return tinypy_typeof( detail::value_cast( _object ) ) == TINYPY_VALUE_BYTEARRAY;
    }
    //////////////////////////////////////////////////////////////////////////
    size_t tinypy_kernel::bytearray_size( PyObject * _object ) const
    {
        return tinypy_bytearray_size( detail::value_cast( _object ) );
    }
    //////////////////////////////////////////////////////////////////////////
    const uint8_t * tinypy_kernel::bytearray_to_data( PyObject * _object ) const
    {
        size_t size;
        return static_cast<const uint8_t *>(tinypy_bytearray_view( detail::value_cast( _object ), &size));
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * tinypy_kernel::bytearray_from_data( const uint8_t * _data, size_t _size )
    {
        return detail::object_cast( tinypy_bytearray_from_bytes( m_vm, _data, _size ) );
    }
    //////////////////////////////////////////////////////////////////////////
    bool tinypy_kernel::unicode_check( PyObject * _object ) const
    {
        return tinypy_typeof( detail::value_cast( _object ) ) == TINYPY_VALUE_UNICODE;
    }
    //////////////////////////////////////////////////////////////////////////
    const wchar_t * tinypy_kernel::unicode_to_wchar( PyObject * _object )
    {
        size_t size;
        return this->unicode_to_wchar_and_size( _object, &size );
    }
    //////////////////////////////////////////////////////////////////////////
    const wchar_t * tinypy_kernel::unicode_to_wchar_and_size( PyObject * _object, size_t * _size )
    {
        size_t bytes;
        size_t codePointCount;
        const char * utf8 = tinypy_unicode_utf8_view( detail::value_cast( _object ), &bytes, &codePointCount );
        assert( bytes != SIZE_MAX );
        size_t capacity = bytes + 1;
        if( m_unicode_cache_capacity < capacity )
        {
            size_t bufferSize = capacity * sizeof( wchar_t );
            if( m_unicode_cache == nullptr )
            {
                m_unicode_cache = static_cast<wchar_t *>(m_allocator->malloc( bufferSize));
            }
            else
            {
                m_unicode_cache = static_cast<wchar_t *>(m_allocator->realloc( m_unicode_cache, bufferSize));
            }

            assert( m_unicode_cache != nullptr );
            m_unicode_cache_capacity = capacity;
        }

        if( detail::decode_utf8( utf8, bytes, m_unicode_cache, m_unicode_cache_capacity, &m_unicode_cache_size ) == false )
        {
            return nullptr;
        }
        *_size = m_unicode_cache_size;
        return m_unicode_cache;
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * tinypy_kernel::unicode_from_wchar( const wchar_t * _value )
    {
        return this->unicode_from_wchar_size( _value, std::wcslen( _value ) );
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * tinypy_kernel::unicode_from_wchar_size( const wchar_t * _value, size_t _size )
    {
        assert( _size <= ( SIZE_MAX - 1 ) / 4 );
        size_t capacity = _size * 4 + 1;
        char * utf8 = static_cast<char *>(m_allocator->malloc( capacity));
        assert( utf8 != nullptr );
        size_t utf8Size = detail::encode_utf8( _value, _size, utf8, capacity );
        tinypy_value_t * unicode = tinypy_unicode_from_utf8( m_vm, utf8, utf8Size );
        m_allocator->free( utf8 );
        return detail::object_cast( unicode );
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * tinypy_kernel::unicode_encode_utf8( PyObject * _object )
    {
        size_t size;
        size_t codePointCount;
        const char * data = tinypy_unicode_utf8_view( detail::value_cast( _object ), &size, &codePointCount );
        return this->string_from_char_size( data, size );
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * tinypy_kernel::unicode_from_utf8( const char * _value )
    {
        return this->unicode_from_utf8_size( _value, std::strlen( _value ) );
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * tinypy_kernel::unicode_from_utf8_size( const char * _value, size_t _size )
    {
        return detail::object_cast( tinypy_unicode_from_utf8( m_vm, _value, _size ) );
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * tinypy_kernel::list_new( size_t _size )
    {
        tinypy_value_t * result = tinypy_list_from_items( m_vm, nullptr, 0 );
        for( size_t index = 0; index != _size; ++index )
        {
            tinypy_value_t * item = tinypy_none_get( m_vm );
            tinypy_list_append( result, item );
            tinypy_release( item );
        }

        return detail::object_cast( result );
    }
    //////////////////////////////////////////////////////////////////////////
    bool tinypy_kernel::list_check( PyObject * _object ) const
    {
        return tinypy_typeof( detail::value_cast( _object ) ) == TINYPY_VALUE_LIST;
    }
    //////////////////////////////////////////////////////////////////////////
    size_t tinypy_kernel::list_size( PyObject * _object ) const
    {
        return tinypy_list_size( detail::value_cast( _object ) );
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * tinypy_kernel::list_getitem( PyObject * _object, size_t _index )
    {
        return detail::object_cast( tinypy_list_get( detail::value_cast( _object ), _index ) );
    }
    //////////////////////////////////////////////////////////////////////////
    bool tinypy_kernel::list_insert( PyObject * _object, size_t _index, PyObject * _item )
    {
        tinypy_list_insert( detail::value_cast( _object ), _index, detail::value_cast( _item ) );
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool tinypy_kernel::list_remove( PyObject * _object, size_t _index )
    {
        tinypy_list_delete( detail::value_cast( _object ), _index );
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool tinypy_kernel::list_setitem( PyObject * _object, size_t _index, PyObject * _item )
    {
        tinypy_list_set( detail::value_cast( _object ), _index, detail::value_cast( _item ) );
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool tinypy_kernel::list_appenditem( PyObject * _object, PyObject * _item )
    {
        tinypy_list_append( detail::value_cast( _object ), detail::value_cast( _item ) );
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * tinypy_kernel::dict_new()
    {
        return detail::object_cast( tinypy_dict_new( m_vm ) );
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * tinypy_kernel::dict_new_presized( size_t _count )
    {
        (void)_count;
        return this->dict_new();
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * tinypy_kernel::dict_from( PyObject * _object )
    {
        if( this->dict_check( _object ) == false )
        {
            this->set_error( error_type_e::Type, "value is not a dict" );
            return nullptr;
        }

        PyObject * result = this->dict_new_presized( this->dict_size( _object ) );
        size_t pos = 0;
        PyObject * key;
        PyObject * value;
        while( this->dict_next( _object, &pos, &key, &value ) == true )
        {
            this->dict_set( result, key, value );
        }

        return result;
    }
    //////////////////////////////////////////////////////////////////////////
    bool tinypy_kernel::dict_check( PyObject * _object ) const
    {
        return tinypy_typeof( detail::value_cast( _object ) ) == TINYPY_VALUE_DICT;
    }
    //////////////////////////////////////////////////////////////////////////
    size_t tinypy_kernel::dict_size( PyObject * _object ) const
    {
        return tinypy_dict_size( detail::value_cast( _object ) );
    }
    //////////////////////////////////////////////////////////////////////////
    bool tinypy_kernel::dict_set( PyObject * _dict, PyObject * _key, PyObject * _value )
    {
        tinypy_dict_set( detail::value_cast( _dict ), detail::value_cast( _key ), detail::value_cast( _value ) );
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool tinypy_kernel::dict_setstring( PyObject * _dict, const char * _key, PyObject * _value )
    {
        tinypy_value_t * key = tinypy_string_from_bytes( m_vm, _key, std::strlen( _key ) );
        tinypy_dict_set( detail::value_cast( _dict ), key, detail::value_cast( _value ) );
        tinypy_release( key );
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool tinypy_kernel::dict_remove( PyObject * _dict, PyObject * _key )
    {
        bool exists = tinypy_dict_contains( detail::value_cast( _dict ), detail::value_cast( _key ) ) != 0;
        if( exists )
        {
            tinypy_dict_delete( detail::value_cast( _dict ), detail::value_cast( _key ) );
        }

        return exists;
    }
    //////////////////////////////////////////////////////////////////////////
    bool tinypy_kernel::dict_removestring( PyObject * _dict, const char * _key )
    {
        tinypy_value_t * key = tinypy_string_from_bytes( m_vm, _key, std::strlen( _key ) );
        bool result = this->dict_remove( _dict, detail::object_cast( key ) );
        tinypy_release( key );
        return result;
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * tinypy_kernel::dict_getstring( PyObject * _dict, const char * _key )
    {
        tinypy_value_t * key = tinypy_string_from_bytes( m_vm, _key, std::strlen( _key ) );
        tinypy_value_t * result = tinypy_dict_get( detail::value_cast( _dict ), key );
        tinypy_release( key );
        return detail::object_cast( result );
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * tinypy_kernel::dict_get( PyObject * _dict, PyObject * _key )
    {
        return detail::object_cast( tinypy_dict_get( detail::value_cast( _dict ), detail::value_cast( _key ) ) );
    }
    //////////////////////////////////////////////////////////////////////////
    bool tinypy_kernel::dict_exist( PyObject * _dict, PyObject * _key )
    {
        return tinypy_dict_contains( detail::value_cast( _dict ), detail::value_cast( _key ) ) != 0;
    }
    //////////////////////////////////////////////////////////////////////////
    bool tinypy_kernel::dict_existstring( PyObject * _dict, const char * _key )
    {
        tinypy_value_t * key = tinypy_string_from_bytes( m_vm, _key, std::strlen( _key ) );
        bool result = tinypy_dict_contains( detail::value_cast( _dict ), key ) != 0;
        tinypy_release( key );
        return result;
    }
    //////////////////////////////////////////////////////////////////////////
    bool tinypy_kernel::dict_next( PyObject * _dict, size_t * const _position, PyObject ** _key, PyObject ** _value )
    {
        tinypy_value_t * key;
        tinypy_value_t * value;
        if( tinypy_dict_next( detail::value_cast( _dict ), _position, &key, &value ) == 0 )
        {
            return false;
        }
        *_key = detail::object_cast( key );
        *_value = detail::object_cast( value );
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * tinypy_kernel::set_new()
    {
        return detail::object_cast( tinypy_set_new( m_vm ) );
    }
    //////////////////////////////////////////////////////////////////////////
    bool tinypy_kernel::set_check( PyObject * _object ) const
    {
        return tinypy_typeof( detail::value_cast( _object ) ) == TINYPY_VALUE_SET;
    }
    //////////////////////////////////////////////////////////////////////////
    size_t tinypy_kernel::set_size( PyObject * _object ) const
    {
        return tinypy_set_size( detail::value_cast( _object ) );
    }
    //////////////////////////////////////////////////////////////////////////
    bool tinypy_kernel::set_set( PyObject * _set, PyObject * _value )
    {
        tinypy_error_t * error = nullptr;
        int32_t result = tinypy_set_add( detail::value_cast( _set ), detail::value_cast( _value ), &error );
        if( result == 0 )
        {
            this->report_error_( error );
        }

        return result != 0;
    }
    //////////////////////////////////////////////////////////////////////////
    bool tinypy_kernel::set_remove( PyObject * _set, PyObject * _value )
    {
        tinypy_error_t * error = nullptr;
        int32_t result = tinypy_set_discard( detail::value_cast( _set ), detail::value_cast( _value ), &error );
        if( error != nullptr )
        {
            this->report_error_( error );
        }

        return result != 0;
    }
    //////////////////////////////////////////////////////////////////////////
    bool tinypy_kernel::set_exist( PyObject * _set, PyObject * _value )
    {
        tinypy_error_t * error = nullptr;
        int32_t result = tinypy_set_contains( detail::value_cast( _set ), detail::value_cast( _value ), &error );
        if( result < 0 )
        {
            this->report_error_( error );
        }

        return result > 0;
    }
    //////////////////////////////////////////////////////////////////////////
    bool tinypy_kernel::iterator_get( PyObject * _collection, PyObject ** _iterator )
    {
        tinypy_error_t * error = nullptr;
        tinypy_value_t * result = tinypy_iter( detail::value_cast( _collection ), &error );
        if( result == nullptr )
        {
            this->report_error_( error );
            return false;
        }
        *_iterator = detail::object_cast( result );
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool tinypy_kernel::iterator_next( PyObject * _iterator, PyObject ** _value )
    {
        tinypy_error_t * error = nullptr;
        tinypy_value_t * result = tinypy_next( detail::value_cast( _iterator ), &error );
        if( result == nullptr )
        {
            if( error != nullptr )
            {
                this->report_error_( error );
            }
            *_value = nullptr;
            return false;
        }
        *_value = detail::object_cast( result );
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void tinypy_kernel::iterator_end( PyObject * _iterator )
    {
        this->decref( _iterator );
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * tinypy_kernel::tuple_new( size_t _size )
    {
        return detail::object_cast( tinypy_tuple_new( m_vm, _size ) );
    }
    //////////////////////////////////////////////////////////////////////////
    bool tinypy_kernel::tuple_check( PyObject * _object ) const
    {
        return tinypy_typeof( detail::value_cast( _object ) ) == TINYPY_VALUE_TUPLE;
    }
    //////////////////////////////////////////////////////////////////////////
    size_t tinypy_kernel::tuple_size( PyObject * _object ) const
    {
        return tinypy_tuple_size( detail::value_cast( _object ) );
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * tinypy_kernel::tuple_getitem( PyObject * _object, size_t _index )
    {
        return detail::object_cast( tinypy_tuple_get( detail::value_cast( _object ), _index ) );
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * tinypy_kernel::tuple_slice( PyObject * _object, size_t _low, size_t _high )
    {
        size_t size = tinypy_tuple_size( detail::value_cast( _object ) );
        if( _high > size )
        {
            _high = size;
        }

        if( _low > _high )
        {
            _low = _high;
        }

        PyObject * result = this->tuple_new( _high - _low );
        for( size_t index = _low; index != _high; ++index )
        {
            this->tuple_setitem( result, index - _low, detail::object_cast( tinypy_tuple_get( detail::value_cast( _object ), index ) ) );
        }

        return result;
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * tinypy_kernel::tuple_slice_tail( PyObject * _object, size_t _size )
    {
        return this->tuple_slice( _object, _size, this->tuple_size( _object ) );
    }
    //////////////////////////////////////////////////////////////////////////
    bool tinypy_kernel::tuple_setitem( PyObject * _object, size_t _index, PyObject * _value )
    {
        tinypy_tuple_set( detail::value_cast( _object ), _index, detail::value_cast( _value ) );
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * tinypy_kernel::exception_new( const char * _message )
    {
        tinypy_value_t * key = tinypy_string_from_bytes( m_vm, "RuntimeError", 12 );
        tinypy_value_t * builtins = tinypy_vm_builtins( m_vm );
        tinypy_value_t * type = tinypy_dict_get( builtins, key );
        tinypy_release( key );
        if( type == nullptr )
        {
            return nullptr;
        }

        tinypy_value_t * message = tinypy_string_from_bytes( m_vm, _message, std::strlen( _message ) );
        tinypy_value_t * args = tinypy_tuple_new( m_vm, 1 );
        tinypy_tuple_set( args, 0, message );
        tinypy_release( message );
        tinypy_error_t * error = nullptr;
        tinypy_value_t * result = tinypy_call( type, args, nullptr, &error );
        tinypy_release( args );
        if( result == nullptr )
        {
            this->report_error_( error );
        }

        return detail::object_cast( result );
    }
    //////////////////////////////////////////////////////////////////////////
    void tinypy_kernel::report_error_( tinypy_error_t * _error )
    {
        if( _error != nullptr )
        {
            tinypy_error_release( _error );
        }

        if( tinypy_vm_has_error( m_vm ) == 0 )
        {
            return;
        }

        tinypy_value_t * exceptionType = tinypy_vm_raised_exception_type( m_vm );
        tinypy_value_t * exceptionValue = tinypy_vm_raised_exception( m_vm );
        tinypy_value_t * traceback = tinypy_vm_raised_traceback( m_vm );

        if( m_excepthook != nullptr && tinypy_is_callable( detail::value_cast( m_excepthook ) ) != 0 && exceptionType != nullptr && exceptionValue != nullptr )
        {
            tinypy_value_t * none = nullptr;

            if( traceback == nullptr )
            {
                none = tinypy_none_get( m_vm );
                traceback = none;
            }

            tinypy_value_t * items[] = { exceptionType, exceptionValue, traceback };
            tinypy_value_t * args = tinypy_tuple_from_items( m_vm, items, 3 );
            tinypy_error_t * hookError = nullptr;
            tinypy_value_t * result = tinypy_call( detail::value_cast( m_excepthook ), args, nullptr, &hookError );

            tinypy_release( args );

            if( none != nullptr )
            {
                tinypy_release( none );
            }

            if( result != nullptr )
            {
                tinypy_release( result );
            }

            if( hookError != nullptr )
            {
                tinypy_error_release( hookError );
            }
        }

        tinypy_vm_clear_error( m_vm );
    }
    //////////////////////////////////////////////////////////////////////////
    kernel_interface * initialize( allocator_interface * _allocator, const kernel_config_t & _config )
    {
        assert( pybind::get_kernel() == nullptr );

        tinypy_kernel * kernel = _allocator->newP<tinypy_kernel>();
        if( kernel->initialize( _allocator, _config ) == false )
        {
            _allocator->deleteT( kernel );
            return nullptr;
        }

        pybind::set_kernel( kernel );
        return kernel;
    }
    //////////////////////////////////////////////////////////////////////////
}
