#pragma once

#include "pybind/return_operator.hpp"
#include "pybind/import_operator.hpp"
#include "pybind/extract_operator.hpp"
#include "pybind/args.hpp"

#include <tuple>
#include <initializer_list>
#include <utility>

namespace pybind
{
    //////////////////////////////////////////////////////////////////////////
    template<>
    struct ptr_throw_specialized <detail::import_operator_t>;
    //////////////////////////////////////////////////////////////////////////
    PYBIND_API args make_args_t( kernel_interface * _kernel, PyObject * _tuple, size_t _size );
    //////////////////////////////////////////////////////////////////////////
    PYBIND_API detail::extract_operator_t list_getitem_t( kernel_interface * _kernel, PyObject * _list, size_t _it );
    PYBIND_API bool list_setitem_i( kernel_interface * _kernel, PyObject * _list, size_t _it, const detail::import_operator_t & _item );
    PYBIND_API bool list_appenditem_i( kernel_interface * _kernel, PyObject * _obj, const detail::import_operator_t & _item );
    //////////////////////////////////////////////////////////////////////////
    template<class T>
    bool list_setitem_t( kernel_interface * _kernel, PyObject * _list, size_t _it, T && _item )
    {
        return list_setitem_i( _kernel, _list, _it, detail::import_operator_t( _kernel, std::forward<T>( _item ) ) );
    }
    //////////////////////////////////////////////////////////////////////////
    template<class T>
    bool list_appenditem_t( kernel_interface * _kernel, PyObject * _obj, T && _item )
    {
        return list_appenditem_i( _kernel, _obj, detail::import_operator_t( _kernel, std::forward<T>( _item ) ) );
    }
    //////////////////////////////////////////////////////////////////////////
    template<class It>
    bool list_appenditems_t( kernel_interface * _kernel, PyObject * _obj, It _begin, It _end )
    {
        for( It it = _begin; it != _end; ++it )
        {
            if( pybind::list_appenditem_t( _kernel, _obj, *it ) == false )
            {
                return false;
            }
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    PYBIND_API detail::extract_operator_t tuple_getitem_t( kernel_interface * _kernel, PyObject * _tuple, size_t _it );
    PYBIND_API bool tuple_setitem_i( kernel_interface * _kernel, PyObject * _tuple, size_t _it, const detail::extract_operator_t & _item );
    PYBIND_API bool tuple_setitem_i( kernel_interface * _kernel, PyObject * _tuple, size_t _it, const detail::import_operator_t & _item );
    //////////////////////////////////////////////////////////////////////////	
    template<class T>
    bool tuple_setitem_t( kernel_interface * _kernel, PyObject * _tuple, size_t _it, T && _item )
    {
        return tuple_setitem_i( _kernel, _tuple, _it, detail::import_operator_t( _kernel, std::forward<T>( _item ) ) );
    }
    //////////////////////////////////////////////////////////////////////////
    PYBIND_API bool dict_setstring_i( kernel_interface * _kernel, PyObject * _dict, const char * _key, detail::import_operator_t && _value );
    PYBIND_API bool dict_setobject_i( kernel_interface * _kernel, PyObject * _dict, PyObject * _key, detail::import_operator_t && _value );
    PYBIND_API detail::extract_operator_t dict_get_i( kernel_interface * _kernel, PyObject * _dict, detail::import_operator_t && _key );
    PYBIND_API bool dict_set_i( kernel_interface * _kernel, PyObject * _dict, detail::import_operator_t && _name, detail::import_operator_t && _value );
    PYBIND_API bool dict_remove_i( kernel_interface * _kernel, PyObject * _dict, const detail::import_operator_t & _key );
    //////////////////////////////////////////////////////////////////////////
    template<class V>
    bool dict_setstring_t( kernel_interface * _kernel, PyObject * _dict, const char * _name, V && _value )
    {
        return dict_setstring_i( _kernel, _dict, _name
            , detail::import_operator_t( _kernel, std::forward<V>( _value ) ) 
        );
    }
    //////////////////////////////////////////////////////////////////////////
    template<class V>
    bool dict_setobject_t( kernel_interface * _kernel, PyObject * _dict, PyObject * _name, V && _value )
    {
        return dict_setobject_i( _kernel, _dict, _name
            , detail::import_operator_t( _kernel, std::forward<V>( _value ) ) 
        );
    }
    //////////////////////////////////////////////////////////////////////////
    template<class K, class V>
    bool dict_set_t( kernel_interface * _kernel, PyObject * _dict, K && _name, V && _value )
    {
        return dict_set_i( _kernel, _dict
            , detail::import_operator_t( _kernel, std::forward<K>( _name ) )
            , detail::import_operator_t( _kernel, std::forward<V>( _value ) )
        );
    }
    //////////////////////////////////////////////////////////////////////////
    template<class K>
    detail::extract_operator_t dict_get_t( kernel_interface * _kernel, PyObject * _dict, K && _key )
    {
        return dict_get_i( _kernel, _dict
            , detail::import_operator_t( _kernel, std::forward<K>( _key ) ) 
        );
    }
    //////////////////////////////////////////////////////////////////////////
    template<class K>
    bool dict_remove_t( kernel_interface * _kernel, PyObject * _dict, K && _key )
    {
        return dict_remove_i( _kernel, _dict
            , detail::import_operator_t( _kernel, std::forward<K>( _key ) )
        );
    }
    //////////////////////////////////////////////////////////////////////////
    template<class K, class V>
    bool dict_next_t( kernel_interface * _kernel, PyObject * _dict, size_t * const _pos, K * const _key, V * const _value )
    {
        PyObject * py_key;
        PyObject * py_value;

        if( _kernel->dict_next( _dict, _pos, &py_key, &py_value ) == false )
        {
            return false;
        }

        *_key = pybind::extract<K>( _kernel, py_key );
        *_value = pybind::extract<V>( _kernel, py_value );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    PYBIND_API bool set_set_i( kernel_interface * _kernel, PyObject * _set, detail::import_operator_t && _value );
    PYBIND_API bool set_remove_i( kernel_interface * _kernel, PyObject * _set, const detail::import_operator_t & _value );
    PYBIND_API bool set_exist_i( kernel_interface * _kernel, PyObject * _set, const detail::import_operator_t & _value );
    //////////////////////////////////////////////////////////////////////////
    template<class V>
    bool set_set_t( kernel_interface * _kernel, PyObject * _set, V && _value )
    {
        return set_set_i( _kernel, _set
            , detail::import_operator_t( _kernel, std::forward<V>( _value ) ) 
        );
    }
    //////////////////////////////////////////////////////////////////////////
    template<class K>
    bool set_remove_t( kernel_interface * _kernel, PyObject * _set, K && _value )
    {
        return set_remove_i( _kernel, _set
            , detail::import_operator_t( _kernel, std::forward<K>( _value ) ) 
        );
    }
    //////////////////////////////////////////////////////////////////////////
    PYBIND_API detail::extract_operator_t ask_tuple_t( kernel_interface * _kernel, PyObject * _obj, pybind::tuple && _tuple );
    //////////////////////////////////////////////////////////////////////////
    PYBIND_API detail::extract_operator_t call_i( kernel_interface * _kernel, PyObject * _obj, std::initializer_list<detail::import_operator_t> && _t );
    //////////////////////////////////////////////////////////////////////////
    template<class ... T>
    detail::extract_operator_t call_t( kernel_interface * _kernel, PyObject * _obj, T && ... _t )
    {
        return call_i( _kernel, _obj
            , {detail::import_operator_t( _kernel, std::forward<T>( _t ) ) ...}
        );
    }
    //////////////////////////////////////////////////////////////////////////
    PYBIND_API detail::extract_operator_t call_args_i( kernel_interface * _kernel, PyObject * _obj, std::initializer_list<detail::import_operator_t> && _t, const args & _args );
    PYBIND_API detail::extract_operator_t call_args_i( kernel_interface * _kernel, PyObject * _obj, std::initializer_list<detail::import_operator_t> && _t, args && _args );
    //////////////////////////////////////////////////////////////////////////
    template<class ... T, size_t ... I>
    detail::extract_operator_t call_args_t_i( kernel_interface * _kernel, PyObject * _obj, std::tuple<T ...> && _t, std::integer_sequence<size_t, I...>, const args & _args )
    {
        return call_args_i( _kernel, _obj
            , {detail::import_operator_t( _kernel, std::get<I>( _t ) ) ...}
            , _args
        );
    }
    //////////////////////////////////////////////////////////////////////////
    template<class ... T, size_t ... I>
    detail::extract_operator_t call_args_t_i( kernel_interface * _kernel, PyObject * _obj, std::tuple<T ...> && _t, std::integer_sequence<size_t, I...>, args && _args )
    {
        return call_args_i( _kernel, _obj
            , {detail::import_operator_t( _kernel, std::get<I>( _t ) ) ...}
            , std::forward<args>( _args )
        );
    }
    //////////////////////////////////////////////////////////////////////////
    template<class ... T>
    detail::extract_operator_t call_args_t( kernel_interface * _kernel, PyObject * _obj, T && ... _t, const args & _args )
    {
        auto t = std::forward_as_tuple( std::forward<T>( _t ) ... );

        return call_args_t_i( _kernel, _obj
            , std::forward<std::tuple<T ...>>( t )
            , std::make_integer_sequence<size_t, sizeof ... (T) - 1u>()
            , _args
        );
    }
    //////////////////////////////////////////////////////////////////////////
    template<class ... T>
    detail::extract_operator_t call_args_t( kernel_interface * _kernel, PyObject * _obj, T && ... _t, args && _args )
    {
        auto t = std::forward_as_tuple( std::forward<T>( _t ) ... );

        return call_args_t_i( _kernel, _obj
            , std::forward<std::tuple<T ...>>( t )
            , std::make_integer_sequence<size_t, sizeof ... (T) - 1u>()
            , std::forward<args>( _args )
        );
    }
    //////////////////////////////////////////////////////////////////////////
    PYBIND_API detail::extract_operator_t extract_t( kernel_interface * _kernel, PyObject * _obj );
    //////////////////////////////////////////////////////////////////////////
}
