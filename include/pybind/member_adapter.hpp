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
		virtual PyObject * get( void * _self, class_type_scope * _scope ) = 0;
		virtual int set( void * _self, PyObject * _args, class_type_scope * _scope ) = 0;
	};

	template<class C, class A>
	class member_adapter
		: public member_adapter_interface
	{
	public:
		member_adapter( const char * _name, A C:: * _member )
			: m_name(_name)
            , m_member(_member)
		{
            m_class_name = class_info<C*>();
            m_scope_name = class_info<C>();
		}

	public:
		PyObject * get( void * _self, class_type_scope * _scope ) override
		{
			C * obj = static_cast<C*>( detail::meta_cast_scope( _self, m_scope_name, m_class_name, _scope ) );

			PyObject * py_value = pybind::ptr( obj->*m_member );

			return py_value;
		}

		int set( void * _self, PyObject * _args, class_type_scope * _scope ) override
		{
			C * obj = static_cast<C*>( detail::meta_cast_scope( _self, m_scope_name, m_class_name, _scope ) );

			obj->*m_member = pybind::extract<A>( _args );

			return 1;
		}

	protected:
        const char * m_name;
		A C:: * m_member;

		size_t m_class_name;
		size_t m_scope_name;
	};
	//////////////////////////////////////////////////////////////////////////
	template<class C, class FG, class FS>
	class member_adapter_property
		: public member_adapter_interface
	{
	public:
		member_adapter_property( const char * _name, FG _get, FS _set )
			: m_name(_name)
            , m_get(_get)
			, m_set(_set)
		{
			m_class_name = class_info<C*>();
			m_scope_name = class_info<C>();
		}

	public:
		PyObject * get( void * _self, class_type_scope * _scope ) override
		{
			C * obj = static_cast<C*>( detail::meta_cast_scope( _self, m_scope_name, m_class_name, _scope ) );

			PyObject * py_value = pybind::ptr( (obj->*m_get)() );

			return py_value;
		}

		int set( void * _self, PyObject * _args, class_type_scope * _scope ) override
		{
			C * obj = static_cast<C*>( detail::meta_cast_scope( _self, m_scope_name, m_class_name, _scope ) );

			typedef typename function_parser<FS>::result f_info;
				
			(obj->*m_set)(
				pybind::extract<typename f_info::param0>( _args )
				);

			return 1;
		}

	protected:
        const char * m_name;

		FG m_get;
		FS m_set;

		size_t m_class_name;
		size_t m_scope_name;
	};
	//////////////////////////////////////////////////////////////////////////
	template<class C, class FG, class FS>
	class member_adapter_property_static
		: public member_adapter_interface
	{
	public:
		member_adapter_property_static( const char * _name, FG _get, FS _set )
			: m_name(_name)
            , m_get(_get)
			, m_set(_set)
		{
            m_class_name = class_info<C*>();
            m_scope_name = class_info<C>();
		}

	public:
		PyObject * get( void * _self, class_type_scope * _scope ) override
		{
			C * obj = static_cast<C*>( detail::meta_cast_scope( _self, m_scope_name, m_class_name, _scope ) );

			PyObject * py_value = pybind::ptr( (*m_get)( obj ) );

			return py_value;
		}

		int set( void * _self, PyObject * _args, class_type_scope * _scope ) override
		{
			C * obj = static_cast<C*>( detail::meta_cast_scope( _self, m_scope_name, m_class_name, _scope ) );

			typedef typename function_parser<FS>::result f_info;

			(*m_set)( obj
				, pybind::extract<typename f_info::param1>( _args )
				);

			return 1;
		}

	protected:
        const char * m_name;

		FG m_get;
		FS m_set;

		size_t m_class_name;
		size_t m_scope_name;
	};
}

