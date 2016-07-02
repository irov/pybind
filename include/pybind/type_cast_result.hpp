#	pragma once

#	include "pybind/type_cast.hpp"

namespace pybind
{
	namespace detail
	{
		template<class T> struct type_down_cast;

		template<class T, class B>
		struct type_down_cast_find
		{
			static type_cast * find()
			{  
				kernel_interface * kernel = pybind::get_kernel();

				uint32_t id = kernel->class_info<T>();

				type_cast * etype = kernel->find_type_info_extract( id );

				if( etype == nullptr )
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
				return nullptr;
			}
		};

		template<class T>
		struct type_down_cast
		{
			static type_cast * find()
			{
				return type_down_cast_find<T, void>::find();
			}
		};

		template<>
		struct type_down_cast<void>
		{
			static type_cast * find()
			{
				return nullptr;
			}
		};

		template<class T>
		struct type_down_cast<T *>
		{
			static type_cast * find()
			{
				return type_down_cast_find<T *, T>::find();
			}
		};

		template<class T>
		struct type_down_cast<const T *>
		{
			static type_cast * find()
			{
				return type_down_cast_find<const T *, T>::find();
			}
		};

		template<class T>
		struct type_down_cast<T &>
		{
			static type_cast * find()
			{
				return type_down_cast_find<T &, T>::find();
			}
		};

		template<class T>
		struct type_down_cast<const T &>
		{
			static type_cast * find()
			{
				return type_down_cast_find<const T &, T>::find();
			}
		};
	}

	template<class T>
	bool instance_of( PyObject * _obj )
	{
		kernel_interface * kernel = pybind::get_kernel();

		uint32_t id = kernel->class_info<T>();

		bool result = kernel->instance_of_type( _obj, id );

		return result;
	}

	template<class T>
	void registration_type_cast( const type_cast_ptr & _type )
	{
		kernel_interface * kernel = pybind::get_kernel();

		uint32_t id = kernel->class_info<T>();

		kernel->register_type_info_extract( id, _type );
	}

	template<class T>
	void unregistration_type_cast()
	{	
		kernel_interface * kernel = pybind::get_kernel();

		uint32_t id = kernel->class_info<T>();

		kernel->unregister_type_info_extract( id );
	}
	   
	template<class T>
	class type_cast_result
		: public type_cast
	{
    public:
		typedef T & TCastValue;
        typedef const T & TCastRef;

	public:
        virtual PyObject * wrap( TCastRef t ) = 0;
		virtual bool apply( PyObject * _obj, TCastValue _value, bool _nothrow ) = 0;
	};
}