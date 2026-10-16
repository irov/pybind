#include "pybind/kernel_interface.hpp"
#include "pybind/extract.hpp"
#include "pybind/function_interface.hpp"
#include "pybind/function.hpp"
#include "pybind/enum.hpp"
#include "pybind/adapter/new_adapter.hpp"
#include "pybind/adapter/destroy_adapter.hpp"
#include "pybind/adapter/smart_pointer_adapter.hpp"
#include "pybind/class_type_scope_interface.hpp"
#include "pybind/object.hpp"
#include "pybind/dict.hpp"
#include "pybind/list.hpp"
#include "pybind/tuple.hpp"
#include "pybind/module.hpp"
#include "pybind/generator/class_.hpp"
#include "pybind/generator/interface_.hpp"
#include "pybind/generator/struct_.hpp"
#include "pybind/generator/superclass_.hpp"
#include "pybind/internal_call.hpp"
#include "pybind/pickle.hpp"

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cwchar>
#include <fstream>
#include <iterator>
#include <string>
#include <vector>

#define PYBIND_CONTRACT_ASSERT( Expression ) \
    do \
    { \
        if( (Expression) == false ) \
        { \
            std::fprintf( stderr, "contract assertion failed: %s (%s:%d)\n", #Expression, __FILE__, __LINE__ ); \
            std::abort(); \
        } \
    } while( false )

namespace detail
{
    enum class signed_native_enum : long
    {
        Unknown = -1,
        Ready = 1
    };

    enum class unsigned_native_enum : unsigned long
    {
        High = 0x80000000UL
    };

    enum class signed_wide_enum : int64_t
    {
        Low = -1099511627776LL
    };

    enum class unsigned_wide_enum : uint64_t
    {
        High = 18446744073709551615ULL
    };

    template<class T>
    static T roundtrip_enum( T _value )
    {
        return _value;
    }

    static bool wide_pair_matches( const wchar_t * _first, const wchar_t * _second )
    {
        return std::wcscmp( _first, L"alpha" ) == 0 && std::wcscmp( _second, L"beta with a longer tail" ) == 0;
    }

    static int32_t wide_char_code( wchar_t _value )
    {
        return static_cast<int32_t>(_value);
    }

    class test_module_finder
    {
    public:
        void set_embed( const pybind::object & _embed )
        {
            m_embed = _embed;
        }

        PyObject * find_module( pybind::kernel_interface * _kernel, PyObject * _module, PyObject * _path )
        {
            (void)_path;
            size_t size;
            const char * name = _kernel->string_to_char_and_size( _module, &size );

            if( size != 6 || std::memcmp( name, "sample", 6 ) != 0 )
            {
                return _kernel->ret_none();
            }

            return m_embed.ret();
        }

        PyObject * load_module( pybind::kernel_interface * _kernel, PyObject * _module )
        {
            static const char source[] = "value = 42\ndef get_value():\n    return value\n";
            const char * name = _kernel->string_to_char( _module );
            PyObject * code = _kernel->compile_string( source, "finder/sample.py" );

            if( code == nullptr ) return nullptr;
            _kernel->module_init( name );
            PyObject * loaded = _kernel->module_execcode( name, code );
            _kernel->decref( code );
            ++m_loads;
            return loaded;
        }

        size_t loads() const
        {
            return m_loads;
        }

        void clear()
        {
            m_embed = nullptr;
        }

    private:
        pybind::object m_embed;
        size_t m_loads = 0;
    };

    class test_allocator
        : public pybind::allocator_interface
    {
    public:
        void * malloc( size_t _size ) override
        {
            ++m_allocations;
            return std::malloc( _size );
        }

        void * calloc( size_t _count, size_t _size ) override
        {
            ++m_allocations;
            return std::calloc( _count, _size );
        }

        void * realloc( void * _memory, size_t _size ) override
        {
            if( _memory == nullptr ) ++m_allocations;
            return std::realloc( _memory, _size );
        }

        void free( void * _memory ) override
        {
            if( _memory != nullptr )
            {
                --m_allocations;
                std::free( _memory );
            }
        }

        size_t allocations() const
        {
            return m_allocations;
        }

    private:
        size_t m_allocations = 0;
    };

    class add_adapter
        : public pybind::function_adapter_interface
    {
    public:
        add_adapter()
            : function_adapter_interface( "add", 2 )
        {
        }

        PyObject * call( pybind::kernel_interface * _kernel, PyObject * _args, PyObject * _kwargs ) override
        {
            if( _kwargs != nullptr )
            {
                size_t kwargsSize = _kernel->dict_size( _kwargs );
                PYBIND_CONTRACT_ASSERT( kwargsSize == 0 );
            }

            int32_t left;
            int32_t right;
            PyObject * leftObject = _kernel->tuple_getitem( _args, 0 );
            PyObject * rightObject = _kernel->tuple_getitem( _args, 1 );
            bool leftResult = _kernel->extract_int32( leftObject, left );
            bool rightResult = _kernel->extract_int32( rightObject, right );
            PYBIND_CONTRACT_ASSERT( leftResult == true );
            PYBIND_CONTRACT_ASSERT( rightResult == true );

            return _kernel->ptr_int32( left + right );
        }
    };

    struct exception_capture_t
    {
        pybind::kernel_interface * kernel = nullptr;
        bool called = false;
        bool traceback_next_borrowed = false;
        uint32_t line = 0;
    };

    struct debugger_capture_t
    {
        pybind::kernel_interface * kernel = nullptr;
        size_t calls = 0;
        size_t lines = 0;
        size_t returns = 0;
        bool evaluatedFrame = false;
        bool changedLocal = false;
    };

    static void capture_debugger( void * _userData, const pybind::debugger_event_t & _event )
    {
        debugger_capture_t * capture = static_cast<debugger_capture_t *>(_userData);

        if( _event.event == pybind::debugger_event_e::call )
        {
            ++capture->calls;
        }
        else if( _event.event == pybind::debugger_event_e::line )
        {
            ++capture->lines;

            uint32_t line = capture->kernel->debugger_frame_line( _event.frame );
            if( line == 2 && capture->evaluatedFrame == false )
            {
                PyObject * globals = capture->kernel->debugger_frame_globals( _event.frame );
                PyObject * locals = capture->kernel->debugger_frame_locals( _event.frame );
                PyObject * evaluated = capture->kernel->eval_string( "value", globals, locals );
                PYBIND_CONTRACT_ASSERT( evaluated != nullptr );
                capture->kernel->decref( evaluated );
                capture->evaluatedFrame = true;
            }
            else if( line == 3 )
            {
                PyObject * replacement = capture->kernel->ptr_int32( 41 );
                capture->changedLocal = capture->kernel->debugger_frame_set( _event.frame, pybind::debugger_scope_e::locals, "value", replacement );
                capture->kernel->decref( replacement );
            }
        }
        else if( _event.event == pybind::debugger_event_e::return_ )
        {
            ++capture->returns;
        }
    }

    static void capture_exception( void * _userData, PyTypeObject * _type, PyObject * _value, PyObject * _traceback )
    {
        (void)_type;
        (void)_value;

        exception_capture_t * capture = static_cast<exception_capture_t *>( _userData );

        PYBIND_CONTRACT_ASSERT( _type != nullptr );
        PYBIND_CONTRACT_ASSERT( _value != nullptr );
        PYBIND_CONTRACT_ASSERT( _traceback != nullptr );
        PYBIND_CONTRACT_ASSERT( capture->kernel->traceback_check( _traceback ) == true );

        capture->called = true;
        capture->line = capture->kernel->traceback_lineno( _traceback );

        PyObject * next = capture->kernel->traceback_next( _traceback );
        PYBIND_CONTRACT_ASSERT( next != nullptr );
        uint32_t nextRefcount = capture->kernel->refcount( next );
        PyObject * repeatedNext = capture->kernel->traceback_next( _traceback );
        PYBIND_CONTRACT_ASSERT( repeatedNext == next );
        capture->traceback_next_borrowed = capture->kernel->refcount( repeatedNext ) == nextRefcount;
    }

    static void suppress_exception( void * _userData, PyTypeObject * _type, PyObject * _value, PyObject * _traceback )
    {
        (void)_type;
        (void)_value;
        (void)_traceback;

        bool * called = static_cast<bool *>( _userData );
        *called = true;
    }

    struct native_value_t
    {
        int32_t value;
    };

    class native_new_adapter
        : public pybind::new_adapter_interface
    {
    public:
        void * call( pybind::kernel_interface * _kernel, const pybind::class_type_scope_interface_ptr & _scope, PyObject * _object, PyObject * _args, PyObject * _kwargs ) override
        {
            (void)_scope;
            (void)_object;
            (void)_kwargs;
            pybind::allocator_interface * allocator = _kernel->get_allocator();
            native_value_t * value = allocator->newP<native_value_t>();
            size_t argumentCount = _kernel->tuple_size( _args );

            if( argumentCount == 0 )
            {
                value->value = 0;
            }
            else
            {
                PyObject * argument = _kernel->tuple_getitem( _args, 0 );
                value->value = pybind::extract<int32_t>( _kernel, argument );
            }

            return value;
        }
    };

    class native_destroy_adapter
        : public pybind::destroy_adapter_interface
    {
    public:
        void call( pybind::kernel_interface * _kernel, const pybind::class_type_scope_interface_ptr & _scope, void * _impl ) override
        {
            (void)_scope;
            pybind::allocator_interface * allocator = _kernel->get_allocator();
            native_value_t * impl = static_cast<native_value_t *>( _impl );
            allocator->deleteT( impl );
        }
    };

    class counting_destroy_adapter
        : public pybind::destroy_adapter_interface
    {
    public:
        explicit counting_destroy_adapter( size_t * _calls )
            : m_calls( _calls )
        {
        }

        void call( pybind::kernel_interface * _kernel, const pybind::class_type_scope_interface_ptr & _scope, void * _impl ) override
        {
            (void)_kernel;
            (void)_scope;
            (void)_impl;
            ++*m_calls;
        }

    private:
        size_t * m_calls;
    };

    class smart_pointer_base_t
        : public stdex::intrusive_ptr_base
    {
    public:
        smart_pointer_base_t()
            : m_refcount( 0 )
        {
            ++s_alive;
        }

        ~smart_pointer_base_t() override
        {
            --s_alive;
        }

    public:
        uint32_t incref() override
        {
            return ++m_refcount;
        }

        void decref() override
        {
            PYBIND_CONTRACT_ASSERT( m_refcount != 0 );

            if( --m_refcount == 0 )
            {
                delete this;
            }
        }

        uint32_t getrefcount() const override
        {
            return m_refcount;
        }

        static void intrusive_ptr_add_ref( smart_pointer_base_t * _self )
        {
            _self->incref();
        }

        static void intrusive_ptr_dec_ref( smart_pointer_base_t * _self )
        {
            _self->decref();
        }

        static size_t alive()
        {
            return s_alive;
        }

        void clearEmbed()
        {
        }

    private:
        uint32_t m_refcount;
        static size_t s_alive;
    };

    size_t smart_pointer_base_t::s_alive = 0;

    class smart_pointer_derived_t
        : public smart_pointer_base_t
    {
    public:
        void set_value( int32_t _value )
        {
            m_value = _value;
        }

        int32_t get_value() const
        {
            return m_value;
        }

    private:
        int32_t m_value = 0;
    };

    class smart_pointer_holder_t
        : public smart_pointer_base_t
    {
    };

    struct cast_primary_t
    {
        int32_t marker = 17;
    };

    struct cast_secondary_t
    {
        void set_secondary( int32_t _value )
        {
            value = _value;
        }

        int32_t value = 0;
    };

    struct cast_middle_t
        : public cast_primary_t
        , public cast_secondary_t
    {
    };

    struct cast_leaf_t
        : public cast_middle_t
    {
    };

    struct pod_value_t
    {
        pod_value_t()
            : value( 37 )
        {
        }

        explicit pod_value_t( int32_t _value )
            : value( _value )
        {
        }

        bool operator == ( const pod_value_t & _other ) const
        {
            return value == _other.value;
        }

        bool operator != ( const pod_value_t & _other ) const
        {
            return value != _other.value;
        }

        pod_value_t operator + ( const pod_value_t & _other ) const
        {
            return pod_value_t( value + _other.value );
        }

        pod_value_t operator - ( const pod_value_t & _other ) const
        {
            return pod_value_t( value - _other.value );
        }

        pod_value_t operator * ( const pod_value_t & _other ) const
        {
            return pod_value_t( value * _other.value );
        }

        pod_value_t operator / ( const pod_value_t & _other ) const
        {
            return pod_value_t( value / _other.value );
        }

        pod_value_t & operator += ( const pod_value_t & _other )
        {
            value += _other.value;
            return *this;
        }

        pod_value_t & operator -= ( const pod_value_t & _other )
        {
            value -= _other.value;
            return *this;
        }

        pod_value_t & operator *= ( const pod_value_t & _other )
        {
            value *= _other.value;
            return *this;
        }

        pod_value_t & operator /= ( const pod_value_t & _other )
        {
            value /= _other.value;
            return *this;
        }

        pod_value_t operator - () const
        {
            return pod_value_t( -value );
        }

        int32_t absolute() const
        {
            return value < 0 ? -value : value;
        }

        int32_t call( int32_t _value ) const
        {
            return value + _value;
        }

        int32_t & operator [] ( size_t _index )
        {
            (void)_index;

            PYBIND_CONTRACT_ASSERT( _index == 0 );
            return value;
        }

        int32_t value;
    };

    class pod_proxy_t
    {
    public:
        int32_t add( pybind::kernel_interface * _kernel, pod_value_t * _value, int32_t _add, const pybind::args & _args ) const
        {
            (void)_kernel;
            (void)_args;

            return _value->value + _add;
        }
    };

    struct scalar_value_t
    {
        scalar_value_t()
            : value( 12.f )
        {
        }

        explicit scalar_value_t( float _value )
            : value( _value )
        {
        }

        scalar_value_t operator * ( float _value ) const
        {
            return scalar_value_t( value * _value );
        }

        scalar_value_t operator / ( float _value ) const
        {
            return scalar_value_t( value / _value );
        }

        scalar_value_t & operator *= ( float _value )
        {
            value *= _value;
            return *this;
        }

        scalar_value_t & operator /= ( float _value )
        {
            value /= _value;
            return *this;
        }

        float value;
    };

    static bool convert_pod_value( pybind::kernel_interface * _kernel, PyObject * _object, pod_value_t * const _value )
    {
        return _kernel->extract_int32( _object, _value->value );
    }

    static const char * repr_pod_value( const pod_value_t * _t )
    {
        (void)_t;
        return "pod-37";
    }

    static int64_t hash_pod_value( pod_value_t * _value )
    {
        return _value->value;
    }

    static int64_t hash_scalar_value( scalar_value_t * _value )
    {
        (void)_value;
        pybind::throw_exception( "scalar hash failure" );
        return 0;
    }

    static void multiply_out( int32_t * const _out, int32_t _value )
    {
        *_out = _value * 2;
    }
    //////////////////////////////////////////////////////////////////////////
    static int32_t s_regression_node_alive = 0;
    static int32_t s_regression_counted_alive = 0;

    class regression_node_t
    {
    public:
        regression_node_t()
        {
            ++s_regression_node_alive;
        }

        ~regression_node_t()
        {
            --s_regression_node_alive;
        }

        int32_t getValue() const
        {
            return 7;
        }
    };

    class regression_other_t
    {
    public:
        int32_t getOther() const
        {
            return 3;
        }
    };

    struct regression_counted_t
    {
        regression_counted_t()
        {
            ++s_regression_counted_alive;
        }

        regression_counted_t( const regression_counted_t & )
        {
            ++s_regression_counted_alive;
        }

        ~regression_counted_t()
        {
            --s_regression_counted_alive;
        }

        float value = 1.f;
    };

    static const char * repr_regression_node( const regression_node_t * _node )
    {
        (void)_node;
        return "regression-node";
    }

    static int32_t take_regression_node( regression_node_t * _node )
    {
        return _node != nullptr ? _node->getValue() : -1;
    }

    static size_t regression_size( size_t _value )
    {
        return _value + 1;
    }

    static const char * regression_null_string()
    {
        return nullptr;
    }

    static pybind::object regression_unpickle( pybind::kernel_interface * _kernel, PyObject * _data )
    {
        size_t size;
        const char * data = _kernel->string_to_char_and_size( _data, &size );
        PyObject * types = _kernel->list_new( 0 );
        PyObject * value = pybind::unpickle( _kernel, data, size, types );
        _kernel->decref( types );

        if( value == nullptr )
        {
            pybind::throw_exception( "regression unpickle rejected the data" );
        }

        return pybind::object( _kernel, value, pybind::borrowed );
    }

    static pybind::object regression_roundtrip( pybind::kernel_interface * _kernel, PyObject * _value )
    {
        PyObject * types = _kernel->list_new( 0 );
        size_t size = 0;
        bool measured = pybind::pickle( _kernel, _value, types, nullptr, 0, &size );

        if( measured == false )
        {
            _kernel->decref( types );

            pybind::throw_exception( "regression pickle rejected the value" );
        }

        void * buffer = std::malloc( size );
        size_t written = 0;
        bool result = pybind::pickle( _kernel, _value, types, buffer, size, &written );
        PYBIND_CONTRACT_ASSERT( result == true );
        PYBIND_CONTRACT_ASSERT( written == size );
        PyObject * value = pybind::unpickle( _kernel, buffer, size, types );
        std::free( buffer );
        _kernel->decref( types );
        PYBIND_CONTRACT_ASSERT( value != nullptr );

        return pybind::object( _kernel, value, pybind::borrowed );
    }

    static void check_review_regressions( pybind::kernel_interface * _kernel, PyObject * _module )
    {
        pybind::class_<regression_node_t>( _kernel, "RegressionNode", true, _module )
            .def_constructor( pybind::init<>() )
            .def( "getValue", &regression_node_t::getValue )
            .def_property( "readonly", &regression_node_t::getValue, nullptr )
            .def_repr( &repr_regression_node )
            ;

        pybind::class_<regression_other_t>( _kernel, "RegressionOther", true, _module )
            .def_constructor( pybind::init<>() )
            .def( "getOther", &regression_other_t::getOther )
            ;

        pybind::struct_<regression_counted_t>( _kernel, "RegressionCounted", true, _module )
            .def_constructor( pybind::init<>() )
            ;

        pybind::def_function( _kernel, "take_regression_node", &take_regression_node );
        pybind::def_function( _kernel, "regression_size", &regression_size );
        pybind::def_function( _kernel, "regression_null_string", &regression_null_string );
        pybind::def_function_kernel( _kernel, "regression_unpickle", &regression_unpickle );
        pybind::def_function_kernel( _kernel, "regression_roundtrip", &regression_roundtrip );

        PyObject * dict = _kernel->module_dict( _module );
        PyObject * builtins = _kernel->get_builtins();
        _kernel->dict_setstring( dict, "__builtins__", builtins );

        const char source[] = R"PY(
class SubNode(RegressionNode):
    pass

sub = SubNode()
regression_subclass_ok = take_regression_node(sub) == 7 and sub.getValue() == 7 and repr(sub) == 'regression-node'
del sub

try:
    RegressionNode().readonly = 5
    regression_readonly_ok = False
except AttributeError:
    regression_readonly_ok = True

try:
    RegressionNode.getValue(RegressionOther())
    regression_receiver_ok = False
except TypeError:
    regression_receiver_ok = True

regression_size_ok = regression_size(41) == 42
regression_null_string_ok = regression_null_string() is None

counted = RegressionCounted()
del counted

regression_values = [2 ** 40, 1760000000.5, u'caf\xe9', (1, {2: 3}), [True, None]]
regression_pickle_ok = regression_roundtrip(regression_values) == regression_values

regression_unpickle_rejects = 0
for regression_data in ('\x08\x01' * 20000, '\x07\xff\xfe\xff\xff\xff', '\x06\x01\xf4', '\x09\x01\x08\x00\x00'):
    try:
        regression_unpickle(regression_data)
    except Exception:
        regression_unpickle_rejects += 1

regression_cycle = [1]
regression_cycle.append(regression_cycle)
try:
    regression_roundtrip(regression_cycle)
    regression_cycle_ok = False
except Exception:
    regression_cycle_ok = True
regression_cycle.pop()
)PY";

        PyObject * result = _kernel->exec_file( source, dict, dict );
        PYBIND_CONTRACT_ASSERT( result != nullptr );
        _kernel->decref( result );

        const char * flags[] = {"regression_subclass_ok", "regression_readonly_ok", "regression_receiver_ok", "regression_size_ok", "regression_null_string_ok", "regression_pickle_ok", "regression_cycle_ok"};

        for( const char * flag : flags )
        {
            PyObject * value = _kernel->dict_getstring( dict, flag );
            bool flagValue = false;
            bool flagResult = _kernel->extract_bool( value, flagValue );

            if( flagResult == false || flagValue == false )
            {
                std::fprintf( stderr, "regression flag '%s' failed\n", flag );
            }

            PYBIND_CONTRACT_ASSERT( flagResult == true && flagValue == true );
        }

        PYBIND_CONTRACT_ASSERT( pybind::extract<int32_t>( _kernel, _kernel->dict_getstring( dict, "regression_unpickle_rejects" ) ) == 4 );
        PYBIND_CONTRACT_ASSERT( s_regression_counted_alive == 0 );
        PYBIND_CONTRACT_ASSERT( s_regression_node_alive == 0 );

#if defined(PYBIND_BACKEND_PYTHON)
        // CPython collects a bound object in a reference cycle; tinypy has no cyclic collector.
        const char cycleSource[] = "import gc\nregression_cycle_node = RegressionNode()\nregression_cycle_node.me = regression_cycle_node\ndel regression_cycle_node\ngc.collect()\n";
        PyObject * cycleResult = _kernel->exec_file( cycleSource, dict, dict );
        PYBIND_CONTRACT_ASSERT( cycleResult != nullptr );
        _kernel->decref( cycleResult );
        PYBIND_CONTRACT_ASSERT( s_regression_node_alive == 0 );
#endif

        regression_node_t * held = new regression_node_t;
        PyObject * holder = _kernel->scope_create_holder_t( held );
        PYBIND_CONTRACT_ASSERT( _kernel->is_class( holder ) == true );
        PYBIND_CONTRACT_ASSERT( _kernel->is_wrap( holder ) == true );
        _kernel->unwrap( holder );
        PYBIND_CONTRACT_ASSERT( _kernel->is_wrap( holder ) == false );
        _kernel->decref( holder );
        delete held;

        PyObject * exception = _kernel->exception_new( "regression" );
        PYBIND_CONTRACT_ASSERT( exception != nullptr );
        PYBIND_CONTRACT_ASSERT( _kernel->is_class( exception ) == false );
        _kernel->decref( exception );

        _kernel->remove_scope<regression_counted_t>();
        _kernel->remove_scope<regression_other_t>();
        _kernel->remove_scope<regression_node_t>();
    }
    //////////////////////////////////////////////////////////////////////////
    static int32_t s_regression_mix_alive = 0;
    static int32_t s_regression_ent_destroys = 0;

    class regression_mix_t
    {
    public:
        regression_mix_t()
            : m_refs( 0 )
        {
            ++s_regression_mix_alive;
        }

        virtual ~regression_mix_t()
        {
            --s_regression_mix_alive;
        }

        static void intrusive_ptr_add_ref( regression_mix_t * _self )
        {
            ++_self->m_refs;
        }

        static void intrusive_ptr_dec_ref( regression_mix_t * _self )
        {
            if( --_self->m_refs == 0 )
            {
                delete _self;
            }
        }

    protected:
        int32_t m_refs;
    };

    class regression_ent_t
        : public regression_mix_t
    {
    public:
        int32_t getValue() const
        {
            return 11;
        }
    };

    class regression_ent_new
        : public pybind::new_adapter_interface
    {
    public:
        void * call( pybind::kernel_interface * _kernel, const pybind::class_type_scope_interface_ptr & _scope, PyObject * _obj, PyObject * _args, PyObject * _kwds ) override
        {
            (void)_kernel;
            (void)_scope;
            (void)_obj;
            (void)_args;
            (void)_kwds;

            // As superclass_new_Entity of Mengine: the adapter keeps a reference
            // that its destroy adapter releases.
            regression_ent_t * ent = new regression_ent_t;
            regression_mix_t::intrusive_ptr_add_ref( ent );

            return ent;
        }
    };

    class regression_ent_destroy
        : public pybind::destroy_adapter_interface
    {
    public:
        void call( pybind::kernel_interface * _kernel, const pybind::class_type_scope_interface_ptr & _scope, void * _impl ) override
        {
            (void)_kernel;
            (void)_scope;

            ++s_regression_ent_destroys;

            regression_mix_t::intrusive_ptr_dec_ref( static_cast<regression_ent_t *>(_impl) );
        }
    };

    struct regression_pod_t
    {
        regression_pod_t()
            : value( 0.f )
        {
        }

        explicit regression_pod_t( float _value )
            : value( _value )
        {
        }

        float value;
    };

    static bool regression_is_type( pybind::kernel_interface * _kernel, PyTypeObject * _type )
    {
        return _kernel->is_type_class( _type );
    }

    static uint32_t regression_list_size( const pybind::list & _list )
    {
        return (uint32_t)_list.size();
    }

    static pybind::object regression_invalid_result( pybind::kernel_interface * _kernel )
    {
        // As a binding that fails: the error it sets is what Python raises.
        _kernel->set_error( pybind::error_type_e::Value, "regression invalid result" );

        return pybind::make_invalid_object_t();
    }

    static int32_t regression_warning( pybind::kernel_interface * _kernel )
    {
        _kernel->warning_traceback( "regression warning" );

        return 42;
    }

    static int64_t regression_int64()
    {
        return 5;
    }

    static bool regression_module_valid( const pybind::module & _module )
    {
        return _module.is_invalid() == false;
    }

    static bool regression_pickle_types( pybind::kernel_interface * _kernel, PyObject * _value, PyObject * _types )
    {
        size_t size = 0;
        bool result = pybind::pickle( _kernel, _value, _types, nullptr, 0, &size );

        return result;
    }

    static void regression_throwing_hook( void * _ud, PyTypeObject * _type, PyObject * _value, PyObject * _traceback )
    {
        (void)_type;
        (void)_value;
        (void)_traceback;

        int32_t * calls = static_cast<int32_t *>(_ud);
        ++(*calls);

        pybind::throw_exception( "regression hook failure" );
    }

    static bool regression_flag( pybind::kernel_interface * _kernel, PyObject * _dict, const char * _flag )
    {
        PyObject * value = _kernel->dict_getstring( _dict, _flag );
        bool flagValue = false;
        bool flagResult = value != nullptr && _kernel->extract_bool( value, flagValue ) == true;

        if( flagResult == false || flagValue == false )
        {
            std::fprintf( stderr, "regression flag '%s' failed\n", _flag );

            return false;
        }

        return true;
    }

    static void check_second_review_regressions( pybind::kernel_interface * _kernel, PyObject * _module )
    {
        pybind::allocator_interface * allocator = _kernel->get_allocator();

        pybind::interface_<regression_mix_t>( _kernel, "RegressionMix", true, _module )
            .def_smart_pointer()
            ;

        pybind::superclass_<regression_ent_t, pybind::bases<regression_mix_t>>( _kernel, "RegressionEnt", nullptr, allocator->newT<regression_ent_new>(), allocator->newT<regression_ent_destroy>(), true, _module )
            .def( "getValue", &regression_ent_t::getValue )
            ;

        pybind::struct_<regression_pod_t>( _kernel, "RegressionPod", true, _module )
            .def_constructor( pybind::init<float>() )
            .def_member( "value", &regression_pod_t::value )
            ;

        pybind::class_<regression_node_t>( _kernel, "RegressionNode", true, _module )
            .def_constructor( pybind::init<>() )
            .def( "getValue", &regression_node_t::getValue )
            ;

        pybind::class_<regression_other_t>( _kernel, "RegressionOther", true, _module )
            .def_constructor( pybind::init<>() )
            .def( "getOther", &regression_other_t::getOther )
            ;

        pybind::def_function_kernel( _kernel, "regression_is_type", &regression_is_type );
        pybind::def_function( _kernel, "regression_list_size", &regression_list_size );
        pybind::def_function_kernel( _kernel, "regression_invalid_result", &regression_invalid_result );
        pybind::def_function_kernel( _kernel, "regression_warning", &regression_warning );
        pybind::def_function( _kernel, "regression_int64", &regression_int64 );
        pybind::def_function( _kernel, "regression_module_valid", &regression_module_valid );
        pybind::def_function_kernel( _kernel, "regression_pickle_types", &regression_pickle_types );

        PyObject * dict = _kernel->module_dict( _module );

        const char source[] = R"PY(
import sys

def raises(error, function, *args):
    try:
        function(*args)
    except error:
        return True
    except Exception:
        return False
    return False

class Old:
    pass

class OldMro(Old, RegressionNode):
    pass

regression2_classic_mro_ok = OldMro().getValue() == 7

class Meta(type):
    pass

class WithMeta(RegressionNode):
    __metaclass__ = Meta

regression2_metaclass_ok = WithMeta().getValue() == 7 and type(WithMeta) is Meta

class WithNew(RegressionNode):
    def __new__(cls):
        return super(WithNew, cls).__new__(cls)

regression2_new_ok = WithNew().getValue() == 7

class Mixed(RegressionNode, RegressionOther):
    pass

regression2_mixed_ok = raises(TypeError, Mixed().getOther)

node = RegressionNode()

def reassign_class():
    node.__class__ = RegressionOther

regression2_class_ok = raises(TypeError, reassign_class) and type(node) is RegressionNode
regression2_receiver_ok = raises(TypeError, RegressionNode.getValue)
del node

ent = RegressionEnt()
regression2_ent_value = ent.getValue()
del ent

regression2_pod_ok = RegressionPod(2.5).value == 2.5

regression2_is_type_ok = regression_is_type(RegressionNode) == True and raises(Exception, regression_is_type, 5)
regression2_list_ok = regression_list_size([1, 2]) == 2 and raises(Exception, regression_list_size, None)
regression2_invalid_ok = raises(ValueError, regression_invalid_result)
regression2_warning_ok = regression_warning() == 42
regression2_int_ok = type(regression_int64()) is int and type(regression_size(4)) is int
regression2_module_ok = regression_module_valid(sys) == True

try:
    assert False
    regression2_assert_ok = False
except AssertionError:
    regression2_assert_ok = True

regression2_pickle_ok = regression_pickle_types(1, [Old, RegressionNode]) == True

pod = PodValue()

class IndexZero(object):
    def __index__(self):
        return 0

def delete_item():
    del pod[0]

regression2_index_ok = raises(TypeError, lambda: pod[1.5]) and pod[IndexZero()] == pod[0] and raises(TypeError, delete_item)
del pod

class Capture(object):
    def __init__(self):
        self.parts = []

    def write(self, text):
        self.parts.append(text)

regression_capture = Capture()
regression_hook_calls = []

def regression_hook(exception_type, value, traceback):
    regression_hook_calls.append(str(value))

class BadHash(object):
    def __hash__(self):
        raise ValueError('bad hash')

class BadEq(object):
    def __eq__(self, other):
        raise ValueError('bad eq')

regression_bad_hash = BadHash()
regression_bad_eq = BadEq()

def regression_noargs():
    return 3
)PY";

        PyObject * result = _kernel->exec_file( source, dict, dict );
        PYBIND_CONTRACT_ASSERT( result != nullptr );
        _kernel->decref( result );

        const char * flags[] = {"regression2_classic_mro_ok", "regression2_metaclass_ok", "regression2_new_ok", "regression2_mixed_ok", "regression2_class_ok", "regression2_receiver_ok", "regression2_pod_ok", "regression2_is_type_ok", "regression2_list_ok", "regression2_invalid_ok", "regression2_warning_ok", "regression2_int_ok", "regression2_module_ok", "regression2_assert_ok", "regression2_pickle_ok", "regression2_index_ok"};

        for( const char * flag : flags )
        {
            PYBIND_CONTRACT_ASSERT( detail::regression_flag( _kernel, dict, flag ) == true );
        }

        // A superclass_ object a smart pointer counts is released by its destroy
        // adapter as well as by the smart pointer.
        PYBIND_CONTRACT_ASSERT( pybind::extract<int32_t>( _kernel, _kernel->dict_getstring( dict, "regression2_ent_value" ) ) == 11 );
        PYBIND_CONTRACT_ASSERT( s_regression_ent_destroys == 1 );
        PYBIND_CONTRACT_ASSERT( s_regression_mix_alive == 0 );

#if defined(PYBIND_BACKEND_PYTHON)
        // CPython only: tinypy has no cyclic collector, and its native slots are
        // not special methods a Python subclass composes with.
        const char pythonSource[] = R"PY(
import gc

class RightAdd(PodValue):
    def __radd__(self, other):
        return 'radd'

regression2_partial_ok = type(RightAdd() + PodValue()) is PodValue and PodValue.__add__ is not None

cycle_node = RegressionNode()
cycle_node.callback = cycle_node.getValue
del cycle_node
gc.collect()
)PY";

        PyObject * pythonResult = _kernel->exec_file( pythonSource, dict, dict );
        PYBIND_CONTRACT_ASSERT( pythonResult != nullptr );
        _kernel->decref( pythonResult );
        PYBIND_CONTRACT_ASSERT( detail::regression_flag( _kernel, dict, "regression2_partial_ok" ) == true );
        PYBIND_CONTRACT_ASSERT( s_regression_node_alive == 0 );
#endif

        // The error display: a class with its module, a classic class, and the
        // last line of a traceback longer than a message buffer.
        PyObject * capture = _kernel->dict_getstring( dict, "regression_capture" );
        PyObject * oldStderr = _kernel->getStdErrorHandle();
        _kernel->incref( oldStderr );
        _kernel->setStdErrorHandle( capture );

        const char * raisingSources[] = {
            "class RegressionError(Exception):\n    pass\nraise RegressionError('custom')\n",
            "class Classic:\n    pass\nraise Classic()\n",
            "def deep_regression_recursion_with_a_long_function_name(count):\n    if count == 0:\n        raise ValueError('deep end')\n    return deep_regression_recursion_with_a_long_function_name(count - 1)\ndeep_regression_recursion_with_a_long_function_name(60)\n"
        };

        for( const char * raisingSource : raisingSources )
        {
            PyObject * raisingResult = _kernel->exec_file( raisingSource, dict, dict );
            PYBIND_CONTRACT_ASSERT( raisingResult == nullptr );
        }

        // A sys.excepthook set from Python reports the error.
        PyObject * oldHook = _kernel->get_sys_excepthook();
        _kernel->incref( oldHook );

        PyObject * hookResult = _kernel->exec_file( "sys.excepthook = regression_hook\n", dict, dict );
        PYBIND_CONTRACT_ASSERT( hookResult != nullptr );
        _kernel->decref( hookResult );

        PyObject * hookedResult = _kernel->exec_file( "raise ValueError('hooked')\n", dict, dict );
        PYBIND_CONTRACT_ASSERT( hookedResult == nullptr );

        // A C++ hook that throws fails its call, which is reported, and does
        // not unwind through the kernel.
        int32_t throwingHookCalls = 0;
        _kernel->set_sys_excepthook_f( &detail::regression_throwing_hook, &throwingHookCalls );

        bool hookEscaped = false;

        try
        {
            PyObject * throwingResult = _kernel->exec_file( "raise ValueError('throwing hook')\n", dict, dict );
            PYBIND_CONTRACT_ASSERT( throwingResult == nullptr );
        }
        catch( const pybind::pybind_exception & )
        {
            hookEscaped = true;
        }

        PYBIND_CONTRACT_ASSERT( hookEscaped == false );
        PYBIND_CONTRACT_ASSERT( throwingHookCalls == 1 );

        _kernel->set_sys_excepthook( oldHook );
        _kernel->decref( oldHook );
        _kernel->setStdErrorHandle( oldStderr );
        _kernel->decref( oldStderr );

        const char displaySource[] = R"PY(
regression_text = ''.join(regression_capture.parts)
regression2_display_module_ok = (__name__ + '.RegressionError: custom') in regression_text
regression2_display_classic_ok = (__name__ + '.Classic') in regression_text
regression2_display_deep_ok = 'ValueError: deep end\n' in regression_text
regression2_hook_ok = regression_hook_calls == ['hooked']
)PY";

        PyObject * displayResult = _kernel->exec_file( displaySource, dict, dict );
        PYBIND_CONTRACT_ASSERT( displayResult != nullptr );
        _kernel->decref( displayResult );

        const char * displayFlags[] = {"regression2_display_module_ok", "regression2_display_classic_ok", "regression2_display_deep_ok", "regression2_hook_ok"};

        for( const char * flag : displayFlags )
        {
            PYBIND_CONTRACT_ASSERT( detail::regression_flag( _kernel, dict, flag ) == true );
        }

        // No error stays pending after a key that cannot be hashed, an equality
        // that raises, or a call with arguments that are not a tuple.
        PyObject * badHash = _kernel->dict_getstring( dict, "regression_bad_hash" );
        PyObject * badHashDict = _kernel->dict_new();
        PYBIND_CONTRACT_ASSERT( _kernel->dict_exist( badHashDict, badHash ) == false );
        PYBIND_CONTRACT_ASSERT( _kernel->dict_get( badHashDict, badHash ) == nullptr );
        _kernel->decref( badHashDict );

        PyObject * badEq = _kernel->dict_getstring( dict, "regression_bad_eq" );
        PYBIND_CONTRACT_ASSERT( _kernel->test_equal( badEq, badHash ) == false );

        PyObject * noargs = _kernel->dict_getstring( dict, "regression_noargs" );
        PyObject * noargsResult = _kernel->ask_native( noargs, nullptr );
        PYBIND_CONTRACT_ASSERT( noargsResult != nullptr );
        PYBIND_CONTRACT_ASSERT( pybind::extract<int32_t>( _kernel, noargsResult ) == 3 );
        _kernel->decref( noargsResult );

        PyObject * notTuple = pybind::ptr( _kernel, 5 );
        PYBIND_CONTRACT_ASSERT( _kernel->ask_native( noargs, notTuple ) == nullptr );

        // Globals without builtins see those of the interpreter.
        PyObject * bareGlobals = _kernel->dict_new();
        PyObject * lengthResult = _kernel->eval_string( "len([1, 2, 3])", bareGlobals, bareGlobals );
        PYBIND_CONTRACT_ASSERT( lengthResult != nullptr );
        PYBIND_CONTRACT_ASSERT( pybind::extract<int32_t>( _kernel, lengthResult ) == 3 );
        _kernel->decref( lengthResult );
        _kernel->decref( bareGlobals );

        // An object that is not bound has no pybind flags or impl.
        PYBIND_CONTRACT_ASSERT( _kernel->is_wrap( notTuple ) == false );
        PYBIND_CONTRACT_ASSERT( _kernel->get_class_impl( notTuple ) == nullptr );
        PYBIND_CONTRACT_ASSERT( _kernel->unwrap( notTuple ) == nullptr );
        PYBIND_CONTRACT_ASSERT( _kernel->type_check( notTuple ) == false );

        size_t sizeValue = 0;
        PYBIND_CONTRACT_ASSERT( pybind::extract_value( _kernel, notTuple, sizeValue, true ) == true );
        PYBIND_CONTRACT_ASSERT( sizeValue == 5 );
        _kernel->decref( notTuple );

        // A slot a caller does not set is None.
        PyObject * slots = _kernel->list_new( 2 );
        PYBIND_CONTRACT_ASSERT( _kernel->is_none( _kernel->list_getitem( slots, 1 ) ) == true );
        _kernel->decref( slots );

        // An entry of sys.modules that is not a module is replaced.
        PyObject * notModuleResult = _kernel->exec_file( "sys.modules['regression_not_module'] = 5\n", dict, dict );
        PYBIND_CONTRACT_ASSERT( notModuleResult != nullptr );
        _kernel->decref( notModuleResult );
        PyObject * notModuleCode = _kernel->compile_string( "regression_marker = 1\n", "regression_not_module.py" );
        PYBIND_CONTRACT_ASSERT( notModuleCode != nullptr );
        PyObject * notModule = _kernel->module_execcode( "regression_not_module", notModuleCode );
        PYBIND_CONTRACT_ASSERT( notModule != nullptr );
        _kernel->decref( notModule );
        _kernel->decref( notModuleCode );

        // Items, elements and modules convert to objects.
        pybind::list source_list( _kernel );
        source_list.append( 10 );
        source_list.append( 20 );
        pybind::list target_list( _kernel );
        target_list.append( 0 );
        target_list[0] = source_list[1];
        int32_t movedItem = target_list[0];
        PYBIND_CONTRACT_ASSERT( movedItem == 20 );

        pybind::tuple elements = pybind::make_tuple_t( _kernel, 30, 40 );
        target_list.append( elements[0] );
        int32_t appendedElement = target_list[1];
        PYBIND_CONTRACT_ASSERT( appendedElement == 30 );

        pybind::dict items( _kernel );
        items["a"] = source_list[0];
        int32_t dictItem = items["a"];
        PYBIND_CONTRACT_ASSERT( dictItem == 10 );

        pybind::module moduleWrapper( _kernel, _module );
        PyObject * moduleValid = _kernel->dict_getstring( dict, "regression_module_valid" );
        bool moduleCalled = pybind::object( _kernel, moduleValid ).call( moduleWrapper );
        PYBIND_CONTRACT_ASSERT( moduleCalled == true );

        std::vector<int32_t> container = {1, 2, 3};
        pybind::tuple containerTuple = pybind::make_tuple_container_t( _kernel, container );
        PYBIND_CONTRACT_ASSERT( containerTuple.size() == 3 );

        regression_node_t internalNode;
        int32_t internalValue = pybind::internal_call_method( _kernel, &internalNode, "getValue" );
        PYBIND_CONTRACT_ASSERT( internalValue == 7 );

        // A method of a missing attribute is not called.
        PyObject * capturedObject = _kernel->dict_getstring( dict, "regression_capture" );
        pybind::object missingCall = pybind::object( _kernel, capturedObject ).call_method( "regression_missing_method", 1 );
        PYBIND_CONTRACT_ASSERT( missingCall.is_invalid() == true );

        _kernel->remove_scope<regression_other_t>();
        _kernel->remove_scope<regression_node_t>();
        _kernel->remove_scope<regression_pod_t>();
        _kernel->remove_scope<regression_ent_t>();
        _kernel->remove_scope<regression_mix_t>();
    }
}
int main()
{
    detail::test_allocator allocator;
    pybind::kernel_config_t config;
    PYBIND_CONTRACT_ASSERT( config.cycle_diagnostics == false );
    config.path = L"";
    config.debug = false;
    config.install_signals = false;
    config.no_site = true;
#if defined(PYBIND_BACKEND_TINYPY) && defined(TINYPY_CYCLE_DIAGNOSTICS)
    config.cycle_diagnostics = true;
#endif
    pybind::kernel_interface * kernel = pybind::initialize( &allocator, config );
    PYBIND_CONTRACT_ASSERT( kernel != nullptr );
#if defined(PYBIND_BACKEND_TINYPY) && defined(TINYPY_CYCLE_DIAGNOSTICS)
    PYBIND_CONTRACT_ASSERT( kernel->is_cycle_diagnostics_enabled() == true );
#else
    PYBIND_CONTRACT_ASSERT( kernel->is_cycle_diagnostics_enabled() == false );
#endif

    pybind::object invalidObject = pybind::detail::extract_operator_t( kernel, nullptr, pybind::borrowed );
    PYBIND_CONTRACT_ASSERT( invalidObject.is_invalid() == true );
    PYBIND_CONTRACT_ASSERT( kernel->object_str( nullptr ).is_invalid() == true );
    PYBIND_CONTRACT_ASSERT( kernel->object_repr( nullptr ).is_invalid() == true );
    PYBIND_CONTRACT_ASSERT( kernel->object_repr_type( nullptr ).is_invalid() == true );
    PyObject * module = kernel->module_init( "contract" );
    kernel->incref( module );
    kernel->set_current_module( module );

    pybind::enum_<detail::signed_native_enum>( kernel, "signed_native_enum" )
        .def( "SIGNED_NATIVE_UNKNOWN", detail::signed_native_enum::Unknown )
        .def( "SIGNED_NATIVE_READY", detail::signed_native_enum::Ready );
    pybind::enum_<detail::unsigned_native_enum>( kernel, "unsigned_native_enum" )
        .def( "UNSIGNED_NATIVE_HIGH", detail::unsigned_native_enum::High );
    pybind::enum_<detail::signed_wide_enum>( kernel, "signed_wide_enum" )
        .def( "SIGNED_WIDE_LOW", detail::signed_wide_enum::Low );
    pybind::enum_<detail::unsigned_wide_enum>( kernel, "unsigned_wide_enum" )
        .def( "UNSIGNED_WIDE_HIGH", detail::unsigned_wide_enum::High );

    pybind::def_function( kernel, "roundtrip_signed_native", &detail::roundtrip_enum<detail::signed_native_enum> );
    pybind::def_function( kernel, "roundtrip_unsigned_native", &detail::roundtrip_enum<detail::unsigned_native_enum> );
    pybind::def_function( kernel, "roundtrip_signed_wide", &detail::roundtrip_enum<detail::signed_wide_enum> );
    pybind::def_function( kernel, "roundtrip_unsigned_wide", &detail::roundtrip_enum<detail::unsigned_wide_enum> );

    std::ifstream enumScript( PYBIND_SANDBOX_SCRIPTS_PATH "/enum_contract.py" );
    PYBIND_CONTRACT_ASSERT( enumScript.is_open() == true );
    std::string enumSource( (std::istreambuf_iterator<char>( enumScript )), std::istreambuf_iterator<char>() );
    PyObject * enumDict = kernel->module_dict( module );
    kernel->dict_setstring( enumDict, "__builtins__", kernel->get_builtins() );
    PyObject * enumResult = kernel->exec_file( enumSource.c_str(), enumDict, enumDict );
    PYBIND_CONTRACT_ASSERT( enumResult != nullptr );
    kernel->decref( enumResult );
    PYBIND_CONTRACT_ASSERT( pybind::extract<bool>( kernel, kernel->dict_getstring( enumDict, "enum_value_ok" ) ) == true );
    PYBIND_CONTRACT_ASSERT( pybind::extract<bool>( kernel, kernel->dict_getstring( enumDict, "enum_roundtrip_ok" ) ) == true );
    PYBIND_CONTRACT_ASSERT( pybind::extract<bool>( kernel, kernel->dict_getstring( enumDict, "enum_overflow_ok" ) ) == true );

    // A function defined before the reload sees the new globals, and a name the
    // new source does not define stays, as reload keeps them.
    PyObject * beforeReload = kernel->exec_file( "def reload_get_value():\n    return value\nreload_kept = 5\n", kernel->module_dict( module ), kernel->module_dict( module ) );
    PYBIND_CONTRACT_ASSERT( beforeReload != nullptr );
    kernel->decref( beforeReload );

    const char reloadSource[] = "value = 17\ndef get_value():\n    return value\n";
    PyObject * reloadedModule = kernel->module_reload_source( module, reloadSource, "contract.py" );
    PYBIND_CONTRACT_ASSERT( reloadedModule == module );
    kernel->decref( reloadedModule );
    PyObject * reloadedValue = kernel->get_attrstring( module, "value" );
    int32_t reloadedInteger;
    PYBIND_CONTRACT_ASSERT( kernel->extract_int32( reloadedValue, reloadedInteger ) == true );
    PYBIND_CONTRACT_ASSERT( reloadedInteger == 17 );
    kernel->decref( reloadedValue );
    PyObject * reloadKept = kernel->eval_string( "reload_get_value() == 17 and reload_kept == 5", kernel->module_dict( module ), kernel->module_dict( module ) );
    PYBIND_CONTRACT_ASSERT( reloadKept != nullptr && pybind::extract<bool>( kernel, reloadKept ) == true );
    kernel->decref( reloadKept );

    const char failedReloadSource[] = "value = 99\nreload_partial = 1\nmissing_reload_function()\n";
    PyObject * failedReloadModule = kernel->module_reload_source( module, failedReloadSource, "contract.py" );
    PYBIND_CONTRACT_ASSERT( failedReloadModule == nullptr );
    PyObject * lastGoodValue = kernel->get_attrstring( module, "value" );
    int32_t lastGoodInteger;
    PYBIND_CONTRACT_ASSERT( kernel->extract_int32( lastGoodValue, lastGoodInteger ) == true );
    PYBIND_CONTRACT_ASSERT( lastGoodInteger == 17 );
    kernel->decref( lastGoodValue );
    PYBIND_CONTRACT_ASSERT( kernel->dict_getstring( kernel->module_dict( module ), "reload_partial" ) == nullptr );

    const char syntaxErrorReloadSource[] = "value =\n";
    PyObject * syntaxErrorReloadModule = kernel->module_reload_source( module, syntaxErrorReloadSource, "contract.py" );
    PYBIND_CONTRACT_ASSERT( syntaxErrorReloadModule == nullptr );
    PyObject * syntaxLastGoodValue = kernel->get_attrstring( module, "value" );
    int32_t syntaxLastGoodInteger;
    PYBIND_CONTRACT_ASSERT( kernel->extract_int32( syntaxLastGoodValue, syntaxLastGoodInteger ) == true );
    PYBIND_CONTRACT_ASSERT( syntaxLastGoodInteger == 17 );
    kernel->decref( syntaxLastGoodValue );

#if !defined(PYBIND_BACKEND_TINYPY) || defined(TINYPY_DEBUGGER)
    detail::debugger_capture_t debuggerCapture;
    debuggerCapture.kernel = kernel;
    PYBIND_CONTRACT_ASSERT( kernel->debugger_set_trace( &detail::capture_debugger, &debuggerCapture ) == true );
    const char debuggerSource[] = "def debug_target():\n    value = 1\n    value = value + 1\n    return value\n";
    PyObject * debuggerCodeResult = kernel->exec_file( debuggerSource, kernel->module_dict( module ), kernel->module_dict( module ) );
    PYBIND_CONTRACT_ASSERT( debuggerCodeResult != nullptr );
    kernel->decref( debuggerCodeResult );
    PyObject * debugTarget = kernel->get_attrstring( module, "debug_target" );
    PyObject * debugArgs = kernel->tuple_new( 0 );
    PyObject * debugResult = kernel->ask_native( debugTarget, debugArgs );
    int32_t debugValue;
    PYBIND_CONTRACT_ASSERT( kernel->extract_int32( debugResult, debugValue ) == true );
    PYBIND_CONTRACT_ASSERT( debugValue == 42 );
    PYBIND_CONTRACT_ASSERT( debuggerCapture.calls != 0 );
    PYBIND_CONTRACT_ASSERT( debuggerCapture.lines != 0 );
    PYBIND_CONTRACT_ASSERT( debuggerCapture.returns != 0 );
    PYBIND_CONTRACT_ASSERT( debuggerCapture.evaluatedFrame == true );
    PYBIND_CONTRACT_ASSERT( debuggerCapture.changedLocal == true );
    kernel->decref( debugResult );
    kernel->decref( debugArgs );
    kernel->decref( debugTarget );
    PYBIND_CONTRACT_ASSERT( kernel->debugger_set_trace( nullptr, nullptr ) == true );
#endif

    PyObject * unicodeValue = kernel->unicode_from_wchar( L"caf\u00e9" );
    PyObject * unicodeUtf8 = kernel->unicode_encode_utf8( unicodeValue );
    size_t unicodeUtf8Size;
    const char * unicodeUtf8Data = kernel->string_to_char_and_size( unicodeUtf8, &unicodeUtf8Size );
    PYBIND_CONTRACT_ASSERT( unicodeUtf8Size == 5 );
    PYBIND_CONTRACT_ASSERT( std::memcmp( unicodeUtf8Data, "caf\xc3\xa9", unicodeUtf8Size ) == 0 );
    size_t unicodeWcharSize;
    const wchar_t * unicodeWcharData = kernel->unicode_to_wchar_and_size( unicodeValue, &unicodeWcharSize );
    PYBIND_CONTRACT_ASSERT( unicodeWcharSize == 4 );
    PYBIND_CONTRACT_ASSERT( std::wmemcmp( unicodeWcharData, L"caf\u00e9", unicodeWcharSize ) == 0 );
    kernel->decref( unicodeUtf8 );
    kernel->decref( unicodeValue );

    pybind::def_function( kernel, "wide_pair_matches", &detail::wide_pair_matches );
    PyObject * widePairDict = kernel->module_dict( module );
    PyObject * widePairExec = kernel->exec_file( "wide_pair_ok = wide_pair_matches(u'alpha', u'beta with a longer tail')\n", widePairDict, widePairDict );
    PYBIND_CONTRACT_ASSERT( widePairExec != nullptr );
    kernel->decref( widePairExec );
    PYBIND_CONTRACT_ASSERT( pybind::extract<bool>( kernel, kernel->dict_getstring( widePairDict, "wide_pair_ok" ) ) == true );

    pybind::def_function( kernel, "wide_char_code", &detail::wide_char_code );
    PyObject * wideCharBuiltins = kernel->get_builtins();
    kernel->dict_setstring( widePairDict, "__builtins__", wideCharBuiltins );
    const char wideCharSource[] =
        "wide_char_ok = wide_char_code(u'z') == 122\n"
        "for wide_char_value in ('abcdefgh', 12345, [1, 2, 3]):\n"
        "    try:\n"
        "        wide_char_code(wide_char_value)\n"
        "        wide_char_ok = False\n"
        "    except Exception:\n"
        "        pass\n";
    PyObject * wideCharExec = kernel->exec_file( wideCharSource, widePairDict, widePairDict );
    PYBIND_CONTRACT_ASSERT( wideCharExec != nullptr );
    kernel->decref( wideCharExec );
    PYBIND_CONTRACT_ASSERT( pybind::extract<bool>( kernel, kernel->dict_getstring( widePairDict, "wide_char_ok" ) ) == true );

    PyObject * oldExcepthook = kernel->get_sys_excepthook();
    kernel->incref( oldExcepthook );
    detail::exception_capture_t exceptionCapture;
    exceptionCapture.kernel = kernel;
    kernel->set_sys_excepthook_f( &detail::capture_exception, &exceptionCapture );
    PyObject * moduleDict = kernel->module_dict( module );
    PyObject * moduleBuiltins = kernel->get_builtins();
    kernel->dict_setstring( moduleDict, "__builtins__", moduleBuiltins );
    const char exceptionSource[] = "def fail_inner():\n    return 1 + 'invalid'\ndef fail():\n    return fail_inner()\n";
    PyObject * exceptionModuleResult = kernel->exec_file( exceptionSource, moduleDict, moduleDict );
    PYBIND_CONTRACT_ASSERT( exceptionModuleResult != nullptr );
    kernel->decref( exceptionModuleResult );
    PyObject * fail = kernel->get_attrstring( module, "fail" );
    PyObject * failArgs = kernel->tuple_new( 0 );
    PyObject * failResult = kernel->ask_native( fail, failArgs );
    PYBIND_CONTRACT_ASSERT( failResult == nullptr );
    PYBIND_CONTRACT_ASSERT( exceptionCapture.called == true );
    PYBIND_CONTRACT_ASSERT( exceptionCapture.line != 0 );
    PYBIND_CONTRACT_ASSERT( exceptionCapture.traceback_next_borrowed == true );
    kernel->decref( failArgs );
    kernel->decref( fail );
#if defined(PYBIND_BACKEND_TINYPY)
    pybind::interface_<detail::test_module_finder>( kernel, "TestModuleFinder", true )
        .def_kernel( "find_module", &detail::test_module_finder::find_module )
        .def_kernel( "load_module", &detail::test_module_finder::load_module )
        ;
    PYBIND_CONTRACT_ASSERT( kernel->has_attrstring( module, "TestModuleFinder" ) == true );
    detail::test_module_finder finder;
    pybind::object finderObject = pybind::make_object_t( kernel, &finder );
    finder.set_embed( finderObject );
    PyObject * finderValue = finderObject.ptr();
    kernel->set_module_finder( finderValue );
#endif

    pybind::function_adapter_interface_ptr adapter = allocator.newT<detail::add_adapter>();
    kernel->def_function_adapter( adapter, true, module );
    pybind::def_function_out( kernel, "multiply_out", &detail::multiply_out, module );
    PyObject * add = kernel->get_attrstring( module, "add" );
    PyObject * left = kernel->ptr_int32( 19 );
    PyObject * right = kernel->ptr_int32( 23 );
    PyObject * args = kernel->tuple_new( 2 );
    kernel->tuple_setitem( args, 0, left );
    kernel->tuple_setitem( args, 1, right );
    PyObject * sum = kernel->ask_native( add, args );
    int32_t sumValue;
    bool sumResult = kernel->extract_int32( sum, sumValue );
    PYBIND_CONTRACT_ASSERT( sumResult == true );
    PYBIND_CONTRACT_ASSERT( sumValue == 42 );
    kernel->decref( sum );
    kernel->decref( args );
    kernel->decref( right );
    kernel->decref( left );
    kernel->decref( add );

    PyObject * multiplyOut = kernel->get_attrstring( module, "multiply_out" );
    PyObject * multiplyOutArgument = kernel->ptr_int32( 21 );
    PyObject * multiplyOutArgs = kernel->tuple_new( 1 );
    kernel->tuple_setitem( multiplyOutArgs, 0, multiplyOutArgument );
    PyObject * multiplyOutResult = kernel->ask_native( multiplyOut, multiplyOutArgs );
    int32_t multiplyOutValue;
    bool multiplyOutExtracted = kernel->extract_int32( multiplyOutResult, multiplyOutValue );
    PYBIND_CONTRACT_ASSERT( multiplyOutExtracted == true );
    PYBIND_CONTRACT_ASSERT( multiplyOutValue == 42 );
    kernel->decref( multiplyOutResult );
    kernel->decref( multiplyOutArgs );
    kernel->decref( multiplyOutArgument );
    kernel->decref( multiplyOut );

    bool missingMethodExceptionCalled = false;
    kernel->set_sys_excepthook_f( &detail::suppress_exception, &missingMethodExceptionCalled );
    PyObject * missingMethodArgs = kernel->tuple_new( 0 );
    PyObject * missingMethodResult = kernel->ask_method_native( module, "missing_method", missingMethodArgs );
    PYBIND_CONTRACT_ASSERT( missingMethodResult == nullptr );
    PYBIND_CONTRACT_ASSERT( missingMethodExceptionCalled == true );
    kernel->decref( missingMethodArgs );
    kernel->set_sys_excepthook( oldExcepthook );
    kernel->decref( oldExcepthook );

    pybind::interface_<detail::smart_pointer_base_t>( kernel, "SmartPointerBase", true, module )
        .def_smart_pointer()
        .def_bindable()
        ;
    pybind::interface_<detail::smart_pointer_derived_t, pybind::bases<detail::smart_pointer_base_t>>( kernel, "SmartPointerDerived", true, module )
        .def( "set_value", &detail::smart_pointer_derived_t::set_value )
        .def( "get_value", &detail::smart_pointer_derived_t::get_value )
        ;
    const pybind::class_type_scope_interface_ptr & smartDerivedScope = kernel->get_class_type_scope_t<detail::smart_pointer_derived_t>();
    PYBIND_CONTRACT_ASSERT( smartDerivedScope->get_bindable() != nullptr );
    stdex::intrusive_ptr<detail::smart_pointer_derived_t> smartPointer( new detail::smart_pointer_derived_t );
    PYBIND_CONTRACT_ASSERT( detail::smart_pointer_base_t::alive() == 1 );
    PyObject * smartObject = pybind::ptr_throw( kernel, smartPointer );
    PYBIND_CONTRACT_ASSERT( smartObject != nullptr );
    const pybind::class_type_scope_interface_ptr & smartObjectScope = kernel->get_class_scope( kernel->get_object_type( smartObject ) );
    PYBIND_CONTRACT_ASSERT( smartObjectScope.get() == smartDerivedScope.get() );
    smartPointer = nullptr;
    PYBIND_CONTRACT_ASSERT( detail::smart_pointer_base_t::alive() == 1 );
    PyObject * setValue = kernel->get_attrstring( smartObject, "set_value" );
    PyObject * setValueArgs = kernel->tuple_new( 1 );
    PyObject * setValueArgument = kernel->ptr_int32( 91 );
    kernel->tuple_setitem( setValueArgs, 0, setValueArgument );
    PyObject * setValueResult = kernel->ask_native( setValue, setValueArgs );
    PYBIND_CONTRACT_ASSERT( setValueResult != nullptr );
    kernel->decref( setValueResult );
    kernel->decref( setValueArgument );
    kernel->decref( setValueArgs );
    kernel->decref( setValue );
    PyObject * getValue = kernel->get_attrstring( smartObject, "get_value" );
    PyObject * getValueArgs = kernel->tuple_new( 0 );
    PyObject * getValueResult = kernel->ask_native( getValue, getValueArgs );
    int32_t smartValue;
    bool smartResult = kernel->extract_int32( getValueResult, smartValue );
    PYBIND_CONTRACT_ASSERT( smartResult == true && smartValue == 91 );
    kernel->decref( getValueResult );
    kernel->decref( getValueArgs );
    kernel->decref( getValue );
    kernel->decref( smartObject );
    PYBIND_CONTRACT_ASSERT( detail::smart_pointer_base_t::alive() == 0 );

    stdex::intrusive_ptr<detail::smart_pointer_derived_t> rawSmartPointer( new detail::smart_pointer_derived_t );
    PyObject * rawSmartObject = smartDerivedScope->create_class( rawSmartPointer.get() );
    PYBIND_CONTRACT_ASSERT( rawSmartObject != nullptr );
    rawSmartPointer = nullptr;
    PYBIND_CONTRACT_ASSERT( detail::smart_pointer_base_t::alive() == 1 );
    kernel->decref( rawSmartObject );
    PYBIND_CONTRACT_ASSERT( detail::smart_pointer_base_t::alive() == 0 );

    size_t holderDestroyCalls = 0;
    pybind::typeid_t holderTypeId = kernel->get_class_type_id( typeid(detail::smart_pointer_holder_t) );
    pybind::new_adapter_interface_ptr holderNew;
    pybind::destroy_adapter_interface_ptr holderDestroy = allocator.newT<detail::counting_destroy_adapter>( &holderDestroyCalls );
    pybind::class_type_scope_interface_ptr holderScope = kernel->create_new_type_scope( holderTypeId, "SmartPointerHolder", nullptr, holderNew, holderDestroy, 0, false );
    typedef decltype(&detail::smart_pointer_holder_t::intrusive_ptr_add_ref) holder_incref_t;
    typedef decltype(&detail::smart_pointer_holder_t::intrusive_ptr_dec_ref) holder_decref_t;
    typedef pybind::smart_pointer_adapter_proxy_function<detail::smart_pointer_holder_t, holder_incref_t, holder_decref_t> holder_smart_pointer_adapter_t;
    pybind::smart_pointer_adapter_interface_ptr holderSmartPointer = allocator.newT<holder_smart_pointer_adapter_t>( "smart_pointer"
        , &detail::smart_pointer_holder_t::intrusive_ptr_add_ref
        , &detail::smart_pointer_holder_t::intrusive_ptr_dec_ref
    );
    holderScope->set_smart_pointer( holderSmartPointer );
    bool holderScopeInitialized = holderScope->initialize( module );
    PYBIND_CONTRACT_ASSERT( holderScopeInitialized == true );
    stdex::intrusive_ptr<detail::smart_pointer_holder_t> holderValue( new detail::smart_pointer_holder_t );
    PyObject * holderObject = holderScope->create_holder( holderValue.get() );
    holderValue = nullptr;
    kernel->decref( holderObject );
    PYBIND_CONTRACT_ASSERT( holderDestroyCalls == 0 );
    PYBIND_CONTRACT_ASSERT( detail::smart_pointer_base_t::alive() == 0 );
    kernel->remove_type_scope( holderTypeId );
    holderScope = nullptr;
    holderSmartPointer = nullptr;
    holderDestroy = nullptr;

    pybind::interface_<detail::cast_primary_t>( kernel, "CastPrimary", true, module );
    pybind::interface_<detail::cast_secondary_t>( kernel, "CastSecondary", true, module )
        .def( "set_secondary", &detail::cast_secondary_t::set_secondary )
        ;
    pybind::interface_<detail::cast_middle_t, pybind::bases<detail::cast_primary_t, detail::cast_secondary_t>>( kernel, "CastMiddle", true, module );
    pybind::interface_<detail::cast_leaf_t, pybind::bases<detail::cast_middle_t>>( kernel, "CastLeaf", true, module );
    detail::cast_leaf_t castLeaf;
    detail::cast_secondary_t * castSecondary = static_cast<detail::cast_secondary_t *>( &castLeaf );
    PYBIND_CONTRACT_ASSERT( static_cast<void *>( &castLeaf ) != static_cast<void *>( castSecondary ) );
    PyObject * castObject = pybind::ptr_throw( kernel, &castLeaf );
    PyObject * setSecondary = kernel->get_attrstring( castObject, "set_secondary" );
    PyObject * setSecondaryArgs = kernel->tuple_new( 1 );
    PyObject * setSecondaryArgument = kernel->ptr_int32( 83 );
    kernel->tuple_setitem( setSecondaryArgs, 0, setSecondaryArgument );
    PyObject * setSecondaryResult = kernel->ask_native( setSecondary, setSecondaryArgs );
    PYBIND_CONTRACT_ASSERT( setSecondaryResult != nullptr );
    PYBIND_CONTRACT_ASSERT( castLeaf.marker == 17 );
    PYBIND_CONTRACT_ASSERT( castLeaf.value == 83 );
    detail::cast_secondary_t * extractedSecondary = pybind::extract<detail::cast_secondary_t *>( kernel, castObject );
    PYBIND_CONTRACT_ASSERT( extractedSecondary == castSecondary );
    kernel->decref( setSecondaryResult );
    kernel->decref( setSecondaryArgument );
    kernel->decref( setSecondaryArgs );
    kernel->decref( setSecondary );
    kernel->decref( castObject );

    detail::pod_proxy_t podProxy;
    pybind::struct_<detail::pod_value_t>( kernel, "PodValue", true, module )
        .def_constructor( pybind::init<>() )
        .def_call( &detail::pod_value_t::call )
        .def_proxy_static_kernel_args( "proxy_kernel_args", &podProxy, &detail::pod_proxy_t::add )
        .def_repr( &detail::repr_pod_value )
        .def_hash( &detail::hash_pod_value )
        .def_compare_equal()
        .def_convert( &detail::convert_pod_value )
        .def_operator_getset( 1 )
        .def_operator_neg()
        .def_abs( &detail::pod_value_t::absolute )
        .def_operator_add()
        .def_operator_sub()
        .def_operator_mul()
        .def_operator_div()
        .def_operator_inplace_add()
        .def_operator_inplace_sub()
        .def_operator_inplace_mul()
        .def_operator_inplace_div()
        ;
    PyObject * podType = kernel->get_attrstring( module, "PodValue" );
    PyObject * podArgs = kernel->tuple_new( 0 );
    PyObject * podObject = kernel->ask_native( podType, podArgs );
    PYBIND_CONTRACT_ASSERT( podObject != nullptr );
    detail::pod_value_t * podValue = static_cast<detail::pod_value_t *>( kernel->get_class_impl( podObject ) );
    PYBIND_CONTRACT_ASSERT( podValue != nullptr && podValue->value == 37 );

    {
        pybind::dict podDictionary( kernel );
        podDictionary.set( 37, 91 );
        PYBIND_CONTRACT_ASSERT( podDictionary.exist( *podValue ) == true );
        int32_t podDictionaryValue = podDictionary.get( *podValue );
        PYBIND_CONTRACT_ASSERT( podDictionaryValue == 91 );
    }

#if defined(PYBIND_BACKEND_TINYPY)
    {
        pybind::object moduleObject( kernel, module );
        moduleObject.set_attr( 37, 92 );
        PYBIND_CONTRACT_ASSERT( moduleObject.has_attr( *podValue ) == true );
        int32_t podAttributeValue = moduleObject.get_attr( *podValue ).extract();
        PYBIND_CONTRACT_ASSERT( podAttributeValue == 92 );
    }
#endif

    kernel->dict_setstring( moduleDict, "pod_object", podObject );

    PyObject * podReprExec = kernel->exec_file( "pod_repr_ok = repr(pod_object) == 'pod-37'\n", moduleDict, moduleDict );
    PYBIND_CONTRACT_ASSERT( podReprExec != nullptr );
    kernel->decref( podReprExec );
    PyObject * podHashExec = kernel->exec_file( "pod_hash_value = hash(pod_object)\npod_hash_ok = pod_hash_value == 37\n", moduleDict, moduleDict );
    PYBIND_CONTRACT_ASSERT( podHashExec != nullptr );
    kernel->decref( podHashExec );
    PyObject * podCallExec = kernel->exec_file( "pod_call_ok = pod_object(5) == 42\n", moduleDict, moduleDict );
    PYBIND_CONTRACT_ASSERT( podCallExec != nullptr );
    kernel->decref( podCallExec );
    PyObject * podProxyKernelArgsExec = kernel->exec_file( "pod_proxy_kernel_args_ok = pod_object.proxy_kernel_args(5) == 42\n", moduleDict, moduleDict );
    PYBIND_CONTRACT_ASSERT( podProxyKernelArgsExec != nullptr );
    kernel->decref( podProxyKernelArgsExec );
    const char podSequenceSource[] =
        "pod_sequence_get_ok = pod_object[0] == 37\n"
        "pod_object[0] = 41\n"
        "pod_sequence_set_ok = pod_object == 41\n"
        "pod_object[0] = 37\n"
        "pod_sequence_bounds_ok = list(pod_object) == [37]\n"
        "for pod_index in (1, -1):\n"
        "    try:\n"
        "        pod_object[pod_index]\n"
        "        pod_sequence_bounds_ok = False\n"
        "    except IndexError:\n"
        "        pass\n"
        "    try:\n"
        "        pod_object[pod_index] = 5\n"
        "        pod_sequence_bounds_ok = False\n"
        "    except IndexError:\n"
        "        pass\n";
    PyObject * podSequenceExec = kernel->exec_file( podSequenceSource, moduleDict, moduleDict );
    PYBIND_CONTRACT_ASSERT( podSequenceExec != nullptr );
    kernel->decref( podSequenceExec );
    const char podNumberSource[] =
        "class ContractUnknown(object):\n    pass\n"
        "pod_compare_ok = pod_object == 37 and 37 == pod_object and pod_object != 38 and 38 != pod_object\n"
        "pod_unknown_compare_ok = pod_object != ContractUnknown() and not (pod_object == ContractUnknown())\n"
        "pod_number_ok = pod_object + 5 == 42 and 5 + pod_object == 42 and pod_object - 5 == 32 and 5 - pod_object == -32 and pod_object * 2 == 74 and 2 * pod_object == 74 and pod_object / 2 == 18 and 74 / pod_object == 2\n"
        "try:\n    pod_object + ContractUnknown()\n    pod_unknown_number_ok = False\nexcept TypeError:\n    pod_unknown_number_ok = True\n"
        "pod_unary_ok = -pod_object == -37 and abs(pod_object) == 37\n";
    PyObject * podNumberExec = kernel->exec_file( podNumberSource, moduleDict, moduleDict );
    PYBIND_CONTRACT_ASSERT( podNumberExec != nullptr );
    kernel->decref( podNumberExec );
    const char podInplaceSource[] =
        "pod_inplace = PodValue()\n"
        "pod_inplace_identity = pod_inplace\n"
        "pod_inplace += 5\n"
        "pod_inplace -= 2\n"
        "pod_inplace *= 2\n"
        "pod_inplace /= 4\n"
        "pod_inplace_ok = pod_inplace is pod_inplace_identity and pod_inplace == 20\n"
        "pod_contract_ok = pod_repr_ok and pod_hash_ok and pod_call_ok and pod_proxy_kernel_args_ok and pod_sequence_get_ok and pod_sequence_set_ok and pod_compare_ok and pod_unknown_compare_ok and pod_number_ok and pod_unknown_number_ok and pod_unary_ok and pod_inplace_ok\n";
    PyObject * podInplaceExec = kernel->exec_file( podInplaceSource, moduleDict, moduleDict );
    PYBIND_CONTRACT_ASSERT( podInplaceExec != nullptr );
    kernel->decref( podInplaceExec );
    PyObject * podContract = kernel->get_attrstring( module, "pod_contract_ok" );
    bool podContractValue;
    bool podContractResult = kernel->extract_bool( podContract, podContractValue );
    int32_t podHashValue = pybind::extract<int32_t>( kernel, kernel->dict_getstring( moduleDict, "pod_hash_value" ) );
    PYBIND_CONTRACT_ASSERT( podHashValue == 37 );
    PYBIND_CONTRACT_ASSERT( pybind::extract<bool>( kernel, kernel->dict_getstring( moduleDict, "pod_repr_ok" ) ) == true );
    PYBIND_CONTRACT_ASSERT( pybind::extract<bool>( kernel, kernel->dict_getstring( moduleDict, "pod_hash_ok" ) ) == true );
    PYBIND_CONTRACT_ASSERT( pybind::extract<bool>( kernel, kernel->dict_getstring( moduleDict, "pod_call_ok" ) ) == true );
    PYBIND_CONTRACT_ASSERT( pybind::extract<bool>( kernel, kernel->dict_getstring( moduleDict, "pod_proxy_kernel_args_ok" ) ) == true );
    PYBIND_CONTRACT_ASSERT( pybind::extract<bool>( kernel, kernel->dict_getstring( moduleDict, "pod_sequence_get_ok" ) ) == true );
    PYBIND_CONTRACT_ASSERT( pybind::extract<bool>( kernel, kernel->dict_getstring( moduleDict, "pod_sequence_set_ok" ) ) == true );
    PYBIND_CONTRACT_ASSERT( pybind::extract<bool>( kernel, kernel->dict_getstring( moduleDict, "pod_sequence_bounds_ok" ) ) == true );
    PYBIND_CONTRACT_ASSERT( pybind::extract<bool>( kernel, kernel->dict_getstring( moduleDict, "pod_compare_ok" ) ) == true );
    PYBIND_CONTRACT_ASSERT( pybind::extract<bool>( kernel, kernel->dict_getstring( moduleDict, "pod_unknown_compare_ok" ) ) == true );
    PYBIND_CONTRACT_ASSERT( pybind::extract<bool>( kernel, kernel->dict_getstring( moduleDict, "pod_number_ok" ) ) == true );
    PYBIND_CONTRACT_ASSERT( pybind::extract<bool>( kernel, kernel->dict_getstring( moduleDict, "pod_unknown_number_ok" ) ) == true );
    PYBIND_CONTRACT_ASSERT( pybind::extract<bool>( kernel, kernel->dict_getstring( moduleDict, "pod_unary_ok" ) ) == true );
    PYBIND_CONTRACT_ASSERT( pybind::extract<bool>( kernel, kernel->dict_getstring( moduleDict, "pod_inplace_ok" ) ) == true );
    PYBIND_CONTRACT_ASSERT( podContractResult == true && podContractValue == true );
    kernel->decref( podContract );
    kernel->dict_removestring( moduleDict, "pod_object" );
    kernel->decref( podObject );

    const pybind::class_type_scope_interface_ptr & podScope = kernel->get_class_type_scope_t<detail::pod_value_t>();
    void * podStorage = nullptr;
    PyObject * wrappedPodObject = podScope->create_pod( &podStorage );
    PYBIND_CONTRACT_ASSERT( wrappedPodObject != nullptr );
    PYBIND_CONTRACT_ASSERT( podStorage != nullptr );
    new (podStorage) detail::pod_value_t();
    kernel->decref( wrappedPodObject );

    kernel->decref( podArgs );
    kernel->decref( podType );

    pybind::struct_<detail::scalar_value_t>( kernel, "ScalarValue", true, module )
        .def_constructor( pybind::init<>() )
        .def_member( "value", &detail::scalar_value_t::value )
        .def_hash( &detail::hash_scalar_value )
        .def_operator_mul_t<float>()
        .def_operator_mul_t<float, int32_t>()
        .def_operator_div_t<float>()
        .def_operator_div_t<float, int32_t>()
        .def_operator_inplace_mul_t<float>()
        .def_operator_inplace_mul_t<float, int32_t>()
        .def_operator_inplace_div_t<float>()
        .def_operator_inplace_div_t<float, int32_t>()
        ;
    const char scalarSource[] =
        "scalar_value = ScalarValue()\n"
        "scalar_mul_float = scalar_value * 2.5\n"
        "scalar_mul_int = 2 * scalar_value\n"
        "scalar_div_float = scalar_value / 1.5\n"
        "scalar_div_int = scalar_value / 3\n"
        "scalar_value *= 2.5\n"
        "scalar_value /= 3\n"
        "scalar_contract_ok = scalar_mul_float.value == 30.0 and scalar_mul_int.value == 24.0 and scalar_div_float.value == 8.0 and scalar_div_int.value == 4.0 and scalar_value.value == 10.0\n"
        "def scalar_rejects(function, *args):\n"
        "    try:\n"
        "        function(*args)\n"
        "    except TypeError:\n"
        "        return True\n"
        "    return False\n"
        "scalar_member_receiver_ok = ScalarValue.value.fget(scalar_value) == 10.0 and scalar_rejects(ScalarValue.value.fget, 12345) and scalar_rejects(ScalarValue.value.fset, 'text', 1.0)\n"
        "scalar_neg_ok = scalar_rejects(lambda: -scalar_value)\n";
    PyObject * scalarExec = kernel->exec_file( scalarSource, moduleDict, moduleDict );
    PYBIND_CONTRACT_ASSERT( scalarExec != nullptr );
    kernel->decref( scalarExec );
    PYBIND_CONTRACT_ASSERT( pybind::extract<bool>( kernel, kernel->dict_getstring( moduleDict, "scalar_member_receiver_ok" ) ) == true );
    PYBIND_CONTRACT_ASSERT( pybind::extract<bool>( kernel, kernel->dict_getstring( moduleDict, "scalar_neg_ok" ) ) == true );
    PyObject * scalarContract = kernel->get_attrstring( module, "scalar_contract_ok" );
    bool scalarContractValue;
    bool scalarContractResult = kernel->extract_bool( scalarContract, scalarContractValue );
    PYBIND_CONTRACT_ASSERT( scalarContractResult == true && scalarContractValue == true );
    kernel->decref( scalarContract );

    PyObject * scalarHashExcepthook = kernel->get_sys_excepthook();
    kernel->incref( scalarHashExcepthook );
    bool scalarHashExceptionCalled = false;
    kernel->set_sys_excepthook_f( &detail::suppress_exception, &scalarHashExceptionCalled );
    PyObject * scalarHashResult = kernel->eval_string( "hash(scalar_value)", moduleDict, moduleDict );
    PYBIND_CONTRACT_ASSERT( scalarHashResult == nullptr );
    PYBIND_CONTRACT_ASSERT( scalarHashExceptionCalled == true );
    kernel->set_sys_excepthook( scalarHashExcepthook );
    kernel->decref( scalarHashExcepthook );

    PyObject * globals = kernel->dict_new();
    PyObject * result = kernel->eval_string( "6 * 7", globals, globals );
    bool evalResult = kernel->extract_int32( result, sumValue );
    PYBIND_CONTRACT_ASSERT( evalResult == true );
    PYBIND_CONTRACT_ASSERT( sumValue == 42 );
    kernel->decref( result );
    kernel->decref( globals );

#if defined(PYBIND_BACKEND_TINYPY)
    bool moduleExisted;
    PyObject * sample = kernel->module_import( "sample", moduleExisted );
    PYBIND_CONTRACT_ASSERT( sample != nullptr && moduleExisted == true );
    PyObject * sampleValue = kernel->get_attrstring( sample, "value" );
    bool sampleResult = kernel->extract_int32( sampleValue, sumValue );
    PYBIND_CONTRACT_ASSERT( sampleResult == true && sumValue == 42 );
    kernel->decref( sampleValue );

    PyObject * reloadedSample = kernel->module_reload( sample );
    PYBIND_CONTRACT_ASSERT( reloadedSample == sample );
    kernel->decref( reloadedSample );
    kernel->decref( sample );
    size_t finderLoads = finder.loads();
    PYBIND_CONTRACT_ASSERT( finderLoads == 2 );
#endif

    pybind::typeid_t typeId = kernel->get_class_type_id( typeid(detail::native_value_t) );
    pybind::new_adapter_interface_ptr nativeNew = allocator.newT<detail::native_new_adapter>();
    pybind::destroy_adapter_interface_ptr nativeDestroy = allocator.newT<detail::native_destroy_adapter>();
    pybind::class_type_scope_interface_ptr scope = kernel->create_new_type_scope( typeId, "NativeValue", nullptr, nativeNew, nativeDestroy, 0, false );
    bool scopeInitialized = scope->initialize( module );
    PYBIND_CONTRACT_ASSERT( scopeInitialized == true );
    PYBIND_CONTRACT_ASSERT( kernel->has_attrstring( module, "NativeValue" ) == true );
    PyObject * constructorValue = kernel->ptr_int32( 73 );
    PyObject * constructorArgs = kernel->tuple_new( 1 );
    kernel->tuple_setitem( constructorArgs, 0, constructorValue );
    PyTypeObject * scopeType = scope->get_typeobject();
    PyObject * scopeObject = reinterpret_cast<PyObject *>( scopeType );
    PyObject * instance = kernel->ask_native( scopeObject, constructorArgs );
    PYBIND_CONTRACT_ASSERT( instance != nullptr );
    void * nativeImpl = kernel->get_class_impl( instance );
    detail::native_value_t * native = static_cast<detail::native_value_t *>( nativeImpl );
    PYBIND_CONTRACT_ASSERT( native != nullptr && native->value == 73 );
    kernel->decref( instance );

    kernel->remove_from_module( "missing_contract_name", module );
    kernel->remove_from_module( "missing_contract_name", module );
    PyObject * builtins = kernel->get_builtins();
    uint32_t builtinsRefcount = kernel->refcount( builtins );
    PyObject * repeatedBuiltins = kernel->get_builtins();
    PYBIND_CONTRACT_ASSERT( repeatedBuiltins == builtins );
    PYBIND_CONTRACT_ASSERT( kernel->refcount( repeatedBuiltins ) == builtinsRefcount );
    PyObject * builtinsDict = kernel->module_dict( builtins );
    PYBIND_CONTRACT_ASSERT( builtinsDict != nullptr );
    PyObject * trueValue = kernel->get_true();
    kernel->dict_setstring( builtinsDict, "contract_marker", trueValue );
    PyObject * contractMarker = kernel->dict_getstring( builtinsDict, "contract_marker" );
    PYBIND_CONTRACT_ASSERT( contractMarker == trueValue );
    kernel->dict_setstring( moduleDict, "__builtins__", builtins );
    const char subclassSource[] = "class PythonBase(object):\n    pass\nclass Derived(NativeValue, PythonBase):\n    pass\n";
    PyObject * subclassResult = kernel->exec_file( subclassSource, moduleDict, moduleDict );
    PYBIND_CONTRACT_ASSERT( subclassResult != nullptr );
    kernel->decref( subclassResult );
    PyObject * derived = kernel->dict_getstring( moduleDict, "Derived" );
    PYBIND_CONTRACT_ASSERT( derived != nullptr );
    PyObject * derivedInstance = kernel->ask_native( derived, constructorArgs );
    PYBIND_CONTRACT_ASSERT( derivedInstance != nullptr );
    void * derivedImpl = kernel->get_class_impl( derivedInstance );
    detail::native_value_t * derivedNative = static_cast<detail::native_value_t *>( derivedImpl );
    PYBIND_CONTRACT_ASSERT( derivedNative != nullptr && derivedNative->value == 73 );
    kernel->decref( derivedInstance );
    PyObject * retiredInstance = kernel->ask_native( scopeObject, constructorArgs );
    PYBIND_CONTRACT_ASSERT( retiredInstance != nullptr );
    kernel->decref( retiredInstance );
    kernel->remove_type_scope( typeId );
    PYBIND_CONTRACT_ASSERT( kernel->has_attrstring( module, "NativeValue" ) == false );
    scope = nullptr;
    kernel->decref( constructorArgs );
    kernel->decref( constructorValue );

    nativeDestroy = nullptr;
    nativeNew = nullptr;
    adapter = nullptr;

    detail::check_review_regressions( kernel, module );
    detail::check_second_review_regressions( kernel, module );

    kernel->set_current_module( nullptr );
    kernel->module_fini( module );
    kernel->remove_scope<detail::smart_pointer_derived_t>();
    kernel->remove_scope<detail::smart_pointer_base_t>();
    kernel->remove_scope<detail::cast_leaf_t>();
    kernel->remove_scope<detail::cast_middle_t>();
    kernel->remove_scope<detail::cast_secondary_t>();
    kernel->remove_scope<detail::cast_primary_t>();
    kernel->remove_scope<detail::pod_value_t>();
    kernel->remove_scope<detail::scalar_value_t>();
#if defined(PYBIND_BACKEND_TINYPY)
    kernel->remove_module_finder();
    finder.clear();
    finderObject = nullptr;
    kernel->remove_scope<detail::test_module_finder>();
#endif
    kernel->decref( module );
    kernel->collect();
    kernel->destroy();
#if defined(PYBIND_BACKEND_TINYPY)
    size_t allocations = allocator.allocations();
    PYBIND_CONTRACT_ASSERT( allocations == 0 );
#endif
    return 0;
}
