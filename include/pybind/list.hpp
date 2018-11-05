#pragma once

#include "pybind/object.hpp"

#include "pybind/helper/list_helper.hpp"
#include "pybind/helper/list_const_iterator.hpp"

namespace pybind
{
    class PYBIND_API list
        : public object
    {
    public:
        typedef uint32_t size_type;

    public:
        list( kernel_interface * _kernel );
        list( kernel_interface * _kernel, size_type _size );
        list( const list & _list );
        list( list && _list );

    public:
        explicit list( pybind::invalid_t );
        list( kernel_interface * _kernel, PyObject * _obj, pybind::borrowed_t );
        list( kernel_interface * _kernel, PyObject * _obj );

    public:
        ~list();

    public:
        list & operator = ( const list & _obj );
        list & operator = ( list && _obj );

    public:
        detail::set_list_operator_t operator [] ( size_type _index );
        detail::extract_operator_t operator [] ( size_type _index ) const;

    public:
        template<class T>
        list & append( const T & _t )
        {
            return this->append_i( detail::import_operator_t( m_kernel, _t ) );
        }

    public:
        list & append_i( const detail::import_operator_t & _t );

    public:
        template<class It>
        list & append( It _begin, It _end )
        {
            for( It it = _begin; it != _end; ++it )
            {
                this->append( *it );
            }

            return *this;
        }

        template<class It>
        void fill( size_type _offset, It _begin, It _end )
        {
            size_type index = 0;

            for( It it = _begin; it != _end; ++it )
            {
                this->operator [] ( _offset + index++ ) = *it;
            }
        }

    public:
        size_type size() const;
        bool empty() const;

    public:
        typedef list_const_iterator const_iterator;

    public:
        const_iterator begin() const;
        const_iterator end() const;
    };
    //////////////////////////////////////////////////////////////////////////
    template<>
    struct extract_specialized<pybind::list>
    {
        pybind::list operator () ( kernel_interface * _kernel, PyObject * _obj ) const;
    };
    //////////////////////////////////////////////////////////////////////////
    template<class C>
    inline pybind::list make_list_container_t( kernel_interface * _kernel, const C & _c )
    {
        typename C::size_type c_size = _c.size();
        pybind::list l( _kernel, (pybind::list::size_type)c_size );

        l.fill( 0, _c.begin(), _c.end() );

        return l;
    }
    //////////////////////////////////////////////////////////////////////////
    PYBIND_API pybind::list make_invalid_list_t();
}