#pragma once

#include "pybind/object.hpp"

namespace pybind
{
    class PYBIND_API tuple
        : public pybind::object
    {
    public:
        typedef uint32_t size_type;

    public:
        tuple( kernel_interface * _kernel );
        tuple( const tuple & _tuple );
        tuple( tuple && _tuple );

    public:
        explicit tuple( pybind::invalid_t );
        tuple( kernel_interface * _kernel, PyObject * _obj, pybind::borrowed_t );
        tuple( kernel_interface * _kernel, PyObject * _obj );

    public:
        ~tuple();

    public:
        tuple & operator = ( const tuple & _obj );
        tuple & operator = ( tuple && _obj );

    public:
        detail::extract_operator_t operator [] ( size_type _index ) const;

    public:
        size_type size() const;
        bool empty() const;
    };
    //////////////////////////////////////////////////////////////////////////	
    PYBIND_API pybind::tuple make_tuple_t( kernel_interface * _kernel );
    PYBIND_API pybind::tuple make_tuple_i( kernel_interface * _kernel, const detail::import_operator_t & _t0 );
    PYBIND_API pybind::tuple make_tuple_i( kernel_interface * _kernel, const detail::import_operator_t & _t0, const detail::import_operator_t & _t1 );
    PYBIND_API pybind::tuple make_tuple_i( kernel_interface * _kernel, const detail::import_operator_t & _t0, const detail::import_operator_t & _t1, const detail::import_operator_t & _t2 );
    PYBIND_API pybind::tuple make_tuple_i( kernel_interface * _kernel, const detail::import_operator_t & _t0, const detail::import_operator_t & _t1, const detail::import_operator_t & _t2, const detail::import_operator_t & _t3 );
    PYBIND_API pybind::tuple make_tuple_i( kernel_interface * _kernel, const detail::import_operator_t & _t0, const detail::import_operator_t & _t1, const detail::import_operator_t & _t2, const detail::import_operator_t & _t3, const detail::import_operator_t & _t4 );
    PYBIND_API pybind::tuple make_tuple_i( kernel_interface * _kernel, const detail::import_operator_t & _t0, const detail::import_operator_t & _t1, const detail::import_operator_t & _t2, const detail::import_operator_t & _t3, const detail::import_operator_t & _t4, const detail::import_operator_t & _t5 );
    PYBIND_API pybind::tuple make_tuple_i( kernel_interface * _kernel, const detail::import_operator_t & _t0, const detail::import_operator_t & _t1, const detail::import_operator_t & _t2, const detail::import_operator_t & _t3, const detail::import_operator_t & _t4, const detail::import_operator_t & _t5, const detail::import_operator_t & _t6 );
    PYBIND_API pybind::tuple make_tuple_i( kernel_interface * _kernel, const detail::import_operator_t & _t0, const detail::import_operator_t & _t1, const detail::import_operator_t & _t2, const detail::import_operator_t & _t3, const detail::import_operator_t & _t4, const detail::import_operator_t & _t5, const detail::import_operator_t & _t6, const detail::import_operator_t & _t7 );
    //////////////////////////////////////////////////////////////////////////
    PYBIND_API pybind::tuple make_tuple_args_t( kernel_interface * _kernel, const args & _args );
    PYBIND_API pybind::tuple make_tuple_args_i( kernel_interface * _kernel, const detail::import_operator_t & _t0, const args & _args );
    PYBIND_API pybind::tuple make_tuple_args_i( kernel_interface * _kernel, const detail::import_operator_t & _t0, const detail::import_operator_t & _t1, const args & _args );
    PYBIND_API pybind::tuple make_tuple_args_i( kernel_interface * _kernel, const detail::import_operator_t & _t0, const detail::import_operator_t & _t1, const detail::import_operator_t & _t2, const args & _args );
    PYBIND_API pybind::tuple make_tuple_args_i( kernel_interface * _kernel, const detail::import_operator_t & _t0, const detail::import_operator_t & _t1, const detail::import_operator_t & _t2, const detail::import_operator_t & _t3, const args & _args );
    PYBIND_API pybind::tuple make_tuple_args_i( kernel_interface * _kernel, const detail::import_operator_t & _t0, const detail::import_operator_t & _t1, const detail::import_operator_t & _t2, const detail::import_operator_t & _t3, const detail::import_operator_t & _t4, const args & _args );
    PYBIND_API pybind::tuple make_tuple_args_i( kernel_interface * _kernel, const detail::import_operator_t & _t0, const detail::import_operator_t & _t1, const detail::import_operator_t & _t2, const detail::import_operator_t & _t3, const detail::import_operator_t & _t4, const detail::import_operator_t & _t5, const args & _args );
    PYBIND_API pybind::tuple make_tuple_args_i( kernel_interface * _kernel, const detail::import_operator_t & _t0, const detail::import_operator_t & _t1, const detail::import_operator_t & _t2, const detail::import_operator_t & _t3, const detail::import_operator_t & _t4, const detail::import_operator_t & _t5, const detail::import_operator_t & _t6, const args & _args );
    PYBIND_API pybind::tuple make_tuple_args_i( kernel_interface * _kernel, const detail::import_operator_t & _t0, const detail::import_operator_t & _t1, const detail::import_operator_t & _t2, const detail::import_operator_t & _t3, const detail::import_operator_t & _t4, const detail::import_operator_t & _t5, const detail::import_operator_t & _t6, const detail::import_operator_t & _t7, const args & _args );
    //////////////////////////////////////////////////////////////////////////	
    template<class ... T>
    pybind::tuple make_tuple_t( kernel_interface * _kernel, const T & ... _t )
    {
        return make_tuple_i( _kernel
            , detail::import_operator_t( _kernel, _t ) ...
        );
    }    
    //////////////////////////////////////////////////////////////////////////
    template<class C>
    inline pybind::tuple make_tuple_container_t( kernel_interface * _kernel, const C & _c )
    {
        typename C::size_type size = _c.size();

        PyObject * py_tuple = _kernel->tuple_new( size );

        for( typename C::size_type i = 0; i != size; ++i )
        {
            const typename C::value_type & v = _c[i];

            pybind::tuple_setitem_t( _kernel, py_tuple, i, v );
        }

        return pybind::tuple( _kernel, py_tuple, pybind::borrowed );
    }
    //////////////////////////////////////////////////////////////////////////
    PYBIND_API pybind::tuple make_invalid_tuple_t( kernel_interface * _kernel );
    //////////////////////////////////////////////////////////////////////////
    template<>
    struct extract_specialized<pybind::tuple>
    {
        pybind::tuple operator () ( kernel_interface * _kernel, PyObject * _obj ) const;
    };
    //////////////////////////////////////////////////////////////////////////
}