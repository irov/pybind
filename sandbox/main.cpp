#	include <pybind/pybind.hpp>

#	include <stdio.h>

pybind::kernel_interface * kernel = nullptr;

class Bar
	: public pybind::bindable
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

	void min( float _a, float _b )
	{
		float m = _a < _b? _a : _b;		
	}

	int getData()
	{
		return data;
	}

	void setData( int _data )
	{
		data = _data;
	}

	PyObject * _embedded() override
	{
		const pybind::class_type_scope_ptr & scope = kernel->class_scope<Bar>();

		PyObject * py_obj = scope->create_holder( kernel, this );

		return py_obj;
	}

protected:
	int data;
};


void test_Bar( Bar * _bar )
{
	_bar->foo();
	_bar->min(3.f, 4.f);
}

struct Vec2
{
	Vec2()
	{}

	Vec2( float _x, float _y )
		: x(_x)
		, y(_y)
	{}

	float x;
	float y;
};

class Helper
{
public:
	void helper_for_script_function( Bar * _bar, float _a, float _b )
	{
		_bar->min(_a * 2.f, _b + 2.f);
	}
};

void test_dict( uint32_t i );
void test_list( uint32_t i );
void test_tuple( uint32_t i );
void test_function_decl();
void test_function( PyObject * m, uint32_t i );

void main()
{
	kernel = pybind::initialize( false, false, true );
	
	bool gc_exist;
	PyObject * gc = pybind::module_import( "gc", gc_exist );

	pybind::call_method( gc, "disable", "()" );

	PyObject * module = pybind::module_init( "Test" );

	pybind::set_currentmodule( module );

	Helper * helper = new Helper;

	pybind::class_<Bar>( kernel, "Bar" )
		.def( "foo", &Bar::foo )
		.def( "min", &Bar::min )
		.def_property( "data", &Bar::getData, &Bar::setData )
		.def_proxy_static( "helper_for_script_function", helper, &Helper::helper_for_script_function )
		;

	pybind::class_<Vec2>( kernel, "Vec2" )
		.def_constructor( pybind::init<float, float>() )
		.def_member( "x", &Vec2::x )
		.def_member( "y", &Vec2::y )
		;

	Bar * b = new Bar;

	PyObject * py_b = pybind::ptr( kernel, b );

	PyObject * g = nullptr;

	bool exist;
	PyObject * m = pybind::module_import( "Test1", exist );

	test_function_decl();
	
	for( uint32_t i = 0; i != 100000; ++i )
	{
		test_dict( i );
		test_list( i );
		test_tuple( i );
		test_function( m, i );
	}

	printf( "done" );
}

void test_dict( uint32_t i )
{
	pybind::dict p( kernel );
	p["aaa"] = 10040 + i;

	PyObject * g = pybind::ptr( kernel, 23200 + i );
	p["aaa2"] = g;
	pybind::decref( g );

	pybind::object o = p["aaa2"];
	p["aaa3"] = o;
}

void test_list( uint32_t i )
{
	pybind::list p( kernel, 5 );

	p[0] = 21221 + i;
	p[1] = 25221 + i;
	p[2] = 35521 - i;
	p[3] = 654 + i;
	p[4] = 212421 + i * 5;

	pybind::object p0 = p[0];
	pybind::object p1 = p[1];

	PyObject * p2 = p[2];
	PyObject * p3 = p[3];
}

void test_tuple( uint32_t i )
{
	pybind::tuple pp0 = pybind::make_tuple_t( kernel );
	pybind::tuple pp1 = pybind::make_tuple_t( kernel, 4234 + i );
	pybind::tuple pp2 = pybind::make_tuple_t( kernel, 4234 + i, 434 + i );
	pybind::tuple pp3 = pybind::make_tuple_t( kernel, 4234 + i, 434 + i, 65465464 + i * 2 );

	pybind::object p1 = pp1[0];
	pybind::object p2 = pp2[0];
	pybind::object p3 = pp2[1];

	PyObject * p4 = pp3[0];
	PyObject * p5 = pp3[1];
	PyObject * p6 = pp3[2];
}

int test_function_1( PyObject * _obj, float j )
{
	return (int)(pybind::extract<float>( kernel, _obj ) + j);
}

PyObject * test_function_2( const pybind::object & _obj, float j )
{
	float j1 = _obj.extract();

	return pybind::ptr( kernel, (int)(j1 + j));
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

void test_function_decl()
{ 
	pybind::def_function( kernel, "test_function_1", &test_function_1 );
	pybind::def_function( kernel, "test_function_2", &test_function_2 );
	pybind::def_function( kernel, "test_function_3", &test_function_3 );
	pybind::def_function( kernel, "test_function_4", &test_function_4 );
	pybind::def_function( kernel, "test_function_5", &test_function_5 );
}

void test_function( PyObject * m, uint32_t i )
{
	pybind::call_method( m, "go", "(i)", i );
}