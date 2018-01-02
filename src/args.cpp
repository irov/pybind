#	include "pybind/args.hpp"
#   include "pybind/helper.hpp"

namespace pybind
{
    //////////////////////////////////////////////////////////////////////////
    args::args()
        : m_kernel( nullptr )
        , m_args( nullptr )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    args::args( const args & _r )
        : m_kernel( _r.m_kernel )
        , m_args( _r.m_args )
    {
        pybind::incref( m_args );
    }
    //////////////////////////////////////////////////////////////////////////
    args::args( kernel_interface * _kernel, PyObject * _args )
        : m_kernel( _kernel )
        , m_args( _args )
    {
        pybind::incref( m_args );
    }
    //////////////////////////////////////////////////////////////////////////
    args::~args()
    {
        pybind::decref( m_args );
    }
    //////////////////////////////////////////////////////////////////////////
    args & args::operator = ( const args & _args )
    {
        m_kernel = _args.m_kernel;

        pybind::decref( m_args );
        m_args = _args.ptr();
        pybind::incref( m_args );

        return *this;
    }
    //////////////////////////////////////////////////////////////////////////
    void args::reset()
    {
        m_kernel = nullptr;

        pybind::decref( m_args );
        m_args = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    size_t args::size() const
    {
        if( m_args == nullptr )
        {
            return 0;
        }

        size_t args_size = pybind::tuple_size( m_args );

        return args_size;
    }
    //////////////////////////////////////////////////////////////////////////
    detail::extract_operator_t args::operator[]( size_t _index ) const
    {
        return pybind::tuple_getitem_t( m_kernel, m_args, _index );
    }
}