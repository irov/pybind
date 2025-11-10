#pragma once

#include "pybind/base.hpp"
#include "pybind/helper.hpp"

#include <tuple>
#include <type_traits>

namespace pybind
{
    class PYBIND_API object
        : public base
    {
    public:
        static const pybind::object & get_invalid();

    public:
        object();
        object( const object & _obj );
        object( object && _obj );
        object( std::nullptr_t );
        object( const detail::extract_operator_t & _extract );

    public:
        object( kernel_interface * _kernel, PyObject * _obj );
        object( kernel_interface * _kernel, PyObject * _obj, pybind::borrowed_t );

    public:
        explicit object( pybind::invalid_t );

    public:
        ~object();

    public:
        object & operator = ( std::nullptr_t );
        object & operator = ( const object & _obj );
        object & operator = ( object && _obj );

    public:
        template<class T>
        bool has_attr( const T & _name ) const
        {
            return this->has_attr_i( 
                detail::import_operator_t( m_kernel, _name )
            );
        }

        template<class T>
        pybind::object get_attr( const T & _name ) const
        {
            return this->get_attr_i( 
                detail::import_operator_t( m_kernel, _name ) 
            );
        }

        template<class K, class V>
        void set_attr( K && _name, V && _value )
        {
            this->set_attr_i( 
                detail::import_operator_t( m_kernel, std::forward<K>( _name ) ),
                detail::import_operator_t( m_kernel, std::forward<V>( _value ) )
            );
        }

    public:
        bool has_attr_i( const detail::import_operator_t & _name ) const;
        pybind::object get_attr_i( const detail::import_operator_t & _name ) const;
        void set_attr_i( const detail::import_operator_t & _name, const detail::import_operator_t & _value );

    public:
        detail::extract_operator_t extract() const;

    public:
        detail::extract_operator_t call_i( std::initializer_list<detail::import_operator_t> && _t ) const;
        detail::extract_operator_t call_args_i( std::initializer_list<detail::import_operator_t> && _t, const args & _args ) const;
        detail::extract_operator_t call_args_i( std::initializer_list<detail::import_operator_t> && _t, args && _args ) const;

    public:
        template<class ... T>
        detail::extract_operator_t call( T && ... _t ) const
        {
            return this->call_i(
                {detail::import_operator_t( m_kernel, std::forward<T>( _t ) ) ...}
            );
        }

    public:
        template<class ... T, size_t ... I>
        detail::extract_operator_t call_args_ii( std::tuple<T ...> && _t, const args & _args, std::integer_sequence<size_t, I ...> ) const
        {
            return this->call_args_i( {detail::import_operator_t( m_kernel, std::get<I>( _t ) ) ...}
                , _args
            );
        }

        template<class ... T, size_t ... I>
        detail::extract_operator_t call_args_ii( std::tuple<T ...> && _t, args && _args, std::integer_sequence<size_t, I ...> ) const
        {
            return this->call_args_i( {detail::import_operator_t( m_kernel, std::get<I>( _t ) ) ...}
                , std::forward<args>( _args )
            );
        }

        template<class ... T>
        detail::extract_operator_t call_args( T && ... _t ) const
        {
            return this->call_args_ii( std::forward_as_tuple( std::forward<T>( _t ) ... )
                , std::get<sizeof ... (T) - 1u>( std::forward_as_tuple( std::forward<T>( _t ) ... ) )
                , std::make_integer_sequence<size_t, sizeof ... (T) - 1u>()
            );
        }

    public:
        template<class N, class ... T>
        detail::extract_operator_t call_method( const N & _name, T && ... _t ) const
        {
            pybind::object method = this->get_attr_i(
                detail::import_operator_t( m_kernel, _name )
            );

            return method.call( std::forward<T>( _t ) ... );
        }

        template<class N, class ... T>
        detail::extract_operator_t call_method_args( const N & _name, T && ... _t ) const
        {
            pybind::object method = this->get_attr_i(
                detail::import_operator_t( m_kernel, _name )
            );

            return this->call_args( _name, std::forward<T>( _t ) ... );
        }

    public:
        detail::extract_operator_t call_native( pybind::tuple && _args ) const;
    };
    //////////////////////////////////////////////////////////////////////////
    template<class It>
    inline void foreach_t( const pybind::object & _cb, It _begin, It _end )
    {
        for( It
            it = _begin,
            it_end = _end;
            it != it_end;
            ++it )
        {
            _cb.call( *it );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    PYBIND_API pybind::object make_none_t( kernel_interface * _kernel );
    PYBIND_API pybind::object make_true_t( kernel_interface * _kernel );
    PYBIND_API pybind::object make_false_t( kernel_interface * _kernel );
    //////////////////////////////////////////////////////////////////////////
    PYBIND_API pybind::object make_borrowed_t( kernel_interface * _kernel, PyObject * _obj );
    //////////////////////////////////////////////////////////////////////////
    PYBIND_API pybind::object make_object_i( kernel_interface * _kernel, const detail::import_operator_t & _t0 );
    //////////////////////////////////////////////////////////////////////////
    template<class T>
    pybind::object make_object_t( kernel_interface * _kernel, T && _t0 )
    {
        return make_object_i( _kernel, detail::import_operator_t( _kernel, std::forward<T>( _t0 ) ) );
    }
    //////////////////////////////////////////////////////////////////////////
    PYBIND_API pybind::object make_invalid_object_t();
    //////////////////////////////////////////////////////////////////////////
}