#include "pybind/kernel_interface.hpp"
#include "pybind/extract.hpp"
#include "pybind/function_interface.hpp"
#include "pybind/adapter/new_adapter.hpp"
#include "pybind/adapter/destroy_adapter.hpp"
#include "pybind/class_type_scope_interface.hpp"
#include "pybind/object.hpp"
#include "pybind/generator/interface_.hpp"

#include <cassert>
#include <cstdlib>
#include <cstring>

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

#if defined(PYBIND_CONTRACT_TINYPY)
    pybind::interface_<detail::test_module_finder>( kernel, "TestModuleFinder", true )
        .def_kernel( "find_module", &detail::test_module_finder::find_module )
        .def_kernel( "load_module", &detail::test_module_finder::load_module )
        ;
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

    PyObject * moduleDict = kernel->module_dict( module );
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
