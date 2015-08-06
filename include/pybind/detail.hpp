#	pragma once

#   include "pybind/types.hpp"
#   include "pybind/exports.hpp"

#	include <typeinfo>

namespace pybind
{
	typedef stdex::intrusive_ptr<class type_cast> type_cast_ptr;
	typedef stdex::intrusive_ptr<class class_type_scope> class_type_scope_ptr;

	typedef stdex::intrusive_ptr<class new_adapter_interface> new_adapter_interface_ptr;
	typedef stdex::intrusive_ptr<class destroy_adapter_interface> destroy_adapter_interface_ptr;

	namespace detail
	{				
		struct class_info_desc_t
		{
			uint32_t id;
			const char * name;
		};
		
		PYBIND_API void * unwrap( PyObject * _obj );
		PYBIND_API bool is_wrap( PyObject * _obj );

		PYBIND_API void * meta_cast_scope( void * _self, uint32_t _scope_name, uint32_t _class_name, const class_type_scope_ptr & _scope );

		template<class T>
		inline T meta_cast_scope_t( void * _self, uint32_t _scope_name, uint32_t _class_name, const class_type_scope_ptr & _scope )
		{
			void * impl = meta_cast_scope( _self, _scope_name, _class_name, _scope );

			T obj = static_cast<T>(impl);

			return obj;
		}

		PYBIND_API uint32_t get_class_type_id( const std::type_info & _info );
		PYBIND_API const char * get_class_type_info( uint32_t _id );

		template<class T>
		uint32_t class_info()
		{
			static uint32_t type_id = 0;

			if( type_id == 0 )
			{
				const std::type_info & ti = typeid(T);

				uint32_t id = detail::get_class_type_id( ti );

				type_id = id;
			}

			return type_id;
		}

		PYBIND_API uint32_t get_next_id();

		PYBIND_API const class_info_desc_t * find_class_info_desc_name( const char * _name );
		PYBIND_API const class_info_desc_t * find_class_info_desc_id( uint32_t _id );

		PYBIND_API class_info_desc_t * alloc_class_info_desc();

		PYBIND_API void register_type_info_extract( uint32_t _info, const type_cast_ptr & _cast );
		PYBIND_API void unregister_type_info_extract( uint32_t _info );
		PYBIND_API type_cast * find_type_info_extract( uint32_t _info );

		PYBIND_API class_type_scope_ptr create_new_type_scope( uint32_t _info, const char * _name, void * _user, const new_adapter_interface_ptr & _pynew, const destroy_adapter_interface_ptr & _pydestructor, uint32_t _pod );

		PYBIND_API bool has_class_type_scope( uint32_t _info );
		PYBIND_API const class_type_scope_ptr & get_class_type_scope( uint32_t _info );

		PYBIND_API void visit_types_scope( visitor_class_type_scope * _getter );

		PYBIND_API const class_type_scope_ptr & get_class_scope( PyTypeObject * _type );

		template<class T>
		const class_type_scope_ptr & class_scope()
		{
			uint32_t ti = detail::class_info<T>();

			const class_type_scope_ptr & scope = detail::get_class_type_scope( ti );

			return scope;
		}

		PYBIND_API bool is_class( PyObject * _obj );
		PYBIND_API bool is_type_class( PyObject * _type );

		PYBIND_API void * check_registred_class( PyObject * _obj, uint32_t _info );

		PYBIND_API void error_invalid_extract( PyObject * _obj, uint32_t _tinfo );
		PYBIND_API bool instance_of_type( PyObject * _obj, uint32_t _tinfo );
		
		PYBIND_API void * get_class_impl( PyObject * _obj );

		PYBIND_API PyObject * get_str_class_type_scope_();
	}

	PYBIND_API bool initialize_detail();
	PYBIND_API void finalize_detail();
}