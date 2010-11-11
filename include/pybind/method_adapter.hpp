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
		virtual PyObject * call( void * _self, class_type_scope * scope, PyObject * _args ) = 0;
	};

	template<class C, class F>
	class method_adapter_helper
		: public class_adapter_interface<C>
	{
	public:
		method_adapter_helper( F _fn, const char * _tag )
			: m_fn(_fn)
			, m_tag(_tag)
		{
		}

	protected:
		F m_fn;
		const char * m_tag;	
	};

	template<class C, class F>
	class method_adapter
		: public method_adapter_helper<C,F>
	{
	public:
		method_adapter( F _fn, const char * _tag )
			: method_adapter_helper<C,F>(_fn,_tag)
		{
		}

	public:
		PyObject * call( void * _self, class_type_scope * _scope, PyObject * _args ) override
		{
			C * impl = (C*)detail::meta_cast_scope( _self, m_scope_name, m_class_name, _scope );

			PyObject *ret = method_call<C,F>::call( impl, m_fn, _args, m_tag );
			return ret;
		}		
	};

	template<class C, class F, class P>
	class method_adapter_proxy_member
		: public method_adapter_helper<C,F>
	{
	public:
		method_adapter_proxy_member( P * _proxy, F _fn, const char * _tag )
			: method_adapter_helper<C,F>(_fn, _tag)
			, m_proxy(_proxy)
		{
		}

	public:
		PyObject * call( void * _self, class_type_scope * _scope ) override
		{
			return this->call( _self, _scope, 0 );
		}

		PyObject * call( void * _self, class_type_scope * _scope, PyObject * _args ) override
		{
			C * impl = (C*)detail::meta_cast_scope( _self, m_scope_name, m_class_name, _scope );

			PyObject *ret = method_proxy_call<P,C,F>::call( m_proxy, impl, m_fn, _args, m_tag );
			return ret;
		}

	protected:
		P * m_proxy;
	};

	template<class C, class F>
	class method_adapter_proxy_function
		: public method_adapter_helper<C,F>
	{
	public:
		method_adapter_proxy_function( F _fn, const char * _tag )
			: method_adapter_helper<C,F>(_fn, _tag)
		{
		}

	public:
		PyObject * call( void * _self, class_type_scope * _scope, PyObject * _args ) override
		{
			C * impl = (C*)detail::meta_cast_scope( _self, m_scope_name, m_class_name, _scope );

			PyObject *ret = function_proxy_call<C,F>::call( impl, m_fn, _args, m_tag );
			return ret;
		}
	};
}

