#	pragma once

#	include "pybind/kernel.hpp"

#	include "config/python.hpp"

#	include "function_python.hpp"
#	include "functor_python.hpp"
#	include "member_python.hpp"
#	include "method_python.hpp"
#	include "pod_python.hpp"

namespace pybind
{
	class kernel_python
		: public kernel_interface
	{
	public:
		kernel_python();
		~kernel_python();

	public:
		bool initialize() override;
		void finalize() override;

	public:
		void def_function_adapter( const function_adapter_interface_ptr & _adapter, bool _native, PyObject * _module ) override;
		void def_functor_adapter( const functor_adapter_interface_ptr & _adapter, bool _native, PyObject * _module ) override;

		PyObject * create_function_adapter( const function_adapter_interface_ptr & _adapter, bool _native ) override;
        PyObject * create_functor_adapter( const functor_adapter_interface_ptr & _adapter, bool _native ) override;

		PyObject * create_member( const member_adapter_interface_ptr & _iadapter ) override;
		PyObject * create_method( const method_adapter_interface_ptr & _iadapter, PyTypeObject * _type ) override;
		method_adapter_interface * get_method_adapter( PyObject * _obj ) override;
		
	public:
		PyTypeObject * get_pod_type( uint32_t _pod, bool _hash ) override;

	public:
		void set_current_module( PyObject * _module ) override;
		PyObject * get_current_module() const override;

	public:
		void * unwrap( PyObject * _obj ) override;
		bool is_wrap( PyObject * _obj ) override;

		bool is_class( PyObject * _obj ) override;
		bool is_type_class( PyObject * _type ) override;

		uint32_t get_class_type_id( const std::type_info & _info ) override;
		const char * get_class_type_info( uint32_t _id ) override;

		uint32_t find_class_info_desc_name( const char * _name ) override;

		void register_type_info_extract( uint32_t _info, const type_cast_ptr & _cast ) override;
		void unregister_type_info_extract( uint32_t _info ) override;
		type_cast * find_type_info_extract( uint32_t _info ) override;

		class_type_scope_ptr create_new_type_scope( uint32_t _info, const char * _name, void * _user, const new_adapter_interface_ptr & _pynew, const destroy_adapter_interface_ptr & _pydestructor, uint32_t _pod, bool _hash ) override;

		bool has_class_type_scope( uint32_t _info ) override;
		const class_type_scope_ptr & get_class_type_scope( uint32_t _info ) override;

		void visit_types_scope( visitor_class_type_scope * _getter ) override;

		PyTypeObject * get_object_type( PyObject * _type ) override;
		uint32_t get_object_type_id( PyObject * _type ) override;
		const class_type_scope_ptr & get_class_scope( PyTypeObject * _type ) override;

		PyObject * scope_create_holder( const class_type_scope_ptr & _scope, void * _ptr ) override;

		void * meta_cast_scope( void * _self, uint32_t _scope_name, uint32_t _class_name, const class_type_scope_ptr & _scope ) override;

		void * check_registred_class( PyObject * _obj, uint32_t _info ) override;

		void error_invalid_extract( PyObject * _obj, uint32_t _tinfo ) override;
		bool instance_of_type( PyObject * _obj, uint32_t _tinfo ) override;

		void * get_class_impl( PyObject * _obj ) override;

		PyObject * get_str_class_type_scope() override;

		PyObject * call_method( void * _self, const class_type_scope_ptr & _scope, const char * _name, PyObject * _args ) override;

	protected:
		function_python m_functions;
		functor_python m_functors;
		member_python m_members;
		method_python m_methods;

		pod_python m_pods;

		PyObject * m_current_module;

		struct class_info_desc_t
		{
			const char * name;
		};

		class_info_desc_t m_class_info_desc[PYBIND_TYPE_COUNT];
		type_cast_ptr m_type_cast[PYBIND_TYPE_COUNT];
		class_type_scope_ptr m_class_type_scopes[PYBIND_TYPE_COUNT];

		uint32_t m_enumerator;

		PyObject * m_str_pybind_class_type_scope;

	protected:
		bool set_class_info_desc( uint32_t _typeId, const char * _info );
		const char * get_class_info_desc_name( uint32_t _id );
		uint32_t get_next_id();
	};
}
