#	pragma once

#	include "pybind/type_cast.hpp"
#	include "pybind/extract.hpp"

#	include <vector>
#	include <map>

namespace pybind
{
	template<class T, class V = std::vector<T> >
	struct extract_stl_vector_type
		: public pybind::type_cast_result<V>
	{
		typedef typename pybind::type_cast_result<V>::TCastRef TCastRef;

		bool apply( PyObject * _obj, V & _vector ) override
		{
			if( pybind::tuple_check( _obj ) == true )
			{
				size_t size = pybind::tuple_size( _obj );

				for( size_t it = 0; it != size; ++it )
				{
					PyObject * py_value = pybind::tuple_getitem( _obj, it );

					T value;
					if( pybind::extract_value( py_value, value ) == false )
					{
						return false;
					}

					_vector.push_back( value );
				}
			}
			else if( pybind::list_check( _obj ) == true )
			{
				size_t size = pybind::list_size( _obj );

				for( size_t it = 0; it != size; ++it )
				{
					PyObject * py_value = pybind::list_getitem( _obj, it );

					T value;
					if( pybind::extract_value( py_value, value ) == false )
					{
						return false;
					}

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
			PyObject * py_vector = pybind::list_new(0);

			for( typename V::const_iterator
				it = _vector.begin(),
				it_end = _vector.end();
			it != it_end;
			++it )
			{
				PyObject * py_value = pybind::ptr( *it );

				pybind::list_appenditem( py_vector, py_value );

				pybind::decref( py_value );
			}

			return py_vector;
		}
	};

	template<class K, class V, class M = std::map<K, V> >
	struct extract_stl_map_type
		: public pybind::type_cast_result<M>
	{
		typedef typename pybind::type_cast_result<M>::TCastRef TCastRef;

		bool apply( PyObject * _obj, M & _map ) override
		{
			if( pybind::dict_check( _obj ) == true )
			{
				size_t dict_pos;

				PyObject * py_key;
				PyObject * py_value;
				while( pybind::dict_next( _obj, dict_pos, &py_key, &py_value ) == true )
				{
					K key;
					if( pybind::extract_value( py_key, key ) == false )
					{
						return false;
					}

					V value;
					if( pybind::extract_value( py_value, value ) == false )
					{
						return false;
					}

					_map.insert( std::make_pair( key, value ) );
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

				PyObject * py_key = pybind::ptr( key );
				PyObject * py_value = pybind::ptr( value );

				pybind::dict_set( py_dict, py_key, py_value );

				pybind::decref( py_key );
				pybind::decref( py_value );
			}

			return py_dict;
		}
	};

	template<class T, class V = std::vector<T> >
	struct registration_stl_vector_type_cast
	{
		registration_stl_vector_type_cast()
		{
			size_t id = class_info<V>();

			detail::register_type_info_extract( id, new pybind::extract_stl_vector_type<T, V> );
		}
	};

	template<class T, class V = std::vector<T> >
	struct unregistration_stl_vector_type_cast
	{	
		unregistration_stl_vector_type_cast()
		{
			size_t id = class_info<V>();

			detail::unregister_type_info_extract( id );
		}
	};

	template<class K, class V, class M = std::map<K, V> >
	struct registration_stl_map_type_cast
	{
		registration_stl_map_type_cast()
		{
			size_t id = class_info<M>();

			detail::register_type_info_extract( id, new pybind::extract_stl_map_type<K, V, M> );
		}
	};

	template<class K, class V, class M = std::map<K, V> >
	struct unregistration_stl_map_type_cast
	{	
		unregistration_stl_map_type_cast()
		{
			size_t id = class_info<M>();

			detail::unregister_type_info_extract( id );
		}
	};

	PYBIND_API bool initialize_stl_type_cast();
	PYBIND_API void finalize_stl_type_cast();
}