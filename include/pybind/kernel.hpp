#	pragma once

#	include "pybind/exports.hpp"
#	include "pybind/types.hpp"

#	include "pybind/function_interface.hpp"
#	include "pybind/functor_interface.hpp"
#	include "pybind/member_interface.hpp"
#	include "pybind/method_interface.hpp"

#	include <typeinfo>

namespace pybind
{
	typedef stdex::intrusive_ptr<class type_cast> type_cast_ptr;
	typedef stdex::intrusive_ptr<class class_type_scope> class_type_scope_ptr;

	typedef stdex::intrusive_ptr<class new_adapter_interface> new_adapter_interface_ptr;
	typedef stdex::intrusive_ptr<class destroy_adapter_interface> destroy_adapter_interface_ptr;

	PYBIND_API class kernel_interface * get_kernel();
	PYBIND_API void set_kernel( class kernel_interface * _kernel );

	class kernel_interface
	{
	public:
		kernel_interface() {};
		virtual ~kernel_interface() {};

	public:
		virtual bool initialize() = 0;
		virtual void finalize() = 0;

	public:
		virtual void def_function_adapter( const function_adapter_interface_ptr & _adapter, bool _native, PyObject * _module ) = 0;
		virtual void def_functor_adapter( const functor_adapter_interface_ptr & _adapter, bool _native, PyObject * _module ) = 0;

		virtual PyObject * create_function_adapter( const function_adapter_interface_ptr & _adapter, bool _native ) = 0;
        virtual PyObject * create_functor_adapter( const functor_adapter_interface_ptr & _adapter, bool _native ) = 0;

		virtual PyObject * create_member( const member_adapter_interface_ptr & _iadapter ) = 0;
		virtual PyObject * create_method( const method_adapter_interface_ptr & _ifunc, PyTypeObject * _type ) = 0;
		virtual method_adapter_interface * get_method_adapter( PyObject * _obj ) = 0;

		virtual PyTypeObject * get_pod_type( uint32_t _pod, bool _hash ) = 0;

	public:
		virtual void set_current_module( PyObject * _module ) = 0;
		virtual PyObject * get_current_module() const = 0;

	public:
		virtual void * unwrap( PyObject * _obj ) = 0;
		virtual bool is_wrap( PyObject * _obj ) = 0;

		virtual bool is_class( PyObject * _obj ) = 0;
		virtual bool is_type_class( PyObject * _type ) = 0;

		virtual uint32_t get_class_type_id( const std::type_info & _info ) = 0;
		virtual const char * get_class_type_info( uint32_t _id ) = 0;

		template<class T>
		const char * get_class_type_info_t()
		{
			uint32_t scope_id = this->class_info<T>();

			const char * type_info = this->get_class_type_info( scope_id );

			return type_info;
		}

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

		virtual uint32_t find_class_info_desc_name( const char * _name ) = 0;

		virtual void register_type_info_extract( uint32_t _info, const type_cast_ptr & _cast ) = 0;

		template<class T>
		void register_type_info_extract_t( const type_cast_ptr & _cast )
		{
			uint32_t id = this->class_info<T>();

			this->register_type_info_extract( id, _cast );
		}

		virtual void unregister_type_info_extract( uint32_t _info ) = 0;

		template<class T>
		void unregister_type_info_extract_t()
		{
			uint32_t id = this->class_info<T>();

			this->unregister_type_info_extract( id );
		}

		virtual type_cast * find_type_info_extract( uint32_t _info ) = 0;

		virtual class_type_scope_ptr create_new_type_scope( uint32_t _info, const char * _name, void * _user, const new_adapter_interface_ptr & _pynew, const destroy_adapter_interface_ptr & _pydestructor, uint32_t _pod, bool _hash ) = 0;

		virtual bool has_class_type_scope( uint32_t _info ) = 0;
		virtual const class_type_scope_ptr & get_class_type_scope( uint32_t _info ) = 0;

		virtual void visit_types_scope( visitor_class_type_scope * _getter ) = 0;

		virtual PyTypeObject * get_object_type( PyObject * _type ) = 0;
		virtual uint32_t get_object_type_id( PyObject * _type ) = 0;
		virtual const class_type_scope_ptr & get_class_scope( PyTypeObject * _type ) = 0;

		template<class T>
		const class_type_scope_ptr & class_scope()
		{
			uint32_t ti = this->class_info<T>();

			const class_type_scope_ptr & scope = this->get_class_type_scope( ti );

			return scope;
		}

		virtual PyObject * scope_create_holder( const class_type_scope_ptr & _scope, void * _ptr ) = 0;

		template<class T>
		PyObject * scope_create_holder_t( T * _ptr )
		{
			const pybind::class_type_scope_ptr & scope = this->class_scope<T>();

			PyObject * py_obj = this->scope_create_holder( scope, _ptr );

			return py_obj;
		}

		virtual void * meta_cast_scope( void * _self, uint32_t _scope_name, uint32_t _class_name, const class_type_scope_ptr & _scope ) = 0;

		template<class T>
		inline T meta_cast_scope_t( void * _self, uint32_t _scope_name, uint32_t _class_name, const class_type_scope_ptr & _scope )
		{
			void * impl = this->meta_cast_scope( _self, _scope_name, _class_name, _scope );

			T obj = static_cast<T>(impl);

			return obj;
		}

		template<class C>
		inline C * meta_cast_class_t( void * _self, const class_type_scope_ptr & _scope )
		{
			uint32_t scope_name = this->class_info<C>();
			uint32_t class_name = this->class_info<C *>();

			void * impl = this->meta_cast_scope_t<C *>( _self, scope_name, class_name, _scope );

			C * obj = static_cast<C *>(impl);

			return obj;
		}

		virtual void * check_registred_class( PyObject * _obj, uint32_t _info ) = 0;

		virtual void error_invalid_extract( PyObject * _obj, uint32_t _tinfo ) = 0;
		virtual bool instance_of_type( PyObject * _obj, uint32_t _tinfo ) = 0;

		template<class C>
		C * get_class_impl_t( PyObject * _obj )
		{
			void * impl = this->get_class_impl( _obj );

			C * obj = static_cast<C *>(impl);

			return obj;
		}

		virtual void * get_class_impl( PyObject * _obj ) = 0;

		virtual PyObject * get_str_class_type_scope() = 0;

		virtual PyObject * call_method( void * _self, const class_type_scope_ptr & _scope, const char * _name, PyObject * _args ) = 0;
	};
}