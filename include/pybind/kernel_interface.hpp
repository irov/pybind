#	pragma once

#   include "pybind/types.hpp"
#   include "pybind/exports.hpp"

#	include <typeinfo>

namespace pybind
{
	typedef stdex::intrusive_ptr<class type_cast> type_cast_ptr;
	typedef stdex::intrusive_ptr<class class_type_scope_interface> class_type_scope_interface_ptr;

	typedef stdex::intrusive_ptr<class new_adapter_interface> new_adapter_interface_ptr;
	typedef stdex::intrusive_ptr<class destroy_adapter_interface> destroy_adapter_interface_ptr;

	class kernel_interface
	{
	public:
		virtual uint32_t get_class_type_id( const std::type_info & _info ) = 0;
		virtual const char * get_class_type_info( uint32_t _id ) const = 0;

		template<class T>
		uint32_t class_info()
		{
			static uint32_t type_id = 0;

			if( type_id == 0 )
			{
				const std::type_info & ti = typeid(T);

				uint32_t id = this->get_class_type_id( ti );

				type_id = id;
			}

			return type_id;
		}

	public:
		virtual void register_type_info_extract( uint32_t _info, const type_cast_ptr & _cast ) = 0;
		virtual void unregister_type_info_extract( uint32_t _info ) = 0;
		virtual type_cast * find_type_info_extract( uint32_t _info ) const = 0;

	public:
		virtual class_type_scope_interface_ptr create_new_type_scope( uint32_t _info, const char * _name, void * _user, const new_adapter_interface_ptr & _pynew, const destroy_adapter_interface_ptr & _pydestructor, uint32_t _pod ) = 0;

		virtual bool has_class_type_scope( uint32_t _info ) const = 0;
		virtual const class_type_scope_interface_ptr & get_class_type_scope( uint32_t _info ) const = 0;

		virtual void visit_types_scope( visitor_class_type_scope * _getter ) = 0;

		virtual const class_type_scope_interface_ptr & get_class_scope( PyTypeObject * _type ) const = 0;

		template<class T>
		const class_type_scope_interface_ptr & class_scope()
		{
			uint32_t ti = this->class_info<T>();

			const class_type_scope_interface_ptr & scope = this->get_class_type_scope( ti );

			return scope;
		}

	public:
		virtual bool is_class( PyObject * _obj ) const = 0;
		virtual bool is_type_class( PyObject * _type ) const = 0;

	public:
		virtual void * check_registred_class( PyObject * _obj, uint32_t _info ) const = 0;

		virtual void error_invalid_extract( PyObject * _obj, uint32_t _tinfo ) = 0;
		virtual bool instance_of_type( PyObject * _obj, uint32_t _tinfo ) const = 0;

	public:
		virtual PyTypeObject * get_pod_type( uint32_t _pod ) = 0;

	public:
		virtual PyObject * get_str_class_type_scope_() const = 0;
	};
}