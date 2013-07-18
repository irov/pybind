#	pragma once

#	include "pybind/system.hpp"
#	include "pybind/exception.hpp"
#	include "pybind/type_cast.hpp"

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

			static bool extract( PyObject * _obj, T_WOCR & _value )
			{
				const std::type_info & tinfo = typeid(T_WOCR);

				type_cast * etype = type_down_cast<T_WOCR>::find();

				if( etype == 0 )
				{
					pybind::check_error();

					const char * type_name = tinfo.name();

					pybind::error_message( "extract invalid find cast for %.256s"
						, type_name
						);

					throw_exception();

					return false;
				}

				type_cast_result<T_WOCR> * etype_impl = static_cast<type_cast_result<T_WOCR> *>(etype);

				if( etype_impl->apply( _obj, _value ) == false )
				{
					pybind::check_error();

                    const char * type_name = tinfo.name();
                    const char * repr_obj = pybind::object_repr( _obj );

					if( repr_obj != 0 )
					{
						pybind::error_message( "extract from %.256s to %.256s"
							, repr_obj
							, type_name
							);
					}
                    else
                    {
                        pybind::error_message( "extract from xxxx to %.256s"
                            , type_name
                            );
                    }

					throw_exception();

					return false;
				}

				return true;
			}
		};
	}

	template<class T>
	bool extract_value( PyObject * _obj, T & _value )
	{
		bool result = detail::extract_check<T>::extract( _obj, _value );

        return result;
	}

	PYBIND_API bool extract_value( PyObject * _obj, bool & _value );
	PYBIND_API bool extract_value( PyObject * _obj, int & _value );
	PYBIND_API bool extract_value( PyObject * _obj, unsigned int & _value );
	PYBIND_API bool extract_value( PyObject * _obj, unsigned long & _value );
	PYBIND_API bool extract_value( PyObject * _obj, float & _value );
	PYBIND_API bool extract_value( PyObject * _obj, double & _value );
	PYBIND_API bool extract_value( PyObject * _obj, const char * & _value );
	PYBIND_API bool extract_value( PyObject * _obj, PyObject * & _value );

    template<class T>
	typename detail::extract_return<T>::type extract( PyObject * _obj )
	{
        typedef typename detail::extract_return<T>::type TValue;
		TValue value;
        
		if( extract_value( _obj, value ) == false )
        {
            const std::type_info & tinfo = typeid(TValue);
            
            const char * type_name = tinfo.name();

            pybind::error_message( "extract invalid %s:%s not cast to %s"
                , pybind::object_repr(_obj)
                , pybind::object_repr_type(_obj)
                , type_name
                );

            pybind::throw_exception();

            return value;
        }
        
		return value;
	}
    
	template<class T>
	PyObject * ptr_throw( const T & _value )
	{
		typedef typename detail::extract_return<T>::type T_WOCR;

		const std::type_info & tinfo = typeid(T_WOCR);

		type_cast * etype = detail::type_down_cast<T_WOCR>::find();

		if( etype == nullptr )
		{
			const char * type_name = tinfo.name();

			pybind::error_message( "ptr invalid find cast for %.256s"
				, type_name
				);

			throw_exception();
		}

		type_cast_result<T_WOCR> * etype_impl = static_cast<type_cast_result<T_WOCR> *>(etype);

		PyObject * result = etype_impl->wrap( _value );

		if( result == nullptr )
		{
			const char * type_name = tinfo.name();

			pybind::error_message( "ptr invalid wrap %.256s"
				, type_name
				);

			throw_exception();
		}

		return result;
	}

	PYBIND_API PyObject * ptr_throw( bool _value );
	PYBIND_API PyObject * ptr_throw( int _value );
	PYBIND_API PyObject * ptr_throw( unsigned int _value );
	PYBIND_API PyObject * ptr_throw( unsigned long _value );
	PYBIND_API PyObject * ptr_throw( float _value );
	PYBIND_API PyObject * ptr_throw( double _value );
	PYBIND_API PyObject * ptr_throw( const char * _value );
	PYBIND_API PyObject * ptr_throw( PyObject * _value );

    template<class T>
    PyObject * ptr( const T & _value )
    {
        try
        {
            PyObject * value = ptr_throw( _value );

            return value;
        }
        catch( const pybind::pybind_exception & )
        {
        }

        check_error();

        return nullptr;
    }
}

