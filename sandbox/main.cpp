#include "pybind/pybind.hpp"

#include <cstdio>

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

void test_dict( pybind::kernel_interface * _kernel, uint32_t i )
{
    pybind::dict p( _kernel );
    p["aaa"] = 10040 + i;

    pybind::object g = pybind::make_object_t( _kernel, 23200 + i );
    p["aaa2"] = g;

    pybind::object o = p["aaa2"];
    p["aaa3"] = o;
}

void test_list( pybind::kernel_interface * _kernel, uint32_t i )
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

void test_tuple( pybind::kernel_interface * _kernel, uint32_t i )
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

void test_function_go1( pybind::kernel_interface * _kernel, PyObject * m, uint32_t i )
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
        m_count += _size;

        return ::malloc( _size );
    }

    void * calloc( size_t _num, size_t _size ) override
    {
        m_count += _num * _size;

        return ::calloc( _num, _size );
    }

    void * realloc( void * _ptr, size_t _size ) override
    {
        if( _ptr != nullptr )
        {
            size_t ptr_size = ::_msize( (_ptr) );
            m_count -= ptr_size;
        }

        m_count += _size;

        return ::realloc( _ptr, _size );
    }

    void free( void * _ptr ) override
    {
        if( _ptr != nullptr )
        {
            size_t ptr_size = ::_msize( (_ptr) );
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
    size_t m_count;
};

void main()
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
        return;
    }

    kernel->call_method( gc, "disable", "()" );

    PyObject * module = kernel->module_init( "Test" );

    if( module == nullptr )
    {
        return;
    }

    kernel->set_currentmodule( module );

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
        return;
    }

    test_function_decl( kernel );

    for( uint32_t i = 0; i != 100000; ++i )
    {
        test_dict( kernel, i );
        test_list( kernel, i );
        test_tuple( kernel, i );
        test_function_go1( kernel, m, i );
    }

    for( uint32_t i = 0; i != 100000; ++i )
    {
        test_function_go2( kernel, m, v );
    }

    syspath.reset();

    kernel->module_fini( module );
    kernel->collect();
    kernel->destroy();

    size_t c = a->getCount();

    printf( "done: %zu", c );
}