#	pragma once

#	include "pybind/adapter_interface.hpp"

#   include "pybind/class_info.hpp"
#	include "pybind/class_type_scope.hpp"

#   include "pybind/function_parser.hpp"

#   include "pybind/extract.hpp"

namespace pybind
{
	class member_adapter_interface
		: public adapter_interface
	{
	public:
		member_adapter_interface( const char * _name )
			: m_name(_name)
		{
		}

	public:
		inline const char * getName() const
		{
			return m_name;
		}

	public:
		virtual PyObject * get( void * _self, const class_type_scope_ptr & _scope ) = 0;
		virtual int set( void * _self, PyObject * _args, const class_type_scope_ptr & _scope ) = 0;

	protected:
		const char * m_name;
	};
    //////////////////////////////////////////////////////////////////////////
    typedef stdex::intrusive_ptr<member_adapter_interface> member_adapter_interface_ptr;
    //////////////////////////////////////////////////////////////////////////
	template<class C, class A>
	class member_adapter
		: public member_adapter_interface
	{
	public:
		member_adapter( const char * _name, A C:: * _member )
			: member_adapter_interface(_name)
            , m_member(_member)
		{
            m_class_id = class_info<C*>();
            m_scope_id = class_info<C>();
		}

	public:
		PyObject * get( void * _self, const class_type_scope_ptr & _scope ) override
		{
			void * impl = detail::meta_cast_scope( _self, m_scope_id, m_class_id, _scope );

			C * obj = static_cast<C *>(impl);

			PyObject * py_value = pybind::ptr_throw( obj->*m_member );

			return py_value;
		}

		int set( void * _self, PyObject * _args, const class_type_scope_ptr & _scope ) override
		{
			void * impl = detail::meta_cast_scope( _self, m_scope_id, m_class_id, _scope );

			C * obj = static_cast<C*>(impl);

			obj->*m_member = pybind::extract_throw<A>( _args );

			return 1;
		}

	protected:
		A C:: * m_member;

		uint32_t m_class_id;
		uint32_t m_scope_id;
	};
	//////////////////////////////////////////////////////////////////////////
	template<class C, class FG, class FS>
	class member_adapter_property
		: public member_adapter_interface
	{
	public:
		member_adapter_property( const char * _name, FG _get, FS _set )
			: member_adapter_interface(_name)
            , m_get(_get)
			, m_set(_set)
		{
			m_class_id = class_info<C*>();
			m_scope_id = class_info<C>();
		}

	public:
		PyObject * get( void * _self, const class_type_scope_ptr & _scope ) override
		{
			void * impl = detail::meta_cast_scope( _self, m_scope_id, m_class_id, _scope );
			
			C * obj = static_cast<C *>(impl);

			PyObject * py_value = pybind::ptr_throw( (obj->*m_get)() );

			return py_value;
		}

		int set( void * _self, PyObject * _args, const class_type_scope_ptr & _scope ) override
		{
			void * impl = detail::meta_cast_scope( _self, m_scope_id, m_class_id, _scope );
			
			C * obj = static_cast<C *>(impl);

			typedef typename function_parser<FS>::result f_info;
				
			(obj->*m_set)(
				pybind::extract_throw<typename f_info::param0>( _args )
				);

			return 1;
		}

	protected:
		FG m_get;
		FS m_set;

		uint32_t m_class_id;
		uint32_t m_scope_id;
	};
	//////////////////////////////////////////////////////////////////////////
	template<class C, class FG, class FS>
	class member_adapter_property_static
		: public member_adapter_interface
	{
	public:
		member_adapter_property_static( const char * _name, FG _get, FS _set )
			: member_adapter_interface(_name)
            , m_get(_get)
			, m_set(_set)
		{
            m_class_id = class_info<C*>();
            m_scope_id = class_info<C>();
		}

	public:
		PyObject * get( void * _self, const class_type_scope_ptr & _scope ) override
		{
			void * impl = detail::meta_cast_scope( _self, m_scope_id, m_class_id, _scope );

			C * obj = static_cast<C *>(impl);

			PyObject * py_value = pybind::ptr_throw( (*m_get)( obj ) );

			return py_value;
		}

		int set( void * _self, PyObject * _args, const class_type_scope_ptr & _scope ) override
		{
			void * impl = detail::meta_cast_scope( _self, m_scope_id, m_class_id, _scope );
			
			C * obj = static_cast<C *>(impl);

			typedef typename function_parser<FS>::result f_info;

			(*m_set)( obj
				, pybind::extract_throw<typename f_info::param1>( _args )
				);

			return 1;
		}

	protected:
		FG m_get;
		FS m_set;

		uint32_t m_class_id;
		uint32_t m_scope_id;
	};
}

