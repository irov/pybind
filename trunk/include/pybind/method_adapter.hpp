#	pragma once

#	include "pybind/adapter_interface.hpp"

#	include "pybind/method_call.hpp"
#	include "pybind/method_proxy_call.hpp"
#	include "pybind/function_proxy_call.hpp"

#   include "pybind/class_info.hpp"

namespace pybind
{
	class class_type_scope;

	namespace detail
	{
		PYBIND_API void * meta_cast_scope( void * _self, size_t _scope_name, size_t _class_name, class_type_scope * scope );
	}

	class method_adapter_interface
		: public adapter_interface
	{
	public:
		virtual PyObject * call( void * _self, class_type_scope * scope, PyObject * _args, PyObject * _kwds ) = 0;
	};

	template<class F>
	class method_adapter_helper
	{
	public:
		method_adapter_helper( F _fn, const char * _tag )
			: m_fn(_fn)
			, m_tag(_tag)
		{
		}

	protected:
		const char * getTag() const
		{
			return m_tag;
		}

		F getFn() const
		{
			return m_fn;
		}

	protected:
		F m_fn;
		const char * m_tag;
	};

	template<class C>
	class class_adapter_helper
	{
	protected:
		class_adapter_helper()
		{             
			m_class_name = class_info<C*>();
			m_scope_name = class_info<C>();
		}

	protected:
		size_t getClassName() const
		{
			return m_class_name;
		}

		size_t getScopeName() const
		{
			return m_scope_name;
		}

	protected:
		size_t m_class_name;
		size_t m_scope_name;
	};

	template<class C, class F>
	class method_adapter
		: public method_adapter_interface
		, public method_adapter_helper<F>
		, public class_adapter_helper<C>
	{
	public:
		method_adapter( F _fn, const char * _tag )
			: method_adapter_helper<F>(_fn, _tag)
		{
		}

	public:
		PyObject * call( void * _self, class_type_scope * _scope, PyObject * _args, PyObject * _kwds ) override
		{
            (void)_kwds;

			size_t scopeName = this->getScopeName();
			size_t className = this->getClassName();

			C * impl = (C*)detail::meta_cast_scope( _self, scopeName, className, _scope );

            const char * tag = this->getTag();
			F fn = this->getFn();

			PyObject *ret = method_call<C,F>::call( impl, fn, _args, tag );

			return ret;
		}		
	};

	template<class C, class P, class F>
	class method_adapter_proxy_member
		: public method_adapter_interface
		, public method_adapter_helper<F>
		, public class_adapter_helper<C>
	{
	public:
		method_adapter_proxy_member( P * _proxy, F _fn, const char * _tag )
			: method_adapter_helper<F>(_fn, _tag)
			, m_proxy(_proxy)
		{
		}

	public:
		PyObject * call( void * _self, class_type_scope * _scope, PyObject * _args, PyObject * _kwds ) override
		{
            (void)_kwds;

			size_t scopeName = this->getScopeName();
			size_t className = this->getClassName();

			C * impl = (C*)detail::meta_cast_scope( _self, scopeName, className, _scope );

            const char * tag = this->getTag();
            F fn = this->getFn();

			PyObject *ret = method_proxy_call<P,C,F>::call( m_proxy, impl, fn, _args, tag );

			return ret;
		}

	protected:
		P * m_proxy;
	};

	template<class C, class F>
	class method_adapter_proxy_function
		: public method_adapter_interface
		, public method_adapter_helper<F>
		, public class_adapter_helper<C>
	{
	public:
		method_adapter_proxy_function( F _fn, const char * _tag )
			: method_adapter_helper<F>(_fn, _tag)
		{
		}

	public:
		PyObject * call( void * _self, class_type_scope * _scope, PyObject * _args, PyObject * _kwds ) override
		{
            (void)_kwds;

			size_t scopeName = this->getScopeName();
			size_t className = this->getClassName();

			C * impl = (C*)detail::meta_cast_scope( _self, scopeName, className, _scope );

            const char * tag = this->getTag();
			F fn = this->getFn();

			PyObject *ret = function_proxy_call<C,F>::call( impl, fn, _args, tag );

			return ret;
		}
	};

	template<class C, class F>
	class method_adapter_native
		: public method_adapter_interface
		, public method_adapter_helper<F>
		, public class_adapter_helper<C>
	{
	public:
		method_adapter_native( F _fn, const char * _tag )
			: method_adapter_helper<F>(_fn, _tag)
		{
		}

	public:
		PyObject * call( void * _self, class_type_scope * _scope, PyObject * _args, PyObject * _kwds ) override
		{
			size_t scopeName = this->getScopeName();
			size_t className = this->getClassName();

			C * impl = (C*)detail::meta_cast_scope( _self, scopeName, className, _scope );

            F fn = this->getFn();

			PyObject * ret = (impl->*fn)( _args, _kwds );

			return ret;
		}
	};
}
