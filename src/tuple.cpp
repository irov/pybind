#include "pybind/tuple.hpp"

namespace pybind
{
    //////////////////////////////////////////////////////////////////////////
    tuple::tuple()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    tuple::tuple( kernel_interface * _kernel )
        : pybind::object( _kernel, _kernel->tuple_new( 0 ), pybind::borrowed )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    tuple::tuple( pybind::invalid_t )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    tuple::tuple( const tuple & _tuple )
        : pybind::object( _tuple )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    tuple::tuple( tuple && _tuple )
        : pybind::object( std::move( _tuple ) )
    {

    }
    //////////////////////////////////////////////////////////////////////////
    tuple::tuple( kernel_interface * _kernel, PyObject * _obj, pybind::borrowed_t _br )
        : pybind::object( _kernel, _obj, _br )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    tuple::tuple( kernel_interface * _kernel, PyObject * _obj )
        : pybind::object( _kernel, _obj )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    tuple::~tuple()
    {
    }
    //////////////////////////////////////////////////////////////////////////    
    tuple & tuple::operator = ( std::nullptr_t )
    {
        this->base::operator = ( nullptr );

        return *this;
    }
    //////////////////////////////////////////////////////////////////////////    
    tuple & tuple::operator = ( const tuple & _obj )
    {
        this->base::operator = ( _obj );

        return *this;
    }
    //////////////////////////////////////////////////////////////////////////
    tuple & tuple::operator = ( tuple && _obj )
    {
        this->base::operator = ( std::move( _obj ) );

        return *this;
    }
    //////////////////////////////////////////////////////////////////////////
    detail::extract_operator_t tuple::operator[]( tuple::size_type _index ) const
    {
        return pybind::tuple_getitem_t( m_kernel, m_obj, _index );
    }
    //////////////////////////////////////////////////////////////////////////
    tuple::size_type tuple::size() const
    {
        return m_kernel->tuple_size( m_obj );
    }
    //////////////////////////////////////////////////////////////////////////
    bool tuple::empty() const
    {
        return this->size() == 0;
    }
    //////////////////////////////////////////////////////////////////////////
    tuple::const_iterator tuple::begin() const
    {
        return const_iterator( m_kernel, m_obj, 0 );
    }
    //////////////////////////////////////////////////////////////////////////
    tuple::const_iterator tuple::end() const
    {
        size_type size = this->size();

        return const_iterator( m_kernel, m_obj, size );
    }
    //////////////////////////////////////////////////////////////////////////	
    pybind::tuple make_tuple_t( kernel_interface * _kernel )
    {
        PyObject * py_tuple = _kernel->tuple_new( 0 );

        if( py_tuple == nullptr )
        {
            pybind::throw_exception( "make_tuple_t 0" );
        }

        return pybind::tuple( _kernel, py_tuple, pybind::borrowed );
    }
    //////////////////////////////////////////////////////////////////////////
    pybind::tuple make_tuple_i( kernel_interface * _kernel, std::initializer_list<detail::import_operator_t> && _t )
    {
        uint32_t size = (uint32_t)_t.size();

        PyObject * py_tuple = _kernel->tuple_new( size );

        uint32_t enumerator = 0;
        for( const detail::import_operator_t & p : _t )
        {
            pybind::tuple_setitem_i( _kernel, py_tuple, enumerator++, p );
        }

        return pybind::tuple( _kernel, py_tuple, pybind::borrowed );
    }
    //////////////////////////////////////////////////////////////////////////
    pybind::tuple make_tuple_args_i( kernel_interface * _kernel, std::initializer_list<detail::import_operator_t> && _t, const args & _args )
    {
        std::initializer_list<detail::import_operator_t>::size_type t_size = _t.size();
        args::size_type args_size = _args.size();

        PyObject * py_tuple = _kernel->tuple_new( (tuple::size_type)t_size + args_size );

        uint32_t enumerator = 0;

        for( const detail::import_operator_t & p : _t )
        {
            pybind::tuple_setitem_i( _kernel, py_tuple, enumerator++, p );
        }

        for( const detail::extract_operator_t & a : _args )
        {
            pybind::tuple_setitem_i( _kernel, py_tuple, enumerator++, a );
        }

        return pybind::tuple( _kernel, py_tuple, pybind::borrowed );
    }
    //////////////////////////////////////////////////////////////////////////
    pybind::tuple make_invalid_tuple_t( kernel_interface * _kernel )
    {
        (void)_kernel;

        return pybind::tuple( pybind::invalid );
    }
    //////////////////////////////////////////////////////////////////////////
    pybind::tuple extract_specialized<pybind::tuple>::operator () ( kernel_interface * _kernel, PyObject * _obj ) const
    {
        pybind::tuple value( pybind::invalid );
        pybind::extract_value( _kernel, _obj, value, true );

        return value;
    }
    //////////////////////////////////////////////////////////////////////////
}