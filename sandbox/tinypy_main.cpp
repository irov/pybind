#include "pybind/kernel_interface.hpp"
#include "pybind/extract.hpp"
#include "pybind/function_interface.hpp"
#include "pybind/adapter/new_adapter.hpp"
#include "pybind/adapter/destroy_adapter.hpp"
#include "pybind/class_type_scope_interface.hpp"
#include "pybind/object.hpp"
#include "pybind/generator/interface_.hpp"
#include "pybind/generator/struct_.hpp"

#include <cassert>
#include <cstdlib>
#include <cstring>
#include <cwchar>

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
                assert( kwargsSize == 0 );
            }

            int32_t left;
            int32_t right;
            PyObject * leftObject = _kernel->tuple_getitem( _args, 0 );
            PyObject * rightObject = _kernel->tuple_getitem( _args, 1 );
            bool leftResult = _kernel->extract_int32( leftObject, left );
            bool rightResult = _kernel->extract_int32( rightObject, right );
            assert( leftResult == true );
            assert( rightResult == true );

            return _kernel->ptr_int32( left + right );
        }
    };

    struct exception_capture_t
    {
        pybind::kernel_interface * kernel = nullptr;
        bool called = false;
        uint32_t line = 0;
    };

    static void capture_exception( void * _userData, PyTypeObject * _type, PyObject * _value, PyObject * _traceback )
    {
        exception_capture_t * capture = static_cast<exception_capture_t *>( _userData );

        assert( _type != nullptr );
        assert( _value != nullptr );
        assert( _traceback != nullptr );
        assert( capture->kernel->traceback_check( _traceback ) == true );

        capture->called = true;
        capture->line = capture->kernel->traceback_lineno( _traceback );
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
            assert( m_refcount != 0 );

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
            assert( _index == 0 );
            return value;
        }

        int32_t value;
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
    assert( kernel != nullptr );

    pybind::object invalidObject = pybind::detail::extract_operator_t( kernel, nullptr, pybind::borrowed );
    assert( invalidObject.is_invalid() == true );
    assert( kernel->object_str( nullptr ).is_invalid() == true );
    assert( kernel->object_repr( nullptr ).is_invalid() == true );
    assert( kernel->object_repr_type( nullptr ).is_invalid() == true );
    PyObject * module = kernel->module_init( "contract" );
    kernel->set_current_module( module );

    PyObject * unicodeValue = kernel->unicode_from_wchar( L"caf\u00e9" );
    PyObject * unicodeUtf8 = kernel->unicode_encode_utf8( unicodeValue );
    size_t unicodeUtf8Size;
    const char * unicodeUtf8Data = kernel->string_to_char_and_size( unicodeUtf8, &unicodeUtf8Size );
    assert( unicodeUtf8Size == 5 );
    assert( std::memcmp( unicodeUtf8Data, "caf\xc3\xa9", unicodeUtf8Size ) == 0 );
    size_t unicodeWcharSize;
    const wchar_t * unicodeWcharData = kernel->unicode_to_wchar_and_size( unicodeValue, &unicodeWcharSize );
    assert( unicodeWcharSize == 4 );
    assert( std::wmemcmp( unicodeWcharData, L"caf\u00e9", unicodeWcharSize ) == 0 );
    kernel->decref( unicodeUtf8 );
    kernel->decref( unicodeValue );

    PyObject * oldExcepthook = kernel->get_sys_excepthook();
    kernel->incref( oldExcepthook );
    detail::exception_capture_t exceptionCapture;
    exceptionCapture.kernel = kernel;
    kernel->set_sys_excepthook_f( &detail::capture_exception, &exceptionCapture );
    PyObject * moduleDict = kernel->module_dict( module );
    const char exceptionSource[] = "def fail():\n    return 1 + 'invalid'\n";
    PyObject * exceptionModuleResult = kernel->exec_file( exceptionSource, moduleDict, moduleDict );
    assert( exceptionModuleResult != nullptr );
    kernel->decref( exceptionModuleResult );
    PyObject * fail = kernel->get_attrstring( module, "fail" );
    PyObject * failArgs = kernel->tuple_new( 0 );
    PyObject * failResult = kernel->ask_native( fail, failArgs );
    assert( failResult == nullptr );
    assert( exceptionCapture.called == true );
    assert( exceptionCapture.line == 2 );
    kernel->decref( failArgs );
    kernel->decref( fail );
    kernel->set_sys_excepthook( oldExcepthook );
    kernel->decref( oldExcepthook );
#if defined(PYBIND_CONTRACT_TINYPY)
    pybind::interface_<detail::test_module_finder>( kernel, "TestModuleFinder", true )
        .def_kernel( "find_module", &detail::test_module_finder::find_module )
        .def_kernel( "load_module", &detail::test_module_finder::load_module )
        ;
    assert( kernel->has_attrstring( module, "TestModuleFinder" ) == true );
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
    assert( sumResult == true );
    assert( sumValue == 42 );
    kernel->decref( sum );
    kernel->decref( args );
    kernel->decref( right );
    kernel->decref( left );
    kernel->decref( add );

    pybind::interface_<detail::smart_pointer_base_t>( kernel, "SmartPointerBase", true, module )
        .def_smart_pointer()
        .def_bindable()
        ;
    pybind::interface_<detail::smart_pointer_derived_t, pybind::bases<detail::smart_pointer_base_t>>( kernel, "SmartPointerDerived", true, module )
        .def( "set_value", &detail::smart_pointer_derived_t::set_value )
        .def( "get_value", &detail::smart_pointer_derived_t::get_value )
        ;
    const pybind::class_type_scope_interface_ptr & smartDerivedScope = kernel->get_class_type_scope_t<detail::smart_pointer_derived_t>();
    assert( smartDerivedScope->get_bindable() != nullptr );
    stdex::intrusive_ptr<detail::smart_pointer_derived_t> smartPointer( new detail::smart_pointer_derived_t );
    assert( detail::smart_pointer_base_t::alive() == 1 );
    PyObject * smartObject = pybind::ptr_throw( kernel, smartPointer );
    assert( smartObject != nullptr );
    const pybind::class_type_scope_interface_ptr & smartObjectScope = kernel->get_class_scope( kernel->get_object_type( smartObject ) );
    assert( smartObjectScope.get() == smartDerivedScope.get() );
    smartPointer = nullptr;
    assert( detail::smart_pointer_base_t::alive() == 1 );
    PyObject * setValue = kernel->get_attrstring( smartObject, "set_value" );
    PyObject * setValueArgs = kernel->tuple_new( 1 );
    PyObject * setValueArgument = kernel->ptr_int32( 91 );
    kernel->tuple_setitem( setValueArgs, 0, setValueArgument );
    PyObject * setValueResult = kernel->ask_native( setValue, setValueArgs );
    assert( setValueResult != nullptr );
    kernel->decref( setValueResult );
    kernel->decref( setValueArgument );
    kernel->decref( setValueArgs );
    kernel->decref( setValue );
    PyObject * getValue = kernel->get_attrstring( smartObject, "get_value" );
    PyObject * getValueArgs = kernel->tuple_new( 0 );
    PyObject * getValueResult = kernel->ask_native( getValue, getValueArgs );
    int32_t smartValue;
    bool smartResult = kernel->extract_int32( getValueResult, smartValue );
    assert( smartResult == true && smartValue == 91 );
    kernel->decref( getValueResult );
    kernel->decref( getValueArgs );
    kernel->decref( getValue );
    kernel->decref( smartObject );
    assert( detail::smart_pointer_base_t::alive() == 0 );

    pybind::interface_<detail::cast_primary_t>( kernel, "CastPrimary", true, module );
    pybind::interface_<detail::cast_secondary_t>( kernel, "CastSecondary", true, module )
        .def( "set_secondary", &detail::cast_secondary_t::set_secondary )
        ;
    pybind::interface_<detail::cast_middle_t, pybind::bases<detail::cast_primary_t, detail::cast_secondary_t>>( kernel, "CastMiddle", true, module );
    pybind::interface_<detail::cast_leaf_t, pybind::bases<detail::cast_middle_t>>( kernel, "CastLeaf", true, module );
    detail::cast_leaf_t castLeaf;
    detail::cast_secondary_t * castSecondary = static_cast<detail::cast_secondary_t *>( &castLeaf );
    assert( static_cast<void *>( &castLeaf ) != static_cast<void *>( castSecondary ) );
    PyObject * castObject = pybind::ptr_throw( kernel, &castLeaf );
    PyObject * setSecondary = kernel->get_attrstring( castObject, "set_secondary" );
    PyObject * setSecondaryArgs = kernel->tuple_new( 1 );
    PyObject * setSecondaryArgument = kernel->ptr_int32( 83 );
    kernel->tuple_setitem( setSecondaryArgs, 0, setSecondaryArgument );
    PyObject * setSecondaryResult = kernel->ask_native( setSecondary, setSecondaryArgs );
    assert( setSecondaryResult != nullptr );
    assert( castLeaf.marker == 17 );
    assert( castLeaf.value == 83 );
    detail::cast_secondary_t * extractedSecondary = pybind::extract<detail::cast_secondary_t *>( kernel, castObject );
    assert( extractedSecondary == castSecondary );
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
    assert( podObject != nullptr );
    detail::pod_value_t * podValue = static_cast<detail::pod_value_t *>( kernel->get_class_impl( podObject ) );
    assert( podValue != nullptr && podValue->value == 37 );
    kernel->dict_setstring( moduleDict, "pod_object", podObject );

    PyObject * podReprExec = kernel->exec_file( "pod_repr_ok = repr(pod_object) == 'pod-37'\n", moduleDict, moduleDict );
    assert( podReprExec != nullptr );
    kernel->decref( podReprExec );
    PyObject * podHashExec = kernel->exec_file( "pod_hash_ok = hash(pod_object) == 37\n", moduleDict, moduleDict );
    assert( podHashExec != nullptr );
    kernel->decref( podHashExec );
    PyObject * podCallExec = kernel->exec_file( "pod_call_ok = pod_object(5) == 42\n", moduleDict, moduleDict );
    assert( podCallExec != nullptr );
    kernel->decref( podCallExec );
    const char podSequenceSource[] =
        "pod_sequence_get_ok = pod_object[0] == 37\n"
        "pod_object[0] = 41\n"
        "pod_sequence_set_ok = pod_object == 41\n"
        "pod_object[0] = 37\n";
    PyObject * podSequenceExec = kernel->exec_file( podSequenceSource, moduleDict, moduleDict );
    assert( podSequenceExec != nullptr );
    kernel->decref( podSequenceExec );
    const char podNumberSource[] =
        "pod_compare_ok = pod_object == 37 and 37 == pod_object and pod_object != 38 and 38 != pod_object\n"
        "pod_number_ok = pod_object + 5 == 42 and 5 + pod_object == 42 and pod_object - 5 == 32 and 5 - pod_object == -32 and pod_object * 2 == 74 and 2 * pod_object == 74 and pod_object / 2 == 18 and 74 / pod_object == 2\n"
        "pod_unary_ok = -pod_object == -37 and abs(pod_object) == 37\n";
    PyObject * podNumberExec = kernel->exec_file( podNumberSource, moduleDict, moduleDict );
    assert( podNumberExec != nullptr );
    kernel->decref( podNumberExec );
    const char podInplaceSource[] =
        "pod_inplace = PodValue()\n"
        "pod_inplace_identity = pod_inplace\n"
        "pod_inplace += 5\n"
        "pod_inplace -= 2\n"
        "pod_inplace *= 2\n"
        "pod_inplace /= 4\n"
        "pod_inplace_ok = pod_inplace is pod_inplace_identity and pod_inplace == 20\n"
        "pod_contract_ok = pod_repr_ok and pod_hash_ok and pod_call_ok and pod_sequence_get_ok and pod_sequence_set_ok and pod_compare_ok and pod_number_ok and pod_unary_ok and pod_inplace_ok\n";
    PyObject * podInplaceExec = kernel->exec_file( podInplaceSource, moduleDict, moduleDict );
    assert( podInplaceExec != nullptr );
    kernel->decref( podInplaceExec );
    PyObject * podContract = kernel->get_attrstring( module, "pod_contract_ok" );
    bool podContractValue;
    bool podContractResult = kernel->extract_bool( podContract, podContractValue );
    assert( podContractResult == true && podContractValue == true );
    kernel->decref( podContract );
    kernel->dict_removestring( moduleDict, "pod_object" );
    kernel->decref( podObject );

    const pybind::class_type_scope_interface_ptr & podScope = kernel->get_class_type_scope_t<detail::pod_value_t>();
    void * podStorage = nullptr;
    PyObject * wrappedPodObject = podScope->create_pod( &podStorage );
    assert( wrappedPodObject != nullptr );
    assert( podStorage != nullptr );
    new (podStorage) detail::pod_value_t();
    kernel->decref( wrappedPodObject );

    kernel->decref( podArgs );
    kernel->decref( podType );

    PyObject * globals = kernel->dict_new();
    PyObject * result = kernel->eval_string( "6 * 7", globals, globals );
    bool evalResult = kernel->extract_int32( result, sumValue );
    assert( evalResult == true );
    assert( sumValue == 42 );
    kernel->decref( result );
    kernel->decref( globals );

#if defined(PYBIND_CONTRACT_TINYPY)
    bool moduleExisted;
    PyObject * sample = kernel->module_import( "sample", moduleExisted );
    assert( sample != nullptr && moduleExisted == true );
    PyObject * sampleValue = kernel->get_attrstring( sample, "value" );
    bool sampleResult = kernel->extract_int32( sampleValue, sumValue );
    assert( sampleResult == true && sumValue == 42 );
    kernel->decref( sampleValue );

    PyObject * reloadedSample = kernel->module_reload( sample );
    assert( reloadedSample == sample );
    kernel->decref( reloadedSample );
    kernel->decref( sample );
    size_t finderLoads = finder.loads();
    assert( finderLoads == 2 );
#endif

    pybind::typeid_t typeId = kernel->get_class_type_id( typeid(detail::native_value_t) );
    pybind::new_adapter_interface_ptr nativeNew = allocator.newT<detail::native_new_adapter>();
    pybind::destroy_adapter_interface_ptr nativeDestroy = allocator.newT<detail::native_destroy_adapter>();
    pybind::class_type_scope_interface_ptr scope = kernel->create_new_type_scope( typeId, "NativeValue", nullptr, nativeNew, nativeDestroy, 0, false );
    bool scopeInitialized = scope->initialize( module );
    assert( scopeInitialized == true );
    assert( kernel->has_attrstring( module, "NativeValue" ) == true );
    PyObject * constructorValue = kernel->ptr_int32( 73 );
    PyObject * constructorArgs = kernel->tuple_new( 1 );
    kernel->tuple_setitem( constructorArgs, 0, constructorValue );
    PyTypeObject * scopeType = scope->get_typeobject();
    PyObject * scopeObject = reinterpret_cast<PyObject *>( scopeType );
    PyObject * instance = kernel->ask_native( scopeObject, constructorArgs );
    assert( instance != nullptr );
    void * nativeImpl = kernel->get_class_impl( instance );
    detail::native_value_t * native = static_cast<detail::native_value_t *>( nativeImpl );
    assert( native != nullptr && native->value == 73 );
    kernel->decref( instance );

    kernel->remove_from_module( "missing_contract_name", module );
    kernel->remove_from_module( "missing_contract_name", module );
    PyObject * builtins = kernel->get_builtins();
    PyObject * builtinsDict = kernel->module_dict( builtins );
    assert( builtinsDict != nullptr );
    PyObject * trueValue = kernel->get_true();
    kernel->dict_setstring( builtinsDict, "contract_marker", trueValue );
    PyObject * contractMarker = kernel->dict_getstring( builtinsDict, "contract_marker" );
    assert( contractMarker == trueValue );
    kernel->dict_setstring( moduleDict, "__builtins__", builtins );
    const char subclassSource[] = "class PythonBase(object):\n    pass\nclass Derived(NativeValue, PythonBase):\n    pass\n";
    PyObject * subclassResult = kernel->exec_file( subclassSource, moduleDict, moduleDict );
    assert( subclassResult != nullptr );
    kernel->decref( subclassResult );
    PyObject * derived = kernel->dict_getstring( moduleDict, "Derived" );
    assert( derived != nullptr );
    PyObject * derivedInstance = kernel->ask_native( derived, constructorArgs );
    assert( derivedInstance != nullptr );
    void * derivedImpl = kernel->get_class_impl( derivedInstance );
    detail::native_value_t * derivedNative = static_cast<detail::native_value_t *>( derivedImpl );
    assert( derivedNative != nullptr && derivedNative->value == 73 );
    kernel->decref( derivedInstance );
    PyObject * retiredInstance = kernel->ask_native( scopeObject, constructorArgs );
    assert( retiredInstance != nullptr );
    kernel->remove_type_scope( typeId );
    scope = nullptr;
    kernel->decref( retiredInstance );
    kernel->decref( constructorArgs );
    kernel->decref( constructorValue );

    nativeDestroy = nullptr;
    nativeNew = nullptr;
    adapter = nullptr;
    kernel->remove_scope<detail::smart_pointer_derived_t>();
    kernel->remove_scope<detail::smart_pointer_base_t>();
    kernel->remove_scope<detail::cast_leaf_t>();
    kernel->remove_scope<detail::cast_middle_t>();
    kernel->remove_scope<detail::cast_secondary_t>();
    kernel->remove_scope<detail::cast_primary_t>();
    kernel->remove_scope<detail::pod_value_t>();
#if defined(PYBIND_CONTRACT_TINYPY)
    kernel->remove_module_finder();
    finder.clear();
    finderObject = nullptr;
    kernel->remove_scope<detail::test_module_finder>();
#endif
    kernel->set_current_module( nullptr );
    kernel->decref( module );
    kernel->collect();
    kernel->destroy();
#if defined(PYBIND_CONTRACT_TINYPY)
    size_t allocations = allocator.allocations();
    assert( allocations == 0 );
#endif
    return 0;
}
