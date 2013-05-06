#	pragma once

#	include "pybind/exports.hpp"
#	include "pybind/types.hpp"

#   include "pybind/class_info.hpp"

namespace pybind
{
	class type_cast;

	namespace detail
	{
		PYBIND_API void register_type_info_extract( size_t _info, type_cast * _type );
		PYBIND_API type_cast * find_type_info_extract( size_t _info );

		PYBIND_API void error_invalid_extract( PyObject * _obj, size_t _tinfo );
		PYBIND_API bool convert_object( PyObject * _obj, size_t _tinfo, void * _place );

		PYBIND_API bool instance_of_type( PyObject * _obj, size_t tinfo );

		template<class T> struct type_down_cast;

		template<class T, class B>
		struct type_down_cast_find
		{
			static type_cast * find()
			{  
				size_t id = class_info<T>();

				type_cast * etype = find_type_info_extract( id );

				if( etype == 0 )
				{
					return type_down_cast<B>::find();
				}

				return etype;
			}
		};

		template<class B>
		struct type_down_cast_find<PyObject, B>
		{
			static type_cast * find()
			{  
				return (type_cast *)0;
			}
		};

		template<class T>
		struct type_down_cast
		{
			static type_cast * find()
			{
				return type_down_cast_find<T,void>::find();
			}
		};

		template<>
		struct type_down_cast<void>
		{
			static type_cast * find()
			{
				return 0;
			}
		};

		template<class T>
		struct type_down_cast<T *>
		{
			static type_cast * find()
			{
				return type_down_cast_find<T *,T>::find();
			}
		};

		template<class T>
		struct type_down_cast<const T *>
		{
			static type_cast * find()
			{
				return type_down_cast_find<const T *,T>::find();
			}
		};

		template<class T>
		struct type_down_cast<T &>
		{
			static type_cast * find()
			{
				return type_down_cast_find<T &,T>::find();
			}
		};

		template<class T>
		struct type_down_cast<const T &>
		{
			static type_cast * find()
			{
				return type_down_cast_find<const T &,T>::find();
			}
		};
	}

	template<class T>
	bool instance_of( PyObject * _obj )
	{
		size_t id = class_info<T>();

		bool result = detail::instance_of_type( _obj, id );

		return result;
	}


	template<class T>
	void registration_type_cast( type_cast * _type )
	{
		size_t id = class_info<T>();

		detail::register_type_info_extract( id, _type );
	}


	class type_cast
	{
	public:
		type_cast()
		{
		}

		virtual ~type_cast()
		{
		}

	protected:
		bool type_info_cast( PyObject * _obj, size_t _tinfo, size_t _tptrinfo, void ** _impl );
	};

	template<class T>
	class type_cast_result
		: public type_cast
	{
    public:
        typedef const T & TCastRef;

	public:
        virtual PyObject * wrap( TCastRef t ) = 0;
		virtual bool apply( PyObject * _obj, T & _value ) = 0;
	};

    PYBIND_API void finialize_type_cast();
}