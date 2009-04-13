#	pragma once

#	include "pybind/exports.hpp"
#	include "pybind/types.hpp"
#	include <typeinfo>

namespace pybind
{
	class type_cast;

	namespace detail
	{
		PYBIND_API void register_type_info_extract( const std::type_info & _info, type_cast * _type );
		PYBIND_API type_cast * find_type_info_extract( const std::type_info & _info );

		template<class T> struct type_down_cast;

		template<class T, class B>
		struct type_down_cast_find
		{
			static type_cast * find()
			{  
				const std::type_info & tinfo =  typeid(T);

				type_cast * etype = find_type_info_extract( tinfo );

				if( etype == 0 )
				{
					return type_down_cast<B>::find();
				}

				return etype;
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
	void registration_type_cast( type_cast * _type )
	{
		const std::type_info & tinfo = typeid(T);

		detail::register_type_info_extract( tinfo, _type );
	}


	class type_cast
	{
	public:
		type_cast()
			: m_valid( false )
		{
		}

	public:
		bool is_valid() const
		{
			return m_valid;
		}

	protected:
		bool m_valid;
	};

	template<class T> 
	class type_cast_result
		: public type_cast
	{
	public:
		type_cast_result()
			: type_cast()
		{
			registration_type_cast<T>( this );
		}

	public:
		virtual PyObject * wrap( T t ) = 0;
		virtual T apply( PyObject * _obj ) = 0;
	};

	template<class T> 
	class type_cast_result<const T &>
		: public type_cast
	{
	public:
		type_cast_result()
			: type_cast( false )
		{
			registration_type_cast<T>( this );
		}

	public:
		virtual PyObject * wrap( T t ) = 0;
		virtual T apply( PyObject * _obj ) = 0;

	};
}

