#	pragma once

#	include "pybind/type_cast.hpp"
#	include "pybind/extract.hpp"

#	include "pybind/list.hpp"

#	include <utility>

namespace pybind
{
	template<class T, class V>
	struct extract_stl_vector_type
		: public pybind::type_cast_result<V>
	{
		typedef typename pybind::type_cast_result<V>::TCastRef TCastRef;

		bool apply( PyObject * _obj, V & _vector, bool _nothrow ) override
		{
			(void)_nothrow;

			if( pybind::tuple_check( _obj ) == true )
			{
				size_t size = pybind::tuple_size( _obj );

				_vector.reserve( size );

				for( size_t it = 0; it != size; ++it )
				{
					T value = pybind::tuple_getitem_t( _obj, it );

					_vector.push_back( value );
				}
			}
			else if( pybind::list_check( _obj ) == true )
			{
				size_t size = pybind::list_size( _obj );

				_vector.reserve( size );

				for( size_t it = 0; it != size; ++it )
				{
					T value = pybind::list_getitem_t( _obj, it );

					_vector.push_back( value );
				}
			}
			else
			{
				return false;
			}

			return true;
		}

		PyObject * wrap( TCastRef _vector ) override
		{
			pybind::list py_vector = pybind::make_list_container_t( _vector );

			return py_vector.ret();
		}
	};

	template<class K, class V, class M>
	struct extract_stl_map_type
		: public pybind::type_cast_result<M>
	{
		typedef typename pybind::type_cast_result<M>::TCastRef TCastRef;

		bool apply( PyObject * _obj, M & _map, bool _nothrow ) override
		{
			(void)_nothrow;

			if( pybind::dict_check( _obj ) == true )
			{
				size_t dict_pos = 0;

				K key;
				V value;
				while( pybind::dict_next_t( _obj, dict_pos, key, value ) == true )
				{
					_map.insert( std::make_pair(key, value) );
				}
			}
			else
			{
				return false;
			}

			return true;
		}

		PyObject * wrap( TCastRef _value ) override
		{
			PyObject * py_dict = pybind::dict_new();

			for( typename M::const_iterator
				it = _value.begin(),
				it_end = _value.end();
			it != it_end;
			++it )
			{
				const K & key = it->first;
				const V & value = it->second;

				pybind::dict_set_t( py_dict, key, value );
			}

			return py_dict;
		}
	};

	template<class T, class V>	
	inline void registration_stl_vector_type_cast()
	{
		uint32_t id = detail::class_info<V>();

		detail::register_type_info_extract( id, new pybind::extract_stl_vector_type<T, V> );
	}

	template<class T, class V>
	inline void unregistration_stl_vector_type_cast()
	{
		uint32_t id = detail::class_info<V>();

		detail::unregister_type_info_extract( id );
	}

	template<class K, class V, class M>
	inline void registration_stl_map_type_cast()
	{
		uint32_t id = detail::class_info<M>();

		detail::register_type_info_extract( id, new pybind::extract_stl_map_type<K, V, M> );
	}
	
	template<class K, class V, class M>
	inline void unregistration_stl_map_type_cast()
	{
		uint32_t id = detail::class_info<M>();

		detail::unregister_type_info_extract( id );
	}

	PYBIND_API bool initialize_stl_type_cast();
	PYBIND_API void finalize_stl_type_cast();
}