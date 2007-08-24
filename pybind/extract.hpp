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
					throw std::exception("invalid extract type");
				}

				type_cast_result<T> * etype_impl = static_cast<type_cast_result<T> *>(etype);

				etype_impl->apply( _obj );

				if( etype_impl->is_valid() == false )
				{
					throw std::exception("invalid extract type");
				}

				return etype_impl->result();
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
					throw std::exception("invalid extract type");
				}

				type_cast_result<const T &> * etype_impl = static_cast<type_cast_result<const T &> *>(etype);

				etype_impl->apply( _obj );

				if( etype_impl->is_valid() == false )
				{
					throw std::exception("invalid extract type");
				}

				return etype_impl->result();
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
	PyObject * wrapp( PyObject * _type, T * _impl )
	{
		
	}

	template<class T>
	PyObject * ptr( T _value )
	{
		type_cast * etype = detail::type_down_cast<T>::find();

		if( etype == 0 )
		{
			throw std::exception("invalid ptr type");
		}

		type_cast_result<T> * etype_impl = static_cast<type_cast_result<T> *>(etype);

		PyObject * result = etype_impl->wrapp( _value );

		if( result == 0 )
		{
			ret_none();
		}

		return result;
	}

	template<class T>
	PyObject * ptr( T _value, const type_info & _tinfo )
	{
		type_cast * etype = detail::find_type_info_extract( _tinfo );

		if( etype == 0 )
		{
			throw std::exception("invalid ptr type");
		}

		type_cast_result<T> * etype_impl = static_cast<type_cast_result<T> *>(etype);

		PyObject * result = etype_impl->wrapp( _value );

		if( result == 0 )
		{
			ret_none();
		}

		return result;
	}
}