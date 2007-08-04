#	pragma once

#	include "pybind/system.hpp"
#	include "pybind/type_cast.hpp"

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
			static const T & extract( PyObject * _obj )
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
	T extract( PyObject * _obj )
	{
		return detail::extract_check<T>::extract( _obj );
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
}