#	pragma once

#	include "pybind/exports.hpp"
#	include "pybind/types.hpp"

#   include "pybind/class_info.hpp"

#   include "stdex/intrusive_ptr_base.h"
#   include "stdex/intrusive_ptr.h"

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
        void intrusive_ptr_destroy();

	public:
		void * operator new ( size_t _size );
		void operator delete ( void * _ptr, size_t _size );

    protected:
        bool type_info_cast( PyObject * _obj, uint32_t _tinfo, uint32_t _tptrinfo, void ** _impl );
    };

    typedef stdex::intrusive_ptr<type_cast> type_cast_ptr;

	namespace detail
	{
		PYBIND_API void register_type_info_extract( uint32_t _info, const type_cast_ptr & _type );
		PYBIND_API void unregister_type_info_extract( uint32_t _info );

		PYBIND_API type_cast * find_type_info_extract( uint32_t _info );

		PYBIND_API void error_invalid_extract( PyObject * _obj, uint32_t _tinfo );
		PYBIND_API bool convert_object( PyObject * _obj, uint32_t _tinfo, void * _place );

		PYBIND_API bool instance_of_type( PyObject * _obj, uint32_t tinfo );

		template<class T> struct type_down_cast;

		template<class T, class B>
		struct type_down_cast_find
		{
			static type_cast * find()
			{  
				uint32_t id = class_info<T>();

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
		uint32_t id = class_info<T>();

		bool result = detail::instance_of_type( _obj, id );

		return result;
	}

	template<class T>
	void registration_type_cast( const type_cast_ptr & _type )
	{
		uint32_t id = class_info<T>();

		detail::register_type_info_extract( id, _type );
	}

	template<class T>
	void unregistration_type_cast()
	{	
		uint32_t id = class_info<T>();

		detail::unregister_type_info_extract( id );
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
		virtual bool apply( PyObject * _obj, TCastValue _value ) = 0;
	};

    PYBIND_API bool initialize_type_cast();
    PYBIND_API void finalize_type_cast();
}