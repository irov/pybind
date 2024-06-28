#pragma once

#include "pybind/exception.hpp"

#include "pybind/type_cast_result.hpp"

#include "stdex/mpl.h"
#include "stdex/intrusive_ptr.h"

#include <typeinfo>
#include <type_traits>
#include <utility>

namespace pybind
{
    class object;
    class list;
    class tuple;
    class dict;
    //////////////////////////////////////////////////////////////////////////
    namespace detail
    {
        template<class T>
        struct extract_check
        {
            typedef typename stdex::mpl::remove_cref<T>::type T_WOCR;

            static bool extract( kernel_interface * _kernel, PyObject * _obj, T_WOCR & _value, bool _nothrow )
            {
                if( _obj == nullptr )
                {
                    if( _nothrow == false )
                    {
                        const std::type_info & tinfo = typeid(T_WOCR);

                        const char * type_name = tinfo.name();

                        pybind::throw_exception( "extract obj is NULL for '%.256s'"
                            , type_name
                        );
                    }

                    return false;
                }

                type_cast * etype = type_down_cast<T_WOCR>::find( _kernel );

                if( etype == nullptr )
                {
                    if( _nothrow == false )
                    {
                        const std::type_info & tinfo = typeid(T_WOCR);

                        const char * type_name = tinfo.name();

                        _kernel->throw_message( "extract: extract invalid find cast for '%.256s'"
                            , type_name
                        );
                    }

                    return false;
                }

                typedef type_cast_result<T_WOCR> type_cast_result_T_WOCR;

                type_cast_result_T_WOCR * etype_impl = static_cast<type_cast_result_T_WOCR *>(etype);

                if( etype_impl->apply( _kernel, _obj, _value, false ) == false )
                {
                    if( _nothrow == false )
                    {
                        const std::type_info & tinfo = typeid(T_WOCR);

                        const char * type_name = tinfo.name();

                        string_view repr_obj = _kernel->object_repr( _obj );

                        if( repr_obj.is_valid() == true )
                        {
                            _kernel->throw_message( "extract from '%.256s' type '%.256s' to '%.256s'"
                                , repr_obj.c_str()
                                , _kernel->object_repr_type( _obj ).c_str()
                                , type_name
                            );
                        }
                        else
                        {
                            _kernel->throw_message( "extract from xxxx to '%.256s'"
                                , type_name
                            );
                        }
                    }

                    return false;
                }

                return true;
            }
        };
    }
    //////////////////////////////////////////////////////////////////////////
    template<class T>
    bool extract_value( kernel_interface * _kernel, PyObject * _obj, T & _value, bool _nothrow )
    {
        bool result = detail::extract_check<T>::extract( _kernel, _obj, _value, _nothrow );

        return result;
    }
    //////////////////////////////////////////////////////////////////////////
    PYBIND_API bool extract_value( kernel_interface * _kernel, PyObject * _obj, bool & _value, bool _nothrow );
    PYBIND_API bool extract_value( kernel_interface * _kernel, PyObject * _obj, int8_t & _value, bool _nothrow );
    PYBIND_API bool extract_value( kernel_interface * _kernel, PyObject * _obj, int16_t & _value, bool _nothrow );
    PYBIND_API bool extract_value( kernel_interface * _kernel, PyObject * _obj, int32_t & _value, bool _nothrow );
    PYBIND_API bool extract_value( kernel_interface * _kernel, PyObject * _obj, int64_t & _value, bool _nothrow );
    PYBIND_API bool extract_value( kernel_interface * _kernel, PyObject * _obj, uint8_t & _value, bool _nothrow );
    PYBIND_API bool extract_value( kernel_interface * _kernel, PyObject * _obj, uint16_t & _value, bool _nothrow );
    PYBIND_API bool extract_value( kernel_interface * _kernel, PyObject * _obj, uint32_t & _value, bool _nothrow );
    PYBIND_API bool extract_value( kernel_interface * _kernel, PyObject * _obj, uint64_t & _value, bool _nothrow );
    PYBIND_API bool extract_value( kernel_interface * _kernel, PyObject * _obj, wchar_t & _value, bool _nothrow );
    PYBIND_API bool extract_value( kernel_interface * _kernel, PyObject * _obj, float & _value, bool _nothrow );
    PYBIND_API bool extract_value( kernel_interface * _kernel, PyObject * _obj, double & _value, bool _nothrow );
    PYBIND_API bool extract_value( kernel_interface * _kernel, PyObject * _obj, const char *& _value, bool _nothrow );
    PYBIND_API bool extract_value( kernel_interface * _kernel, PyObject * _obj, const wchar_t *& _value, bool _nothrow );
    PYBIND_API bool extract_value( kernel_interface * _kernel, PyObject * _obj, PyObject *& _value, bool _nothrow );
    PYBIND_API bool extract_value( kernel_interface * _kernel, PyObject * _obj, PyTypeObject *& _value, bool _nothrow );
    PYBIND_API bool extract_value( kernel_interface * _kernel, PyObject * _obj, pybind::object & _value, bool _nothrow );
    PYBIND_API bool extract_value( kernel_interface * _kernel, PyObject * _obj, pybind::list & _value, bool _nothrow );
    PYBIND_API bool extract_value( kernel_interface * _kernel, PyObject * _obj, pybind::tuple & _value, bool _nothrow );
    PYBIND_API bool extract_value( kernel_interface * _kernel, PyObject * _obj, pybind::dict & _value, bool _nothrow );
    //////////////////////////////////////////////////////////////////////////
    template<class T>
    typename stdex::mpl::remove_cref<T>::type extract_throw( kernel_interface * _kernel, PyObject * _obj )
    {
        typedef typename stdex::mpl::remove_cref<T>::type type_value;
        type_value value;

        if( pybind::extract_value( _kernel, _obj, value, false ) == false )
        {
            const std::type_info & tinfo = typeid(type_value);

            const char * type_name = tinfo.name();

            pybind::throw_exception( "extract_throw<T>: extract invalid '%s:%s' not cast to '%s'"
                , _kernel->object_repr( _obj ).c_str()
                , _kernel->object_repr_type( _obj ).c_str()
                , type_name
            );
        }

        return std::move( value );
    }
    //////////////////////////////////////////////////////////////////////////
    template<typename T, class = void>
    struct extract_specialized
    {
        typename stdex::mpl::remove_cref<T>::type operator () ( kernel_interface * _kernel, PyObject * _obj ) const
        {
            typedef typename stdex::mpl::remove_cref<T>::type type_value;

            type_value value;
            pybind::extract_value( _kernel, _obj, value, true );

            return std::move( value );
        }
    };
    //////////////////////////////////////////////////////////////////////////
    template<class T>
    struct extract_specialized<T
        , std::enable_if_t<std::is_enum_v<T>>
    >
    {
        T operator () ( kernel_interface * _kernel, PyObject * _obj ) const
        {
            uint32_t value;
            pybind::extract_value( _kernel, _obj, value, true );

            return static_cast<T>(value);
        }
    };
    //////////////////////////////////////////////////////////////////////////
    template<typename T>
    struct extract_specialized<T *>
    {
        T * operator () ( kernel_interface * _kernel, PyObject * _obj ) const
        {
            T * value = nullptr;
            pybind::extract_value( _kernel, _obj, value, true );

            return value;
        }
    };
    //////////////////////////////////////////////////////////////////////////
    template<typename T, typename D>
    struct extract_specialized<stdex::intrusive_ptr<T, D> >
    {
        stdex::intrusive_ptr<T, D> operator () ( kernel_interface * _kernel, PyObject * _obj ) const
        {
            T * value = nullptr;
            pybind::extract_value( _kernel, _obj, value, true );

            return stdex::intrusive_ptr<T, D>( value );
        }
    };
    //////////////////////////////////////////////////////////////////////////
    template<class T>
    typename stdex::mpl::remove_cref<T>::type extract( kernel_interface * _kernel, PyObject * _obj )
    {
        return extract_specialized<T>()(_kernel, _obj);
    }
    //////////////////////////////////////////////////////////////////////////
    template<class T>
    PyObject * ptr_throw_i( kernel_interface * _kernel, const T & _value )
    {
        typedef typename stdex::mpl::remove_cref<T>::type T_WOCR;

        type_cast * etype = detail::type_down_cast<T_WOCR>::find( _kernel );

        if( etype == nullptr )
        {
            const std::type_info & tinfo = typeid(T_WOCR);
            const char * type_name = tinfo.name();

            pybind::throw_exception( "ptr invalid find cast for '%.256s'"
                , type_name
            );
        }

        typedef type_cast_result<T_WOCR> type_cast_result_T_WOCR;

        type_cast_result_T_WOCR * etype_impl = static_cast<type_cast_result_T_WOCR *>(etype);

        PyObject * result = etype_impl->wrap( _kernel, _value );

        if( result == nullptr )
        {
            const std::type_info & tinfo = typeid(T_WOCR);

            const char * type_name = tinfo.name();

            pybind::throw_exception( "ptr invalid wrap '%.256s'"
                , type_name
            );
        }

        return result;
    }
    //////////////////////////////////////////////////////////////////////////
    PYBIND_API PyObject * ptr_throw_i( kernel_interface * _kernel, bool _value );
    PYBIND_API PyObject * ptr_throw_i( kernel_interface * _kernel, int8_t _value );
    PYBIND_API PyObject * ptr_throw_i( kernel_interface * _kernel, int16_t _value );
    PYBIND_API PyObject * ptr_throw_i( kernel_interface * _kernel, int32_t _value );
    PYBIND_API PyObject * ptr_throw_i( kernel_interface * _kernel, int64_t _value );
    PYBIND_API PyObject * ptr_throw_i( kernel_interface * _kernel, uint8_t _value );
    PYBIND_API PyObject * ptr_throw_i( kernel_interface * _kernel, uint16_t _value );
    PYBIND_API PyObject * ptr_throw_i( kernel_interface * _kernel, uint32_t _value );
    PYBIND_API PyObject * ptr_throw_i( kernel_interface * _kernel, uint64_t _value );
    PYBIND_API PyObject * ptr_throw_i( kernel_interface * _kernel, wchar_t _value );
    PYBIND_API PyObject * ptr_throw_i( kernel_interface * _kernel, float _value );
    PYBIND_API PyObject * ptr_throw_i( kernel_interface * _kernel, double _value );
    PYBIND_API PyObject * ptr_throw_i( kernel_interface * _kernel, char * _value );
    PYBIND_API PyObject * ptr_throw_i( kernel_interface * _kernel, wchar_t * _value );
    PYBIND_API PyObject * ptr_throw_i( kernel_interface * _kernel, const char * _value );
    PYBIND_API PyObject * ptr_throw_i( kernel_interface * _kernel, const wchar_t * _value );
    PYBIND_API PyObject * ptr_throw_i( kernel_interface * _kernel, PyObject * _value );
    PYBIND_API PyObject * ptr_throw_i( kernel_interface * _kernel, PyTypeObject * _value );
    PYBIND_API PyObject * ptr_throw_i( kernel_interface * _kernel, const pybind::object & _value );
    PYBIND_API PyObject * ptr_throw_i( kernel_interface * _kernel, const pybind::tuple & _value );
    PYBIND_API PyObject * ptr_throw_i( kernel_interface * _kernel, const pybind::list & _value );
    PYBIND_API PyObject * ptr_throw_i( kernel_interface * _kernel, const pybind::dict & _value );
    //////////////////////////////////////////////////////////////////////////
    template<class T, class = void>
    struct ptr_throw_specialized2
    {
        PyObject * operator () ( kernel_interface * _kernel, const T & _t ) const
        {
            return pybind::ptr_throw_i( _kernel, _t );
        }
    };
    //////////////////////////////////////////////////////////////////////////
    template<class T>
    struct ptr_throw_specialized2<T
        , std::enable_if_t<std::is_enum_v<T>>
    >
    {
        PyObject * operator () ( kernel_interface * _kernel, T _t ) const
        {
            return pybind::ptr_throw_i( _kernel, static_cast<uint32_t>(_t) );
        }
    };
    //////////////////////////////////////////////////////////////////////////
    namespace detail
    {
        template<typename T>
        class has_getEmbed
        {
        private:
            template <typename C> static std::true_type test( decltype(&C::getEmbed) );
            template <typename C> static std::false_type test( ... );

        public:
            static constexpr bool value = std::is_same<std::true_type, decltype(test<T>( 0 ))>::value;
        };
    }
    //////////////////////////////////////////////////////////////////////////
    template<class T>
    struct ptr_throw_specialized2<T *
        , std::enable_if_t<detail::has_getEmbed<T>::value>
    >
    {
        PyObject * operator () ( kernel_interface * _kernel, T * _t ) const
        {
            if( _t == nullptr )
            {
                return _kernel->ret_none();
            }

            PyObject * py_obj = _t->getEmbed( _kernel );

            return py_obj;
        }
    };
    //////////////////////////////////////////////////////////////////////////
    template<class T, class = void>
    struct ptr_throw_specialized
    {
        PyObject * operator () ( kernel_interface * _kernel, const T & _t ) const
        {
            return ptr_throw_specialized2<T>()(_kernel, _t);
        }
    };
    //////////////////////////////////////////////////////////////////////////
    template<class T, class D>
    struct ptr_throw_specialized<stdex::intrusive_ptr<T, D>>
    {
        PyObject * operator () ( kernel_interface * _kernel, const stdex::intrusive_ptr<T, D> & _t ) const
        {
            T * t_ptr = _t.get();

            return ptr_throw_specialized2<T *>()(_kernel, t_ptr);
        }
    };
    //////////////////////////////////////////////////////////////////////////
    template<class T>
    PyObject * ptr_throw( kernel_interface * _kernel, const T & _value )
    {
        PyObject * py_ptr = ptr_throw_specialized<T>()(_kernel, _value);

        return py_ptr;
    }
    //////////////////////////////////////////////////////////////////////////
    template<class T>
    PyObject * ptr( kernel_interface * _kernel, const T & _value )
    {
        try
        {
            PyObject * value = pybind::ptr_throw( _kernel, _value );

            return value;
        }
        catch( const pybind::pybind_exception & _ex )
        {
            _kernel->error_message( "ptr value: %s"
                , _ex.what()
            );
        }

        return nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
}

