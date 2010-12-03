#	pragma once

#	include "pybind/system.hpp"
#	include "pybind/type_cast.hpp"

#	include <exception>

namespace pybind
{
	namespace detail
	{
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
		struct extract_check
		{
			typedef typename extract_return<T>::type T_WOCR;

			static T_WOCR extract( PyObject * _obj )
			{
				const std::type_info & tinfo = typeid(T_WOCR);

				type_cast * etype = type_down_cast<T_WOCR>::find();

				if( etype == 0 )
				{
					const char * type_name = tinfo.name();

					pybind::error_message( "extract invalid find cast for %.256s"
						, type_name
						);

					throw_exception();
				}

				type_cast_result<T_WOCR> * etype_impl = static_cast<type_cast_result<T_WOCR> *>(etype);

				T_WOCR t = etype_impl->apply( _obj );

				if( etype_impl->is_valid() == false )
				{
					pybind::check_error();

					if( const char * repr = pybind::object_to_string( _obj ) )
					{
						const char * type_name = tinfo.name();
	
						pybind::error_message( "extract from %.256s to %.256s"
							, repr
							, type_name
							);
					}

					throw_exception();
				}

				return t;
			}
		};
	}

	template<class T>
	typename detail::extract_return<T>::type extract( PyObject * _obj )
	{
		return detail::extract_check<T>::extract( _obj );
	}

	template<class T>
	typename detail::extract_return<T>::type extract_nt( PyObject * _obj )
	{
		try
		{
			return detail::extract_check<T>::extract( _obj );
		}
		catch( const pybind_exception & )
		{
		}

		return 0;
	}

	template<class T>
	typename detail::extract_return<T>::type extract_item( PyObject * _obj, std::size_t _it )
	{
		PyObject * item = tuple_getitem( _obj, 0 );
		return detail::extract_check<T>::extract( item );
	}


	template<class T>
	PyObject * ptr( T _value )
	{
		typedef typename detail::extract_return<T>::type T_WOCR;

		const std::type_info & tinfo = typeid(T_WOCR);

		type_cast * etype = detail::type_down_cast<T_WOCR>::find();

		if( etype == 0 )
		{
			const char * type_name = tinfo.name();

			pybind::error_message( "ptr invalid find cast for %.256s"
				, type_name
				);

			throw_exception();
		}

		type_cast_result<T_WOCR> * etype_impl = static_cast<type_cast_result<T_WOCR> *>(etype);

		PyObject * result = etype_impl->wrap( _value );

		if( result == 0 )
		{
			const char * type_name = tinfo.name();

			pybind::error_message( "ptr invalid wrap %.256s"
				, type_name
				);

			throw_exception();
		}

		return result;
	}
}

