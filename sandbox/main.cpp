#include "pybind/pybind.hpp"

#if defined(_WIN32)
#   include <malloc.h>
#elif defined(__APPLE__)
#   include <malloc/malloc.h>
#endif

#include <cstdio>
#include <climits>
#include <cstring>
#include <limits>
#include <vector>

class Bar
{
public:
    Bar()
        : data( 0 )
    {
    }

    ~Bar()
    {
    }

public:
    void foo()
    {
        data++;
    }

    float min( float _a, float _b )
    {
        return _a < _b ? _a : _b;
    }

    int getData()
    {
        return data;
    }

    void setData( int _data )
    {
        data = _data;
    }

    void clearEmbed()
    {
        //Empty
    }

    PyObject * getEmbed( pybind::kernel_interface * _kernel )
    {
        const pybind::class_type_scope_interface_ptr & scope = _kernel->get_class_type_scope_t<Bar>();

        PyObject * py_obj = scope->create_holder( this );

        return py_obj;
    }

protected:
    int data;
};


void test_Bar( Bar * _bar )
{
    _bar->foo();
    _bar->min( 3.f, 4.f );
}

struct Vec2
{
    Vec2()
    {}

    Vec2( float _x, float _y )
        : x( _x )
        , y( _y )
    {}

    float x;
    float y;
};

class Helper
{
public:
    void helper_for_script_function( Bar * _bar, float _a, float _b )
    {
        _bar->min( _a * 2.f, _b + 2.f );
    }
};

void test_dict( pybind::kernel_interface * _kernel, int i )
{
    pybind::dict p( _kernel );
    p["aaa"] = 10040 + i;

    pybind::object g = pybind::make_object_t( _kernel, 23200 + i );
    p["aaa2"] = g;

    pybind::object o = p["aaa2"];
    p["aaa3"] = o;
}

void test_list( pybind::kernel_interface * _kernel, int i )
{
    pybind::list p( _kernel, 5 );

    p[0] = 21221 + i;
    p[1] = 25221 + i;
    p[2] = 35521 - i;
    p[3] = 654 + i;
    p[4] = 212421 + i * 5;

    pybind::object p0 = p[0];
    pybind::object p1 = p[1];

    (void)p0;
    (void)p1;
}

void test_tuple( pybind::kernel_interface * _kernel, int i )
{
    pybind::tuple pp0 = pybind::make_tuple_t( _kernel );
    pybind::tuple pp1 = pybind::make_tuple_t( _kernel, 4234 + i );
    pybind::tuple pp2 = pybind::make_tuple_t( _kernel, 4234 + i, 434 + i );
    pybind::tuple pp3 = pybind::make_tuple_t( _kernel, 4234 + i, 434 + i, 65465464 + i * 2 );

    pybind::object p1 = pp1[0];
    pybind::object p2 = pp2[0];
    pybind::object p3 = pp2[1];

    PyObject * p4 = pp3[0];
    PyObject * p5 = pp3[1];
    PyObject * p6 = pp3[2];

    (void)p1;
    (void)p2;
    (void)p3;
    (void)p4;
    (void)p5;
    (void)p6;
}

bool test_pickle_refcounts( pybind::kernel_interface * _kernel )
{
    pybind::list types( _kernel );

    PyObject * py_tuple_value = _kernel->unicode_from_utf8( "tuple unicode value" );
    pybind::object tuple_value( _kernel, py_tuple_value, pybind::borrowed );
    PyObject * py_tuple = _kernel->tuple_new( 1 );
    pybind::object tuple( _kernel, py_tuple, pybind::borrowed );
    _kernel->tuple_setitem( py_tuple, 0, py_tuple_value );

    PyObject * py_list_value = _kernel->unicode_from_utf8( "list unicode value" );
    pybind::object list_value( _kernel, py_list_value, pybind::borrowed );
    PyObject * py_list = _kernel->list_new( 1 );
    pybind::object list( _kernel, py_list, pybind::borrowed );
    _kernel->list_setitem( py_list, 0, py_list_value );

    PyObject * py_dict_key = _kernel->unicode_from_utf8( "dict unicode key" );
    pybind::object dict_key( _kernel, py_dict_key, pybind::borrowed );
    PyObject * py_dict_value = _kernel->unicode_from_utf8( "dict unicode value" );
    pybind::object dict_value( _kernel, py_dict_value, pybind::borrowed );
    PyObject * py_dict = _kernel->dict_new();
    pybind::object dict( _kernel, py_dict, pybind::borrowed );
    _kernel->dict_set( py_dict, py_dict_key, py_dict_value );

    PyObject * py_set_value = _kernel->unicode_from_utf8( "set unicode value" );
    pybind::object set_value( _kernel, py_set_value, pybind::borrowed );
    PyObject * py_set = _kernel->set_new();
    pybind::object set( _kernel, py_set, pybind::borrowed );
    _kernel->set_set( py_set, py_set_value );

    PyObject * py_source = _kernel->list_new( 4 );
    pybind::object source( _kernel, py_source, pybind::borrowed );
    _kernel->list_setitem( py_source, 0, py_tuple );
    _kernel->list_setitem( py_source, 1, py_list );
    _kernel->list_setitem( py_source, 2, py_dict );
    _kernel->list_setitem( py_source, 3, py_set );

    PyObject * source_objects[] = {
        py_tuple_value,
        py_tuple,
        py_list_value,
        py_list,
        py_dict_key,
        py_dict_value,
        py_dict,
        py_set_value,
        py_set
    };
    uint32_t source_refcounts[sizeof( source_objects ) / sizeof( source_objects[0] )];

    for( size_t i = 0; i != sizeof( source_objects ) / sizeof( source_objects[0] ); ++i )
    {
        source_refcounts[i] = _kernel->refcount( source_objects[i] );
    }

    size_t pickle_size;
    if( pybind::pickle( _kernel, py_source, types.ptr(), nullptr, 0, &pickle_size ) == false )
    {
        return false;
    }

    std::vector<uint8_t> buffer( pickle_size );

    for( uint32_t i = 0; i != 32; ++i )
    {
        size_t write_size;
        if( pybind::pickle( _kernel, py_source, types.ptr(), buffer.data(), buffer.size(), &write_size ) == false || write_size != pickle_size )
        {
            return false;
        }
    }

    for( size_t i = 0; i != sizeof( source_objects ) / sizeof( source_objects[0] ); ++i )
    {
        if( _kernel->refcount( source_objects[i] ) != source_refcounts[i] )
        {
            return false;
        }
    }

    PyObject * py_unpickled = pybind::unpickle( _kernel, buffer.data(), buffer.size(), types.ptr() );
    pybind::object unpickled( _kernel, py_unpickled, pybind::borrowed );

    if( py_unpickled == nullptr || _kernel->refcount( py_unpickled ) != 1 )
    {
        return false;
    }

    PyObject * py_unpickled_tuple = _kernel->list_getitem( py_unpickled, 0 );
    PyObject * py_unpickled_list = _kernel->list_getitem( py_unpickled, 1 );
    PyObject * py_unpickled_dict = _kernel->list_getitem( py_unpickled, 2 );
    PyObject * py_unpickled_set = _kernel->list_getitem( py_unpickled, 3 );

    if( _kernel->refcount( py_unpickled_tuple ) != 1 ||
        _kernel->refcount( py_unpickled_list ) != 1 ||
        _kernel->refcount( py_unpickled_dict ) != 1 ||
        _kernel->refcount( py_unpickled_set ) != 1 )
    {
        return false;
    }

    PyObject * py_unpickled_tuple_value = _kernel->tuple_getitem( py_unpickled_tuple, 0 );
    PyObject * py_unpickled_list_value = _kernel->list_getitem( py_unpickled_list, 0 );

    size_t dict_position = 0;
    PyObject * py_unpickled_dict_key;
    PyObject * py_unpickled_dict_value;

    if( _kernel->dict_next( py_unpickled_dict, &dict_position, &py_unpickled_dict_key, &py_unpickled_dict_value ) == false )
    {
        return false;
    }

    if( _kernel->refcount( py_unpickled_tuple_value ) != 1 ||
        _kernel->refcount( py_unpickled_list_value ) != 1 ||
        _kernel->refcount( py_unpickled_dict_key ) != 1 ||
        _kernel->refcount( py_unpickled_dict_value ) != 1 )
    {
        return false;
    }

    PyObject * py_set_iterator;
    if( _kernel->iterator_get( py_unpickled_set, &py_set_iterator ) == false )
    {
        return false;
    }

    pybind::object set_iterator( _kernel, py_set_iterator, pybind::borrowed );

    PyObject * py_unpickled_set_value;
    if( _kernel->iterator_next( py_set_iterator, &py_unpickled_set_value ) == false )
    {
        return false;
    }

    pybind::object unpickled_set_value( _kernel, py_unpickled_set_value, pybind::borrowed );

    if( _kernel->refcount( py_unpickled_set_value ) != 2 )
    {
        return false;
    }

    PyObject * py_utf8 = _kernel->unicode_encode_utf8( py_unpickled_tuple_value );
    pybind::object utf8( _kernel, py_utf8, pybind::borrowed );

    if( py_utf8 == nullptr || _kernel->refcount( py_utf8 ) != 1 )
    {
        return false;
    }

    size_t utf8_size;
    const char * utf8_value = _kernel->string_to_char_and_size( py_utf8, &utf8_size );

    return utf8_size == 19 && std::memcmp( utf8_value, "tuple unicode value", utf8_size ) == 0;
}

bool test_integer_construction( pybind::kernel_interface * _kernel )
{
    const int64_t values[] = {
        std::numeric_limits<int64_t>::min(),
        -1,
        0,
        1,
        std::numeric_limits<int64_t>::max()
    };

    for( int64_t value : values )
    {
        PyObject * py_value = _kernel->ptr_integer( value );
        pybind::object value_owner( _kernel, py_value, pybind::borrowed );

        if( py_value == nullptr )
        {
            return false;
        }

        int64_t extracted_value;

        if( _kernel->extract_int64( py_value, extracted_value ) == false || extracted_value != value )
        {
            return false;
        }

        if( _kernel->get_python_version() < 300 )
        {
            bool fits_python_int = value >= static_cast<int64_t>(LONG_MIN) && value <= static_cast<int64_t>(LONG_MAX);

            if( fits_python_int == true && _kernel->int_check( py_value ) == false )
            {
                return false;
            }

            if( fits_python_int == false && _kernel->long_check( py_value ) == false )
            {
                return false;
            }
        }
    }

    return true;
}

int test_function_1( pybind::kernel_interface * _kernel, PyObject * _obj, float j )
{
    return (int)(pybind::extract<float>( _kernel, _obj ) + j);
}

PyObject * test_function_2( pybind::kernel_interface * _kernel, const pybind::object & _obj, float j )
{
    float j1 = _obj.extract();

    return pybind::ptr( _kernel, (int)(j1 + j) );
}

pybind::object test_function_3( const pybind::object & _obj )
{
    return _obj;
}

void test_function_4( const pybind::object & _cb )
{
    Bar * b = new Bar;

    _cb.call( b );

    delete b;
}

Bar * test_function_5( Bar * _b )
{
    return _b;
}

Vec2 test_function_6( const Vec2 & _v )
{
    return Vec2( _v.y, _v.x );
}

void test_function_decl( pybind::kernel_interface * _kernel )
{
    pybind::def_function_kernel( _kernel, "test_function_1", &test_function_1 );
    pybind::def_function_kernel( _kernel, "test_function_2", &test_function_2 );
    pybind::def_function( _kernel, "test_function_3", &test_function_3 );
    pybind::def_function( _kernel, "test_function_4", &test_function_4 );
    pybind::def_function( _kernel, "test_function_5", &test_function_5 );
    pybind::def_function( _kernel, "test_function_6", &test_function_6 );
}

void test_function_go1( pybind::kernel_interface * _kernel, PyObject * m, int i )
{
    _kernel->call_method( m, "go1", "(i)", i );
}

void test_function_go2( pybind::kernel_interface * _kernel, PyObject * m, const Vec2 & v )
{
    _kernel->call_method( m, "go2", "(O)", pybind::ptr( _kernel, v ) );
}

class malloc_allocator
    : public pybind::allocator_interface
{
public:
    malloc_allocator()
        : m_count( 0 )
    {
    }
    

public:
    void * malloc( size_t _size ) override
    {
        void * ptr = ::malloc( _size );

        m_count += this->getPtrSize_( ptr );

        return ptr;
    }

    void * calloc( size_t _num, size_t _size ) override
    {
        void * ptr = ::calloc( _num, _size );

        m_count += this->getPtrSize_( ptr );

        return ptr;
    }

    void * realloc( void * _ptr, size_t _size ) override
    {
        size_t old_size = 0;

        if( _ptr != nullptr )
        {
            old_size = this->getPtrSize_( _ptr );
        }

        void * ptr = ::realloc( _ptr, _size );

        if( ptr == nullptr )
        {
            return nullptr;
        }

        m_count -= old_size;
        m_count += this->getPtrSize_( ptr );

        return ptr;
    }

    void free( void * _ptr ) override
    {
        if( _ptr != nullptr )
        {
            size_t ptr_size = this->getPtrSize_( _ptr );
            m_count -= ptr_size;
        }

        ::free( _ptr );
    }

public:
    size_t getCount() const
    {
        return m_count;
    }

protected:
    size_t getPtrSize_( void * _ptr ) const
    {
        if( _ptr == nullptr )
        {
            return 0;
        }

#if defined(_WIN32)
        return ::_msize( _ptr );
#elif defined(__APPLE__)
        return ::malloc_size( _ptr );
#else
        (void)_ptr;

        return 0;
#endif
    }

protected:
    size_t m_count;
};

int main()
{
    malloc_allocator * a = new malloc_allocator;

    pybind::kernel_interface * kernel = pybind::initialize( a, L"", false, false, true );

    pybind::list syspath( kernel );
    syspath.append( PYBIND_SANDBOX_SCRIPTS_PATH );

    kernel->set_syspath( syspath.ret() );

    bool gc_exist;
    PyObject * gc = kernel->module_import( "gc", gc_exist );

    if( gc == nullptr )
    {
        return 1;
    }

    kernel->call_method( gc, "disable", "()" );

    PyObject * module = kernel->module_init( "Test" );

    if( module == nullptr )
    {
        return 1;
    }

    kernel->set_current_module( module );

    if( test_pickle_refcounts( kernel ) == false )
    {
        printf( "pickle refcount test failed\n" );

        return 1;
    }

    if( test_integer_construction( kernel ) == false )
    {
        printf( "integer construction test failed\n" );

        return 1;
    }

    Helper * helper = new Helper;

    pybind::class_<Bar>( kernel, "Bar" )
        .def_bindable()
        .def( "foo", &Bar::foo )
        .def( "min", &Bar::min )
        .def_property( "data", &Bar::getData, &Bar::setData )
        .def_proxy_static( "helper_for_script_function", helper, &Helper::helper_for_script_function )
        ;

    pybind::struct_<Vec2>( kernel, "Vec2" )
        .def_constructor( pybind::init<float, float>() )
        .def_member( "x", &Vec2::x )
        .def_member( "y", &Vec2::y )
        ;

    Vec2 v( 1.f, 2.f );

    Bar * b = new Bar;

    PyObject * py_b = pybind::ptr( kernel, b );
    (void)py_b;

    bool exist;
    PyObject * m = kernel->module_import( "Test1", exist );

    if( m == nullptr )
    {
        return 1;
    }

    test_function_decl( kernel );

    for( size_t i = 0; i != 100000; ++i )
    {
        int index = static_cast<int>(i);

        test_dict( kernel, index );
        test_list( kernel, index );
        test_tuple( kernel, index );
        test_function_go1( kernel, m, index );
    }

    for( size_t i = 0; i != 100000; ++i )
    {
        test_function_go2( kernel, m, v );
    }

    syspath.reset();

    kernel->module_fini( module );
    kernel->collect();
    kernel->destroy();

    size_t c = a->getCount();

    printf( "done: %zu", c );

    return 0;
}
