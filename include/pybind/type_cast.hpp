#	pragma once

#	include "pybind/exports.hpp"
#	include "pybind/types.hpp"

#   include "pybind/kernel_interface.hpp"

#	include "config/stdex.hpp"

namespace pybind
{
    class type_cast
        : public stdex::intrusive_ptr_base<type_cast>
    {
    public:
        type_cast()
        {
        }

        virtual ~type_cast()
        {
        }

    public:
		static void intrusive_ptr_destroy( type_cast * _ptr );

	public:
		void * operator new ( size_t _size );
		void operator delete ( void * _ptr, size_t _size );

    protected:
		static bool type_info_cast( kernel_interface * _kernel, PyObject * _obj, uint32_t _tinfo, uint32_t _tptrinfo, void ** _impl );
    };

    typedef stdex::intrusive_ptr<type_cast> type_cast_ptr;

	namespace detail
	{
		template<class T> struct type_down_cast;

		template<class T, class B>
		struct type_down_cast_find
		{
			static type_cast * find( kernel_interface * _kernel )
			{  
				uint32_t id = _kernel->class_info<T>();

				type_cast * etype = _kernel->find_type_info_extract( id );

				if( etype == nullptr )
				{
					return type_down_cast<B>::find( _kernel );
				}

				return etype;
			}
		};

		template<class B>
		struct type_down_cast_find<PyObject, B>
		{
			static type_cast * find( kernel_interface * _kernel )
			{  
				(void)_kernel;

				return nullptr;
			}
		};

		template<class T>
		struct type_down_cast
		{
			static type_cast * find( kernel_interface * _kernel )
			{
				return type_down_cast_find<T, void>::find( _kernel );
			}
		};

		template<>
		struct type_down_cast<void>
		{
			static type_cast * find( kernel_interface * _kernel )
			{
				(void)_kernel;

				return nullptr;
			}
		};

		template<class T>
		struct type_down_cast<T *>
		{
			static type_cast * find( kernel_interface * _kernel )
			{
				return type_down_cast_find<T *, T>::find( _kernel );
			}
		};

		template<class T>
		struct type_down_cast<const T *>
		{
			static type_cast * find( kernel_interface * _kernel )
			{
				return type_down_cast_find<const T *, T>::find( _kernel );
			}
		};

		template<class T>
		struct type_down_cast<T &>
		{
			static type_cast * find( kernel_interface * _kernel )
			{
				return type_down_cast_find<T &, T>::find( _kernel );
			}
		};

		template<class T>
		struct type_down_cast<const T &>
		{
			static type_cast * find( kernel_interface * _kernel )
			{
				return type_down_cast_find<const T &, T>::find( _kernel );
			}
		};
	}

	template<class T>
	bool instance_of( kernel_interface * _kernel, PyObject * _obj )
	{
		uint32_t id = _kernel->class_info<T>();

		bool result = _kernel->instance_of_type( _obj, id );

		return result;
	}

	template<class T>
	void registration_type_cast( kernel_interface * _kernel, const type_cast_ptr & _type )
	{
		uint32_t id = _kernel->class_info<T>();

		_kernel->register_type_info_extract( id, _type );
	}

	template<class T>
	void unregistration_type_cast( kernel_interface * _kernel )
	{	
		uint32_t id = _kernel->class_info<T>();

		_kernel->unregister_type_info_extract( id );
	}

    
	template<class T>
	class type_cast_result
		: public type_cast
	{
    public:
		typedef T & TCastValue;
        typedef const T & TCastRef;

	public:
        virtual PyObject * wrap( kernel_interface * _kernel, TCastRef t ) = 0;
		virtual bool apply( kernel_interface * _kernel, PyObject * _obj, TCastValue _value ) = 0;
	};
}