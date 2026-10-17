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
#include "pybind/set.hpp"
#include "pybind/module.hpp"
#include "pybind/generator/class_.hpp"
#include "pybind/generator/interface_.hpp"
#include "pybind/generator/proxy_.hpp"
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

    static int32_t s_regression3_node_alive = 0;
    static int32_t s_regression3_takes = 0;
    static int32_t s_regression3_owner_destroys = 0;
    static int32_t s_regression3_owner_notified = 0;
    static bool s_regression3_exists = false;
    static bool s_regression3_found = false;
    static bool s_regression3_removed = false;
    static bool s_regression3_extracted = true;

    class regression3_node_t
    {
    public:
        regression3_node_t()
        {
            ++s_regression3_node_alive;
        }

        ~regression3_node_t()
        {
            --s_regression3_node_alive;
        }

        int32_t getValue() const
        {
            return 7;
        }

        void take( int32_t _value )
        {
            (void)_value;

            ++s_regression3_takes;
        }
    };

    class regression3_other_t
    {
    public:
        int32_t getOther() const
        {
            return 3;
        }
    };

    class regression3_vector_t
    {
    public:
        int32_t getItem( size_t _index ) const
        {
            return (int32_t)_index * 10;
        }
    };

    class regression3_lookup_t
    {
    public:
        int32_t getItem( size_t _index ) const
        {
            return (int32_t)_index * 10;
        }

        int32_t getMapped( PyObject * _key ) const
        {
            (void)_key;

            return 222;
        }
    };

    class regression3_map_base_t
    {
    public:
        int32_t getMapped( PyObject * _key ) const
        {
            (void)_key;

            return 222;
        }
    };

    class regression3_map_derived_t
        : public regression3_map_base_t
    {
    public:
        int32_t getItem( size_t _index ) const
        {
            return (int32_t)_index * 1000;
        }
    };

    class regression3_protected_t
    {
    protected:
        ~regression3_protected_t()
        {
        }
    };

    class regression3_target_t
    {
    public:
        regression3_target_t()
            : value( 5 )
        {
        }

        int32_t value;
    };

    static regression3_target_t s_regression3_target;

    class regression3_owner_t
    {
    public:
        regression3_owner_t()
            : m_callback( nullptr )
        {
        }

        ~regression3_owner_t()
        {
            ++s_regression3_owner_destroys;

            if( m_callback == nullptr )
            {
                return;
            }

            // As a C++ object that notifies Python as it is released, while the
            // kernel may be destroying the interpreter.
            pybind::kernel_interface * kernel = pybind::get_kernel();

            PyObject * target = kernel->scope_create_weak_t( &s_regression3_target );
            PyObject * args = kernel->tuple_new( 1 );
            kernel->tuple_setitem( args, 0, target );
            PyObject * result = kernel->ask_native( m_callback, args );

            if( result != nullptr )
            {
                ++s_regression3_owner_notified;

                kernel->decref( result );
            }

            kernel->decref( args );
            kernel->decref( target );
            kernel->decref( m_callback );
        }

        void setCallback( PyObject * _callback )
        {
            pybind::get_kernel()->incref( _callback );

            m_callback = _callback;
        }

    protected:
        PyObject * m_callback;
    };

    static const char * repr_regression3_node( const regression3_node_t * _node )
    {
        (void)_node;
        return "regression3-node";
    }

    static pybind::object regression3_pending_dict( pybind::kernel_interface * _kernel, PyObject * _dict, PyObject * _key )
    {
        // An error raised before the lookups is neither taken for theirs nor
        // cleared by them, so Python sees it.
        _kernel->set_error( pybind::error_type_e::Index, "regression pending" );

        s_regression3_exists = _kernel->dict_exist( _dict, _key );
        s_regression3_found = _kernel->dict_get( _dict, _key ) != nullptr;
        s_regression3_removed = _kernel->dict_remove( _dict, _key );

        return pybind::make_invalid_object_t();
    }

    static pybind::object regression3_pending_repr( pybind::kernel_interface * _kernel, PyObject * _object )
    {
        // An error path formats its object while the error is pending.
        _kernel->set_error( pybind::error_type_e::Index, "regression pending" );

        pybind::string_view repr = _kernel->object_repr( _object );
        PYBIND_CONTRACT_ASSERT( repr.is_invalid() == true );

        return pybind::make_invalid_object_t();
    }

    static pybind::object regression3_extract_double( pybind::kernel_interface * _kernel, PyObject * _object )
    {
        double value;
        s_regression3_extracted = _kernel->extract_double( _object, value );

        // An error the extract left pending would keep this one from being set.
        _kernel->set_error( pybind::error_type_e::Value, "regression after extract" );

        return pybind::make_invalid_object_t();
    }

    static void regression3_hook( void * _ud, PyTypeObject * _type, PyObject * _value, PyObject * _traceback )
    {
        (void)_value;
        (void)_traceback;

        int32_t * calls = static_cast<int32_t *>(_ud);

        if( _type != nullptr )
        {
            ++calls[0];
        }
        else
        {
            ++calls[1];
        }
    }

    static void check_third_review_regressions( pybind::kernel_interface * _kernel, PyObject * _module )
    {
        pybind::class_<regression3_node_t>( _kernel, "Regression3Node", true, _module )
            .def_constructor( pybind::init<>() )
            .def( "getValue", &regression3_node_t::getValue )
            .def( "take", &regression3_node_t::take )
            .def_repr( &repr_regression3_node )
            ;

        pybind::class_<regression3_other_t>( _kernel, "Regression3Other", true, _module )
            .def_constructor( pybind::init<>() )
            .def( "getOther", &regression3_other_t::getOther )
            ;

        pybind::class_<regression3_vector_t>( _kernel, "Regression3Vector", true, _module )
            .def_constructor( pybind::init<>() )
            .def_sequence_get( &regression3_vector_t::getItem )
            ;

        // The sequence is set before the mapping, which shadows it.
        pybind::class_<regression3_lookup_t>( _kernel, "Regression3Lookup", true, _module )
            .def_constructor( pybind::init<>() )
            .def_sequence_get( &regression3_lookup_t::getItem )
            .def_mapping( &regression3_lookup_t::getMapped )
            ;

        pybind::class_<regression3_map_base_t>( _kernel, "Regression3MapBase", true, _module )
            .def_constructor( pybind::init<>() )
            .def_mapping( &regression3_map_base_t::getMapped )
            ;

        pybind::class_<regression3_map_derived_t, pybind::bases<regression3_map_base_t>>( _kernel, "Regression3MapDerived", true, _module )
            .def_constructor( pybind::init<>() )
            .def_sequence_get( &regression3_map_derived_t::getItem )
            ;

        // A class with no public destructor is only ever wrapped as a holder.
        pybind::proxy_<regression3_protected_t>( _kernel, "Regression3Protected", true, _module );

        pybind::def_function_kernel( _kernel, "regression3_pending_dict", &regression3_pending_dict );
        pybind::def_function_kernel( _kernel, "regression3_pending_repr", &regression3_pending_repr );
        pybind::def_function_kernel( _kernel, "regression3_extract_double", &regression3_extract_double );

        PyObject * dict = _kernel->module_dict( _module );

        PyObject * longValue = _kernel->ptr_long( 5 );
        PyObject * ulongValue = _kernel->ptr_ulong( 7 );
        _kernel->dict_setstring( dict, "regression3_long", longValue );
        _kernel->dict_setstring( dict, "regression3_ulong", ulongValue );
        _kernel->decref( longValue );
        _kernel->decref( ulongValue );

        const char source[] = R"PY(
class Regression3Equal(object):
    def __eq__(self, other):
        return True

regression3_equal_left = Regression3Equal()
regression3_equal_right = Regression3Equal()

class Regression3BadRepr(object):
    def __repr__(self):
        raise ValueError('bad repr')

class Regression3BadBool(object):
    def __nonzero__(self):
        raise KeyError('bad bool')

regression3_bad_bool = Regression3BadBool()

regression3_pending_dict_ok = raises(IndexError, regression3_pending_dict, {1: 2}, 1)
regression3_pending_repr_ok = raises(IndexError, regression3_pending_repr, Regression3BadRepr())
regression3_extract_double_ok = raises(ValueError, regression3_extract_double, 10 ** 400)
regression3_int_ok = type(regression3_long) is int and type(regression3_ulong) is int
regression3_default_hook_ok = callable(sys.excepthook) and callable(sys.__excepthook__)
regression3_name_ok = regression_size.__name__ == 'regression_size'
regression3_module_ok = raises(Exception, regression_module_valid, 5)

class Regression3Sized(Regression3Vector):
    def __len__(self):
        return 3

regression3_negative_ok = Regression3Sized()[-1] == 20 and raises(IndexError, lambda: Regression3Vector()[-1])

class Regression3LookupSub(Regression3Lookup):
    pass

regression3_lookup_ok = Regression3Lookup()['k'] == 222 and Regression3LookupSub()['k'] == 222 and Regression3LookupSub()[1] == Regression3Lookup()[1]
regression3_derived_ok = Regression3MapDerived()[2] == 2000 and Regression3MapBase()['k'] == 222

regression3_node = Regression3Node()

def regression3_set_class(value):
    object.__dict__['__class__'].__set__(regression3_node, value)

def regression3_bypass():
    regression3_set_class(Regression3Other)
    return regression3_node.getOther()

regression3_bypass_ok = raises(Exception, regression3_bypass)

try:
    regression3_set_class(Regression3Node)
except Exception:
    pass

class Regression3Sub(Regression3Node):
    pass

regression3_sub = Regression3Sub()

def regression3_rebase():
    Regression3Sub.__bases__ = (Regression3Other,)
    return regression3_sub.getOther()

regression3_rebase_ok = raises(Exception, regression3_rebase)

try:
    Regression3Sub.__bases__ = (Regression3Node,)
except Exception:
    pass

regression3_class_ok = regression3_bypass_ok and regression3_rebase_ok and regression3_node.getValue() == 7 and regression3_sub.getValue() == 7
del regression3_node
del regression3_sub

regression3_capture = Capture()
regression3_hook_calls = []
)PY";

        PyObject * result = _kernel->exec_file( source, dict, dict );
        PYBIND_CONTRACT_ASSERT( result != nullptr );
        _kernel->decref( result );

        PYBIND_CONTRACT_ASSERT( s_regression3_exists == true );
        PYBIND_CONTRACT_ASSERT( s_regression3_found == true );
        PYBIND_CONTRACT_ASSERT( s_regression3_removed == true );
        PYBIND_CONTRACT_ASSERT( s_regression3_extracted == false );

        // An equality is the one __eq__ decides.
        PyObject * equalLeft = _kernel->dict_getstring( dict, "regression3_equal_left" );
        PyObject * equalRight = _kernel->dict_getstring( dict, "regression3_equal_right" );
        PYBIND_CONTRACT_ASSERT( _kernel->test_equal( equalLeft, equalRight ) == true );

        PyObject * capture = _kernel->dict_getstring( dict, "regression3_capture" );
        PyObject * oldStderr = _kernel->getStdErrorHandle();
        _kernel->incref( oldStderr );
        _kernel->setStdErrorHandle( capture );

        // A value that does not extract or decode leaves no error for the code
        // that runs next.
        PyObject * badBool = _kernel->dict_getstring( dict, "regression3_bad_bool" );
        bool boolValue = true;
        PYBIND_CONTRACT_ASSERT( _kernel->extract_bool( badBool, boolValue ) == false );
        PyObject * afterBool = _kernel->exec_file( "regression3_after_bool = 1\n", dict, dict );
        PYBIND_CONTRACT_ASSERT( afterBool != nullptr );
        _kernel->decref( afterBool );

        PYBIND_CONTRACT_ASSERT( _kernel->unicode_from_utf8_size( "\xf4", 1 ) == nullptr );
        PyObject * afterUnicode = _kernel->exec_file( "regression3_after_unicode = 1\n", dict, dict );
        PYBIND_CONTRACT_ASSERT( afterUnicode != nullptr );
        _kernel->decref( afterUnicode );

        // As PyImport_ExecCodeModule: the module names its file, a failure
        // leaves no module, and the result is the entry the code leaves.
        PyObject * fileCode = _kernel->compile_string( "regression3_file_marker = 1\n", "regression3_file.py" );
        PYBIND_CONTRACT_ASSERT( fileCode != nullptr );
        PyObject * fileModule = _kernel->module_execcode( "regression3_file", fileCode );
        PYBIND_CONTRACT_ASSERT( fileModule != nullptr );
        PyObject * fileName = _kernel->get_attrstring( fileModule, "__file__" );
        PYBIND_CONTRACT_ASSERT( fileName != nullptr && std::strcmp( _kernel->string_to_char( fileName ), "regression3_file.py" ) == 0 );
        _kernel->decref( fileName );
        _kernel->decref( fileModule );
        _kernel->decref( fileCode );

        PyObject * failCode = _kernel->compile_string( "regression3_half = 1\nregression3_missing_function()\n", "regression3_fail.py" );
        PYBIND_CONTRACT_ASSERT( failCode != nullptr );
        PYBIND_CONTRACT_ASSERT( _kernel->module_execcode( "regression3_fail", failCode ) == nullptr );
        _kernel->decref( failCode );

        PyObject * replaceCode = _kernel->compile_string( "import sys\nclass Replacement(object):\n    marker = 3\nsys.modules['regression3_replace'] = Replacement()\n", "regression3_replace.py" );
        PYBIND_CONTRACT_ASSERT( replaceCode != nullptr );
        PyObject * replaced = _kernel->module_execcode( "regression3_replace", replaceCode );
        PYBIND_CONTRACT_ASSERT( replaced != nullptr && _kernel->has_attrstring( replaced, "marker" ) == true );
        _kernel->decref( replaced );
        _kernel->decref( replaceCode );

        // The default sys.excepthook is a function a hook set later chains to,
        // and a C++ hook takes the class of a classic exception as unknown.
        PyObject * oldHook = _kernel->get_sys_excepthook();
        _kernel->incref( oldHook );

        PyObject * chainResult = _kernel->exec_file( "regression3_old_hook = sys.excepthook\ndef regression3_chain(t, v, tb):\n    regression3_hook_calls.append(str(v))\n    regression3_old_hook(t, v, tb)\nsys.excepthook = regression3_chain\n", dict, dict );
        PYBIND_CONTRACT_ASSERT( chainResult != nullptr );
        _kernel->decref( chainResult );
        PYBIND_CONTRACT_ASSERT( _kernel->exec_file( "raise ValueError('chained')\n", dict, dict ) == nullptr );

        int32_t hookCalls[2] = {0, 0};
        _kernel->set_sys_excepthook_f( &detail::regression3_hook, hookCalls );
        PYBIND_CONTRACT_ASSERT( _kernel->exec_file( "class Regression3Classic:\n    pass\nraise Regression3Classic()\n", dict, dict ) == nullptr );
        PYBIND_CONTRACT_ASSERT( _kernel->exec_file( "raise ValueError('typed')\n", dict, dict ) == nullptr );
        PYBIND_CONTRACT_ASSERT( hookCalls[0] == 1 );
        PYBIND_CONTRACT_ASSERT( hookCalls[1] == 1 );

        _kernel->set_sys_excepthook( oldHook );
        _kernel->decref( oldHook );
        _kernel->setStdErrorHandle( oldStderr );
        _kernel->decref( oldStderr );

        // A missing dict key reads as None, and a null item is set nowhere.
        pybind::dict missingDict( _kernel );
        pybind::list missingList( _kernel );
        missingList.append( missingDict["missing"] );
        PYBIND_CONTRACT_ASSERT( missingList.size() == 1 );
        PYBIND_CONTRACT_ASSERT( _kernel->is_none( _kernel->list_getitem( missingList.ptr(), 0 ) ) == true );
        PYBIND_CONTRACT_ASSERT( _kernel->list_appenditem( missingList.ptr(), nullptr ) == false );
        PYBIND_CONTRACT_ASSERT( _kernel->list_setitem( missingList.ptr(), 0, nullptr ) == false );
        PYBIND_CONTRACT_ASSERT( _kernel->dict_setstring( missingDict.ptr(), "missing", nullptr ) == false );
        PYBIND_CONTRACT_ASSERT( _kernel->dict_set( missingDict.ptr(), nullptr, nullptr ) == false );
        PYBIND_CONTRACT_ASSERT( missingList.size() == 1 );

        PyObject * nullSet = _kernel->set_new();
        PYBIND_CONTRACT_ASSERT( _kernel->set_set( nullSet, nullptr ) == false );
        PYBIND_CONTRACT_ASSERT( _kernel->set_size( nullSet ) == 0 );
        _kernel->decref( nullSet );

        PYBIND_CONTRACT_ASSERT( _kernel->set_attrstring( _module, "regression3_kept", _kernel->get_true() ) == true );
        PYBIND_CONTRACT_ASSERT( _kernel->set_attrstring( _module, "regression3_kept", nullptr ) == false );
        PYBIND_CONTRACT_ASSERT( _kernel->has_attrstring( _module, "regression3_kept" ) == true );

        // A null element makes no arguments, so nothing is called.
        std::vector<pybind::object> failedObjects = {pybind::make_invalid_object_t()};
        pybind::tuple failedTuple = pybind::make_tuple_container_t( _kernel, failedObjects );
        PYBIND_CONTRACT_ASSERT( failedTuple.is_invalid() == true );

        {
            regression3_node_t internalNode;
            pybind::object failedArgument = pybind::make_invalid_object_t();
            pybind::object internalResult = pybind::internal_call_method( _kernel, &internalNode, "take", failedArgument );
            PYBIND_CONTRACT_ASSERT( internalResult.is_invalid() == true );
            PYBIND_CONTRACT_ASSERT( s_regression3_takes == 0 );
        }

        // A slot of an unwrapped object fails in every build.
        PyObject * unwrapResult = _kernel->exec_file( "regression3_unwrapped = Regression3Node()\n", dict, dict );
        PYBIND_CONTRACT_ASSERT( unwrapResult != nullptr );
        _kernel->decref( unwrapResult );
        PyObject * unwrapped = _kernel->dict_getstring( dict, "regression3_unwrapped" );
        regression3_node_t * unwrappedNode = static_cast<regression3_node_t *>(_kernel->unwrap( unwrapped ));
        PYBIND_CONTRACT_ASSERT( unwrappedNode != nullptr );
        delete unwrappedNode;

        // A class whose scope is removed creates no instance.
        PyObject * removedResult = _kernel->exec_file( "regression3_unwrapped_ok = raises(RuntimeError, repr, regression3_unwrapped)\ndel regression3_unwrapped\nregression3_removed_type = Regression3Vector\n", dict, dict );
        PYBIND_CONTRACT_ASSERT( removedResult != nullptr );
        _kernel->decref( removedResult );

        _kernel->remove_scope<regression3_vector_t>();

        const char finalSource[] = R"PY(
regression3_removed_ok = raises(TypeError, regression3_removed_type)
del regression3_removed_type
regression3_text = ''.join(regression3_capture.parts)
regression3_chain_ok = regression3_hook_calls == ['chained'] and 'ValueError: chained' in regression3_text
)PY";

        PyObject * finalResult = _kernel->exec_file( finalSource, dict, dict );
        PYBIND_CONTRACT_ASSERT( finalResult != nullptr );
        _kernel->decref( finalResult );

        const char * flags[] = {"regression3_pending_dict_ok", "regression3_pending_repr_ok", "regression3_extract_double_ok", "regression3_int_ok", "regression3_default_hook_ok", "regression3_name_ok", "regression3_module_ok", "regression3_negative_ok", "regression3_lookup_ok", "regression3_derived_ok", "regression3_class_ok", "regression3_unwrapped_ok", "regression3_removed_ok", "regression3_chain_ok"};

        for( const char * flag : flags )
        {
            PYBIND_CONTRACT_ASSERT( detail::regression_flag( _kernel, dict, flag ) == true );
        }

        PYBIND_CONTRACT_ASSERT( s_regression3_node_alive == 0 );

        _kernel->remove_scope<regression3_protected_t>();
        _kernel->remove_scope<regression3_map_derived_t>();
        _kernel->remove_scope<regression3_map_base_t>();
        _kernel->remove_scope<regression3_lookup_t>();
        _kernel->remove_scope<regression3_other_t>();
        _kernel->remove_scope<regression3_node_t>();
    }

    static int32_t s_regression4_plugin_alive = 0;
    static bool s_regression4_has_attr = false;
    static bool s_regression4_removed = false;
    static bool s_regression4_ended = false;
    static bool s_regression4_long = false;
    static bool s_regression4_imported = false;
#if defined(PYBIND_BACKEND_PYTHON)
    // Kept past the kernel, as CPython keeps what its finalize leaves.
    static PyObject * s_regression4_keeper = nullptr;
#endif

    struct regression4_base_t
    {
        int32_t base = 2;
    };

    struct regression4_derived_t
        : public regression4_base_t
    {
        // A vtable puts the base at an offset of the object.
        virtual ~regression4_derived_t()
        {
        }
    };

    class regression4_plugin_t
    {
    public:
        regression4_plugin_t()
        {
            ++s_regression4_plugin_alive;
        }

        ~regression4_plugin_t()
        {
            --s_regression4_plugin_alive;

            // As a plugin that unloads its bindings as it is released.
            pybind::kernel_interface * kernel = pybind::get_kernel();
            kernel->remove_scope<regression4_plugin_t>();
        }
    };

    struct regression4_doubles_t
    {
        double x = 1.0;
        double y = 2.0;
    };

    class regression4_compared_t
    {
    };

    static int32_t regression4_read_base( regression4_base_t * _base )
    {
        return _base->base;
    }

    static const std::string & regression4_pick( const std::string & _first, const std::string & _second )
    {
        return _first.size() >= _second.size() ? _first : _second;
    }

    static size_t regression4_wide_size( const std::wstring & _value )
    {
        return _value.size();
    }

    static void regression4_throw()
    {
        pybind::throw_exception( "regression4 thrown" );
    }

    static bool regression4_compare_int( pybind::kernel_interface * _kernel, PyObject * _obj, regression4_compared_t * _self, PyObject * _compare, pybind::PybindOperatorCompare _op, bool * const _result )
    {
        (void)_kernel;
        (void)_obj;
        (void)_self;
        (void)_compare;

        *_result = _op == pybind::POC_Equal;

        return true;
    }

    static void regression4_hook( void * _ud, PyTypeObject * _type, PyObject * _value, PyObject * _traceback )
    {
        (void)_ud;
        (void)_type;
        (void)_value;
        (void)_traceback;
    }

    static pybind::object regression4_pending_ops( pybind::kernel_interface * _kernel, PyObject * _object, PyObject * _dict, PyObject * _iterable, PyObject * _long )
    {
        // An error raised before these operations stays the one Python sees.
        _kernel->set_error( pybind::error_type_e::Index, "regression pending" );

        s_regression4_has_attr = _kernel->has_attrstring( _object, "regression4_missing" ) == false;
        s_regression4_removed = _kernel->dict_removestring( _dict, "regression4_missing" ) == false;

        PyObject * iterator;
        PyObject * item;

        if( _kernel->iterator_get( _iterable, &iterator ) == true )
        {
            s_regression4_ended = _kernel->iterator_next( iterator, &item ) == false;

            _kernel->iterator_end( iterator );
        }

        int64_t value = 0;
        s_regression4_long = _kernel->extract_int64( _long, value ) == true && value == -1;

        bool exists = false;
        PyObject * sys = _kernel->module_import( "sys", exists );
        s_regression4_imported = sys != nullptr && exists == true;

        if( sys != nullptr )
        {
            _kernel->decref( sys );
        }

        return pybind::make_invalid_object_t();
    }

    static void check_fourth_review_regressions( pybind::kernel_interface * _kernel, PyObject * _module )
    {
        pybind::class_<regression4_base_t>( _kernel, "Regression4Base", true, _module )
            .def_constructor( pybind::init<>() )
            .def_member( "base", &regression4_base_t::base )
            ;

        pybind::class_<regression4_derived_t, pybind::bases<regression4_base_t>>( _kernel, "Regression4Derived", true, _module )
            .def_constructor( pybind::init<>() )
            ;

        pybind::class_<regression4_plugin_t>( _kernel, "Regression4Plugin", true, _module )
            .def_constructor( pybind::init<>() )
            ;

        pybind::struct_<regression4_doubles_t>( _kernel, "Regression4Doubles", true, _module )
            .def_constructor( pybind::init<>() )
            ;

        pybind::class_<regression4_compared_t>( _kernel, "Regression4Compared", true, _module )
            .def_constructor( pybind::init<>() )
            .def_compare_t<decltype(&regression4_compare_int), int32_t>( &regression4_compare_int )
            ;

        pybind::def_function( _kernel, "regression4_read_base", &regression4_read_base );
        pybind::def_function( _kernel, "regression4_pick", &regression4_pick );
        pybind::def_function( _kernel, "regression4_wide_size", &regression4_wide_size );
        pybind::def_function( _kernel, "regression4_throw", &regression4_throw );
        pybind::def_function_kernel( _kernel, "regression4_pending_ops", &regression4_pending_ops );

        regression4_derived_t derived;
        PYBIND_CONTRACT_ASSERT( static_cast<void *>( &derived ) != static_cast<void *>( static_cast<regression4_base_t *>( &derived ) ) );

        PyObject * dict = _kernel->module_dict( _module );

        const char source[] = R"PY(
regression4_derived = Regression4Derived()
regression4_derived_ok = regression4_read_base(regression4_derived) == 2

regression4_plugin = Regression4Plugin()
del regression4_plugin

regression4_doubles = Regression4Doubles()
regression4_compared = Regression4Compared()

class Regression4Unicode(unicode):
    pass

regression4_long_text = 'long text'
regression4_pick_ok = regression4_pick('a', regression4_long_text) == 'long text' and regression4_pick('abc', 'de') == 'abc'
regression4_unicode_ok = regression4_wide_size(Regression4Unicode(u'abc')) == 3

try:
    regression4_throw()
    regression4_message_ok = False
except RuntimeError as regression4_error:
    regression4_message_ok = str(regression4_error) == 'regression4 thrown'

regression4_kwargs_ok = raises(TypeError, lambda: regression_size(1, value=2)) and regression_size(1) == 2
regression4_arity_ok = raises(Exception, regression_size)
regression4_pending_ok = raises(IndexError, regression4_pending_ops, Capture(), {}, iter([]), -1L)

class Regression4Dir(object):
    def __init__(self):
        self.names = ['a', 'b', 'c']

    def __dir__(self):
        return self.names

    def __getattr__(self, name):
        del self.names[:]
        return 1

regression4_dir = Regression4Dir()

regression4_premade = type(sys)('regression4_premade')
sys.modules['regression4_premade'] = regression4_premade

regression4_capture = Capture()

def regression4_warn():
    for index in range(3):
        regression_warning()
)PY";

        PyObject * result = _kernel->exec_file( source, dict, dict );
        PYBIND_CONTRACT_ASSERT( result != nullptr );
        _kernel->decref( result );

        PYBIND_CONTRACT_ASSERT( s_regression4_plugin_alive == 0 );
        PYBIND_CONTRACT_ASSERT( s_regression4_has_attr == true );
        PYBIND_CONTRACT_ASSERT( s_regression4_removed == true );
        PYBIND_CONTRACT_ASSERT( s_regression4_ended == true );
        PYBIND_CONTRACT_ASSERT( s_regression4_long == true );
        PYBIND_CONTRACT_ASSERT( s_regression4_imported == true );

        // A struct of doubles is built at an address they may take.
        PyObject * doubles = _kernel->dict_getstring( dict, "regression4_doubles" );
        void * doublesImpl = _kernel->get_class_impl( doubles );
        PYBIND_CONTRACT_ASSERT( reinterpret_cast<uintptr_t>( doublesImpl ) % alignof( regression4_doubles_t ) == 0 );

        // The attributes of an object whose dir() list a lookup empties.
        PyObject * dirObject = _kernel->dict_getstring( dict, "regression4_dir" );
        PyObject * dirAttributes = _kernel->dict_from( dirObject );
        PYBIND_CONTRACT_ASSERT( dirAttributes != nullptr );
        _kernel->decref( dirAttributes );

        // A module made elsewhere gets the builtins.
        PyObject * premade = _kernel->module_init( "regression4_premade" );
        PYBIND_CONTRACT_ASSERT( premade != nullptr && _kernel->has_attrstring( premade, "__builtins__" ) == true );

        // A list takes an index past its end as CPython does, and no null item.
        pybind::list items( _kernel );
        items.append( 1 );
        PyObject * itemsObject = items.ptr();
        PyObject * two = _kernel->ptr_int32( 2 );
        PYBIND_CONTRACT_ASSERT( _kernel->list_insert( itemsObject, 5, two ) == true );
        PYBIND_CONTRACT_ASSERT( items.size() == 2 && pybind::extract<int32_t>( _kernel, _kernel->list_getitem( itemsObject, 1 ) ) == 2 );
        PYBIND_CONTRACT_ASSERT( _kernel->list_remove( itemsObject, 7 ) == true );
        PYBIND_CONTRACT_ASSERT( _kernel->list_insert( itemsObject, 0, nullptr ) == false );
        PYBIND_CONTRACT_ASSERT( items.size() == 2 );
        _kernel->decref( two );

        // A query with the null object of a failed call finds nothing.
        pybind::object failed = pybind::object( _kernel, _module ).get_attr( "regression4_missing" );
        PYBIND_CONTRACT_ASSERT( failed.is_invalid() == true && failed == nullptr );
        PYBIND_CONTRACT_ASSERT( failed.is_none() == false && failed.is_callable() == false && failed.is_type_class() == false );
        PYBIND_CONTRACT_ASSERT( failed.get_attr( "x" ).is_invalid() == true );
        PYBIND_CONTRACT_ASSERT( failed.has_attr( "x" ) == false );
        PYBIND_CONTRACT_ASSERT( pybind::object( _kernel, _module ).has_attr( failed ) == false );
        pybind::dict queried( _kernel );
        PYBIND_CONTRACT_ASSERT( queried.exist( failed ) == false );

        // An object that is not a type is no type of a class.
        PyObject * number = _kernel->ptr_int32( 5 );
        PYBIND_CONTRACT_ASSERT( pybind::object( _kernel, number ).is_type_class() == false );
        _kernel->decref( number );

        // Code does not run while an error is pending, which is reported.
        PyObject * capture = _kernel->dict_getstring( dict, "regression4_capture" );
        PyObject * oldStderr = _kernel->getStdErrorHandle();
        _kernel->incref( oldStderr );
        _kernel->setStdErrorHandle( capture );

        _kernel->set_error( pybind::error_type_e::Value, "regression4 pending" );
        PYBIND_CONTRACT_ASSERT( _kernel->exec_file( "regression4_ran = True\n", dict, dict ) == nullptr );
        PYBIND_CONTRACT_ASSERT( _kernel->dict_getstring( dict, "regression4_ran" ) == nullptr );

        // A warning of one place is written once.
        PyObject * warnResult = _kernel->exec_file( "regression4_warn()\n", dict, dict );
        PYBIND_CONTRACT_ASSERT( warnResult != nullptr );
        _kernel->decref( warnResult );

        _kernel->setStdErrorHandle( oldStderr );
        _kernel->decref( oldStderr );

        // An unwrapped object fails its comparison as any other slot.
        PyObject * compared = _kernel->dict_getstring( dict, "regression4_compared" );
        regression4_compared_t * comparedImpl = static_cast<regression4_compared_t *>(_kernel->unwrap( compared ));
        PYBIND_CONTRACT_ASSERT( comparedImpl != nullptr );
        delete comparedImpl;

        // The object of a derived class whose scope is removed is no object of
        // its base, whose impl it does not cast to.
        _kernel->remove_scope<regression4_derived_t>();

        const char finalSource[] = R"PY(
regression4_text = ''.join(regression4_capture.parts)
regression4_pending_reported_ok = 'regression4 pending' in regression4_text
regression4_warning_ok = regression4_text.count('regression warning') == 1
regression4_compare_ok = raises(RuntimeError, lambda: regression4_compared == None)
del regression4_compared
regression4_removed_ok = raises(Exception, regression4_read_base, regression4_derived)
del regression4_derived

class Regression4Keeper(object):
    pass

Regression4Keeper.getter = Regression4Base.__dict__['base'].fget
)PY";

        PyObject * finalResult = _kernel->exec_file( finalSource, dict, dict );
        PYBIND_CONTRACT_ASSERT( finalResult != nullptr );
        _kernel->decref( finalResult );

        const char * flags[] = {"regression4_derived_ok", "regression4_pick_ok", "regression4_unicode_ok", "regression4_message_ok", "regression4_kwargs_ok", "regression4_arity_ok", "regression4_pending_ok", "regression4_pending_reported_ok", "regression4_warning_ok", "regression4_compare_ok", "regression4_removed_ok"};

        for( const char * flag : flags )
        {
            PYBIND_CONTRACT_ASSERT( detail::regression_flag( _kernel, dict, flag ) == true );
        }

        // A C++ hook a class keeps past the kernel, as an accessor it keeps.
        PyObject * oldHook = _kernel->get_sys_excepthook();
        _kernel->incref( oldHook );
        _kernel->set_sys_excepthook_f( &detail::regression4_hook, nullptr );

        PyObject * keepResult = _kernel->exec_file( "Regression4Keeper.hook = sys.excepthook\n", dict, dict );
        PYBIND_CONTRACT_ASSERT( keepResult != nullptr );
        _kernel->decref( keepResult );

        _kernel->set_sys_excepthook( oldHook );
        _kernel->decref( oldHook );

#if defined(PYBIND_BACKEND_PYTHON)
        s_regression4_keeper = _kernel->dict_getstring( dict, "Regression4Keeper" );
        _kernel->incref( s_regression4_keeper );
#endif

        _kernel->remove_scope<regression4_compared_t>();
        _kernel->remove_scope<regression4_doubles_t>();
        _kernel->remove_scope<regression4_base_t>();
    }

    static bool s_regression5_iterated = false;
    static bool s_regression5_generated = false;
    static bool s_regression5_looked_up = false;
    static bool s_regression5_converted = false;
    static bool s_regression5_attributes = false;
    static bool s_regression5_imported = false;
    static bool s_regression5_import_error = false;

    struct regression5_entity_t
    {
        virtual ~regression5_entity_t()
        {
        }

        virtual const char * kind() const
        {
            return "entity";
        }
    };

    struct regression5_pad_t
    {
        virtual ~regression5_pad_t()
        {
        }

        int32_t pad = 0;
    };

    struct regression5_scene_t
        : public regression5_pad_t
        , public regression5_entity_t
    {
        const char * kind() const override
        {
            return "scene";
        }
    };

    struct regression5_vec_t
    {
        regression5_vec_t()
        {
        }

        regression5_vec_t( float _x )
            : x( _x )
        {
        }

        regression5_vec_t operator * ( float _factor ) const
        {
            return regression5_vec_t( x * _factor );
        }

        float x = 0.f;
    };

    struct regression5_scale_t
    {
        regression5_scale_t()
        {
        }

        regression5_scale_t( float _k )
            : k( _k )
        {
        }

        float k = 1.f;
    };

    // The vector knows nothing of the scale, which knows the vector in both
    // orders.
    static regression5_vec_t operator * ( const regression5_scale_t & _scale, const regression5_vec_t & _vec )
    {
        return regression5_vec_t( _scale.k * _vec.x );
    }

    static regression5_vec_t operator * ( const regression5_vec_t & _vec, const regression5_scale_t & _scale )
    {
        return regression5_vec_t( _vec.x * _scale.k * 10.f );
    }

    class regression5_thrower_t
    {
    public:
        regression5_thrower_t( int32_t _value )
        {
            if( _value < 0 )
            {
                pybind::throw_exception( "regression5 constructor" );
            }
        }

    public:
        int32_t method()
        {
            pybind::throw_exception( "regression5 method" );

            return 0;
        }

        int32_t add( PyObject * _other, bool _rotate )
        {
            (void)_other;
            (void)_rotate;

            pybind::throw_exception( "regression5 add" );

            return 0;
        }

        int32_t getValue() const
        {
            return 0;
        }

        void setValue( int32_t _value )
        {
            (void)_value;

            pybind::throw_exception( "regression5 setter" );
        }

        int32_t call()
        {
            // As a plugin that unloads its bindings from the call of one of them.
            pybind::kernel_interface * kernel = pybind::get_kernel();
            kernel->remove_scope<regression5_thrower_t>();

            pybind::throw_exception( "regression5 removed" );

            return 0;
        }
    };

    class regression5_erring_t
    {
    public:
        ~regression5_erring_t()
        {
            pybind::kernel_interface * kernel = pybind::get_kernel();
            kernel->set_error( pybind::error_type_e::Value, "regression5 left by destructor" );
        }
    };

    static const char * regression5_scene_kind( regression5_scene_t * _scene )
    {
        return _scene->kind();
    }

    static PyObject * regression5_utf8( pybind::kernel_interface * _kernel, PyObject * _unicode )
    {
        return _kernel->unicode_encode_utf8( _unicode );
    }

    static pybind::object regression5_string_size( pybind::kernel_interface * _kernel, PyObject * _string )
    {
        size_t size;
        const char * data = _kernel->string_to_char_and_size( _string, &size );

        if( data == nullptr )
        {
            return pybind::make_invalid_object_t();
        }

        return pybind::make_object_t( _kernel, size );
    }

    static pybind::object regression5_pickle( pybind::kernel_interface * _kernel, PyObject * _value, PyObject * _types )
    {
        size_t size = 0;

        if( pybind::pickle( _kernel, _value, _types, nullptr, 0, &size ) == false )
        {
            return pybind::make_invalid_object_t();
        }

        std::vector<char> buffer( size );
        size_t written = 0;

        if( pybind::pickle( _kernel, _value, _types, buffer.data(), size, &written ) == false )
        {
            return pybind::make_invalid_object_t();
        }

        PyObject * data = _kernel->string_from_char_size( buffer.data(), written );

        return pybind::object( _kernel, data, pybind::borrowed );
    }

    static pybind::object regression5_unpickle( pybind::kernel_interface * _kernel, PyObject * _data, PyObject * _types )
    {
        size_t size;
        const char * data = _kernel->string_to_char_and_size( _data, &size );
        PyObject * value = pybind::unpickle( _kernel, data, size, _types );

        if( value == nullptr )
        {
            return pybind::make_invalid_object_t();
        }

        return pybind::object( _kernel, value, pybind::borrowed );
    }

    static pybind::object regression5_pending_ops( pybind::kernel_interface * _kernel, PyObject * _iterable, PyObject * _generator, PyObject * _key, PyObject * _bool, PyObject * _object )
    {
        // An error raised before these operations stays the one Python sees, and
        // their Python code, which swallows errors of its own, runs without it.
        _kernel->set_error( pybind::error_type_e::Index, "regression5 pending" );

        PyObject * iterator;
        PyObject * item;

        if( _kernel->iterator_get( _iterable, &iterator ) == true )
        {
            int64_t sum = 0;

            while( _kernel->iterator_next( iterator, &item ) == true )
            {
                int64_t value = 0;
                _kernel->extract_int64( item, value );
                _kernel->decref( item );

                sum += value;
            }

            _kernel->iterator_end( iterator );

            s_regression5_iterated = sum == 3;
        }

        // The error of a step is reported, and the iteration ends.
        if( _kernel->iterator_get( _generator, &iterator ) == true )
        {
            uint32_t count = 0;

            while( _kernel->iterator_next( iterator, &item ) == true )
            {
                _kernel->decref( item );

                ++count;
            }

            _kernel->iterator_end( iterator );

            s_regression5_generated = count == 1;
        }

        PyObject * dict = _kernel->dict_new();
        PyObject * set = _kernel->set_new();
        s_regression5_looked_up = _kernel->dict_exist( dict, _key ) == false && _kernel->dict_get( dict, _key ) == nullptr && _kernel->set_exist( set, _key ) == false && _kernel->set_remove( set, _key ) == true;
        _kernel->decref( set );
        _kernel->decref( dict );

        bool value = false;
        s_regression5_converted = _kernel->extract_bool( _bool, value ) == true && value == true && _kernel->get_object_type_id( _bool ) == 0;

        PyObject * attribute = _kernel->get_attrstring( _object, "regression5_value" );
        s_regression5_attributes = attribute != nullptr && _kernel->set_attrstring( _object, "regression5_set", attribute ) == true && _kernel->test_equal( _object, _key ) == true;

        if( attribute != nullptr )
        {
            _kernel->decref( attribute );
        }

        return pybind::make_invalid_object_t();
    }

    static pybind::object regression5_pending_import( pybind::kernel_interface * _kernel )
    {
        // As the other operations: an import that runs Python code.
        _kernel->set_error( pybind::error_type_e::Index, "regression5 pending" );

        bool exists = false;
        PyObject * module = _kernel->module_import( "regression5_fresh", exists );

        if( module != nullptr )
        {
            PyObject * value = _kernel->get_attrstring( module, "value" );
            int32_t number = 0;
            s_regression5_imported = exists == true && value != nullptr && _kernel->extract_int32( value, number ) == true && number == 5;

            if( value != nullptr )
            {
                _kernel->decref( value );
            }

            _kernel->decref( module );
        }

        return pybind::make_invalid_object_t();
    }

    static void regression5_hook( void * _ud, PyTypeObject * _type, PyObject * _value, PyObject * _traceback )
    {
        (void)_value;
        (void)_traceback;

        pybind::kernel_interface * kernel = static_cast<pybind::kernel_interface *>(_ud);

        s_regression5_import_error = _type != nullptr && std::strstr( kernel->type_name( _type ), "ImportError" ) != nullptr;
    }

    static void check_fifth_review_regressions( pybind::kernel_interface * _kernel, PyObject * _module )
    {
        pybind::class_<regression5_entity_t>( _kernel, "Regression5Entity", true, _module )
            .def_constructor( pybind::init<>() )
            .def( "kind", &regression5_entity_t::kind )
            ;

        pybind::class_<regression5_scene_t, pybind::bases<regression5_entity_t>>( _kernel, "Regression5Scene", true, _module )
            .def_constructor( pybind::init<>() )
            ;

        pybind::struct_<regression5_vec_t>( _kernel, "Regression5Vec", true, _module )
            .def_constructor( pybind::init<float>() )
            .def_member( "x", &regression5_vec_t::x )
            .def_operator_mul_t<float>()
            ;

        pybind::struct_<regression5_scale_t>( _kernel, "Regression5Scale", true, _module )
            .def_constructor( pybind::init<float>() )
            .def_operator_mul_t<regression5_vec_t>()
            ;

        pybind::class_<regression5_thrower_t>( _kernel, "Regression5Thrower", true, _module )
            .def_constructor( pybind::init<int32_t>() )
            .def( "method", &regression5_thrower_t::method )
            .def_add( &regression5_thrower_t::add )
            .def_property( "value", &regression5_thrower_t::getValue, &regression5_thrower_t::setValue )
            .def_call( &regression5_thrower_t::call )
            ;

        pybind::class_<regression5_erring_t>( _kernel, "Regression5Erring", true, _module )
            .def_constructor( pybind::init<>() )
            ;

        pybind::def_function( _kernel, "regression5_scene_kind", &regression5_scene_kind );
        pybind::def_function_kernel( _kernel, "regression5_utf8", &regression5_utf8 );
        pybind::def_function_kernel( _kernel, "regression5_string_size", &regression5_string_size );
        pybind::def_function_kernel( _kernel, "regression5_pickle", &regression5_pickle );
        pybind::def_function_kernel( _kernel, "regression5_unpickle", &regression5_unpickle );
        pybind::def_function_kernel( _kernel, "regression5_pending_ops", &regression5_pending_ops );
        pybind::def_function_kernel( _kernel, "regression5_pending_import", &regression5_pending_import );

        PyObject * dict = _kernel->module_dict( _module );

        const char source[] = R"PY(
class Regression5Mixin(Regression5Entity):
    pass

class Regression5MyScene(Regression5Mixin, Regression5Scene):
    pass

regression5_scene = Regression5MyScene()
regression5_scene_ok = regression5_scene.kind() == 'scene' and raises(Exception, regression5_scene_kind, regression5_scene) == False
del regression5_scene

regression5_mul_ok = (Regression5Vec(3.0) * Regression5Scale(2.0)).x == 60.0 and (Regression5Scale(2.0) * Regression5Vec(3.0)).x == 6.0

def regression5_inplace():
    vec = Regression5Vec(3.0)
    vec *= Regression5Scale(2.0)
    return vec.x

regression5_inplace_ok = regression5_inplace() == 60.0

def regression5_message(expected, function, *args):
    try:
        function(*args)
    except RuntimeError as error:
        return str(error) == expected
    return False

def regression5_set_value(thrower):
    thrower.value = 1

regression5_thrower = Regression5Thrower(1)
regression5_messages_ok = regression5_message('regression5 constructor', Regression5Thrower, -1) and regression5_message('regression5 method', regression5_thrower.method) and regression5_message('regression5 add', lambda: regression5_thrower + 1) and regression5_message('regression5 setter', regression5_set_value, regression5_thrower)
regression5_removed_ok = regression5_message('regression5 removed', regression5_thrower)
del regression5_thrower

regression5_copy_ok = raises(TypeError, Regression5Entity().__reduce_ex__, 2) and raises(TypeError, Regression5Vec(1.0).__reduce__)
regression5_utf8_ok = regression5_utf8(u'a\ud83d\ude00b') == 'a\xf0\x9f\x98\x80b'
regression5_string_ok = regression5_string_size('abc') == 3 and raises(TypeError, regression5_string_size, u'abc')

def regression5_delete_last():
    pod = PodValue()
    del pod[-1]

regression5_delete_ok = raises(TypeError, regression5_delete_last)

class Regression5Holder(object):
    pass

regression5_holder = Regression5Holder()

class Regression5Shrinking(object):
    @property
    def value(self):
        del regression5_holder.items[:]
        return 1

regression5_holder.items = [Regression5Shrinking(), 2, 3]
regression5_shrink_ok = raises(RuntimeError, regression5_pickle, regression5_holder.items, [Regression5Shrinking])

class Regression5Typed(object):
    def __init__(self, value):
        self.value = value

regression5_data = regression5_pickle(Regression5Typed(Regression5Typed(5)), [Regression5Typed])

def regression5_init(self, value):
    self.value = value
    del regression5_types[:]

regression5_types = [type('Regression5Shrink', (object,), {'__init__': regression5_init})]
regression5_loaded = regression5_unpickle(regression5_data, regression5_types)
regression5_unpickle_ok = type(regression5_loaded).__name__ == 'Regression5Shrink' and regression5_loaded.value.value == 5

class Regression5Iterator(object):
    def __init__(self):
        self.count = 0

    def __iter__(self):
        getattr(self, 'regression5_missing', None)
        return self

    def next(self):
        self.count += 1
        if self.count > 2:
            raise StopIteration
        return getattr(self, 'regression5_missing', self.count)

def regression5_generator():
    yield 1
    raise KeyError('regression5 generator')

class Regression5Key(object):
    def __hash__(self):
        return getattr(self, 'regression5_missing', 7)

class Regression5Bool(object):
    def __nonzero__(self):
        return getattr(self, 'regression5_missing', True)

class Regression5Object(object):
    def __eq__(self, other):
        return getattr(type(self), 'regression5_missing', True)

    def __getattr__(self, name):
        return getattr(type(self), 'regression5_missing', 5)

    def __setattr__(self, name, value):
        getattr(type(self), 'regression5_missing', None)
        object.__setattr__(self, name, value)

class Regression5Finder(object):
    def find_module(self, name, path=None):
        if name == 'regression5_fresh':
            return self
        return None

    def load_module(self, name):
        module = type(sys)(name)
        module.__dict__['__builtins__'] = __builtins__
        sys.modules[name] = module
        exec 'value = 5\n' in module.__dict__
        return module

regression5_finder = Regression5Finder()
regression5_capture = Capture()

def regression5_release():
    erring = Regression5Erring()
    del erring
    return 'returned'
)PY";

        PyObject * result = _kernel->exec_file( source, dict, dict );
        PYBIND_CONTRACT_ASSERT( result != nullptr );
        _kernel->decref( result );

        // The errors these report go to a capture.
        PyObject * capture = _kernel->dict_getstring( dict, "regression5_capture" );
        PyObject * oldStderr = _kernel->getStdErrorHandle();
        _kernel->incref( oldStderr );
        _kernel->setStdErrorHandle( capture );

        PyObject * finder = _kernel->dict_getstring( dict, "regression5_finder" );
        _kernel->set_module_finder( finder );

        const char pendingSource[] = R"PY(
regression5_pending_ok = raises(IndexError, regression5_pending_ops, Regression5Iterator(), regression5_generator(), Regression5Key(), Regression5Bool(), Regression5Object())
regression5_import_ok = raises(IndexError, regression5_pending_import)
regression5_release_ok = regression5_release() == 'returned'
)PY";

        PyObject * pendingResult = _kernel->exec_file( pendingSource, dict, dict );
        PYBIND_CONTRACT_ASSERT( pendingResult != nullptr );
        _kernel->decref( pendingResult );

        _kernel->remove_module_finder();

        _kernel->setStdErrorHandle( oldStderr );
        _kernel->decref( oldStderr );

        PYBIND_CONTRACT_ASSERT( s_regression5_iterated == true );
        PYBIND_CONTRACT_ASSERT( s_regression5_generated == true );
        PYBIND_CONTRACT_ASSERT( s_regression5_looked_up == true );
        PYBIND_CONTRACT_ASSERT( s_regression5_converted == true );
        PYBIND_CONTRACT_ASSERT( s_regression5_attributes == true );
        PYBIND_CONTRACT_ASSERT( s_regression5_imported == true );

        // A module whose code removes it from sys.modules fails to import.
        PyObject * oldHook = _kernel->get_sys_excepthook();
        _kernel->incref( oldHook );
        _kernel->set_sys_excepthook_f( &detail::regression5_hook, _kernel );

        PyObject * goneCode = _kernel->compile_string( "import sys\ndel sys.modules[__name__]\n", "regression5_gone.py" );
        PYBIND_CONTRACT_ASSERT( goneCode != nullptr );
        PYBIND_CONTRACT_ASSERT( _kernel->module_execcode( "regression5_gone", goneCode ) == nullptr );
        PYBIND_CONTRACT_ASSERT( s_regression5_import_error == true );
        _kernel->decref( goneCode );

        _kernel->set_sys_excepthook( oldHook );
        _kernel->decref( oldHook );

        // A name a finalizer removes as its module is cleared stays removed.
        PyObject * finiModule = _kernel->module_init( "regression5_fini" );
        _kernel->incref( finiModule );
        PyObject * finiDict = _kernel->module_dict( finiModule );

        const char finiSource[] = R"PY(
regression5_globals = globals()

class Regression5Finalized(object):
    def __del__(self):
        regression5_globals.pop('regression5_popped', None)

regression5_finalized = Regression5Finalized()
regression5_popped = 3
)PY";

        PyObject * finiResult = _kernel->exec_file( finiSource, finiDict, finiDict );
        PYBIND_CONTRACT_ASSERT( finiResult != nullptr );
        _kernel->decref( finiResult );

        _kernel->module_fini( finiModule );
        PYBIND_CONTRACT_ASSERT( _kernel->dict_existstring( finiDict, "regression5_popped" ) == false );
        _kernel->decref( finiModule );

        // A member a scope does not have is not found, as a method.
        const pybind::class_type_scope_interface_ptr & entityScope = _kernel->get_class_type_scope_t<regression5_entity_t>();
        bool memberThrown = false;

        try
        {
            entityScope->find_member( "regression5_missing" );
        }
        catch( const pybind::pybind_exception & )
        {
            memberThrown = true;
        }

        PYBIND_CONTRACT_ASSERT( memberThrown == true );

        // The null container of a failed call is empty, and takes nothing.
        pybind::list nullList( _kernel, nullptr, pybind::borrowed );
        pybind::tuple nullTuple( _kernel, nullptr, pybind::borrowed );
        pybind::dict nullDict( _kernel, nullptr, pybind::borrowed );
        pybind::set nullSet( _kernel, nullptr, pybind::borrowed );
        pybind::module nullModule( _kernel, nullptr, pybind::borrowed );
        PYBIND_CONTRACT_ASSERT( nullList.size() == 0 && nullList.empty() == true && nullList.begin() == nullList.end() );
        PYBIND_CONTRACT_ASSERT( nullTuple.size() == 0 && nullTuple.begin() == nullTuple.end() );
        PYBIND_CONTRACT_ASSERT( nullDict.size() == 0 && nullDict.exist( "regression5" ) == false && nullDict.begin() == nullDict.end() );
        PYBIND_CONTRACT_ASSERT( nullSet.size() == 0 && nullSet.exist( 1 ) == false );
        PYBIND_CONTRACT_ASSERT( nullModule.has_attr( "regression5" ) == false );
        nullList.append( 1 );
        nullDict["regression5"] = 1;
        nullSet.add( 1 );
        PYBIND_CONTRACT_ASSERT( nullList.size() == 0 && nullDict.size() == 0 && nullSet.size() == 0 );

        // An item is assigned from another, and a list from nullptr.
        pybind::list items( _kernel );
        items.append( 1 );
        items.append( 2 );
        items[1] = items[0];
        PYBIND_CONTRACT_ASSERT( pybind::extract<int32_t>( _kernel, _kernel->list_getitem( items.ptr(), 1 ) ) == 1 );
        items = nullptr;
        PYBIND_CONTRACT_ASSERT( items.is_invalid() == true );

        pybind::dict values( _kernel );
        values["a"] = 1;
        values["b"] = values["a"];
        PYBIND_CONTRACT_ASSERT( pybind::extract<int32_t>( _kernel, _kernel->dict_getstring( values.ptr(), "b" ) ) == 1 );

        const char finalSource[] = R"PY(
regression5_text = ''.join(regression5_capture.parts)
regression5_reported_ok = 'regression5 generator' in regression5_text and 'regression5 left by destructor' in regression5_text
)PY";

        PyObject * finalResult = _kernel->exec_file( finalSource, dict, dict );
        PYBIND_CONTRACT_ASSERT( finalResult != nullptr );
        _kernel->decref( finalResult );

        const char * flags[] = {"regression5_scene_ok", "regression5_mul_ok", "regression5_inplace_ok", "regression5_messages_ok", "regression5_removed_ok", "regression5_copy_ok", "regression5_utf8_ok", "regression5_string_ok", "regression5_delete_ok", "regression5_shrink_ok", "regression5_unpickle_ok", "regression5_pending_ok", "regression5_import_ok", "regression5_release_ok", "regression5_reported_ok"};

        for( const char * flag : flags )
        {
            PYBIND_CONTRACT_ASSERT( detail::regression_flag( _kernel, dict, flag ) == true );
        }

        // The thrower removed its own scope.
        _kernel->remove_scope<regression5_erring_t>();
        _kernel->remove_scope<regression5_scale_t>();
        _kernel->remove_scope<regression5_vec_t>();
        _kernel->remove_scope<regression5_scene_t>();
        _kernel->remove_scope<regression5_entity_t>();
    }

    static void check_kernel_reinitialize( pybind::allocator_interface * _allocator, const pybind::kernel_config_t & _config )
    {
        // A kernel initialized again works, and the C++ destructor of an object
        // the interpreter releases as it is destroyed may wrap a bound object
        // and call Python.
        pybind::kernel_interface * kernel = pybind::initialize( _allocator, _config );
        PYBIND_CONTRACT_ASSERT( kernel != nullptr );

        PyObject * module = kernel->module_init( "contract_again" );
        kernel->incref( module );
        kernel->set_current_module( module );

        pybind::class_<regression3_target_t>( kernel, "Regression3Target", true, module )
            .def_member( "value", &regression3_target_t::value )
            ;

        pybind::class_<regression3_owner_t>( kernel, "Regression3Owner", true, module )
            .def_constructor( pybind::init<>() )
            .def( "setCallback", &regression3_owner_t::setCallback )
            ;

        PyObject * dict = kernel->module_dict( module );
        kernel->dict_setstring( dict, "__builtins__", kernel->get_builtins() );

        // The kernel holds the module it defines into, which Python removes.
        PyObject * transient = kernel->module_init( "contract_transient" );
        kernel->set_current_module( transient );
        PyObject * removeResult = kernel->exec_file( "import sys\ndel sys.modules['contract_transient']\n", dict, dict );
        PYBIND_CONTRACT_ASSERT( removeResult != nullptr );
        kernel->decref( removeResult );
        pybind::def_function( kernel, "regression4_after_removal", &regression_size );
        kernel->set_current_module( module );

#if defined(PYBIND_BACKEND_PYTHON)
        // A C++ hook and a member accessor the first kernel left outlive it,
        // and are released by this one.
        PyObject * getter = kernel->get_attrstring( s_regression4_keeper, "getter" );
        PYBIND_CONTRACT_ASSERT( getter != nullptr );

        {
            pybind::string_view getterRepr = kernel->object_repr( getter );
            PYBIND_CONTRACT_ASSERT( std::strstr( getterRepr.c_str(), "getmethod" ) != nullptr );
        }

        kernel->decref( getter );

        kernel->decref( s_regression4_keeper );
        s_regression4_keeper = nullptr;
        kernel->collect();
#endif

        const char source[] = "regression3_seen = []\ndef regression3_notify(target):\n    regression3_seen.append(target.value)\nregression3_owner = Regression3Owner()\nregression3_owner.setCallback(regression3_notify)\n";
        PyObject * result = kernel->exec_file( source, dict, dict );
        PYBIND_CONTRACT_ASSERT( result != nullptr );
        kernel->decref( result );

        kernel->set_current_module( nullptr );
        kernel->decref( module );
        kernel->destroy();

#if defined(PYBIND_BACKEND_TINYPY)
        // CPython runs no bound destructor at Py_Finalize.
        PYBIND_CONTRACT_ASSERT( s_regression3_owner_destroys == 1 );
        PYBIND_CONTRACT_ASSERT( s_regression3_owner_notified == 1 );
#endif
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
    pybind::typeid_t secondaryId = kernel->class_info<detail::cast_secondary_t>();
    void * registeredSecondary = kernel->check_registred_class( castObject, secondaryId );
    PYBIND_CONTRACT_ASSERT( registeredSecondary == castSecondary );
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
    detail::check_third_review_regressions( kernel, module );
    detail::check_fourth_review_regressions( kernel, module );
    detail::check_fifth_review_regressions( kernel, module );

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

    detail::check_kernel_reinitialize( &allocator, config );
#if defined(PYBIND_BACKEND_TINYPY)
    size_t reinitializedAllocations = allocator.allocations();
    PYBIND_CONTRACT_ASSERT( reinitializedAllocations == 0 );
#endif
    return 0;
}
