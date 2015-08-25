//#	pragma once
//
//#	include "pybind/type_cast.hpp"
//#	include "pybind/extract.hpp"
//
//#	include "config/stdex.hpp"
//
//namespace pybind
//{
//	template<class T, class V = stdex::intrusive_ptr<T> >
//	struct extract_stdex_intrusive_ptr_type
//		: public pybind::type_cast_result<V>
//	{
//		typedef typename pybind::type_cast_result<V>::TCastRef TCastRef;
//		
//		bool apply( PyObject * _obj, V & _ptr ) override
//		{
//			if( pybind::tuple_check( _obj ) == true )
//			{
//				size_t size = pybind::tuple_size( _obj );
//
//				_vector.reserve( size );
//
//				for( size_t it = 0; it != size; ++it )
//				{
//					PyObject * py_value = pybind::tuple_getitem( _obj, it );
//
//					T value;
//					if( pybind::extract_value( py_value, value ) == false )
//					{
//						return false;
//					}
//
//					_vector.push_back( value );
//				}
//			}
//			else if( pybind::list_check( _obj ) == true )
//			{
//				size_t size = pybind::list_size( _obj );
//
//				_vector.reserve( size );
//
//				for( size_t it = 0; it != size; ++it )
//				{
//					PyObject * py_value = pybind::list_getitem( _obj, it );
//
//					T value;
//					if( pybind::extract_value( py_value, value ) == false )
//					{
//						return false;
//					}
//
//					_vector.push_back( value );
//				}
//			}
//			else
//			{
//				return false;
//			}
//
//			return true;
//		}
//
//		PyObject * wrap( TCastRef _vector ) override
//		{
//			size_t vector_size = _vector.size();
//
//			PyObject * py_vector = pybind::list_new( vector_size );
//
//			pybind::list_appenditems_t( py_vector, _vector.begin(), _vector.end() );
//
//			return py_vector;
//		}
//	};
//}