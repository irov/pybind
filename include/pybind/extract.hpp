#	pragma once

#	include "pybind/system.hpp"
#	include "pybind/type_cast.hpp"

#	include <exception>

namespace pybind
{
	namespace detail
	{
		template<class T>
		struct extract_check
		{
			static T extract( PyObject * _obj )
			{
				const type_info & tinfo = typeid(T);

				type_cast * etype = type_down_cast<T>::find();

				if( etype == 0 )
				{
					throw std::exception();
				}

				type_cast_result<T> * etype_impl = static_cast<type_cast_result<T> *>(etype);

				T t = etype_impl->apply( _obj );

				if( etype_impl->is_valid() == false )
				{
					pybind::check_error();
					throw std::exception();
				}

				return t;
			}
		};

		template<class T>
		struct extract_check<const T &>
		{
			static T extract( PyObject * _obj )
			{
				const type_info & tinfo = typeid(T);

				type_cast * etype = type_down_cast<T>::find();

				if( etype == 0 )
				{
					throw std::exception();
				}

				type_cast_result<const T &> * etype_impl = static_cast<type_cast_result<const T &> *>(etype);

				T t = etype_impl->apply( _obj );

				if( etype_impl->is_valid() == false )
				{
					throw std::exception();
				}

				return t;
			}
		};
	}

	template<class T>
	struct extract_return
	{
		typedef T type;
	};

	template<class T>
	struct extract_return<const T &>
	{
		typedef T type;
	};

	template<class T>
	typename extract_return<T>::type extract( PyObject * _obj )
	{
		return detail::extract_check<T>::extract( _obj );
	}

	template<class T>
	PyObject * ptr( T _value )
	{
		type_cast * etype = detail::type_down_cast<T>::find();

		if( etype == 0 )
		{
			ret_none();
		}

		type_cast_result<T> * etype_impl = static_cast<type_cast_result<T> *>(etype);

		PyObject * result = etype_impl->wrap( _value );

		if( result == 0 )
		{
			ret_none();
		}

		return result;
	}
}

