#pragma once

#include <type_traits>
#include <utility>

namespace pybind
{
    namespace mpl
    {
        //////////////////////////////////////////////////////////////////////////
        template<class T, class = void>
        struct has_c_str
            : std::false_type
        {
        };
        //////////////////////////////////////////////////////////////////////////
        template<class T>
        struct has_c_str<T, std::void_t<decltype( std::declval<const T &>().c_str() )>>
            : std::is_convertible<decltype( std::declval<const T &>().c_str() ), const char *>
        {
        };
        //////////////////////////////////////////////////////////////////////////
        template<class T>
        struct is_char_pointer
            : std::integral_constant<bool
                , std::is_same<typename std::decay<T>::type, const char *>::value == true
                || std::is_same<typename std::decay<T>::type, char *>::value == true
            >
        {
        };
        //////////////////////////////////////////////////////////////////////////
        template<class T>
        struct is_string_like
            : std::integral_constant<bool, has_c_str<T>::value == true || is_char_pointer<T>::value == true>
        {
        };
        //////////////////////////////////////////////////////////////////////////
        template<class T>
        const char * string_like_c_str( const T & _value )
        {
            if constexpr( has_c_str<T>::value == true )
            {
                return _value.c_str();
            }
            else
            {
                return _value;
            }
        }
        //////////////////////////////////////////////////////////////////////////
    }
}
