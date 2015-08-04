#	pragma once

#   include "pybind/types.hpp"
#   include "pybind/exports.hpp"

namespace pybind
{
	namespace detail
	{
		PYBIND_API void * get_class_impl( PyObject * _obj );
		
		PYBIND_API void * unwrap( PyObject * _obj );
		PYBIND_API bool is_wrap( PyObject * _obj );

		PYBIND_API void * check_registred_class( PyObject * _obj, uint32_t _info );

		PYBIND_API void * meta_cast_scope( void * _self, uint32_t _scope_name, uint32_t _class_name, const class_type_scope_interface_ptr & _scope );

		template<class T>
		inline T meta_cast_scope_t( void * _self, uint32_t _scope_name, uint32_t _class_name, const class_type_scope_interface_ptr & _scope )
		{
			void * impl = meta_cast_scope( _self, _scope_name, _class_name, _scope );

			T obj = static_cast<T>(impl);

			return obj;
		}

		PYBIND_API bool convert_object( PyObject * _obj, uint32_t _tinfo, void * _place );

		PYBIND_API bool instance_of_type( PyObject * _obj, uint32_t tinfo );
	}
}