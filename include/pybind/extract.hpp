#	pragma once

#	include "pybind/system.hpp"
#	include "pybind/exception.hpp"
#	include "pybind/logger.hpp"
#	include "pybind/bindable.hpp"

#	include "pybind/type_cast_result.hpp"

#	include "stdex/mpl.h"
#	include "stdex/intrusive_ptr.h"

#	include <typeinfo>

namespace pybind
{
	class object;
	class list;
	class tuple;
	class dict;

	namespace detail
	{
		template<class T>
		struct extract_check
		{
			typedef typename stdex::mpl::remove_cref<T>::type T_WOCR;

			static bool extract( kernel_interface * _kernel, PyObject * _obj, T_WOCR & _value, bool _nothrow )
			{				
				if( _obj == nullptr )
				{
					if( _nothrow == false )
					{
						const std::type_info & tinfo = typeid(T_WOCR);

						const char * type_name = tinfo.name();

						pybind::throw_exception( "extract obj is NULL for '%.256s'"
							, type_name
							);
					}

					return false;
				}

				type_cast * etype = type_down_cast<T_WOCR>::find( _kernel );

				if( etype == nullptr )
				{
					if( _nothrow == false )
					{
						pybind::check_error();

						const std::type_info & tinfo = typeid(T_WOCR);

						const char * type_name = tinfo.name();

						pybind::throw_exception( "extract: extract invalid find cast for '%.256s'"
							, type_name
							);
					}

					return false;
				}
				
				typedef type_cast_result<T_WOCR> type_cast_result_T_WOCR;

				type_cast_result_T_WOCR * etype_impl = static_cast<type_cast_result_T_WOCR *>(etype);

				if( etype_impl->apply( _kernel, _obj, _value, false ) == false )
				{
					if( _nothrow == false )
					{
						pybind::check_error();

						const std::type_info & tinfo = typeid(T_WOCR);

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
					}
					
					return false;
				}

				return true;
			}
		};
	}

	template<class T>
	bool extract_value( kernel_interface * _kernel, PyObject * _obj, T & _value, bool _nothrow )
	{
		bool result = detail::extract_check<T>::extract( _kernel, _obj, _value, _nothrow );

        return result;
	}

	PYBIND_API bool extract_value( kernel_interface * _kernel, PyObject * _obj, bool & _value, bool _nothrow );
	PYBIND_API bool extract_value( kernel_interface * _kernel, PyObject * _obj, int8_t & _value, bool _nothrow );
	PYBIND_API bool extract_value( kernel_interface * _kernel, PyObject * _obj, int16_t & _value, bool _nothrow );
	PYBIND_API bool extract_value( kernel_interface * _kernel, PyObject * _obj, int32_t & _value, bool _nothrow );
	PYBIND_API bool extract_value( kernel_interface * _kernel, PyObject * _obj, int64_t & _value, bool _nothrow );
	PYBIND_API bool extract_value( kernel_interface * _kernel, PyObject * _obj, uint8_t & _value, bool _nothrow );
	PYBIND_API bool extract_value( kernel_interface * _kernel, PyObject * _obj, uint16_t & _value, bool _nothrow );
	PYBIND_API bool extract_value( kernel_interface * _kernel, PyObject * _obj, uint32_t & _value, bool _nothrow );
	PYBIND_API bool extract_value( kernel_interface * _kernel, PyObject * _obj, uint64_t & _value, bool _nothrow );
	PYBIND_API bool extract_value( kernel_interface * _kernel, PyObject * _obj, wchar_t & _value, bool _nothrow );
	PYBIND_API bool extract_value( kernel_interface * _kernel, PyObject * _obj, long & _value, bool _nothrow );
	PYBIND_API bool extract_value( kernel_interface * _kernel, PyObject * _obj, unsigned long & _value, bool _nothrow );
	PYBIND_API bool extract_value( kernel_interface * _kernel, PyObject * _obj, float & _value, bool _nothrow );
	PYBIND_API bool extract_value( kernel_interface * _kernel, PyObject * _obj, double & _value, bool _nothrow );
	PYBIND_API bool extract_value( kernel_interface * _kernel, PyObject * _obj, const char * & _value, bool _nothrow );
	PYBIND_API bool extract_value( kernel_interface * _kernel, PyObject * _obj, const wchar_t * & _value, bool _nothrow );
	PYBIND_API bool extract_value( kernel_interface * _kernel, PyObject * _obj, PyObject * & _value, bool _nothrow );
	PYBIND_API bool extract_value( kernel_interface * _kernel, PyObject * _obj, pybind::object & _value, bool _nothrow );
	PYBIND_API bool extract_value( kernel_interface * _kernel, PyObject * _obj, pybind::list & _value, bool _nothrow );
	PYBIND_API bool extract_value( kernel_interface * _kernel, PyObject * _obj, pybind::tuple & _value, bool _nothrow );
	PYBIND_API bool extract_value( kernel_interface * _kernel, PyObject * _obj, pybind::dict & _value, bool _nothrow );

    template<class T>
	typename stdex::mpl::remove_cref<T>::type extract_throw( kernel_interface * _kernel, PyObject * _obj )
	{
		typedef typename stdex::mpl::remove_cref<T>::type type_value;
		type_value value;
		       
		if( extract_value( _kernel, _obj, value, false ) == false )
        {
			const std::type_info & tinfo = typeid(type_value);
            
            const char * type_name = tinfo.name();

            pybind::throw_exception( "extract_throw: extract invalid %s:%s not cast to '%s'"
                , pybind::object_repr(_obj)
                , pybind::object_repr_type(_obj)
                , type_name
                );
        }
        
		return value;
	}

	template<typename T>
	struct extract_specialized
	{
		typename stdex::mpl::remove_cref<T>::type operator () ( kernel_interface * _kernel, PyObject * _obj )
		{
			typedef typename stdex::mpl::remove_cref<T>::type type_value;
			type_value value;

			if( extract_value( _kernel, _obj, value, true ) == false )
			{
				const std::type_info & tinfo = typeid(type_value);

				const char * type_name = tinfo.name();

				pybind::log( "extract_value<T>: extract invalid %s:%s not cast to '%s'"
					, pybind::object_repr( _obj )
					, pybind::object_repr_type( _obj )
					, type_name
					);
			}

			return value;
		}
	};

	template<typename T>
	struct extract_specialized<T *>
	{
		T * operator () ( kernel_interface * _kernel, PyObject * _obj )
		{
			T * value = nullptr;

			if( extract_value( _kernel, _obj, value, true ) == false )
			{
				const std::type_info & tinfo = typeid(T *);

				const char * type_name = tinfo.name();

				pybind::log( "extract_value<T*>: extract invalid %s:%s not cast to '%s'"
					, pybind::object_repr( _obj )
					, pybind::object_repr_type( _obj )
					, type_name
					);
			}

			return value;
		}
	};

	template<typename T>
	struct extract_specialized<stdex::intrusive_ptr<T> >
	{
		T * operator () ( kernel_interface * _kernel, PyObject * _obj )
		{
			T * value = nullptr;

			if( extract_value( _kernel, _obj, value, true ) == false )
			{
				const std::type_info & tinfo = typeid(T *);

				const char * type_name = tinfo.name();

				pybind::log( "extract_value<intrusive>: extract invalid %s:%s not cast to '%s'"
					, pybind::object_repr( _obj )
					, pybind::object_repr_type( _obj )
					, type_name
					);
			}

			return value;
		}
	};

	template<class T>
	typename stdex::mpl::remove_cref<T>::type extract( kernel_interface * _kernel, PyObject * _obj )
	{
		return extract_specialized<T>()(_kernel, _obj);		
	}

	template<class T>
	PyObject * ptr_throw_i( kernel_interface * _kernel, const T & _value )
	{
		typedef typename stdex::mpl::remove_cref<T>::type T_WOCR;
				
		type_cast * etype = detail::type_down_cast<T_WOCR>::find( _kernel );

		if( etype == nullptr )
		{
			const std::type_info & tinfo = typeid(T_WOCR);
			const char * type_name = tinfo.name();
		
			pybind::throw_exception( "ptr invalid find cast for '%.256s'"
				, type_name
				);
		}

		typedef type_cast_result<T_WOCR> type_cast_result_T_WOCR;

		type_cast_result_T_WOCR * etype_impl = static_cast<type_cast_result_T_WOCR *>(etype);

		PyObject * result = etype_impl->wrap( _kernel, _value );

		if( result == nullptr )
		{
			const std::type_info & tinfo = typeid(T_WOCR);

			const char * type_name = tinfo.name();

			pybind::throw_exception( "ptr invalid wrap '%.256s'"
				, type_name
				);
		}

		return result;
	}

	PYBIND_API PyObject * ptr_throw_i( kernel_interface * _kernel, bool _value );
	PYBIND_API PyObject * ptr_throw_i( kernel_interface * _kernel, int8_t _value );
	PYBIND_API PyObject * ptr_throw_i( kernel_interface * _kernel, int16_t _value );
	PYBIND_API PyObject * ptr_throw_i( kernel_interface * _kernel, int32_t _value );
	PYBIND_API PyObject * ptr_throw_i( kernel_interface * _kernel, int64_t _value );
	PYBIND_API PyObject * ptr_throw_i( kernel_interface * _kernel, uint8_t _value );
	PYBIND_API PyObject * ptr_throw_i( kernel_interface * _kernel, uint16_t _value );
	PYBIND_API PyObject * ptr_throw_i( kernel_interface * _kernel, uint32_t _value );	
	PYBIND_API PyObject * ptr_throw_i( kernel_interface * _kernel, uint64_t _value );	
	PYBIND_API PyObject * ptr_throw_i( kernel_interface * _kernel, wchar_t _value );
	PYBIND_API PyObject * ptr_throw_i( kernel_interface * _kernel, long _value );
	PYBIND_API PyObject * ptr_throw_i( kernel_interface * _kernel, unsigned long _value );
	PYBIND_API PyObject * ptr_throw_i( kernel_interface * _kernel, float _value );
	PYBIND_API PyObject * ptr_throw_i( kernel_interface * _kernel, double _value );
	PYBIND_API PyObject * ptr_throw_i( kernel_interface * _kernel, char * _value );
	PYBIND_API PyObject * ptr_throw_i( kernel_interface * _kernel, wchar_t * _value );
	PYBIND_API PyObject * ptr_throw_i( kernel_interface * _kernel, const char * _value );
	PYBIND_API PyObject * ptr_throw_i( kernel_interface * _kernel, const wchar_t * _value );
	PYBIND_API PyObject * ptr_throw_i( kernel_interface * _kernel, PyObject * _value );	
	PYBIND_API PyObject * ptr_throw_i( kernel_interface * _kernel, pybind::bindable * _value );		
	PYBIND_API PyObject * ptr_throw_i( kernel_interface * _kernel, const pybind::object & _value );
	PYBIND_API PyObject * ptr_throw_i( kernel_interface * _kernel, const pybind::tuple & _value );
	PYBIND_API PyObject * ptr_throw_i( kernel_interface * _kernel, const pybind::list & _value );
	PYBIND_API PyObject * ptr_throw_i( kernel_interface * _kernel, const pybind::dict & _value );

	template<typename T, typename = void>
	struct ptr_throw_specialized
	{
		PyObject * operator () ( kernel_interface * _kernel, const T & _t )
		{
			return ptr_throw_i( _kernel, _t );
		}
	};

	template<typename T>
	struct ptr_throw_specialized < stdex::intrusive_ptr<T> >
	{
		PyObject * operator () ( kernel_interface * _kernel, const stdex::intrusive_ptr<T> & _t )
		{
			T * t_ptr = _t.get();

			return ptr_throw_i( _kernel, t_ptr );
		}
	};

	template<typename T>
	struct ptr_throw_specialized < T, typename stdex::mpl::enable_if<stdex::mpl::is_base_of<pybind::bindable, typename stdex::mpl::remove_ptr<T>::type>::value>::type >
	{
		PyObject * operator () ( kernel_interface * _kernel, pybind::bindable * _t )
		{
			return ptr_throw_i( _kernel, _t );
		}
	};

	template<class T>
	PyObject * ptr_throw( kernel_interface * _kernel, const T & _value )
	{ 
		PyObject * py_ptr = ptr_throw_specialized<T>()(_kernel, _value);

		return py_ptr;
	}
	
    template<class T>
	PyObject * ptr( kernel_interface * _kernel, const T & _value )
    {
        try
        {
			PyObject * value = ptr_throw( _kernel, _value );

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

