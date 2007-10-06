#	pragma once

#	include "pybind/method_call.hpp"

namespace pybind
{
	class class_type_scope;

	namespace detail
	{
		PYBIND_API void * meta_cast_scope( void * _self, const char * _scope_name, const char * _class_name, class_type_scope * scope );
	}

	class method_proxy_interface
	{
	public:
		virtual ~method_proxy_interface(){};

	public:
		virtual PyObject * call( void * _self, class_type_scope * scope ) = 0;
		virtual PyObject * call( void * _self, class_type_scope * scope, PyObject * _args ) = 0;
	};

	template<class C, class F>
	class method_proxy
		: public method_proxy_interface
	{
	public:
		method_proxy( const char * _name, F f )
		: m_fn_name( _name )
		, m_fn( f )
		{
			const type_info & class_info = typeid(C*);
			m_class_name = class_info.name();

			const type_info & scope_info = typeid(C);
			m_scope_name = scope_info.name();
		}

	public:
		PyObject * call( void * _self, class_type_scope * _scope ) override
		{
			C * impl = (C*)detail::meta_cast_scope( _self, m_scope_name, m_class_name, _scope );
			PyObject *ret = method_call<C,F>::call( impl, m_fn, 0 );
			return ret;
		}

		PyObject * call( void * _self, class_type_scope * _scope, PyObject * _args ) override
		{
			C * impl = (C*)detail::meta_cast_scope( _self, m_scope_name, m_class_name, _scope );
			PyObject *ret = method_call<C,F>::call( impl, m_fn, _args );
			return ret;
		}

	protected:
		const char * m_fn_name;
		const char * m_class_name;
		const char * m_scope_name;
		F m_fn;
	};
}