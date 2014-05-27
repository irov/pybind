#	pragma once

#	include "pybind/type_cast.hpp"
#	include "pybind/extract.hpp"

#	include <stdex/intrusive_ptr.h>

namespace pybind
{
	template<class T, class V = stdex::intrusive_ptr<T> >
	struct extract_stdex_intrusive_ptr_type
		: public pybind::type_cast_result<V>
	{
		typedef typename pybind::type_cast_result<V>::TCastRef TCastRef;

		bool apply( PyObject * _obj, V & _ptr ) override
		{
			if( pybind::tuple_check( _obj ) == true )
			{
				size_t size = pybind::tuple_size( _obj );

				_vector.reserve( size );

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

				_vector.reserve( size );

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
			size_t vector_size = _vector.size();

			PyObject * py_vector = pybind::list_new( vector_size );

			for( typename V::size_type
				it = 0,
				it_end = vector_size;
			it != it_end;
			++it )
			{
				const T & value = _vector[it];

				PyObject * py_value = pybind::ptr( value );

				pybind::list_setitem( py_vector, it, py_value );
			}

			return py_vector;
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
}