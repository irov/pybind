#pragma once

#include "pybind/object.hpp"

#include "pybind/helper/tuple_const_iterator.hpp"

namespace pybind
{
    class PYBIND_API tuple
        : public pybind::object
    {
    public:
        typedef uint32_t size_type;

    public:
        tuple();
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
        tuple & operator = ( std::nullptr_t );
        tuple & operator = ( const tuple & _obj );
        tuple & operator = ( tuple && _obj );

    public:
        detail::extract_operator_t operator [] ( size_type _index ) const;

    public:
        size_type size() const;
        bool empty() const;

    public:
        typedef tuple_const_iterator const_iterator;

    public:
        const_iterator begin() const;
        const_iterator end() const;
    };
    //////////////////////////////////////////////////////////////////////////	
    PYBIND_API pybind::tuple make_tuple_i( kernel_interface * _kernel, std::initializer_list<detail::import_operator_t> && _t );
    PYBIND_API pybind::tuple make_tuple_args_i( kernel_interface * _kernel, std::initializer_list<detail::import_operator_t> && _t, const args & _args );
    //////////////////////////////////////////////////////////////////////////	
    template<class ... T>
    pybind::tuple make_tuple_t( kernel_interface * _kernel, const T & ... _t )
    {
        return make_tuple_i( _kernel
            , {detail::import_operator_t( _kernel, _t ) ...}
        );
    }
    //////////////////////////////////////////////////////////////////////////
    template<class C>
    inline pybind::tuple make_tuple_container_t( kernel_interface * _kernel, const C & _c )
    {
        typename C::size_type size = _c.size();

        PyObject * py_tuple = _kernel->tuple_new( size );

        uint32_t enumerator = 0;
        for( const typename C::value_type & v : _c )
        {
            pybind::tuple_setitem_t( _kernel, py_tuple, enumerator++, v );
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