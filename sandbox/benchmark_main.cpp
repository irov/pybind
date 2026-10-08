#include "pybind/pybind.hpp"

#include <cstdio>
#include <cstdlib>
#include <cstring>

namespace detail
{
    class benchmark_allocator
        : public pybind::allocator_interface
    {
    public:
        void * malloc( size_t _size ) override
        {
            return std::malloc( _size );
        }

        void * calloc( size_t _count, size_t _size ) override
        {
            return std::calloc( _count, _size );
        }

        void * realloc( void * _memory, size_t _size ) override
        {
            return std::realloc( _memory, _size );
        }

        void free( void * _memory ) override
        {
            std::free( _memory );
        }
    };

    class node_t
    {
    public:
        float getAlpha() const
        {
            return m_alpha;
        }

        void setAlpha( float _alpha )
        {
            m_alpha = _alpha;
        }

        void setPosition( float _x, float _y )
        {
            m_x = _x;
            m_y = _y;
        }

        bool isEnable() const
        {
            return m_enable;
        }

    protected:
        float m_alpha = 1.f;
        float m_x = 0.f;
        float m_y = 0.f;
        bool m_enable = true;
    };

    static float s_time = 0.f;
    static float s_x = 0.f;
    static float s_y = 0.f;

    static float getTime()
    {
        return s_time;
    }

    static void setPosition( float _x, float _y )
    {
        s_x = _x;
        s_y = _y;
    }

    struct benchmark_case_t
    {
        const char * name;
        const char * statement;
    };

    static const benchmark_case_t cases[] = {
        {"loop", "pass"},
        {"function_getter", "getTime()"},
        {"function_setter", "setPosition(1.0, 2.0)"},
        {"method_getter", "node.getAlpha()"},
        {"method_setter", "node.setPosition(1.0, 2.0)"},
        {"method_predicate", "node.isEnable()"},
        {"property_get", "node.alpha"},
        {"property_set", "node.alpha = 0.5"},
        {"bound_method", "f()"},
        {"unbound_method", "Node.getAlpha(node)"}
    };
}
//////////////////////////////////////////////////////////////////////////
int main( int argc, char ** argv )
{
    if( argc != 3 )
    {
        std::fprintf( stderr, "usage: sandbox_benchmark <case> <iterations>\n" );
        return 1;
    }

    const detail::benchmark_case_t * benchmark = nullptr;

    for( const detail::benchmark_case_t & candidate : detail::cases )
    {
        if( std::strcmp( candidate.name, argv[1] ) == 0 )
        {
            benchmark = &candidate;
        }
    }

    if( benchmark == nullptr )
    {
        std::fprintf( stderr, "unknown case '%s'\n", argv[1] );
        return 1;
    }

    long iterations = std::strtol( argv[2], nullptr, 10 );

    detail::benchmark_allocator allocator;
    pybind::kernel_config_t config;
    config.path = L"";
    config.debug = false;
    config.install_signals = false;
    config.no_site = true;

    pybind::kernel_interface * kernel = pybind::initialize( &allocator, config );

    if( kernel == nullptr )
    {
        return 1;
    }

    PyObject * module = kernel->module_init( "benchmark" );
    kernel->incref( module );
    kernel->set_current_module( module );

    pybind::def_function( kernel, "getTime", &detail::getTime );
    pybind::def_function( kernel, "setPosition", &detail::setPosition );

    pybind::class_<detail::node_t>( kernel, "Node" )
        .def( "getAlpha", &detail::node_t::getAlpha )
        .def( "setPosition", &detail::node_t::setPosition )
        .def( "isEnable", &detail::node_t::isEnable )
        .def_property( "alpha", &detail::node_t::getAlpha, &detail::node_t::setAlpha )
        ;

    detail::node_t * node = new detail::node_t;
    kernel->module_addobject( module, "node", kernel->scope_create_holder_t( node ) );

    char source[512];
    std::snprintf( source, sizeof( source ), "def run(node):\n    f = node.getAlpha\n    for i in xrange(%ld):\n        %s\nrun(node)\n", iterations, benchmark->statement );

    PyObject * moduleDict = kernel->module_dict( module );
    kernel->dict_setstring( moduleDict, "__builtins__", kernel->get_builtins() );
    PyObject * result = kernel->exec_file( source, moduleDict, moduleDict );

    if( result == nullptr )
    {
        return 1;
    }

    kernel->decref( result );
    kernel->set_current_module( nullptr );
    kernel->module_fini( module );
    kernel->remove_scope<detail::node_t>();
    kernel->decref( module );
    kernel->collect();
    kernel->destroy();

    delete node;

    return 0;
}
