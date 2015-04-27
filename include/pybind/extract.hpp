#	pragma once

#	include "pybind/system.hpp"
#	include "pybind/exception.hpp"
#	include "pybind/logger.hpp"
#	include "pybind/bindable.hpp"
#	include "pybind/type_cast.hpp"
#	include "pybind/type_traits.hpp"

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

				if( _obj == nullptr )
				{
					const char * type_name = tinfo.name();

					pybind::throw_exception( "extract obj is NULL for '%.256s'"
						, type_name
						);

					return false;
				}

				type_cast * etype = type_down_cast<T_WOCR>::find();

				if( etype == nullptr )
				{
					pybind::check_error();

					const char * type_name = tinfo.name();

					pybind::throw_exception( "extract invalid find cast for '%.256s'"
						, type_name
						);

					return false;
				}

				typedef type_cast_result<T_WOCR> type_cast_result_T_WOCR;

				type_cast_result_T_WOCR * etype_impl = static_cast<type_cast_result_T_WOCR *>(etype);

				if( etype_impl->apply( _obj, _value ) == false )
				{
					pybind::check_error();

                    const char * type_name = tinfo.name();
                    const char * repr_obj = pybind::object_repr( _obj );
					const char * repr_obj_type = pybind::object_repr_type( _obj );
					
					if( repr_obj != nullptr )
					{
						pybind::throw_exception( "extract from '%.256s' type '%.256s' to '%.256s'"
							, repr_obj
							, repr_obj_type
							, type_name
							);
					}
                    else
                    {
                        pybind::throw_exception( "extract from xxxx to '%.256s'"
                            , type_name
                            );
                    }
					
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
	PYBIND_API bool extract_value( PyObject * _obj, int8_t & _value );	
	PYBIND_API bool extract_value( PyObject * _obj, int16_t & _value );	
	PYBIND_API bool extract_value( PyObject * _obj, int32_t & _value );	
	PYBIND_API bool extract_value( PyObject * _obj, int64_t & _value );
	PYBIND_API bool extract_value( PyObject * _obj, uint8_t & _value );
	PYBIND_API bool extract_value( PyObject * _obj, uint16_t & _value );
	PYBIND_API bool extract_value( PyObject * _obj, uint32_t & _value );
	PYBIND_API bool extract_value( PyObject * _obj, uint64_t & _value );
	PYBIND_API bool extract_value( PyObject * _obj, long & _value );
	PYBIND_API bool extract_value( PyObject * _obj, unsigned long & _value );
	PYBIND_API bool extract_value( PyObject * _obj, float & _value );
	PYBIND_API bool extract_value( PyObject * _obj, double & _value );
	PYBIND_API bool extract_value( PyObject * _obj, const char * & _value );
	PYBIND_API bool extract_value( PyObject * _obj, const wchar_t * & _value );
	PYBIND_API bool extract_value( PyObject * _obj, PyObject * & _value );

    template<class T>
	typename detail::extract_return<T>::type extract_throw( PyObject * _obj )
	{
        typedef typename detail::extract_return<T>::type TValue;
		TValue value;
		       
		if( extract_value( _obj, value ) == false )
        {
            const std::type_info & tinfo = typeid(TValue);
            
            const char * type_name = tinfo.name();

            pybind::throw_exception( "extract invalid %s:%s not cast to '%s'"
                , pybind::object_repr(_obj)
                , pybind::object_repr_type(_obj)
                , type_name
                );
        }
        
		return value;
	}

	template<class T>
	typename detail::extract_return<T>::type extract( PyObject * _obj )
	{
		typedef typename detail::extract_return<T>::type TValue;
		TValue value;

		if( extract_value( _obj, value ) == false )
		{
			const std::type_info & tinfo = typeid(TValue);

			const char * type_name = tinfo.name();

			pybind::log( "extract invalid %s:%s not cast to '%s'"
				, pybind::object_repr( _obj )
				, pybind::object_repr_type( _obj )
				, type_name
				);
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

			pybind::throw_exception( "ptr invalid find cast for '%.256s'"
				, type_name
				);
		}

		typedef type_cast_result<T_WOCR> type_cast_result_T_WOCR;

		type_cast_result_T_WOCR * etype_impl = static_cast<type_cast_result_T_WOCR *>(etype);

		PyObject * result = etype_impl->wrap( _value );

		if( result == nullptr )
		{
			const char * type_name = tinfo.name();

			pybind::throw_exception( "ptr invalid wrap '%.256s'"
				, type_name
				);
		}

		return result;
	}

	PYBIND_API PyObject * ptr_throw( bool _value );
	PYBIND_API PyObject * ptr_throw( int8_t _value );
	PYBIND_API PyObject * ptr_throw( int16_t _value );
	PYBIND_API PyObject * ptr_throw( int32_t _value );
	PYBIND_API PyObject * ptr_throw( int64_t _value );
	PYBIND_API PyObject * ptr_throw( uint8_t _value );
	PYBIND_API PyObject * ptr_throw( uint16_t _value );
	PYBIND_API PyObject * ptr_throw( uint32_t _value );	
	PYBIND_API PyObject * ptr_throw( uint64_t _value );	
	PYBIND_API PyObject * ptr_throw( long _value );
	PYBIND_API PyObject * ptr_throw( unsigned long _value );
	PYBIND_API PyObject * ptr_throw( float _value );
	PYBIND_API PyObject * ptr_throw( double _value );
	PYBIND_API PyObject * ptr_throw( char * _value );
	PYBIND_API PyObject * ptr_throw( wchar_t * _value );
	PYBIND_API PyObject * ptr_throw( const char * _value );
	PYBIND_API PyObject * ptr_throw( const wchar_t * _value );
	PYBIND_API PyObject * ptr_throw( PyObject * _value );
	PYBIND_API PyObject * ptr_throw( pybind::bindable * _value );

	template<typename T, typename = void>
	struct ptr_throw_specialized
	{
		PyObject * operator () ( const T & _t )
		{
			return ptr_throw( _t );
		}
	};

	template<typename T>
	struct ptr_throw_specialized < T, typename detail::enable_if<detail::is_base_of<pybind::bindable, typename detail::remove_ptr<T>::type>::value>::type >
	{
		PyObject * operator () ( pybind::bindable * _t )
		{
			return ptr_throw( _t );
		}
	};
	
    template<class T>
    PyObject * ptr( const T & _value )
    {
        try
        {
			PyObject * value = ptr_throw_specialized<T>()( _value );

            return value;
        }
        catch( const pybind::pybind_exception & _ex )
        {
			pybind::error_message("ptr value: %s"
				, _ex.what()
				);
        }

        return nullptr;
    }
}

