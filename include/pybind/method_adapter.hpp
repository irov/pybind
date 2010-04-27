#	pragma once

#	include "pybind/adapter_interface.hpp"

#	include "pybind/method_call.hpp"
#	include "pybind/method_proxy_call.hpp"
#	include "pybind/function_proxy_call.hpp"


namespace pybind
{
	class class_type_scope;

	namespace detail
	{
		PYBIND_API void * meta_cast_scope( void * _self, const char * _scope_name, const char * _class_name, class_type_scope * scope );
	}

	class method_adapter_interface
		: public adapter_interface
	{
	public:
		virtual PyObject * call( void * _self, class_type_scope * scope ) = 0;
		virtual PyObject * call( void * _self, class_type_scope * scope, PyObject * _args ) = 0;
	};

	template<class C, class F>
	class method_adapter
		: public method_adapter_interface
	{
	public:
		method_adapter( const char * _name, F f )
		: m_fn_name( _name )
		, m_fn( f )
		{
			const std::type_info & class_info = typeid(C*);
			m_class_name = class_info.name();

			const std::type_info & scope_info = typeid(C);
			m_scope_name = scope_info.name();
		}

	public:
		PyObject * call( void * _self, class_type_scope * _scope ) override
		{
			return method_adapter::call( _self, _scope, 0 );
		}

		PyObject * call( void * _self, class_type_scope * _scope, PyObject * _args ) override
		{
			C * impl = (C*)detail::meta_cast_scope( _self, m_scope_name, m_class_name, _scope );

			PyObject *ret = method_call<C,F>::call( impl, m_fn, _args );
			return ret;
		}

	protected:
		const char * m_fn_name;
		F m_fn;

		const char * m_class_name;
		const char * m_scope_name;
	};

	template<class C, class P, class F>
	class method_adapter_proxy_member
		: public method_adapter_interface
	{
	public:
		method_adapter_proxy_member( const char * _name, P * _proxy, F f )
			: m_fn_name(_name)
			, m_proxy(_proxy)
			, m_fn(f)
		{
			const std::type_info & class_info = typeid(C*);
			m_class_name = class_info.name();

			const std::type_info & scope_info = typeid(C);
			m_scope_name = scope_info.name();
		}

	public:
		PyObject * call( void * _self, class_type_scope * _scope ) override
		{
			return this->call( _self, _scope, 0 );
		}

		PyObject * call( void * _self, class_type_scope * _scope, PyObject * _args ) override
		{
			C * impl = (C*)detail::meta_cast_scope( _self, m_scope_name, m_class_name, _scope );

			PyObject *ret = method_proxy_call<P,C,F>::call( m_proxy, impl, m_fn, _args );
			return ret;
		}

	protected:
		const char * m_fn_name;
		P * m_proxy;
		F m_fn;

		const char * m_class_name;
		const char * m_scope_name;
	};

	template<class C, class F>
	class method_adapter_proxy_function
		: public method_adapter_interface
	{
	public:
		method_adapter_proxy_function( const char * _name, F f )
			: m_fn_name(_name)
			, m_fn(f)
		{
			const std::type_info & class_info = typeid(C*);
			m_class_name = class_info.name();

			const std::type_info & scope_info = typeid(C);
			m_scope_name = scope_info.name();
		}

	public:
		PyObject * call( void * _self, class_type_scope * _scope ) override
		{
			return this->call( _self, _scope, 0 );
		}

		PyObject * call( void * _self, class_type_scope * _scope, PyObject * _args ) override
		{
			C * impl = (C*)detail::meta_cast_scope( _self, m_scope_name, m_class_name, _scope );

			PyObject *ret = function_proxy_call<C,F>::call( impl, m_fn, _args );
			return ret;
		}

	protected:
		const char * m_fn_name;
		F m_fn;

		const char * m_class_name;
		const char * m_scope_name;
	};
}

