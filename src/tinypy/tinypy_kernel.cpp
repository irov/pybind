#include "tinypy_kernel.hpp"

#include "tinypy_class_type_scope.hpp"

#include "pybind/debug.hpp"
#include "pybind/exception.hpp"
#include "pybind/function.hpp"
#include "pybind/adapter/new_adapter.hpp"
#include "pybind/adapter/destroy_adapter.hpp"

#ifdef PYBIND_STL_SUPPORT
#   include "pybind/stl/stl_type_cast.hpp"
#endif

#include <cassert>
#include <climits>
#include <cstdio>
#include <cstring>
#include <cwchar>
#include <stdexcept>

//////////////////////////////////////////////////////////////////////////
#if defined(PYBIND_DEBUG)
#   define PYBIND_TINYPY_CHECK_MAIN_THREAD() this->check_main_thread_()
#else
#   define PYBIND_TINYPY_CHECK_MAIN_THREAD()
#endif
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
            bool native;
            function_adapter_interface_ptr function;
            functor_adapter_interface_ptr functor;
        };

        struct excepthook_holder_t
        {
            tinypy_kernel * kernel;
            pybind_excepthook_handler_f handler;
            void * userData;
        };

        struct cycle_diagnostic_holder_t
        {
            pybind_cycle_diagnostic_handler_f handler;
            void * userData;
        };
        //////////////////////////////////////////////////////////////////////////
        static void * allocator_allocate( void * _userData, size_t _size, size_t _alignment )
        {
            (void)_alignment;
            allocator_interface * allocator = static_cast<allocator_interface *>(_userData);
            return allocator->malloc( _size );
        }
        //////////////////////////////////////////////////////////////////////////
        static void * allocator_reallocate( void * _userData, void * _memory, size_t _oldSize, size_t _newSize, size_t _alignment )
        {
            (void)_oldSize;
            (void)_alignment;
            allocator_interface * allocator = static_cast<allocator_interface *>(_userData);
            return allocator->realloc( _memory, _newSize );
        }
        //////////////////////////////////////////////////////////////////////////
        static void allocator_deallocate( void * _userData, void * _memory, size_t _size, size_t _alignment )
        {
            (void)_size;
            (void)_alignment;
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
        static tinypy_value_t * call_excepthook( tinypy_value_t * _function, tinypy_value_t * _self, tinypy_value_t * const * _items, size_t _count, tinypy_value_t * _kwargs, tinypy_error_t ** _outError )
        {
            (void)_self;
            (void)_count;
            (void)_kwargs;
            excepthook_holder_t * holder = static_cast<excepthook_holder_t *>(tinypy_native_function_user_data( _function ));
            PyObject * value = object_cast( _items[1] );
            PyObject * traceback = object_cast( _items[2] );

            // The class of a classic exception is not a type, which the handler
            // takes as unknown.
            PyTypeObject * type = nullptr;

            if( tinypy_typeof( _items[0] ) == TINYPY_VALUE_TYPE )
            {
                type = reinterpret_cast<PyTypeObject *>(object_cast( _items[0] ));
            }

            // As call_adapter and the function proxy of the CPython backend: a
            // throwing handler fails the hook call, which reports it.
            try
            {
                holder->handler( holder->userData, type, value, traceback );
            }
            catch( const pybind_exception & exception )
            {
                const char * message = exception.what();
                holder->kernel->set_error( error_type_e::Runtime, message );
                if( _outError != nullptr )
                {
                    *_outError = nullptr;
                }

                return nullptr;
            }

            return tinypy_none_get( holder->kernel->vm() );
        }
        //////////////////////////////////////////////////////////////////////////
        static tinypy_value_t * call_adapter( tinypy_value_t * _function, tinypy_value_t * _args, tinypy_value_t * _kwargs, void * _userData, tinypy_error_t ** _outError )
        {
            (void)_function;
            callable_holder_t * holder = static_cast<callable_holder_t *>(_userData);
            PyObject * args = object_cast( _args );
            PyObject * kwargs = object_cast( _kwargs );

            // As the functions of the CPython backend: only a native adapter
            // takes keyword arguments.
            if( holder->native == false && _kwargs != nullptr && tinypy_dict_size( _kwargs ) != 0 )
            {
                const char * name = holder->kind == CallableFunction ? holder->function->getName() : holder->functor->getName();

                char message[PYBIND_EXCEPTION_MESSAGE_SIZE + 1];
                std::snprintf( message, sizeof( message ), "%s() takes no keyword arguments", name );

                holder->kernel->set_error( error_type_e::Type, message );
                if( _outError != nullptr )
                {
                    *_outError = nullptr;
                }

                return nullptr;
            }

            try
            {
                if( holder->kind == CallableFunction )
                {
                    DEBUG_PYBIND_NOTIFY_BIND_CALL_SCOPE( holder->kernel, "", holder->function->getName(), holder->function->getCallDebugSilent(), args, kwargs );

                    PyObject * result = holder->function->call( holder->kernel, args, kwargs );
                    return value_cast( result );
                }

                DEBUG_PYBIND_NOTIFY_BIND_CALL_SCOPE( holder->kernel, "", holder->functor->getName(), holder->functor->getCallDebugSilent(), args, kwargs );

                PyObject * result = holder->functor->call( holder->kernel, args, kwargs );
                return value_cast( result );
            }
            catch( const pybind_exception & exception )
            {
                const char * message = exception.what();
                holder->kernel->set_error( error_type_e::Runtime, message );
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
        static const char * code_text( const tinypy_value_t * _value )
        {
            if( _value == nullptr || tinypy_typeof( _value ) != TINYPY_VALUE_STRING )
            {
                return "";
            }

            size_t size;
            const char * text = static_cast<const char *>(tinypy_string_view( _value, &size ));

            return text;
        }
        //////////////////////////////////////////////////////////////////////////
        static tinypy_value_t * get_builtin( tinypy_vm_t * _vm, const char * _name )
        {
            tinypy_value_t * key = tinypy_string_from_bytes( _vm, _name, std::strlen( _name ) );
            tinypy_value_t * value = tinypy_dict_get( tinypy_vm_builtins( _vm ), key );
            tinypy_release( key );

            return value;
        }
        //////////////////////////////////////////////////////////////////////////
        struct raised_error_t
        {
            tinypy_value_t * exception;
            tinypy_value_t * traceback;
        };
        //////////////////////////////////////////////////////////////////////////
        static void fetch_raised_error( tinypy_vm_t * _vm, raised_error_t * const _raised )
        {
            // As PyErr_Fetch: the pending exception is taken, normalized, so that
            // code runs without it.
            _raised->exception = nullptr;
            _raised->traceback = nullptr;

            if( tinypy_vm_has_error( _vm ) == 0 )
            {
                return;
            }

            tinypy_vm_normalize_error( _vm, nullptr );

            _raised->exception = tinypy_vm_raised_exception( _vm );
            _raised->traceback = tinypy_vm_raised_traceback( _vm );

            tinypy_retain( _raised->exception );

            if( _raised->traceback != nullptr )
            {
                tinypy_retain( _raised->traceback );
            }

            tinypy_vm_clear_error( _vm );
        }
        //////////////////////////////////////////////////////////////////////////
        static void restore_raised_error( raised_error_t * const _raised )
        {
            // As PyErr_Restore: the exception fetch_raised_error took is pending
            // again.
            if( _raised->exception == nullptr )
            {
                return;
            }

            tinypy_exception_raise( _raised->exception, _raised->traceback, nullptr );

            tinypy_release( _raised->exception );

            if( _raised->traceback != nullptr )
            {
                tinypy_release( _raised->traceback );
            }
        }
        //////////////////////////////////////////////////////////////////////////
        static void host_emit_output( void * _userData, tinypy_output_channel_e _channel, const void * _bytes, size_t _size )
        {
            (void)_userData;

            // As the default streams of CPython: what is written to sys.stdout
            // and sys.stderr while they are not redirected reaches the process.
            FILE * stream = stdout;

            if( _channel != TINYPY_OUTPUT_STDOUT )
            {
                stream = stderr;
            }

            std::fwrite( _bytes, 1, _size, stream );
            std::fflush( stream );
        }
        //////////////////////////////////////////////////////////////////////////
        static void restore_dict( tinypy_vm_t * _vm, tinypy_value_t * _dict, tinypy_value_t * _snapshot )
        {
            // The names bound since the snapshot are removed, collected first as
            // removing them changes the dict, and the others take their values
            // again, so the dict is never left empty meanwhile.
            tinypy_value_t * added = tinypy_list_from_items( _vm, nullptr, 0 );

            size_t position = 0;
            tinypy_value_t * key = nullptr;
            tinypy_value_t * value = nullptr;

            while( tinypy_dict_next( _dict, &position, &key, &value ) != 0 )
            {
                if( tinypy_dict_contains( _snapshot, key ) == 0 )
                {
                    tinypy_list_append( added, key );
                }
            }

            size_t addedCount = tinypy_list_size( added );

            for( size_t index = 0; index != addedCount; ++index )
            {
                tinypy_dict_delete( _dict, tinypy_list_get( added, index ) );
            }

            tinypy_release( added );

            position = 0;

            while( tinypy_dict_next( _snapshot, &position, &key, &value ) != 0 )
            {
                tinypy_dict_set( _dict, key, value );
            }
        }
        //////////////////////////////////////////////////////////////////////////
        static size_t append_text( char * const _buffer, size_t _capacity, size_t _length, const char * _text )
        {
            if( _length + 1 >= _capacity )
            {
                return _length;
            }

            size_t size = std::strlen( _text );
            size_t room = _capacity - _length - 1;
            size_t count = size < room ? size : room;
            std::memcpy( _buffer + _length, _text, count );
            _buffer[_length + count] = '\0';

            return _length + count;
        }
        //////////////////////////////////////////////////////////////////////////
        static uint32_t type_hash_index( const tinypy_type_t * _type, uint32_t _probe )
        {
            // As _Py_HashPointer: the low bits of an aligned address are zero.
            uint64_t address = static_cast<uint64_t>(reinterpret_cast<uintptr_t>(_type));
            uint64_t hash = ( address >> 4 ) | ( address << 60 );
            uint32_t index = static_cast<uint32_t>(( hash + static_cast<uint64_t>(_probe) * 17 ) % PYBIND_TYPE_COUNT_HASH);

            return index;
        }
        //////////////////////////////////////////////////////////////////////////
        static size_t encode_utf8( const wchar_t * _value, size_t _size, char * const _result, size_t _capacity )
        {
            (void)_capacity;

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
        , m_cycleDiagnostics( false )
        , m_vm( nullptr )
        , m_current_module( nullptr )
        , m_sys( nullptr )
        , m_observer( nullptr )
        , m_enumerator( 4 )
        , m_retired_scope_count( 0 )
#if defined(TINYPY_DEBUGGER)
        , m_debuggerHandler( nullptr )
        , m_debuggerUserData( nullptr )
#endif
    {
    }
    //////////////////////////////////////////////////////////////////////////
#if defined(TINYPY_DEBUGGER)
    void tinypy_kernel::debugger_trace_( void * _userData, const tinypy_debugger_event_t * _event )
    {
        tinypy_kernel * kernel = static_cast<tinypy_kernel *>(_userData);
        if( kernel->m_debuggerHandler == nullptr )
        {
            return;
        }

        debugger_event_t event;
        event.frame = detail::object_cast( _event->frame );
        event.argument = detail::object_cast( _event->exception );

        switch( _event->event )
        {
        case TINYPY_DEBUGGER_EVENT_LINE:
            event.event = debugger_event_e::line;
            break;
        case TINYPY_DEBUGGER_EVENT_CALL:
            event.event = debugger_event_e::call;
            break;
        case TINYPY_DEBUGGER_EVENT_RETURN:
            event.event = debugger_event_e::return_;
            break;
        case TINYPY_DEBUGGER_EVENT_EXCEPTION:
            event.event = debugger_event_e::exception;
            break;
        default:
            return;
        }

        kernel->m_debuggerHandler( kernel->m_debuggerUserData, event );
    }
#endif
    //////////////////////////////////////////////////////////////////////////
    bool tinypy_kernel::debugger_set_trace( pybind_debugger_handler_f _handler, void * _userData )
    {
#if defined(TINYPY_DEBUGGER)
        m_debuggerHandler = _handler;
        m_debuggerUserData = _userData;

        if( _handler == nullptr )
        {
            return tinypy_debugger_set( m_vm, nullptr ) != 0;
        }

        tinypy_debugger_t debugger;
        debugger.abi_version = TINYPY_ABI_VERSION;
        debugger.struct_size = static_cast<uint32_t>(sizeof(debugger));
        debugger.user_data = this;
        debugger.callback = &tinypy_kernel::debugger_trace_;

        return tinypy_debugger_set( m_vm, &debugger ) != 0;
#else
        (void)_userData;
        return _handler == nullptr;
#endif
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * tinypy_kernel::debugger_frame_back( PyObject * _frame )
    {
        return detail::object_cast( tinypy_frame_back( detail::value_cast( _frame ) ) );
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * tinypy_kernel::debugger_frame_code( PyObject * _frame )
    {
        return detail::object_cast( tinypy_frame_code( detail::value_cast( _frame ) ) );
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * tinypy_kernel::debugger_frame_locals( PyObject * _frame )
    {
        return detail::object_cast( tinypy_frame_locals( detail::value_cast( _frame ) ) );
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * tinypy_kernel::debugger_frame_globals( PyObject * _frame )
    {
        return detail::object_cast( tinypy_frame_globals( detail::value_cast( _frame ) ) );
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t tinypy_kernel::debugger_frame_line( PyObject * _frame )
    {
        int32_t line = tinypy_frame_line_number( detail::value_cast( _frame ) );
        return line > 0 ? static_cast<uint32_t>(line) : 0U;
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * tinypy_kernel::debugger_frame_get( PyObject * _frame, debugger_scope_e _scope, const char * _name )
    {
#if defined(TINYPY_DEBUGGER)
        tinypy_debugger_scope_e scope = _scope == debugger_scope_e::locals ? TINYPY_DEBUGGER_SCOPE_LOCALS : TINYPY_DEBUGGER_SCOPE_GLOBALS;
        return detail::object_cast( tinypy_debugger_frame_get( detail::value_cast( _frame ), scope, _name, std::strlen( _name ) ) );
#else
        (void)_frame;
        (void)_scope;
        (void)_name;
        return nullptr;
#endif
    }
    //////////////////////////////////////////////////////////////////////////
    bool tinypy_kernel::debugger_frame_set( PyObject * _frame, debugger_scope_e _scope, const char * _name, PyObject * _value )
    {
#if defined(TINYPY_DEBUGGER)
        tinypy_debugger_scope_e scope = _scope == debugger_scope_e::locals ? TINYPY_DEBUGGER_SCOPE_LOCALS : TINYPY_DEBUGGER_SCOPE_GLOBALS;
        return tinypy_debugger_frame_set( detail::value_cast( _frame ), scope, _name, std::strlen( _name ), detail::value_cast( _value ) ) != 0;
#else
        (void)_frame;
        (void)_scope;
        (void)_name;
        (void)_value;
        return false;
#endif
    }
    //////////////////////////////////////////////////////////////////////////
    bool tinypy_kernel::debugger_frame_delete( PyObject * _frame, debugger_scope_e _scope, const char * _name )
    {
#if defined(TINYPY_DEBUGGER)
        tinypy_debugger_scope_e scope = _scope == debugger_scope_e::locals ? TINYPY_DEBUGGER_SCOPE_LOCALS : TINYPY_DEBUGGER_SCOPE_GLOBALS;
        return tinypy_debugger_frame_delete( detail::value_cast( _frame ), scope, _name, std::strlen( _name ) ) != 0;
#else
        (void)_frame;
        (void)_scope;
        (void)_name;
        return false;
#endif
    }
    //////////////////////////////////////////////////////////////////////////
    void tinypy_kernel::debugger_frame_sync_locals( PyObject * _frame )
    {
#if defined(TINYPY_DEBUGGER)
        tinypy_debugger_frame_sync( detail::value_cast( _frame ) );
#else
        (void)_frame;
#endif
    }
    //////////////////////////////////////////////////////////////////////////
    void tinypy_kernel::cycle_diagnostic_( void * _userData, const tinypy_diagnostic_t * _diagnostic )
    {
        detail::cycle_diagnostic_holder_t * holder = static_cast<detail::cycle_diagnostic_holder_t *>(_userData);

        if( holder->handler == nullptr || _diagnostic == nullptr )
        {
            return;
        }

        holder->handler(
            holder->userData,
            _diagnostic->message,
            _diagnostic->message_size );
    }
    //////////////////////////////////////////////////////////////////////////
    bool tinypy_kernel::initialize( allocator_interface * _allocator, const kernel_config_t & _config )
    {
        m_allocator = _allocator;
        m_optimize_level = _config.optimize_level;

#if defined(TINYPY_CYCLE_DIAGNOSTICS)
        m_cycleDiagnostics = _config.cycle_diagnostics;
#endif

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
        tinypy_host_t host{};
        host.abi_version = TINYPY_ABI_VERSION;
        host.struct_size = sizeof( host );
        host.emit_output = &detail::host_emit_output;

        config.host = &host;
        config.max_heap_bytes = _config.max_heap_bytes;
        config.feature_flags = _config.feature_flags;
        config.optimize_level = _config.optimize_level;
        config.cycle_diagnostics = m_cycleDiagnostics == true ? 1 : 0;
        m_vm = tinypy_vm_create( &config );

        if( m_vm == nullptr )
        {
            return false;
        }

        for( typeid_t id = 0; id != PYBIND_TYPE_COUNT; ++id )
        {
            m_class_names[id] = nullptr;
            m_type_casts[id] = nullptr;
            m_scopes[id] = nullptr;
            m_retired_scopes[id] = nullptr;
        }

        m_dummy_scope = m_allocator->newT<tinypy_class_type_scope>( this, "__dummy__", 0, nullptr, nullptr, nullptr, 0, false );
#if defined(PYBIND_DEBUG)
        m_main_thread_id = std::this_thread::get_id();
#endif

        bool sysExists;
        m_sys = this->module_import( "sys", sysExists );
        assert( m_sys != nullptr );

        // As the CPython backend, the hook and the streams are those sys has
        // when used. As sys.excepthook and sys.__excepthook__ of CPython, the
        // default hook displays the error on sys.stderr.
        tinypy_value_t * hook = tinypy_native_function_new_items( m_vm, "excepthook", 10, &tinypy_kernel::call_default_excepthook_, 3, 3, this, nullptr );
        this->set_sys_value_( "excepthook", detail::object_cast( hook ) );
        this->set_sys_value_( "__excepthook__", detail::object_cast( hook ) );
        tinypy_release( hook );

#ifdef PYBIND_STL_SUPPORT
        pybind::initialize_stl_type_cast( this );
#endif

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
#if defined(PYBIND_DEBUG)
        m_main_thread_id = std::this_thread::get_id();
#endif
    }
#if defined(PYBIND_DEBUG)
    //////////////////////////////////////////////////////////////////////////
    void tinypy_kernel::check_main_thread_() const
    {
        // As PYBIND_CHECK_MAIN_THREAD of the CPython backend: the VM runs on the
        // thread that initialized it or last called update_main_thread.
        if( m_main_thread_id != std::this_thread::get_id() )
        {
            throw std::runtime_error( "not main thread" );
        }
    }
#endif
    //////////////////////////////////////////////////////////////////////////
    bool tinypy_kernel::is_cycle_diagnostics_enabled() const
    {
        return m_cycleDiagnostics;
    }
    //////////////////////////////////////////////////////////////////////////
    size_t tinypy_kernel::cycle_diagnostics( pybind_cycle_diagnostic_handler_f _handler, void * _userData )
    {
#if defined(TINYPY_CYCLE_DIAGNOSTICS)
        if( m_cycleDiagnostics == false || _handler == nullptr )
        {
            return 0;
        }

        detail::cycle_diagnostic_holder_t holder;
        holder.handler = _handler;
        holder.userData = _userData;

        return tinypy_vm_report_cycles( m_vm, &tinypy_kernel::cycle_diagnostic_, &holder );
#else
        (void)_handler;
        (void)_userData;

        return 0;
#endif
    }
    //////////////////////////////////////////////////////////////////////////
    void tinypy_kernel::destroy()
    {
        // As the CPython backend: no host callback runs while the VM is torn down.
        this->debugger_set_trace( nullptr, nullptr );
        this->remove_module_finder();

        this->set_current_module( nullptr );

        if( m_sys != nullptr )
        {
            this->decref( m_sys );
            m_sys = nullptr;
        }

        // The finalizers of the instances the VM releases run C++ destructors,
        // which may still wrap bound objects and strings, so the scopes and the
        // type casts outlive the VM, which releases their types.
        tinypy_vm_destroy( m_vm );
        m_vm = nullptr;

        for( class_type_scope_interface_ptr & scope : m_scopes )
        {
            if( scope != nullptr )
            {
                static_cast<tinypy_class_type_scope *>(scope.get())->detach();
            }
        }

#ifdef PYBIND_STL_SUPPORT
        pybind::finalize_stl_type_cast( this );
#endif

        kernel_interface * currentKernel = pybind::get_kernel();

        if( currentKernel == this )
        {
            pybind::set_kernel( nullptr );
        }

        for( typeid_t id = 0; id != PYBIND_TYPE_COUNT; ++id )
        {
            m_scopes[id] = nullptr;
            m_retired_scopes[id] = nullptr;

            if( m_class_names[id] != nullptr )
            {
                m_allocator->free( m_class_names[id] );
                m_class_names[id] = nullptr;
            }
        }

        for( class_type_scope_interface_ptr & scope : m_class_type_hashes )
        {
            scope = nullptr;
        }

        m_dummy_scope = nullptr;

        m_allocator->deleteT( this );
    }
    //////////////////////////////////////////////////////////////////////////
    void tinypy_kernel::remove_from_module( const char * _name, PyObject * _module )
    {
        PyObject * module = _module != nullptr ? _module : m_current_module;
        if( module == nullptr )
        {
            pybind::throw_exception( "remove_from_module '%s' not setup module!", _name );
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
        detail::callable_holder_t * holder = m_allocator->newP<detail::callable_holder_t>( detail::callable_holder_t{ this, detail::CallableFunction, _native, _adapter, nullptr } );
        return detail::object_cast( tinypy_native_function_new( m_vm, _adapter->getName(), std::strlen( _adapter->getName() ), &detail::call_adapter, holder, &detail::finalize_callable ) );
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * tinypy_kernel::create_functor_adapter( const functor_adapter_interface_ptr & _adapter, bool _native )
    {
        detail::callable_holder_t * holder = m_allocator->newP<detail::callable_holder_t>( detail::callable_holder_t{ this, detail::CallableFunctor, _native, nullptr, _adapter } );
        return detail::object_cast( tinypy_native_function_new( m_vm, _adapter->getName(), std::strlen( _adapter->getName() ), &detail::call_adapter, holder, &detail::finalize_callable ) );
    }
    //////////////////////////////////////////////////////////////////////////
    void tinypy_kernel::def_function_adapter( const function_adapter_interface_ptr & _adapter, bool _native, PyObject * _module )
    {
        PyObject * module = _module != nullptr ? _module : m_current_module;

        if( module == nullptr )
        {
            pybind::throw_exception( "def_function_adapter not setup module!" );
            return;
        }

        const char * name = _adapter->getName();

#if defined(PYBIND_DEBUG)
        if( this->dict_existstring( this->module_dict( module ), name ) == true )
        {
            pybind::throw_exception( "def_function_adapter module '%s' overriding object '%s'"
                , this->object_repr( module ).c_str()
                , name
            );

            return;
        }
#endif

        PyObject * function = this->create_function_adapter( _adapter, _native );
        this->module_addobject( module, name, function );
    }
    //////////////////////////////////////////////////////////////////////////
    void tinypy_kernel::def_functor_adapter( const functor_adapter_interface_ptr & _adapter, bool _native, PyObject * _module )
    {
        PyObject * module = _module != nullptr ? _module : m_current_module;

        if( module == nullptr )
        {
            pybind::throw_exception( "def_functor_adapter not setup module!" );
            return;
        }

        const char * name = _adapter->getName();

#if defined(PYBIND_DEBUG)
        if( this->dict_existstring( this->module_dict( module ), name ) == true )
        {
            pybind::throw_exception( "def_functor_adapter module '%s' overriding object '%s'"
                , this->object_repr( module ).c_str()
                , name
            );

            return;
        }
#endif

        PyObject * function = this->create_functor_adapter( _adapter, _native );
        this->module_addobject( module, name, function );
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * tinypy_kernel::create_member_adapter( const member_adapter_interface_ptr & _adapter, PyTypeObject * _type )
    {
        (void)_adapter;
        (void)_type;
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
        tinypy_class_type_scope::payload_t * payload = tinypy_class_type_scope::instance_payload( _object );

        if( payload == nullptr || ( payload->flags & tinypy_class_type_scope::PayloadUnwrapped ) != 0 )
        {
            return nullptr;
        }

        payload->flags |= tinypy_class_type_scope::PayloadUnwrapped;
        void * impl = payload->impl;
        payload->impl = nullptr;
        return impl;
    }
    //////////////////////////////////////////////////////////////////////////
    bool tinypy_kernel::is_wrap( PyObject * _object )
    {
        const tinypy_class_type_scope::payload_t * payload = tinypy_class_type_scope::instance_payload( _object );

        if( payload == nullptr )
        {
            return false;
        }

        return ( payload->flags & tinypy_class_type_scope::PayloadUnwrapped ) == 0;
    }
    //////////////////////////////////////////////////////////////////////////
    bool tinypy_kernel::is_class( PyObject * _object )
    {
        PyTypeObject * type = this->get_object_type( _object );

        return this->is_type_class( type );
    }
    //////////////////////////////////////////////////////////////////////////
    bool tinypy_kernel::is_type_class( PyTypeObject * _type )
    {
        return this->get_class_scope( _type )->get_type_id() != 0;
    }
    //////////////////////////////////////////////////////////////////////////
    bool tinypy_kernel::set_class_info_desc_( typeid_t _id, const char * _name )
    {
        if( _id == 0 || _id >= PYBIND_TYPE_COUNT || m_class_names[_id] != nullptr )
        {
            return false;
        }

        size_t size = ::strlen( _name );
        char * name = static_cast<char *>(m_allocator->malloc( size + 1 ));

        if( name == nullptr )
        {
            return false;
        }

        ::memcpy( name, _name, size + 1 );
        m_class_names[_id] = name;
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    typeid_t tinypy_kernel::next_type_id_()
    {
        if( m_enumerator == PYBIND_TYPE_COUNT )
        {
            return 0;
        }

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

        if( this->set_class_info_desc_( id, name ) == false )
        {
            pybind::throw_exception( "invalid create new type info '%s'", name );

            return 0;
        }

        return id;
    }
    //////////////////////////////////////////////////////////////////////////
    const char * tinypy_kernel::get_class_type_info( typeid_t _id )
    {
        if( _id >= PYBIND_TYPE_COUNT || m_class_names[_id] == nullptr )
        {
            return "";
        }

        return m_class_names[_id];
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
            const char * name = m_class_names[id];

            if( name == nullptr || ::strcmp( name, _name ) != 0 )
            {
                continue;
            }

            return id;
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

        // Instances outlive their scope until the VM finalizes them, so the
        // scope is retired instead of released.
        if( m_retired_scope_count == PYBIND_TYPE_COUNT )
        {
            pybind::throw_exception( "remove_type_scope '%s' retired scope count overflow", m_scopes[_info]->get_name() );

            return;
        }

        class_type_scope_interface_ptr scope = m_scopes[_info];
        scope->finalize();
        m_scopes[_info] = nullptr;
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

        if( scope->get_type_id() != 0 )
        {
            return scope->get_type_id();
        }

        if( this->is_none( _object ) == true
            || this->tuple_check( _object ) == true
            || this->list_check( _object ) == true
            || this->dict_check( _object ) == true
            || this->set_check( _object ) == true )
        {
            return 0;
        }

        if( this->int_check( _object ) == true || this->long_check( _object ) == true )
        {
            return this->class_info<int>();
        }

        if( this->float_check( _object ) == true )
        {
            return this->class_info<float>();
        }

        if( this->string_check( _object ) == true )
        {
            return this->class_info<const char *>();
        }

        if( this->unicode_check( _object ) == true )
        {
            return this->class_info<const wchar_t *>();
        }

        return 0;
    }
    //////////////////////////////////////////////////////////////////////////
    const class_type_scope_interface_ptr & tinypy_kernel::get_class_scope( PyTypeObject * _type )
    {
        const tinypy_type_t * type = detail::tinypy_type_cast( _type );
        size_t mroCount = tinypy_type_mro_size( type );

        for( size_t mroIndex = 0; mroIndex != mroCount; ++mroIndex )
        {
            const tinypy_type_t * mroType = tinypy_type_mro_at( type, mroIndex );
            const class_type_scope_interface_ptr & scope = this->get_class_scope_exact_( mroType );

            if( scope != m_dummy_scope )
            {
                return scope;
            }
        }

        return m_dummy_scope;
    }
    //////////////////////////////////////////////////////////////////////////
    const class_type_scope_interface_ptr & tinypy_kernel::get_class_scope_exact_( const tinypy_type_t * _type ) const
    {
        for( uint32_t probe = 0; probe != PYBIND_TYPE_COUNT_HASH; ++probe )
        {
            uint32_t index = detail::type_hash_index( _type, probe );
            const class_type_scope_interface_ptr & scope = m_class_type_hashes[index];

            if( scope == nullptr )
            {
                break;
            }

            if( detail::tinypy_type_cast( scope->get_typeobject() ) != _type )
            {
                continue;
            }

            return scope;
        }

        return m_dummy_scope;
    }
    //////////////////////////////////////////////////////////////////////////
    void tinypy_kernel::cache_class_scope_type( const class_type_scope_interface_ptr & _scope )
    {
        const tinypy_type_t * type = detail::tinypy_type_cast( _scope->get_typeobject() );

        for( uint32_t probe = 0; probe != PYBIND_TYPE_COUNT_HASH; ++probe )
        {
            uint32_t index = detail::type_hash_index( type, probe );
            class_type_scope_interface_ptr & scope = m_class_type_hashes[index];

            // The dummy scope marks a removed entry and matches no type.
            if( scope != nullptr && scope != m_dummy_scope )
            {
                continue;
            }

            scope = _scope;

            return;
        }

        pybind::throw_exception( "scope '%s' type hash table overflow", _scope->get_name() );
    }
    //////////////////////////////////////////////////////////////////////////
    void tinypy_kernel::remove_class_scope_type( const class_type_scope_interface_ptr & _scope )
    {
        const tinypy_type_t * type = detail::tinypy_type_cast( _scope->get_typeobject() );

        if( type == nullptr )
        {
            return;
        }

        for( uint32_t probe = 0; probe != PYBIND_TYPE_COUNT_HASH; ++probe )
        {
            uint32_t index = detail::type_hash_index( type, probe );
            class_type_scope_interface_ptr & scope = m_class_type_hashes[index];

            if( scope == nullptr )
            {
                return;
            }

            if( detail::tinypy_type_cast( scope->get_typeobject() ) != type )
            {
                continue;
            }

            scope = m_dummy_scope;

            return;
        }
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
        // As the CPython backend: the failed extract is reported, not left
        // pending for code that may run later.
        this->error_traceback( "invalid extract from %s to %s"
            , this->object_type_name( _object )
            , this->get_class_type_info( _info )
        );
    }
    //////////////////////////////////////////////////////////////////////////
    bool tinypy_kernel::instance_of_type( PyObject * _object, typeid_t _info )
    {
        if( _info >= PYBIND_TYPE_COUNT || m_scopes[_info] == nullptr || m_scopes[_info]->is_instance( this->get_object_type( _object ) ) == false )
        {
            return false;
        }

        return this->is_wrap( _object );
    }
    //////////////////////////////////////////////////////////////////////////
    void * tinypy_kernel::get_class_impl( PyObject * _object )
    {
        const tinypy_class_type_scope::payload_t * payload = tinypy_class_type_scope::instance_payload( _object );

        if( payload == nullptr )
        {
            return nullptr;
        }

        // As the CPython backend: an object whose class the scope of a base
        // answers for, once the scope that created it is removed, is no object
        // of that base, as its impl is not cast to it.
        const class_type_scope_interface_ptr & scope = this->get_class_scope( this->get_object_type( _object ) );

        if( scope != m_dummy_scope && scope.get() != payload->scope )
        {
            return nullptr;
        }

        return payload->impl;
    }
    //////////////////////////////////////////////////////////////////////////
    bool tinypy_kernel::is_class_weak( PyObject * _object )
    {
        const tinypy_class_type_scope::payload_t * payload = tinypy_class_type_scope::instance_payload( _object );

        if( payload == nullptr )
        {
            return false;
        }

        return ( payload->flags & tinypy_class_type_scope::PayloadWeak ) != 0;
    }
    //////////////////////////////////////////////////////////////////////////
    bool tinypy_kernel::type_initialize( PyObject * _object )
    {
        PyTypeObject * type = reinterpret_cast<PyTypeObject *>(_object);
        const class_type_scope_interface_ptr & scope = this->get_class_scope( type );

        if( scope->get_type_id() == 0 )
        {
            return false;
        }

        scope->type_initialize( type );
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
            const char * reason = "";
            int reasonSize = 0;

            if( error.message != nullptr )
            {
                reason = error.message;
                reasonSize = (int)error.message_size;
            }

            char message[PYBIND_EXCEPTION_MESSAGE_SIZE + 1];
            std::snprintf( message, sizeof( message ), "invalid marshal v2 code at offset %zu: %.*s", error.offset, reasonSize, reason );

            this->set_error( error_type_e::Value, message );
            this->report_error_( nullptr );

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
        tinypy_value_t * hook = this->get_sys_value_( "excepthook" );

        return detail::object_cast( hook );
    }
    //////////////////////////////////////////////////////////////////////////
    void tinypy_kernel::set_sys_excepthook( PyObject * _hook )
    {
        this->set_sys_value_( "excepthook", _hook );
    }
    //////////////////////////////////////////////////////////////////////////
    void tinypy_kernel::set_sys_excepthook_f( pybind_excepthook_handler_f _handler, void * _ud )
    {
        detail::excepthook_holder_t * holder = m_allocator->newP<detail::excepthook_holder_t>( detail::excepthook_holder_t{ this, _handler, _ud } );
        PyObject * hook = detail::object_cast( tinypy_native_function_new_items( m_vm, "excepthook", 10, &detail::call_excepthook, 3, 3, holder, &detail::finalize_excepthook ) );
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
        PyObject * result = this->ask_native_kw( _object, _args, nullptr );

        return result;
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * tinypy_kernel::ask_native_kw( PyObject * _object, PyObject * _args, PyObject * _kwargs )
    {
        PYBIND_TINYPY_CHECK_MAIN_THREAD();

        // As PyEval_CallObjectWithKeywords of the CPython backend: null
        // arguments are none, and arguments that are not a tuple or keywords
        // that are not a dict raise TypeError.
        if( _args != nullptr && this->tuple_check( _args ) == false )
        {
            this->set_error( error_type_e::Type, "argument list must be a tuple" );
            this->report_error_( nullptr );

            return nullptr;
        }

        if( _kwargs != nullptr && this->dict_check( _kwargs ) == false )
        {
            this->set_error( error_type_e::Type, "keyword list must be a dictionary" );
            this->report_error_( nullptr );

            return nullptr;
        }

        tinypy_value_t * args = detail::value_cast( _args );
        tinypy_value_t * empty = nullptr;

        if( args == nullptr )
        {
            empty = tinypy_tuple_new( m_vm, 0 );
            args = empty;
        }

        tinypy_error_t * error = nullptr;
        tinypy_value_t * result = tinypy_call( detail::value_cast( _object ), args, detail::value_cast( _kwargs ), &error );

        if( empty != nullptr )
        {
            tinypy_release( empty );
        }

        // As the CPython backend: an error the call left pending, or one pending
        // before it, is reported after the call.
        if( result == nullptr || tinypy_vm_has_error( m_vm ) != 0 )
        {
            this->report_error_( error );
        }

        return detail::object_cast( result );
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * tinypy_kernel::ask_method_native( PyObject * _object, const char * _method, PyObject * _args )
    {
        tinypy_error_t * error = nullptr;
        tinypy_value_t * method = tinypy_object_get_attr( detail::value_cast( _object ), _method, std::strlen( _method ), &error );

        if( method == nullptr )
        {
            this->report_error_( error );

            return nullptr;
        }

        PyObject * result = this->ask_native( detail::object_cast( method ), _args );
        tinypy_release( method );
        return result;
    }
    //////////////////////////////////////////////////////////////////////////
    void tinypy_kernel::call_method( PyObject * _object, const char * _method, const char * _format, ... )
    {
        va_list args;
        va_start( args, _format );
        PyObject * values = this->build_value_va( _format, args );
        va_end( args );
        if( values == nullptr )
        {
            this->report_error_( nullptr );
            return;
        }

        this->call_method_native( _object, _method, values );
        this->decref( values );
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
            this->report_error_( nullptr );
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

#if defined(PYBIND_DEBUG)
        if( adapter == nullptr )
        {
            pybind::throw_exception( "scope '%s' has no method '%s'"
                , _scope->get_name()
                , _name
            );

            return nullptr;
        }
#endif

        if( adapter == nullptr )
        {
            return nullptr;
        }

        PyObject * result = adapter->call( this, _self, _scope, _args, nullptr );

        return result;
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * tinypy_kernel::compile_string( const char * _source, const char * _file )
    {
        PYBIND_TINYPY_CHECK_MAIN_THREAD();

        if( this->report_pending_error_() == true )
        {
            return nullptr;
        }

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
        PYBIND_TINYPY_CHECK_MAIN_THREAD();

        if( this->report_pending_error_() == true )
        {
            return nullptr;
        }

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
        PYBIND_TINYPY_CHECK_MAIN_THREAD();

        if( this->report_pending_error_() == true )
        {
            return nullptr;
        }

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
    PyObject * tinypy_kernel::exec_source( const char * _source, const char * _filename, PyObject * _globals, PyObject * _locals )
    {
        PYBIND_TINYPY_CHECK_MAIN_THREAD();

        if( this->report_pending_error_() == true )
        {
            return nullptr;
        }

        tinypy_compile_options_t options;
        tinypy_compile_options_init( &options, TINYPY_COMPILE_EXEC );
        options.optimize_level = m_optimize_level;

        tinypy_error_t * error = nullptr;
        tinypy_value_t * code = tinypy_compile_source( m_vm, _source, std::strlen( _source ), _filename, std::strlen( _filename ), &options, &error );
        if( code == nullptr )
        {
            this->report_error_( error );

            return nullptr;
        }

        tinypy_value_t * result = tinypy_exec_code( code, detail::value_cast( _globals ), detail::value_cast( _locals ), &error );
        tinypy_release( code );
        if( result == nullptr )
        {
            this->report_error_( error );

            return nullptr;
        }

        return detail::object_cast( result );
    }
    //////////////////////////////////////////////////////////////////////////
    void tinypy_kernel::setStdOutHandle( PyObject * _object )
    {
        this->set_sys_value_( "stdout", _object );
    }
    //////////////////////////////////////////////////////////////////////////
    void tinypy_kernel::setStdErrorHandle( PyObject * _object )
    {
        this->set_sys_value_( "stderr", _object );
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * tinypy_kernel::getStdOutHandle()
    {
        tinypy_value_t * stream = this->get_sys_value_( "stdout" );

        return detail::object_cast( stream );
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * tinypy_kernel::getStdErrorHandle()
    {
        tinypy_value_t * stream = this->get_sys_value_( "stderr" );

        return detail::object_cast( stream );
    }
    //////////////////////////////////////////////////////////////////////////
    bool tinypy_kernel::get_traceback_top( char * const _filename, size_t _filenameSize, char * const _function, size_t _functionSize, uint32_t * _line )
    {
        tinypy_value_t * frame = tinypy_vm_current_frame( m_vm );

        if( frame == nullptr )
        {
            return false;
        }

        tinypy_value_t * code = tinypy_frame_code( frame );
        const char * filename = detail::code_text( tinypy_code_filename( code ) );
        const char * function = detail::code_text( tinypy_code_name( code ) );
        std::snprintf( _filename, _filenameSize, "%s", filename );
        std::snprintf( _function, _functionSize, "%s", function );
        *_line = this->debugger_frame_line( detail::object_cast( frame ) );
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool tinypy_kernel::get_traceback( char * _buffer, size_t _size, bool _append )
    {
        tinypy_value_t * frame = tinypy_vm_current_frame( m_vm );

        if( frame == nullptr )
        {
            return false;
        }

        if( _size == 0 )
        {
            return true;
        }

        if( _append == false )
        {
            _buffer[0] = '\0';
        }

        size_t length = std::strlen( _buffer );

        if( _append == true )
        {
            length = detail::append_text( _buffer, _size, length, "\n" );
        }

        for( ; frame != nullptr; frame = tinypy_frame_back( frame ) )
        {
            tinypy_value_t * code = tinypy_frame_code( frame );
            const char * filename = detail::code_text( tinypy_code_filename( code ) );
            const char * function = detail::code_text( tinypy_code_name( code ) );
            uint32_t line = this->debugger_frame_line( detail::object_cast( frame ) );

            char text[1024 + 1];
            std::snprintf( text, sizeof( text ), "File \"%s\", line %u, in %s\n", filename, line, function );

            length = detail::append_text( _buffer, _size, length, text );
        }

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
        this->report_traceback_( _format, _args );
    }
    //////////////////////////////////////////////////////////////////////////
    void tinypy_kernel::report_traceback_( const char * _format, va_list _args )
    {
        // As the CPython backend: the message names the current frame and is
        // reported at once, leaving no error pending.
        if( tinypy_vm_has_error( m_vm ) != 0 )
        {
            return;
        }

        tinypy_value_t * frame = tinypy_vm_current_frame( m_vm );

        if( frame == nullptr )
        {
            return;
        }

        char message[PYBIND_EXCEPTION_MESSAGE_SIZE + 1];
        std::vsnprintf( message, sizeof( message ), _format, _args );

        tinypy_value_t * code = tinypy_frame_code( frame );
        const char * filename = detail::code_text( tinypy_code_filename( code ) );
        uint32_t line = this->debugger_frame_line( detail::object_cast( frame ) );

        char text[PYBIND_EXCEPTION_MESSAGE_SIZE + 256 + 1];
        std::snprintf( text, sizeof( text ), "%s[%u]: %s", filename, line, message );

        this->set_error( error_type_e::Runtime, text );
        this->report_error_( nullptr );
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
        else if( _type == error_type_e::Index )
        {
            kind = TINYPY_ERROR_INDEX;
        }
        else if( _type == error_type_e::Attribute )
        {
            kind = TINYPY_ERROR_ATTRIBUTE;
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
        // As PyErr_WarnEx of the CPython backend under the default filters: the
        // RuntimeWarning is written on sys.stderr rather than raised.
        if( tinypy_vm_has_error( m_vm ) != 0 )
        {
            return;
        }

        tinypy_value_t * frame = tinypy_vm_current_frame( m_vm );

        if( frame == nullptr )
        {
            return;
        }

        char message[PYBIND_EXCEPTION_MESSAGE_SIZE + 1];
        std::vsnprintf( message, sizeof( message ), _format, _args );

        tinypy_value_t * code = tinypy_frame_code( frame );
        const char * filename = detail::code_text( tinypy_code_filename( code ) );
        uint32_t line = this->debugger_frame_line( detail::object_cast( frame ) );

        char text[PYBIND_EXCEPTION_MESSAGE_SIZE + 512 + 1];
        std::snprintf( text, sizeof( text ), "%s:%u: RuntimeWarning: %s[%u]: %s\n", filename, line, filename, line, message );

        // As the default filter of the warnings module: a warning is written
        // once for its place, which the registry of the globals of the frame
        // keeps.
        tinypy_value_t * globals = tinypy_frame_globals( frame );

        if( globals != nullptr && tinypy_typeof( globals ) == TINYPY_VALUE_DICT )
        {
            tinypy_value_t * registryName = tinypy_string_from_bytes( m_vm, "__warningregistry__", 19 );
            tinypy_value_t * registry = tinypy_dict_get( globals, registryName );

            if( registry == nullptr )
            {
                registry = tinypy_dict_new( m_vm );
                tinypy_dict_set( globals, registryName, registry );
                tinypy_release( registry );
            }

            tinypy_release( registryName );

            if( tinypy_typeof( registry ) == TINYPY_VALUE_DICT )
            {
                tinypy_value_t * key = tinypy_string_from_bytes( m_vm, text, std::strlen( text ) );
                bool written = tinypy_dict_contains( registry, key ) != 0;

                if( written == false )
                {
                    tinypy_value_t * value = tinypy_bool_from_i32( m_vm, 1 );
                    tinypy_dict_set( registry, key, value );
                    tinypy_release( value );
                }

                tinypy_release( key );

                if( written == true )
                {
                    return;
                }
            }
        }

        this->write_stderr_( text );
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
        tinypy_value_t * value = tinypy_build_value_va( m_vm, _format, _args, nullptr );

        if( value == nullptr )
        {
            this->report_error_( nullptr );
        }

        return detail::object_cast( value );
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
        return detail::object_cast( next );
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * tinypy_kernel::module_import( const char * _name, bool & _exists )
    {
        PYBIND_TINYPY_CHECK_MAIN_THREAD();

        size_t nameSize = std::strlen( _name );
        tinypy_error_t * error = nullptr;
        tinypy_value_t * package = tinypy_import_module( m_vm, _name, nameSize, nullptr, nullptr, 0, &error );

        if( package == nullptr )
        {
            // As the CPython backend: a module that is not found does not exist,
            // while one that fails as it runs does.
            bool notFound = error != nullptr && tinypy_error_kind( error ) == TINYPY_ERROR_IMPORT;

            tinypy_vm_normalize_error( m_vm, nullptr );

            tinypy_value_t * raisedType = tinypy_vm_raised_exception_type( m_vm );
            tinypy_value_t * importErrorType = detail::get_builtin( m_vm, "ImportError" );

            if( raisedType != nullptr && importErrorType != nullptr && tinypy_exception_matches( raisedType, importErrorType, nullptr ) == 1 )
            {
                notFound = true;
            }

            _exists = notFound == false;

            this->report_error_( error );

            return nullptr;
        }

        _exists = true;

        // As PyImport_ImportModule: the module of a dotted name, not the package
        // that __import__ returns for it.
        tinypy_value_t * key = tinypy_string_from_bytes( m_vm, _name, nameSize );
        tinypy_value_t * module = tinypy_dict_get( tinypy_vm_modules( m_vm ), key );
        tinypy_release( key );

        if( module == nullptr )
        {
            return detail::object_cast( package );
        }

        tinypy_retain( module );
        tinypy_release( package );

        return detail::object_cast( module );
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * tinypy_kernel::module_init( const char * _name )
    {
        size_t nameSize = std::strlen( _name );
        tinypy_value_t * key = tinypy_string_from_bytes( m_vm, _name, nameSize );
        tinypy_value_t * modules = tinypy_vm_modules( m_vm );
        tinypy_value_t * module = tinypy_dict_get( modules, key );

        if( module != nullptr && tinypy_typeof( module ) == TINYPY_VALUE_MODULE )
        {
            tinypy_release( key );

            // As the CPython backend: a module made elsewhere gets the builtins
            // it lacks.
            if( tinypy_module_get_value( module, "__builtins__", 12 ) == nullptr )
            {
                tinypy_module_add_value( module, "__builtins__", 12, tinypy_vm_builtins( m_vm ) );
            }

            return detail::object_cast( module );
        }

        module = tinypy_module_new( m_vm, _name, nameSize );
        tinypy_module_add_value( module, "__name__", 8, key );
        tinypy_module_add_value( module, "__builtins__", 12, tinypy_vm_builtins( m_vm ) );
        tinypy_dict_set( modules, key, module );
        tinypy_release( key );
        tinypy_release( module );
        return detail::object_cast( module );
    }
    //////////////////////////////////////////////////////////////////////////
    void tinypy_kernel::module_fini( PyObject * _module )
    {
        // As _PyModule_Clear of the CPython backend: the values become None,
        // first those of names with a single leading underscore, and
        // __builtins__ stays, so code that runs as they are released still
        // finds the names. The names are taken first, as that code may change
        // the dict.
        tinypy_value_t * dict = tinypy_module_dict( detail::value_cast( _module ) );
        tinypy_value_t * names = tinypy_list_from_items( m_vm, nullptr, 0 );

        size_t position = 0;
        tinypy_value_t * name;
        tinypy_value_t * value;

        while( tinypy_dict_next( dict, &position, &name, &value ) != 0 )
        {
            if( tinypy_typeof( name ) == TINYPY_VALUE_STRING )
            {
                tinypy_list_append( names, name );
            }
        }

        tinypy_value_t * none = tinypy_none_get( m_vm );
        size_t count = tinypy_list_size( names );

        for( uint32_t pass = 0; pass != 2; ++pass )
        {
            for( size_t index = 0; index != count; ++index )
            {
                tinypy_value_t * key = tinypy_list_get( names, index );
                const char * text = detail::code_text( key );

                bool underscore = text[0] == '_' && text[1] != '_';

                if( pass == 0 && underscore == false )
                {
                    continue;
                }

                if( pass == 1 && std::strcmp( text, "__builtins__" ) == 0 )
                {
                    continue;
                }

                tinypy_dict_set( dict, key, none );
            }
        }

        tinypy_release( none );
        tinypy_release( names );
    }
    //////////////////////////////////////////////////////////////////////////
    bool tinypy_kernel::module_check( PyObject * _module ) const
    {
        return tinypy_typeof( detail::value_cast( _module ) ) == TINYPY_VALUE_MODULE;
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
        PYBIND_TINYPY_CHECK_MAIN_THREAD();

        if( this->report_pending_error_() == true )
        {
            return nullptr;
        }

        // As PyImport_ExecCodeModule through PyImport_AddModule: an entry of
        // sys.modules that is not a module is replaced, the module names the
        // file of its code, a failure removes it from sys.modules, and the result
        // is the entry the code leaves there.
        tinypy_value_t * module = detail::value_cast( this->module_init( _name ) );
        tinypy_retain( module );

        tinypy_value_t * code = detail::value_cast( _code );
        tinypy_module_add_value( module, "__file__", 8, tinypy_code_filename( code ) );

        tinypy_value_t * key = tinypy_string_from_bytes( m_vm, _name, std::strlen( _name ) );
        tinypy_value_t * modules = tinypy_vm_modules( m_vm );

        tinypy_error_t * error = nullptr;
        tinypy_value_t * none = tinypy_exec_code( code, tinypy_module_dict( module ), tinypy_module_dict( module ), &error );

        if( none == nullptr )
        {
            // As PyImport_ExecCodeModule: the module is removed before the error
            // is reported, so the hook does not see it.
            tinypy_dict_delete( modules, key );
            tinypy_release( key );
            tinypy_release( module );

            this->report_error_( error );

            return nullptr;
        }

        tinypy_release( none );
        tinypy_release( module );

        tinypy_value_t * entry = tinypy_dict_get( modules, key );
        tinypy_release( key );

        if( entry == nullptr )
        {
            char message[PYBIND_EXCEPTION_MESSAGE_SIZE + 1];
            std::snprintf( message, sizeof( message ), "Loaded module %s not found in sys.modules", _name );

            this->set_error( error_type_e::Runtime, message );
            this->report_error_( nullptr );

            return nullptr;
        }

        tinypy_retain( entry );

        return detail::object_cast( entry );
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * tinypy_kernel::module_reload( PyObject * _module )
    {
        PYBIND_TINYPY_CHECK_MAIN_THREAD();

        // As PyImport_ReloadModule: the builtin reload runs the source of the
        // module again in its own dict, so the functions defined before see the
        // new globals and the names the source no longer defines stay.
        tinypy_value_t * reload = detail::get_builtin( m_vm, "reload" );

        if( reload == nullptr )
        {
            this->set_error( error_type_e::Runtime, "module_reload: no builtin reload" );

            return nullptr;
        }

        tinypy_value_t * module = detail::value_cast( _module );
        tinypy_value_t * args = tinypy_tuple_from_items( m_vm, &module, 1 );
        tinypy_error_t * error = nullptr;
        tinypy_value_t * reloaded = tinypy_call( reload, args, nullptr, &error );
        tinypy_release( args );

        if( reloaded == nullptr )
        {
            this->report_error_( error );

            return nullptr;
        }

        return detail::object_cast( reloaded );
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * tinypy_kernel::module_reload_source( PyObject * _module, const char * _source, const char * _filename )
    {
        PYBIND_TINYPY_CHECK_MAIN_THREAD();

        if( this->report_pending_error_() == true )
        {
            return nullptr;
        }

        tinypy_compile_options_t options;
        tinypy_compile_options_init( &options, TINYPY_COMPILE_EXEC );
        options.optimize_level = m_optimize_level;

        tinypy_error_t * error = nullptr;
        tinypy_value_t * code = tinypy_compile_source( m_vm, _source, std::strlen( _source ), _filename, std::strlen( _filename ), &options, &error );
        if( code == nullptr )
        {
            this->report_error_( error );

            return nullptr;
        }

        // As reload, the source runs in the dict of the module, so the functions
        // defined before see the new globals and the names the source does not
        // define stay, as __file__ and __path__ its loader set. A snapshot of
        // the names restores them when the source fails.
        tinypy_value_t * module = detail::value_cast( _module );
        tinypy_value_t * live_dict = tinypy_module_dict( module );
        tinypy_value_t * snapshot = tinypy_dict_new( m_vm );

        size_t position = 0;
        tinypy_value_t * key = nullptr;
        tinypy_value_t * value = nullptr;

        while( tinypy_dict_next( live_dict, &position, &key, &value ) != 0 )
        {
            tinypy_dict_set( snapshot, key, value );
        }

        tinypy_value_t * execution_result = tinypy_exec_code( code, live_dict, live_dict, &error );
        tinypy_release( code );

        if( execution_result == nullptr )
        {
            this->report_error_( error );

            detail::restore_dict( m_vm, live_dict, snapshot );
            tinypy_release( snapshot );

            return nullptr;
        }

        tinypy_release( execution_result );
        tinypy_release( snapshot );
        tinypy_retain( module );

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
    bool tinypy_kernel::type_check( PyObject * _object ) const
    {
        return tinypy_typeof( detail::value_cast( _object ) ) == TINYPY_VALUE_TYPE;
    }
    //////////////////////////////////////////////////////////////////////////
    bool tinypy_kernel::test_equal( PyObject * _left, PyObject * _right )
    {
        // As PyObject_RichCompareBool of the CPython backend: __eq__ decides, and
        // the error it raises is reported.
        tinypy_error_t * error = nullptr;
        int32_t result = tinypy_compare_bool( detail::value_cast( _left ), detail::value_cast( _right ), TINYPY_COMPARE_EQUAL, &error );

        if( result < 0 )
        {
            this->report_error_( error );

            return false;
        }

        return result != 0;
    }
    //////////////////////////////////////////////////////////////////////////
    bool tinypy_kernel::has_attr( PyObject * _object, PyObject * _attribute )
    {
        // As tuple_setitem: a null object stands for the error that produced
        // it, so nothing is looked up.
        if( _object == nullptr || _attribute == nullptr )
        {
            return false;
        }

        return tinypy_object_has_attr_value( detail::value_cast( _object ), detail::value_cast( _attribute ) ) != 0;
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * tinypy_kernel::get_attr( PyObject * _object, PyObject * _attribute )
    {
        // As tuple_setitem: a null object stands for the error that produced
        // it, so nothing is looked up.
        if( _object == nullptr || _attribute == nullptr )
        {
            return nullptr;
        }

        tinypy_error_t * error = nullptr;
        tinypy_value_t * result = tinypy_object_get_attr_value( detail::value_cast( _object ), detail::value_cast( _attribute ), &error );
        if( result == nullptr )
        {
            this->report_error_( error );
        }

        return detail::object_cast( result );
    }
    //////////////////////////////////////////////////////////////////////////
    bool tinypy_kernel::set_attr( PyObject * _object, PyObject * _attribute, PyObject * _value )
    {
        // As tuple_setitem: a null item stands for the error that produced it.
        if( _object == nullptr || _attribute == nullptr || _value == nullptr )
        {
            return false;
        }

        tinypy_error_t * error = nullptr;
        int32_t result = tinypy_object_set_attr_value( detail::value_cast( _object ), detail::value_cast( _attribute ), detail::value_cast( _value ), &error );
        if( result == 0 )
        {
            this->report_error_( error );
        }

        return result != 0;
    }
    //////////////////////////////////////////////////////////////////////////
    bool tinypy_kernel::set_attrstring( PyObject * _object, const char * _attribute, PyObject * _value )
    {
        // As tuple_setitem: a null item stands for the error that produced it.
        if( _object == nullptr || _value == nullptr )
        {
            return false;
        }

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
        // As tuple_setitem: a null object stands for the error that produced
        // it, so nothing is looked up.
        if( _object == nullptr )
        {
            return false;
        }

        // As has_attr: the error of the lookup is cleared, not one pending
        // before it.
        return tinypy_object_has_attr( detail::value_cast( _object ), _attribute, std::strlen( _attribute ) ) != 0;
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * tinypy_kernel::get_attrstring( PyObject * _object, const char * _attribute )
    {
        // As tuple_setitem: a null object stands for the error that produced
        // it, so nothing is looked up.
        if( _object == nullptr )
        {
            return nullptr;
        }

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
        tinypy_value_t * function = detail::get_builtin( m_vm, "dir" );
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

        // An error path formats its object while the error is pending, which
        // Python code must not run with, nor clear.
        detail::raised_error_t raised;
        detail::fetch_raised_error( m_vm, &raised );

        tinypy_error_t * error = nullptr;
        tinypy_value_t * result = tinypy_object_str( detail::value_cast( _object ), &error );
        if( result == nullptr )
        {
            this->report_error_( error );
        }

        detail::restore_raised_error( &raised );

        return string_view( this, detail::object_cast( result ), pybind::borrowed );
    }
    //////////////////////////////////////////////////////////////////////////
    string_view tinypy_kernel::object_repr( PyObject * _object )
    {
        if( _object == nullptr )
        {
            return string_view( this, nullptr, pybind::borrowed );
        }

        // An error path formats its object while the error is pending, which
        // Python code must not run with, nor clear.
        detail::raised_error_t raised;
        detail::fetch_raised_error( m_vm, &raised );

        tinypy_error_t * error = nullptr;
        tinypy_value_t * result = tinypy_object_repr( detail::value_cast( _object ), &error );
        if( result == nullptr )
        {
            this->report_error_( error );
        }

        detail::restore_raised_error( &raised );

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
        if( _object == nullptr )
        {
            return false;
        }

        // As the CPython backend: a value that does not extract leaves no
        // error, and one pending before stays.
        detail::raised_error_t raised;
        detail::fetch_raised_error( m_vm, &raised );

        tinypy_error_t * error = nullptr;
        int32_t result = tinypy_truth( detail::value_cast( _object ), &error );
        if( result < 0 )
        {
            if( error != nullptr )
            {
                tinypy_error_release( error );
            }

            tinypy_vm_clear_error( m_vm );
            detail::restore_raised_error( &raised );

            return false;
        }

        detail::restore_raised_error( &raised );

        _value = result != 0;
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool tinypy_kernel::extract_int64( PyObject * _object, int64_t & _value )
    {
        if( _object == nullptr )
        {
            return false;
        }

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
                if( raw > ( UINT64_MAX >> 15 ) )
                {
                    return false;
                }

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

        if( kind == TINYPY_VALUE_FLOAT )
        {
            double value = tinypy_float_as_double( detail::value_cast( _object ) );

            // NaN fails both comparisons.
            if( ( value >= -9223372036854775808.0 && value < 9223372036854775808.0 ) == false )
            {
                return false;
            }

            _value = static_cast<int64_t>(value);
            return true;
        }

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
        if( _object == nullptr )
        {
            return false;
        }

        tinypy_value_type_e kind = tinypy_typeof( detail::value_cast( _object ) );
        if( kind == TINYPY_VALUE_LONG )
        {
            return detail::long_to_u64( detail::value_cast( _object ), &_value );
        }

        if( kind == TINYPY_VALUE_FLOAT )
        {
            double value = tinypy_float_as_double( detail::value_cast( _object ) );

            // NaN fails both comparisons.
            if( ( value > -1.0 && value < 18446744073709551616.0 ) == false )
            {
                return false;
            }

            _value = static_cast<uint64_t>(value);
            return true;
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
        if( _object == nullptr )
        {
            return false;
        }

        tinypy_value_t * object = detail::value_cast( _object );
        tinypy_value_type_e kind = tinypy_typeof( object );

        switch( kind )
        {
        case TINYPY_VALUE_FLOAT:
            {
                _value = tinypy_float_as_double( object );
                return true;
            }
        case TINYPY_VALUE_LONG:
            {
                // As the CPython backend: a long a double cannot hold does not
                // extract and leaves no OverflowError, and an error pending
                // before stays.
                detail::raised_error_t raised;
                detail::fetch_raised_error( m_vm, &raised );

                tinypy_bool_t converted = tinypy_long_as_double( object, &_value, nullptr );

                tinypy_vm_clear_error( m_vm );
                detail::restore_raised_error( &raised );

                return converted != 0;
            }
        case TINYPY_VALUE_INTEGER:
            {
                int64_t integer = tinypy_integer_as_i64( object );
                _value = static_cast<double>(integer);
                return true;
            }
        default:
            return false;
        }
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
        if( _object == nullptr )
        {
            return false;
        }

        if( this->unicode_check( _object ) == false )
        {
            return false;
        }

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
        // As Python 2: a value an int holds is an int.
        return this->ptr_int64( (int64_t)_value );
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * tinypy_kernel::ptr_ulong( unsigned long _value )
    {
        return this->ptr_uint64( (uint64_t)_value );
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
        if( this->string_check( _object ) == false )
        {
            return 0;
        }

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
        return this->string_to_char_and_size( _object, &size );
    }
    //////////////////////////////////////////////////////////////////////////
    const char * tinypy_kernel::string_to_char_and_size( PyObject * _object, size_t * _size )
    {
        if( this->string_check( _object ) == false )
        {
            this->set_error( error_type_e::Type, "bad argument type for built-in operation" );
            return nullptr;
        }

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
        assert( this->bytearray_check( _object ) == true );

        return tinypy_bytearray_size( detail::value_cast( _object ) );
    }
    //////////////////////////////////////////////////////////////////////////
    const uint8_t * tinypy_kernel::bytearray_to_data( PyObject * _object ) const
    {
        assert( this->bytearray_check( _object ) == true );

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
        if( this->unicode_check( _object ) == false )
        {
            this->set_error( error_type_e::Type, "bad argument type for built-in operation" );
            return nullptr;
        }

        tinypy_error_t * error = nullptr;
        const wchar_t * value = tinypy_unicode_wchar_view( detail::value_cast( _object ), _size, &error );
        if( value == nullptr )
        {
            this->report_error_( error );
        }

        return value;
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

        // As unicode_from_utf8: a code point a unicode cannot hold, past
        // U+10FFFF, is reported rather than stored.
        tinypy_error_t * error = nullptr;
        tinypy_value_t * unicode = tinypy_unicode_decode_utf8( m_vm, utf8, utf8Size, &error );
        m_allocator->free( utf8 );

        if( unicode == nullptr )
        {
            this->report_error_( error );
        }

        return detail::object_cast( unicode );
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * tinypy_kernel::unicode_encode_utf8( PyObject * _object )
    {
        if( this->unicode_check( _object ) == false )
        {
            this->set_error( error_type_e::Type, "bad argument type for built-in operation" );
            return nullptr;
        }

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
        tinypy_error_t * error = nullptr;
        tinypy_value_t * unicode = tinypy_unicode_decode_utf8( m_vm, _value, _size, &error );
        if( unicode == nullptr )
        {
            this->report_error_( error );
        }

        return detail::object_cast( unicode );
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * tinypy_kernel::list_new( size_t _size )
    {
        tinypy_value_t * result = tinypy_list_from_items( m_vm, nullptr, 0 );
        tinypy_value_t * none = tinypy_none_get( m_vm );

        for( size_t index = 0; index != _size; ++index )
        {
            tinypy_list_append( result, none );

            if( tinypy_list_size( result ) != index + 1 )
            {
                break;
            }
        }

        tinypy_release( none );

        if( tinypy_list_size( result ) != _size )
        {
            tinypy_release( result );

            if( tinypy_vm_has_error( m_vm ) == 0 )
            {
                tinypy_vm_raise_error( m_vm, TINYPY_ERROR_MEMORY, "list is too large" );
            }

            this->report_error_( nullptr );

            return nullptr;
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
        // As the CPython backend: an index out of range raises IndexError in
        // every build.
        if( _index >= tinypy_list_size( detail::value_cast( _object ) ) )
        {
            this->set_error( error_type_e::Index, "list index out of range" );
            this->report_error_( nullptr );

            return nullptr;
        }

        return detail::object_cast( tinypy_list_get( detail::value_cast( _object ), _index ) );
    }
    //////////////////////////////////////////////////////////////////////////
    bool tinypy_kernel::list_insert( PyObject * _object, size_t _index, PyObject * _item )
    {
        // As tuple_setitem: a null item stands for the error that produced it.
        if( _item == nullptr )
        {
            return false;
        }

        // As PyList_Insert: an index past the end appends.
        tinypy_value_t * list = detail::value_cast( _object );
        size_t size = tinypy_list_size( list );
        size_t index = _index < size ? _index : size;

        tinypy_list_insert( list, index, detail::value_cast( _item ) );
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool tinypy_kernel::list_remove( PyObject * _object, size_t _index )
    {
        // As the slice deletion of the CPython backend: an index past the end
        // removes nothing.
        tinypy_value_t * list = detail::value_cast( _object );

        if( _index >= tinypy_list_size( list ) )
        {
            return true;
        }

        tinypy_list_delete( list, _index );
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool tinypy_kernel::list_setitem( PyObject * _object, size_t _index, PyObject * _item )
    {
        // As tuple_setitem: a null item stands for the error that produced it.
        if( _item == nullptr )
        {
            return false;
        }

        // As the CPython backend: an index out of range raises IndexError in
        // every build.
        if( _index >= tinypy_list_size( detail::value_cast( _object ) ) )
        {
            this->set_error( error_type_e::Index, "list assignment index out of range" );
            this->report_error_( nullptr );

            return false;
        }

        tinypy_list_set( detail::value_cast( _object ), _index, detail::value_cast( _item ) );
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool tinypy_kernel::list_appenditem( PyObject * _object, PyObject * _item )
    {
        // As tuple_setitem: a null item stands for the error that produced it.
        if( _item == nullptr )
        {
            return false;
        }

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
        // As the CPython backend: the attributes dir() lists, but those that raise.
        PyObject * dir = this->object_dir( _object );

        if( dir == nullptr )
        {
            return nullptr;
        }

        PyObject * result = this->dict_new();

        // The list may be the one __dir__ returned, which a lookup may change,
        // so its size is read for each name, and the name is held.
        for( size_t index = 0; index < this->list_size( dir ); ++index )
        {
            tinypy_value_t * key = tinypy_list_get( detail::value_cast( dir ), index );
            tinypy_retain( key );

            tinypy_error_t * error = nullptr;
            tinypy_value_t * value = tinypy_object_get_attr_value( detail::value_cast( _object ), key, &error );

            if( value == nullptr )
            {
                if( error != nullptr )
                {
                    tinypy_error_release( error );
                }

                tinypy_vm_clear_error( m_vm );
                tinypy_release( key );

                continue;
            }

            tinypy_dict_set( detail::value_cast( result ), key, value );
            tinypy_release( value );
            tinypy_release( key );
        }

        this->decref( dir );

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
        // As tuple_setitem: a null item stands for the error that produced it.
        if( _key == nullptr || _value == nullptr )
        {
            return false;
        }

        // Unlike tinypy_dict_set, reports an unhashable key as PyDict_SetItem does.
        tinypy_error_t * error = nullptr;
        if( tinypy_set_item( detail::value_cast( _dict ), detail::value_cast( _key ), detail::value_cast( _value ), &error ) == 0 )
        {
            this->report_error_( error );
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool tinypy_kernel::dict_setstring( PyObject * _dict, const char * _key, PyObject * _value )
    {
        // As tuple_setitem: a null item stands for the error that produced it.
        if( _value == nullptr )
        {
            return false;
        }

        tinypy_value_t * key = tinypy_string_from_bytes( m_vm, _key, std::strlen( _key ) );
        tinypy_dict_set( detail::value_cast( _dict ), key, detail::value_cast( _value ) );
        tinypy_release( key );
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool tinypy_kernel::dict_remove( PyObject * _dict, PyObject * _key )
    {
        // As tuple_setitem: a null object stands for the error that produced
        // it, so nothing is looked up.
        if( _key == nullptr )
        {
            return false;
        }

        bool exists = this->dict_exist( _dict, _key );

        if( exists == false )
        {
            return false;
        }

        tinypy_dict_delete( detail::value_cast( _dict ), detail::value_cast( _key ) );

        return true;
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
        // As tuple_setitem: a null object stands for the error that produced
        // it, so nothing is looked up.
        if( _key == nullptr )
        {
            return nullptr;
        }

        bool pending = tinypy_vm_has_error( m_vm ) != 0;

        tinypy_value_t * value = tinypy_dict_get( detail::value_cast( _dict ), detail::value_cast( _key ) );

        // As PyDict_GetItem: the error of a key that cannot be hashed is
        // suppressed, and one pending before stays.
        if( value == nullptr && pending == false && tinypy_vm_has_error( m_vm ) != 0 )
        {
            tinypy_vm_clear_error( m_vm );
        }

        return detail::object_cast( value );
    }
    //////////////////////////////////////////////////////////////////////////
    bool tinypy_kernel::dict_exist( PyObject * _dict, PyObject * _key )
    {
        // As tuple_setitem: a null object stands for the error that produced
        // it, so nothing is looked up.
        if( _key == nullptr )
        {
            return false;
        }

        bool pending = tinypy_vm_has_error( m_vm ) != 0;

        bool exists = tinypy_dict_contains( detail::value_cast( _dict ), detail::value_cast( _key ) ) != 0;

        // As the CPython backend: the error of a key that cannot be hashed is
        // reported, while one pending before is not the lookup's.
        if( pending == false && tinypy_vm_has_error( m_vm ) != 0 )
        {
            this->report_error_( nullptr );

            return false;
        }

        return exists;
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
        // As tuple_setitem: a null item stands for the error that produced it.
        if( _value == nullptr )
        {
            return false;
        }

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
        // As tuple_setitem: a null object stands for the error that produced
        // it, so nothing is looked up.
        if( _value == nullptr )
        {
            return false;
        }

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
        // As tuple_setitem: a null object stands for the error that produced
        // it, so nothing is looked up.
        if( _value == nullptr )
        {
            return false;
        }

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
        // As the CPython backend: an index out of range raises IndexError in
        // every build.
        if( _index >= tinypy_tuple_size( detail::value_cast( _object ) ) )
        {
            this->set_error( error_type_e::Index, "tuple index out of range" );
            this->report_error_( nullptr );

            return nullptr;
        }

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
        // As the CPython backend: a null item stands for the error that
        // produced it.
        if( _value == nullptr )
        {
            return false;
        }

        // As the CPython backend: an index out of range raises IndexError in
        // every build.
        if( _index >= tinypy_tuple_size( detail::value_cast( _object ) ) )
        {
            this->set_error( error_type_e::Index, "tuple assignment index out of range" );
            this->report_error_( nullptr );

            return false;
        }

        tinypy_tuple_set( detail::value_cast( _object ), _index, detail::value_cast( _value ) );
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * tinypy_kernel::exception_new( const char * _message )
    {
        tinypy_value_t * type = detail::get_builtin( m_vm, "RuntimeError" );
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

        // As PyErr_PrintEx: normalize the pending exception (the raised value may
        // be unnormalized, e.g. after sys.exit or `raise E, v`), then fetch it so
        // the hook runs without an exception pending.
        tinypy_vm_normalize_error( m_vm, nullptr );

        tinypy_value_t * exceptionType = tinypy_vm_raised_exception_type( m_vm );
        tinypy_value_t * exceptionValue = tinypy_vm_raised_exception( m_vm );
        tinypy_value_t * traceback = tinypy_vm_raised_traceback( m_vm );

        if( exceptionType != nullptr )
        {
            tinypy_retain( exceptionType );
        }
        if( exceptionValue != nullptr )
        {
            tinypy_retain( exceptionValue );
        }
        if( traceback != nullptr )
        {
            tinypy_retain( traceback );
        }

        tinypy_vm_clear_error( m_vm );

        if( exceptionType != nullptr && exceptionValue != nullptr )
        {
            tinypy_value_t * hook = this->get_sys_value_( "excepthook" );

            // As PyErr_PrintEx and the CPython backend: a hook of None is
            // missing, and one that is not callable fails as any other.
            if( hook != nullptr && tinypy_typeof( hook ) != TINYPY_VALUE_NONE )
            {
                // The hook may replace itself on sys while it runs.
                tinypy_retain( hook );

                tinypy_value_t * none = nullptr;
                tinypy_value_t * hookTraceback = traceback;

                if( hookTraceback == nullptr )
                {
                    none = tinypy_none_get( m_vm );
                    hookTraceback = none;
                }

                tinypy_value_t * items[] = { exceptionType, exceptionValue, hookTraceback };
                tinypy_value_t * args = tinypy_tuple_from_items( m_vm, items, 3 );
                tinypy_error_t * hookError = nullptr;
                tinypy_value_t * result = tinypy_call( hook, args, nullptr, &hookError );

                tinypy_release( args );
                tinypy_release( hook );

                if( none != nullptr )
                {
                    tinypy_release( none );
                }

                if( hookError != nullptr )
                {
                    tinypy_error_release( hookError );
                }

                if( result != nullptr )
                {
                    tinypy_release( result );
                }
                else
                {
                    // As PyErr_PrintEx: both the failure of the hook and the
                    // original exception are displayed.
                    tinypy_vm_normalize_error( m_vm, nullptr );
                    tinypy_value_t * hookItems[] = { tinypy_vm_raised_exception_type( m_vm ), tinypy_vm_raised_exception( m_vm ), tinypy_vm_raised_traceback( m_vm ) };

                    for( tinypy_value_t * hookItem : hookItems )
                    {
                        if( hookItem != nullptr )
                        {
                            tinypy_retain( hookItem );
                        }
                    }

                    tinypy_vm_clear_error( m_vm );

                    this->write_stderr_( "Error in sys.excepthook:\n" );

                    if( hookItems[0] != nullptr && hookItems[1] != nullptr )
                    {
                        this->display_error_( hookItems[0], hookItems[1], hookItems[2] );
                    }

                    this->write_stderr_( "\nOriginal exception was:\n" );
                    this->display_error_( exceptionType, exceptionValue, traceback );

                    for( tinypy_value_t * hookItem : hookItems )
                    {
                        if( hookItem != nullptr )
                        {
                            tinypy_release( hookItem );
                        }
                    }
                }
            }
            else
            {
                this->write_stderr_( "sys.excepthook is missing\n" );
                this->display_error_( exceptionType, exceptionValue, traceback );
            }
        }

        if( exceptionType != nullptr )
        {
            tinypy_release( exceptionType );
        }
        if( exceptionValue != nullptr )
        {
            tinypy_release( exceptionValue );
        }
        if( traceback != nullptr )
        {
            tinypy_release( traceback );
        }

        tinypy_vm_clear_error( m_vm );
    }
    //////////////////////////////////////////////////////////////////////////
    void tinypy_kernel::display_error_( tinypy_value_t * _type, tinypy_value_t * _value, tinypy_value_t * _traceback )
    {
        // As PyErr_Display: the traceback, written entry by entry as
        // PyTraceBack_Print does, then the class, as module.name unless it is
        // a builtin exception, and the str() of the value.
        if( _traceback != nullptr && tinypy_typeof( _traceback ) == TINYPY_VALUE_TRACEBACK )
        {
            this->write_stderr_( "Traceback (most recent call last):\n" );

            for( tinypy_value_t * entry = _traceback; entry != nullptr; entry = tinypy_traceback_next( entry ) )
            {
                tinypy_value_t * code = tinypy_frame_code( tinypy_traceback_frame( entry ) );
                const char * filename = detail::code_text( tinypy_code_filename( code ) );
                const char * function = detail::code_text( tinypy_code_name( code ) );
                int32_t line = tinypy_traceback_line_number( entry );

                char entryText[1024 + 1];
                std::snprintf( entryText, sizeof( entryText ), "  File \"%s\", line %d, in %s\n", filename, line, function );

                this->write_stderr_( entryText );
            }
        }

        const char * className = "<unknown>";
        tinypy_value_type_e typeKind = tinypy_typeof( _type );

        if( typeKind == TINYPY_VALUE_TYPE )
        {
            size_t classNameSize;
            className = tinypy_type_name( tinypy_value_as_const_type( _type ), &classNameSize );
        }
        else if( typeKind == TINYPY_VALUE_CLASS )
        {
            className = detail::code_text( tinypy_class_name( _type ) );
        }

        const char * classDot = std::strrchr( className, '.' );

        if( classDot != nullptr )
        {
            className = classDot + 1;
        }

        tinypy_error_t * error = nullptr;
        tinypy_value_t * moduleName = tinypy_object_get_attr( _type, "__module__", 10, &error );

        if( moduleName != nullptr )
        {
            const char * moduleText = detail::code_text( moduleName );

            if( moduleText[0] != '\0' && std::strcmp( moduleText, "exceptions" ) != 0 )
            {
                this->write_stderr_( moduleText );
                this->write_stderr_( "." );
            }

            tinypy_release( moduleName );
        }
        else
        {
            if( error != nullptr )
            {
                tinypy_error_release( error );
            }

            tinypy_vm_clear_error( m_vm );
        }

        this->write_stderr_( className );

        if( _value != nullptr && tinypy_typeof( _value ) != TINYPY_VALUE_NONE )
        {
            error = nullptr;
            tinypy_value_t * rendered = tinypy_object_str( _value, &error );

            if( rendered != nullptr )
            {
                size_t renderedSize = 0;
                size_t renderedCount = 0;
                const char * renderedText = tinypy_typeof( rendered ) == TINYPY_VALUE_UNICODE ? tinypy_unicode_utf8_view( rendered, &renderedSize, &renderedCount ) : detail::code_text( rendered );

                // Only a str() that is not empty follows a colon.
                if( renderedText[0] != '\0' )
                {
                    this->write_stderr_( ": " );
                    this->write_stderr_( renderedText );
                }

                tinypy_release( rendered );
            }
            else
            {
                if( error != nullptr )
                {
                    tinypy_error_release( error );
                }

                tinypy_vm_clear_error( m_vm );

                this->write_stderr_( ": <exception str() failed>" );
            }
        }

        this->write_stderr_( "\n" );
    }
    //////////////////////////////////////////////////////////////////////////
    bool tinypy_kernel::report_pending_error_()
    {
        // As the CPython backend, whose evaluation fails on an error pending
        // before it: the error is reported, and no code runs.
        if( tinypy_vm_has_error( m_vm ) == 0 )
        {
            return false;
        }

        this->report_error_( nullptr );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    tinypy_value_t * tinypy_kernel::call_default_excepthook_( tinypy_value_t * _function, tinypy_value_t * _self, tinypy_value_t * const * _items, size_t _count, tinypy_value_t * _kwargs, tinypy_error_t ** _outError )
    {
        (void)_self;
        (void)_count;
        (void)_kwargs;
        (void)_outError;

        tinypy_kernel * kernel = static_cast<tinypy_kernel *>(tinypy_native_function_user_data( _function ));

        kernel->display_error_( _items[0], _items[1], _items[2] );

        return tinypy_none_get( kernel->vm() );
    }
    //////////////////////////////////////////////////////////////////////////
    void tinypy_kernel::write_stderr_( const char * _text )
    {
        tinypy_value_t * stream = this->get_sys_value_( "stderr" );

        if( stream != nullptr )
        {
            // A write may replace sys.stderr.
            tinypy_retain( stream );

            tinypy_error_t * error = nullptr;
            tinypy_value_t * write = tinypy_object_get_attr( stream, "write", 5, &error );

            tinypy_value_t * result = nullptr;

            if( write != nullptr )
            {
                tinypy_value_t * text = tinypy_string_from_bytes( m_vm, _text, std::strlen( _text ) );
                tinypy_value_t * args = tinypy_tuple_from_items( m_vm, &text, 1 );
                result = tinypy_call( write, args, nullptr, &error );

                tinypy_release( args );
                tinypy_release( text );
                tinypy_release( write );
            }

            tinypy_release( stream );

            if( result != nullptr )
            {
                tinypy_release( result );

                return;
            }

            if( error != nullptr )
            {
                tinypy_error_release( error );
            }

            tinypy_vm_clear_error( m_vm );
        }

        std::fputs( _text, stderr );
        std::fflush( stderr );
    }
    //////////////////////////////////////////////////////////////////////////
    tinypy_value_t * tinypy_kernel::get_sys_value_( const char * _name ) const
    {
        // As PySys_GetObject: a borrowed value of sys, or null, as it is once
        // the kernel released sys while the VM finalizes.
        if( m_sys == nullptr )
        {
            return nullptr;
        }

        tinypy_value_t * value = tinypy_module_get_value( detail::value_cast( m_sys ), _name, std::strlen( _name ) );

        return value;
    }
    //////////////////////////////////////////////////////////////////////////
    void tinypy_kernel::set_sys_value_( const char * _name, PyObject * _value )
    {
        // As PySys_SetObject: null removes the value.
        if( m_sys == nullptr )
        {
            return;
        }

        tinypy_value_t * sys = detail::value_cast( m_sys );

        if( _value == nullptr )
        {
            tinypy_value_t * key = tinypy_string_from_bytes( m_vm, _name, std::strlen( _name ) );
            tinypy_dict_delete( tinypy_module_dict( sys ), key );
            tinypy_release( key );

            return;
        }

        tinypy_module_add_value( sys, _name, std::strlen( _name ), detail::value_cast( _value ) );
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
