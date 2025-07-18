#pragma once

#include "pybind/exports.hpp"
#include "pybind/type_cast_result.hpp"
#include "pybind/make_type_cast.hpp"
#include "pybind/extract.hpp"
#include "pybind/list.hpp"

#ifdef PYBIND_STL_SUPPORT

namespace pybind
{
    template<class T, class V>
    struct extract_stl_optional_type
        : public pybind::type_cast_result<V>
    {
        typedef typename pybind::type_cast_result<V>::TCastRef TCastRef;

        bool apply( kernel_interface * _kernel, PyObject * _obj, V & _optional, bool _nothrow ) override
        {
            (void)_nothrow;

            if( _kernel->is_none( _obj ) == true )
            {
                _optional.reset();

                return true;
            }

            T value = pybind::extract<T>( _kernel, _obj );

            _optional.emplace( value );

            return true;
        }

        PyObject * wrap( kernel_interface * _kernel, TCastRef _optional ) override
        {
            if( _optional.has_value() == false )
            {
                return _kernel->ret_none();
            }

            T value = *_optional;

            PyObject * ptr = pybind::ptr( _kernel, value );

            return ptr;
        }
    };

    template<class T, class V>
    struct extract_stl_vector_type
        : public pybind::type_cast_result<V>
    {
        typedef typename pybind::type_cast_result<V>::TCastRef TCastRef;

        bool apply( kernel_interface * _kernel, PyObject * _obj, V & _vector, bool _nothrow ) override
        {
            (void)_nothrow;

            if( _kernel->tuple_check( _obj ) == true )
            {
                size_t size = _kernel->tuple_size( _obj );

                _vector.reserve( size );

                for( size_t it = 0; it != size; ++it )
                {
                    T value = pybind::tuple_getitem_t( _kernel, _obj, it );

                    _vector.push_back( value );
                }
            }
            else if( _kernel->list_check( _obj ) == true )
            {
                size_t size = _kernel->list_size( _obj );

                _vector.reserve( size );

                for( size_t it = 0; it != size; ++it )
                {
                    T value = pybind::list_getitem_t( _kernel, _obj, it );

                    _vector.push_back( value );
                }
            }
            else
            {
                return false;
            }

            return true;
        }

        PyObject * wrap( kernel_interface * _kernel, TCastRef _vector ) override
        {
            pybind::list py_vector = pybind::make_list_container_t( _kernel, _vector );

            return py_vector.ret();
        }
    };

    template<class K, class V, class M>
    struct extract_stl_map_type
        : public pybind::type_cast_result<M>
    {
        typedef typename pybind::type_cast_result<M>::TCastRef TCastRef;

        bool apply( kernel_interface * _kernel, PyObject * _obj, M & _map, bool _nothrow ) override
        {
            (void)_nothrow;

            if( _kernel->dict_check( _obj ) == true )
            {
                size_t pos = 0;

                K key;
                V value;
                while( pybind::dict_next_t( _kernel, _obj, &pos, &key, &value ) == true )
                {
                    _map.insert( typename M::value_type( key, value ) );
                }
            }
            else
            {
                return false;
            }

            return true;
        }

        PyObject * wrap( kernel_interface * _kernel, TCastRef _value ) override
        {
            PyObject * py_dict = _kernel->dict_new();

            for( typename M::const_iterator
                it = _value.begin(),
                it_end = _value.end();
                it != it_end;
                ++it )
            {
                const K & key = it->first;
                const V & value = it->second;

                pybind::dict_set_t( _kernel, py_dict, key, value );
            }

            return py_dict;
        }
    };

    template<class T>
    inline void registration_stl_optional_type_cast( kernel_interface * _kernel )
    {
        _kernel->register_type_info_extract_t<T>( pybind::make_type_cast<extract_stl_optional_type<typename T::value_type, T>>(_kernel) );
    }

    template<class T>
    inline void unregistration_stl_optional_type_cast( kernel_interface * _kernel )
    {
        _kernel->unregister_type_info_extract_t<T>();
    }

    template<class T>
    inline void registration_stl_vector_type_cast( kernel_interface * _kernel )
    {
        _kernel->register_type_info_extract_t<T>( pybind::make_type_cast<extract_stl_vector_type<typename T::value_type, T>>(_kernel) );
    }

    template<class T>
    inline void unregistration_stl_vector_type_cast( kernel_interface * _kernel )
    {
        _kernel->unregister_type_info_extract_t<T>();
    }

    template<class T>
    inline void registration_stl_map_type_cast( kernel_interface * _kernel )
    {
        _kernel->register_type_info_extract_t<T>( pybind::make_type_cast<extract_stl_map_type<typename T::key_type, typename T::mapped_type, T>>(_kernel) );
    }

    template<class T>
    inline void unregistration_stl_map_type_cast( kernel_interface * _kernel )
    {
        _kernel->unregister_type_info_extract_t<T>();
    }

    PYBIND_API bool initialize_stl_type_cast( kernel_interface * _kernel );
    PYBIND_API void finalize_stl_type_cast( kernel_interface * _kernel );
}

#endif
