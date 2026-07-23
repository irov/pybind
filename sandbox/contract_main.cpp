#include "pybind/kernel_interface.hpp"
#include "pybind/extract.hpp"
#include "pybind/function_interface.hpp"
#include "pybind/adapter/new_adapter.hpp"
#include "pybind/adapter/destroy_adapter.hpp"
#include "pybind/adapter/smart_pointer_adapter.hpp"
#include "pybind/class_type_scope_interface.hpp"
#include "pybind/object.hpp"
#include "pybind/dict.hpp"
#include "pybind/module.hpp"
#include "pybind/generator/interface_.hpp"
#include "pybind/generator/struct_.hpp"

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cwchar>

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
            static const char source[] = "value = 42\n";
            const char * name = _kernel->string_to_char( _module );
            PyObject * code = _kernel->compile_string( source, "finder/sample.py" );

            if( code == nullptr ) return nullptr;
            PyObject * initialized = _kernel->module_init( name );
            PyObject * loaded = _kernel->module_execcode( name, code );
            _kernel->decref( initialized );
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

    struct string_like_name_t
    {
        const char * c_str() const
        {
            return value;
        }

        const char * value;
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

    static const char * repr_pod_value( pod_value_t * _value )
    {
        (void)_value;
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
}
int main()
{
    detail::test_allocator allocator;
    pybind::kernel_config_t config;
    config.path = L"";
    config.debug = false;
    config.install_signals = false;
    config.no_site = true;
    pybind::kernel_interface * kernel = pybind::initialize( &allocator, config );
    PYBIND_CONTRACT_ASSERT( kernel != nullptr );

    pybind::object invalidObject = pybind::detail::extract_operator_t( kernel, nullptr, pybind::borrowed );
    PYBIND_CONTRACT_ASSERT( invalidObject.is_invalid() == true );
    PYBIND_CONTRACT_ASSERT( kernel->object_str( nullptr ).is_invalid() == true );
    PYBIND_CONTRACT_ASSERT( kernel->object_repr( nullptr ).is_invalid() == true );
    PYBIND_CONTRACT_ASSERT( kernel->object_repr_type( nullptr ).is_invalid() == true );
    PyObject * module = kernel->module_init( "contract" );
    kernel->set_current_module( module );

    {
        const detail::string_like_name_t key = {"string_like_key"};
        const detail::string_like_name_t missing = {"string_like_missing"};
        pybind::dict values( kernel );
        values.set( key, 41 );
        const pybind::dict & constValues = values;
        int32_t value = constValues[key];
        PYBIND_CONTRACT_ASSERT( value == 41 );
        PYBIND_CONTRACT_ASSERT( values.exist( key ) == true );
        PYBIND_CONTRACT_ASSERT( values.get_default( missing, int32_t( -1 ) ) == -1 );
        values.remove( key );
        PYBIND_CONTRACT_ASSERT( values.exist( key ) == false );
        PYBIND_CONTRACT_ASSERT( pybind::dict_set_t( kernel, values.ptr(), key, 42 ) == true );
        int32_t helperValue = pybind::dict_get_t( kernel, values.ptr(), key );
        PYBIND_CONTRACT_ASSERT( helperValue == 42 );
        PYBIND_CONTRACT_ASSERT( pybind::dict_remove_t( kernel, values.ptr(), key ) == true );

        const detail::string_like_name_t attribute = {"string_like_attribute"};
        pybind::object moduleObject( kernel, module );
        moduleObject.set_attr( attribute, 43 );
        PYBIND_CONTRACT_ASSERT( moduleObject.has_attr( attribute ) == true );
        int32_t objectAttribute = moduleObject.get_attr( attribute ).extract();
        pybind::module moduleWrapper( kernel, module );
        int32_t moduleAttribute = moduleWrapper.get_attr( attribute );
        PYBIND_CONTRACT_ASSERT( objectAttribute == 43 );
        PYBIND_CONTRACT_ASSERT( moduleWrapper.has_attr( attribute ) == true );
        PYBIND_CONTRACT_ASSERT( moduleAttribute == 43 );
    }

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

    pybind::struct_<detail::pod_value_t>( kernel, "PodValue", true, module )
        .def_constructor( pybind::init<>() )
        .def_call( &detail::pod_value_t::call )
        .def_repr( &detail::repr_pod_value )
        .def_hash( &detail::hash_pod_value )
        .def_compare_equal()
        .def_convert( &detail::convert_pod_value )
        .def_operator_getset()
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
    const char podSequenceSource[] =
        "pod_sequence_get_ok = pod_object[0] == 37\n"
        "pod_object[0] = 41\n"
        "pod_sequence_set_ok = pod_object == 41\n"
        "pod_object[0] = 37\n";
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
        "pod_contract_ok = pod_repr_ok and pod_hash_ok and pod_call_ok and pod_sequence_get_ok and pod_sequence_set_ok and pod_compare_ok and pod_unknown_compare_ok and pod_number_ok and pod_unknown_number_ok and pod_unary_ok and pod_inplace_ok\n";
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
    PYBIND_CONTRACT_ASSERT( pybind::extract<bool>( kernel, kernel->dict_getstring( moduleDict, "pod_sequence_get_ok" ) ) == true );
    PYBIND_CONTRACT_ASSERT( pybind::extract<bool>( kernel, kernel->dict_getstring( moduleDict, "pod_sequence_set_ok" ) ) == true );
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
        "scalar_contract_ok = scalar_mul_float.value == 30.0 and scalar_mul_int.value == 24.0 and scalar_div_float.value == 8.0 and scalar_div_int.value == 4.0 and scalar_value.value == 10.0\n";
    PyObject * scalarExec = kernel->exec_file( scalarSource, moduleDict, moduleDict );
    PYBIND_CONTRACT_ASSERT( scalarExec != nullptr );
    kernel->decref( scalarExec );
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
