#include "pybind/args.hpp"
#include "pybind/helper.hpp"

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
        if( m_kernel != nullptr )
        {
            m_kernel->incref( m_args );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    args::args( args && _r )
        : m_kernel( _r.m_kernel )
        , m_args( _r.m_args )
    {
        _r.m_kernel = nullptr;
        _r.m_args = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    args::args( kernel_interface * _kernel, PyObject * _args )
        : m_kernel( _kernel )
        , m_args( _args )
    {
        if( m_kernel != nullptr )
        {
            m_kernel->incref( m_args );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    args::args( kernel_interface * _kernel, PyObject * _args, pybind::borrowed_t )
        : m_kernel( _kernel )
        , m_args( _args )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    args::~args()
    {
        if( m_kernel != nullptr )
        {
            m_kernel->decref( m_args );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    args & args::operator = ( const args & _args )
    {
        if( m_kernel != nullptr )
        {
            m_kernel->decref( m_args );
        }

        m_kernel = _args.m_kernel;
        m_args = _args.ptr();

        if( m_kernel != nullptr )
        {
            m_kernel->incref( m_args );
        }

        return *this;
    }
    //////////////////////////////////////////////////////////////////////////
    void args::reset()
    {
        if( m_kernel != nullptr )
        {
            m_kernel->decref( m_args );
            m_args = nullptr;
        }

        m_kernel = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    args::size_type args::size() const
    {
        if( m_args == nullptr )
        {
            return 0;
        }

        size_type args_size = m_kernel->tuple_size( m_args );

        return args_size;
    }
    //////////////////////////////////////////////////////////////////////////
    detail::extract_operator_t args::operator[]( args::size_type _index ) const
    {
        return pybind::tuple_getitem_t( m_kernel, m_args, _index );
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * args::repr() const
    {
        PyObject * py_repr = m_kernel->object_repr( m_args );

        return py_repr;
    }
    //////////////////////////////////////////////////////////////////////////
    args::const_iterator args::begin() const
    {
        return const_iterator( m_kernel, m_args, 0 );
    }
    //////////////////////////////////////////////////////////////////////////
    args::const_iterator args::end() const
    {
        size_type size = this->size();

        return const_iterator( m_kernel, m_args, size );
    }
}