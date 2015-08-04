#	pragma once

#	include "class_type_scope.hpp"

#   include "pybind/kernel_interface.hpp"

#   include "pybind/type_cast.hpp"

#	include "config/python.hpp"

namespace pybind
{
	//////////////////////////////////////////////////////////////////////////
	struct class_info_desc_t
	{
		uint32_t id;
		const char * name;
	};
	//////////////////////////////////////////////////////////////////////////
	class kernel
		: public kernel_interface
	{
	public:
		kernel();
		~kernel();

	public:
		bool initialize();
		void finalize();

	public:
		uint32_t get_class_type_id( const std::type_info & _info ) override;
		const char * get_class_type_info( uint32_t _id ) const override;

	protected:
		uint32_t get_next_id();

		const class_info_desc_t * find_class_info_desc_name( const char * _name ) const;
		const class_info_desc_t * find_class_info_desc_id( uint32_t _id ) const;

		class_info_desc_t * alloc_class_info_desc();

	public:
		void register_type_info_extract( uint32_t _info, const type_cast_ptr & _cast ) override;
		void unregister_type_info_extract( uint32_t _info ) override;
		type_cast * find_type_info_extract( uint32_t _info ) const override;

	public:
		class_type_scope_interface_ptr create_new_type_scope( uint32_t _info, const char * _name, void * _user, const new_adapter_interface_ptr & _pynew, const destroy_adapter_interface_ptr & _pydestructor, uint32_t _pod ) override;

		bool has_class_type_scope( uint32_t _info ) const override;
		const class_type_scope_interface_ptr & get_class_type_scope( uint32_t _info ) const override;

		void visit_types_scope( visitor_class_type_scope * _getter ) override;

		const class_type_scope_interface_ptr & get_class_scope( PyTypeObject * _type ) const override;

	public:
		bool is_class( PyObject * _obj ) const override;
		bool is_type_class( PyObject * _type ) const override;

	public:
		void * check_registred_class( PyObject * _obj, uint32_t _info ) const override;

		void error_invalid_extract( PyObject * _obj, uint32_t _tinfo ) override;
		bool instance_of_type( PyObject * _obj, uint32_t _tinfo ) const override;

	public:
		PyTypeObject * get_pod_type( uint32_t _pod ) override;

	public:
		PyObject * get_str_class_type_scope_() const override;

	protected:
		class_info_desc_t m_class_info_desc[PYBIND_TYPE_COUNT];
		type_cast_ptr m_type_cast[PYBIND_TYPE_COUNT];
		class_type_scope_interface_ptr m_class_type_scope[PYBIND_TYPE_COUNT];

		uint32_t m_enumerator;		

		PyObject * m_str_pybind_class_type_scope;

		PyTypeObject m_pod8_type;
		PyTypeObject m_pod16_type;
		PyTypeObject m_pod32_type;
		PyTypeObject m_pod64_type;

		PyTypeObject m_ptr_type;
	};
}