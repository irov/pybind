#pragma once

#include "pybind/base.hpp"

#include "pybind/helper.hpp"

namespace pybind
{
    class PYBIND_API object
        : public base
    {
    public:
        static const pybind::object & get_invalid();

    public:
        object();
        object( const base & _base );
        object( const object & _obj );
        object( object && _obj );
        object( const detail::extract_operator_t & _extract );

    public:        
        object( kernel_interface * _kernel, PyObject * _obj );
        object( kernel_interface * _kernel, PyObject * _obj, pybind::borrowed );

    public:
        explicit object( pybind::invalid );

    public:
        ~object();

    public:
        object & operator = ( const object & _obj );
        object & operator = ( object && _obj );

    public:
        template<class T>
        bool has_attr( const T & _name ) const
        {
            return this->has_attr_i( detail::import_operator_t( m_kernel, _name ) );
        }

        template<class T>
        pybind::object get_attr( const T & _name ) const
        {
            return this->get_attr_i( detail::import_operator_t( m_kernel, _name ) );
        }

    public:
        bool has_attr_i( const detail::import_operator_t & _name ) const;
        pybind::object get_attr_i( const detail::import_operator_t & _name ) const;

    public:
        detail::extract_operator_t extract() const;

    public:
        template<class T0>
        detail::extract_operator_t call( const T0 & _t0 ) const
        {
            return this->call_i(
                detail::import_operator_t( m_kernel, _t0 )
            );
        }

        template<class T0, class T1>
        detail::extract_operator_t call( const T0 & _t0, const T1 & _t1 ) const
        {
            return this->call_i(
                detail::import_operator_t( m_kernel, _t0 ),
                detail::import_operator_t( m_kernel, _t1 )
            );
        }

        template<class T0, class T1, class T2>
        detail::extract_operator_t call( const T0 & _t0, const T1 & _t1, const T2 & _t2 ) const
        {
            return this->call_i(
                detail::import_operator_t( m_kernel, _t0 ),
                detail::import_operator_t( m_kernel, _t1 ),
                detail::import_operator_t( m_kernel, _t2 )
            );
        }

        template<class T0, class T1, class T2, class T3>
        detail::extract_operator_t call( const T0 & _t0, const T1 & _t1, const T2 & _t2, const T3 & _t3 ) const
        {
            return this->call_i(
                detail::import_operator_t( m_kernel, _t0 ),
                detail::import_operator_t( m_kernel, _t1 ),
                detail::import_operator_t( m_kernel, _t2 ),
                detail::import_operator_t( m_kernel, _t3 )
            );
        }

        template<class T0, class T1, class T2, class T3, class T4>
        detail::extract_operator_t call( const T0 & _t0, const T1 & _t1, const T2 & _t2, const T3 & _t3, const T4 & _t4 ) const
        {
            return this->call_i(
                detail::import_operator_t( m_kernel, _t0 ),
                detail::import_operator_t( m_kernel, _t1 ),
                detail::import_operator_t( m_kernel, _t2 ),
                detail::import_operator_t( m_kernel, _t3 ),
                detail::import_operator_t( m_kernel, _t4 )
            );
        }

        template<class T0, class T1, class T2, class T3, class T4, class T5>
        detail::extract_operator_t call( const T0 & _t0, const T1 & _t1, const T2 & _t2, const T3 & _t3, const T4 & _t4, const T5 & _t5 ) const
        {
            return this->call_i(
                detail::import_operator_t( m_kernel, _t0 ),
                detail::import_operator_t( m_kernel, _t1 ),
                detail::import_operator_t( m_kernel, _t2 ),
                detail::import_operator_t( m_kernel, _t3 ),
                detail::import_operator_t( m_kernel, _t4 ),
                detail::import_operator_t( m_kernel, _t5 )
            );
        }

        template<class T0, class T1, class T2, class T3, class T4, class T5, class T6>
        detail::extract_operator_t call( const T0 & _t0, const T1 & _t1, const T2 & _t2, const T3 & _t3, const T4 & _t4, const T5 & _t5, const T6 & _t6 ) const
        {
            return this->call_i(
                detail::import_operator_t( m_kernel, _t0 ),
                detail::import_operator_t( m_kernel, _t1 ),
                detail::import_operator_t( m_kernel, _t2 ),
                detail::import_operator_t( m_kernel, _t3 ),
                detail::import_operator_t( m_kernel, _t4 ),
                detail::import_operator_t( m_kernel, _t5 ),
                detail::import_operator_t( m_kernel, _t6 )
            );
        }

        template<class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7>
        detail::extract_operator_t call( const T0 & _t0, const T1 & _t1, const T2 & _t2, const T3 & _t3, const T4 & _t4, const T5 & _t5, const T6 & _t6, const T7 & _t7 ) const
        {
            return this->call_i(
                detail::import_operator_t( m_kernel, _t0 ),
                detail::import_operator_t( m_kernel, _t1 ),
                detail::import_operator_t( m_kernel, _t2 ),
                detail::import_operator_t( m_kernel, _t3 ),
                detail::import_operator_t( m_kernel, _t4 ),
                detail::import_operator_t( m_kernel, _t5 ),
                detail::import_operator_t( m_kernel, _t6 ),
                detail::import_operator_t( m_kernel, _t7 )
            );
        }

    public:
        detail::extract_operator_t call() const;
        detail::extract_operator_t call_i( const detail::import_operator_t & _t0 ) const;
        detail::extract_operator_t call_i( const detail::import_operator_t & _t0, const detail::import_operator_t & _t1 ) const;
        detail::extract_operator_t call_i( const detail::import_operator_t & _t0, const detail::import_operator_t & _t1, const detail::import_operator_t & _t2 ) const;
        detail::extract_operator_t call_i( const detail::import_operator_t & _t0, const detail::import_operator_t & _t1, const detail::import_operator_t & _t2, const detail::import_operator_t & _t3 ) const;
        detail::extract_operator_t call_i( const detail::import_operator_t & _t0, const detail::import_operator_t & _t1, const detail::import_operator_t & _t2, const detail::import_operator_t & _t3, const detail::import_operator_t & _t4 ) const;
        detail::extract_operator_t call_i( const detail::import_operator_t & _t0, const detail::import_operator_t & _t1, const detail::import_operator_t & _t2, const detail::import_operator_t & _t3, const detail::import_operator_t & _t4, const detail::import_operator_t & _t5 ) const;
        detail::extract_operator_t call_i( const detail::import_operator_t & _t0, const detail::import_operator_t & _t1, const detail::import_operator_t & _t2, const detail::import_operator_t & _t3, const detail::import_operator_t & _t4, const detail::import_operator_t & _t5, const detail::import_operator_t & _t6 ) const;
        detail::extract_operator_t call_i( const detail::import_operator_t & _t0, const detail::import_operator_t & _t1, const detail::import_operator_t & _t2, const detail::import_operator_t & _t3, const detail::import_operator_t & _t4, const detail::import_operator_t & _t5, const detail::import_operator_t & _t6, const detail::import_operator_t & _t7 ) const;

    public:
        template<class T0>
        detail::extract_operator_t call_args( const T0 & _t0, const args & _args ) const
        {
            return this->call_args_i(
                detail::import_operator_t( m_kernel, _t0 ),
                _args
            );
        }

        template<class T0, class T1>
        detail::extract_operator_t call_args( const T0 & _t0, const T1 & _t1
            , const args & _args ) const
        {
            return this->call_args_i(
                detail::import_operator_t( m_kernel, _t0 ),
                detail::import_operator_t( m_kernel, _t1 ),
                _args
            );
        }

        template<class T0, class T1, class T2>
        detail::extract_operator_t call_args( const T0 & _t0, const T1 & _t1, const T2 & _t2
            , const args & _args ) const
        {
            return this->call_args_i(
                detail::import_operator_t( m_kernel, _t0 ),
                detail::import_operator_t( m_kernel, _t1 ),
                detail::import_operator_t( m_kernel, _t2 ),
                _args
            );
        }

        template<class T0, class T1, class T2, class T3>
        detail::extract_operator_t call_args( const T0 & _t0, const T1 & _t1, const T2 & _t2, const T3 & _t3
            , const args & _args ) const
        {
            return this->call_args_i(
                detail::import_operator_t( m_kernel, _t0 ),
                detail::import_operator_t( m_kernel, _t1 ),
                detail::import_operator_t( m_kernel, _t2 ),
                detail::import_operator_t( m_kernel, _t3 ),
                _args
            );
        }

        template<class T0, class T1, class T2, class T3, class T4>
        detail::extract_operator_t call_args( const T0 & _t0, const T1 & _t1, const T2 & _t2, const T3 & _t3, const T4 & _t4
            , const args & _args ) const
        {
            return this->call_args_i(
                detail::import_operator_t( m_kernel, _t0 ),
                detail::import_operator_t( m_kernel, _t1 ),
                detail::import_operator_t( m_kernel, _t2 ),
                detail::import_operator_t( m_kernel, _t3 ),
                detail::import_operator_t( m_kernel, _t4 ),
                _args
            );
        }

        template<class T0, class T1, class T2, class T3, class T4, class T5>
        detail::extract_operator_t call_args( const T0 & _t0, const T1 & _t1, const T2 & _t2, const T3 & _t3, const T4 & _t4, const T5 & _t5
            , const args & _args ) const
        {
            return this->call_args_i(
                detail::import_operator_t( m_kernel, _t0 ),
                detail::import_operator_t( m_kernel, _t1 ),
                detail::import_operator_t( m_kernel, _t2 ),
                detail::import_operator_t( m_kernel, _t3 ),
                detail::import_operator_t( m_kernel, _t4 ),
                detail::import_operator_t( m_kernel, _t5 ),
                _args
            );
        }

        template<class T0, class T1, class T2, class T3, class T4, class T5, class T6>
        detail::extract_operator_t call_args( const T0 & _t0, const T1 & _t1, const T2 & _t2, const T3 & _t3, const T4 & _t4, const T5 & _t5, const T6 & _t6
            , const args & _args ) const
        {
            return this->call_args_i(
                detail::import_operator_t( m_kernel, _t0 ),
                detail::import_operator_t( m_kernel, _t1 ),
                detail::import_operator_t( m_kernel, _t2 ),
                detail::import_operator_t( m_kernel, _t3 ),
                detail::import_operator_t( m_kernel, _t4 ),
                detail::import_operator_t( m_kernel, _t5 ),
                detail::import_operator_t( m_kernel, _t6 ),
                _args
            );
        }

        template<class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7>
        detail::extract_operator_t call_args( const T0 & _t0, const T1 & _t1, const T2 & _t2, const T3 & _t3, const T4 & _t4, const T5 & _t5, const T6 & _t6, const T7 & _t7
            , const args & _args ) const
        {
            return this->call_args_i(
                detail::import_operator_t( m_kernel, _t0 ),
                detail::import_operator_t( m_kernel, _t1 ),
                detail::import_operator_t( m_kernel, _t2 ),
                detail::import_operator_t( m_kernel, _t3 ),
                detail::import_operator_t( m_kernel, _t4 ),
                detail::import_operator_t( m_kernel, _t5 ),
                detail::import_operator_t( m_kernel, _t6 ),
                detail::import_operator_t( m_kernel, _t7 ),
                _args
            );
        }

    public:
        detail::extract_operator_t call_args( const args & _args ) const;
        detail::extract_operator_t call_args_i( const detail::import_operator_t & _t0, const args & _args ) const;
        detail::extract_operator_t call_args_i( const detail::import_operator_t & _t0, const detail::import_operator_t & _t1, const args & _args ) const;
        detail::extract_operator_t call_args_i( const detail::import_operator_t & _t0, const detail::import_operator_t & _t1, const detail::import_operator_t & _t2, const args & _args ) const;
        detail::extract_operator_t call_args_i( const detail::import_operator_t & _t0, const detail::import_operator_t & _t1, const detail::import_operator_t & _t2, const detail::import_operator_t & _t3, const args & _args ) const;
        detail::extract_operator_t call_args_i( const detail::import_operator_t & _t0, const detail::import_operator_t & _t1, const detail::import_operator_t & _t2, const detail::import_operator_t & _t3, const detail::import_operator_t & _t4, const args & _args ) const;
        detail::extract_operator_t call_args_i( const detail::import_operator_t & _t0, const detail::import_operator_t & _t1, const detail::import_operator_t & _t2, const detail::import_operator_t & _t3, const detail::import_operator_t & _t4, const detail::import_operator_t & _t5, const args & _args ) const;
        detail::extract_operator_t call_args_i( const detail::import_operator_t & _t0, const detail::import_operator_t & _t1, const detail::import_operator_t & _t2, const detail::import_operator_t & _t3, const detail::import_operator_t & _t4, const detail::import_operator_t & _t5, const detail::import_operator_t & _t6, const args & _args ) const;
        detail::extract_operator_t call_args_i( const detail::import_operator_t & _t0, const detail::import_operator_t & _t1, const detail::import_operator_t & _t2, const detail::import_operator_t & _t3, const detail::import_operator_t & _t4, const detail::import_operator_t & _t5, const detail::import_operator_t & _t6, const detail::import_operator_t & _t7, const args & _args ) const;
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
    PYBIND_API pybind::object make_object_i( kernel_interface * _kernel, const detail::import_operator_t & _t0 );
    //////////////////////////////////////////////////////////////////////////
    template<class T>
    pybind::object make_object_t( kernel_interface * _kernel, const T & _t0 )
    {
        return make_object_i( _kernel, detail::import_operator_t( _kernel, _t0 ) );
    }
    PYBIND_API pybind::object make_invalid_object_t();
}