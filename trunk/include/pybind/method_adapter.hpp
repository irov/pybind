#	pragma once

#	include "pybind/adapter_interface.hpp"

#	include "pybind/method_call.hpp"
#	include "pybind/method_proxy_call.hpp"
#	include "pybind/function_proxy_call.hpp"

#   include "pybind/class_info.hpp"
#   include "pybind/class_type_scope.hpp"

namespace pybind
{
	class class_type_scope;

	class method_adapter_interface
		: public adapter_interface
	{
	public:
		virtual const char * getName() const = 0;

	public:
		virtual PyObject * call( void * _self, const class_type_scope_ptr & scope, PyObject * _args, PyObject * _kwds ) = 0;
	};
    //////////////////////////////////////////////////////////////////////////
    typedef stdex::intrusive_ptr<method_adapter_interface> method_adapter_interface_ptr;
    //////////////////////////////////////////////////////////////////////////
	template<class F>
	class method_adapter_helper
		: public method_adapter_interface
	{
	public:
		method_adapter_helper( F _fn, const char * _name )
			: m_fn(_fn)
			, m_name(_name)
		{
		}

	protected:
		const char * getName() const override
		{
			return m_name;
		}

		F getFn() const
		{
			return m_fn;
		}

	protected:
		F m_fn;
		const char * m_name;
	};

	template<class C>
	class class_adapter_helper
	{
	protected:
		class_adapter_helper()
		{             
			m_class_id = class_info<C*>();
			m_scope_id = class_info<C>();
		}

	protected:
		size_t getClassId() const
		{
			return m_class_id;
		}

		size_t getScopeId() const
		{
			return m_scope_id;
		}

	protected:
		size_t m_class_id;
		size_t m_scope_id;
	};

	template<class C, class F>
	class method_adapter
		: public method_adapter_helper<F>
		, public class_adapter_helper<C>
	{
	public:
		method_adapter( F _fn, const char * _tag )
			: method_adapter_helper<F>(_fn, _tag)
		{
		}

	protected:
		PyObject * call( void * _self, const class_type_scope_ptr & _scope, PyObject * _args, PyObject * _kwds ) override
		{
            (void)_kwds;

			size_t scopeId = this->getScopeId();
			size_t classId = this->getClassId();

			C * impl = (C*)detail::meta_cast_scope( _self, scopeId, classId, _scope );

			F fn = this->getFn();

			PyObject *ret = method_call<C,F>::call( impl, fn, _args );

			return ret;
		}		
	};

    template<class C, class F>
    class method_adapter_depricated
        : public method_adapter<C, F>
    {
    public:
        method_adapter_depricated( F _fn, const char * _tag, const char * _doc )
            : method_adapter<C, F>(_fn, _tag)
            , m_doc(_doc)
        {
        }

    protected:
        PyObject * call( void * _self, const class_type_scope_ptr & _scope, PyObject * _args, PyObject * _kwds ) override
        {
            size_t scopeId = this->getScopeId();
            const char * scopeName = detail::get_class_type_info( scopeId );

            const char * name = this->getName();

            pybind::error_traceback("method %s::%s depricated '%s'"
                , scopeName
                , name
                , m_doc
                );

            pybind::check_error();

            PyObject * ret = method_adapter<C, F>::call( _self, _scope, _args, _kwds );

            return ret;
        }

    protected:
        const char * m_doc;
    };

	template<class C, class P, class F>
	class method_adapter_proxy_member
		: public method_adapter_helper<F>
		, public class_adapter_helper<C>
	{
	public:
		method_adapter_proxy_member( P * _proxy, F _fn, const char * _tag )
			: method_adapter_helper<F>(_fn, _tag)
			, m_proxy(_proxy)
		{
		}

	protected:
		PyObject * call( void * _self, const class_type_scope_ptr & _scope, PyObject * _args, PyObject * _kwds ) override
		{
            (void)_kwds;

			size_t scopeId = this->getScopeId();
			size_t classId = this->getClassId();

			C * impl = (C*)detail::meta_cast_scope( _self, scopeId, classId, _scope );

            F fn = this->getFn();

			PyObject *ret = method_proxy_call<P,C,F>::call( m_proxy, impl, fn, _args );

			return ret;
		}

	protected:
		P * m_proxy;
	};

	template<class C, class P, class F>
	class method_adapter_proxy_native
		: public method_adapter_helper<F>
		, public class_adapter_helper<C>
	{
	public:
		method_adapter_proxy_native( P * _proxy, F _fn, const char * _tag )
			: method_adapter_helper<F>(_fn, _tag)
			, m_proxy(_proxy)
		{
		}

	protected:
		PyObject * call( void * _self, const class_type_scope_ptr & _scope, PyObject * _args, PyObject * _kwds ) override
		{
			size_t scopeId = this->getScopeId();
			size_t classId = this->getClassId();

			C * impl = (C*)detail::meta_cast_scope( _self, scopeId, classId, _scope );

			F fn = this->getFn();

			PyObject * ret = (m_proxy->*fn)( impl, _args, _kwds );

			return ret;
		}

	protected:
		P * m_proxy;
	};

	template<class C, class F>
	class method_adapter_proxy_function
		: public method_adapter_helper<F>
		, public class_adapter_helper<C>
	{
	public:
		method_adapter_proxy_function( F _fn, const char * _tag )
			: method_adapter_helper<F>(_fn, _tag)
		{
		}

	protected:
		PyObject * call( void * _self, const class_type_scope_ptr & _scope, PyObject * _args, PyObject * _kwds ) override
		{
            (void)_kwds;

			size_t scopeId = this->getScopeId();
			size_t classId = this->getClassId();

			C * impl = (C*)detail::meta_cast_scope( _self, scopeId, classId, _scope );

			F fn = this->getFn();

			PyObject * ret = function_proxy_call<C,F>::call( impl, fn, _args );

			return ret;
		}
	};

	template<class C, class F>
	class method_adapter_native
		: public method_adapter_helper<F>
		, public class_adapter_helper<C>
	{
	public:
		method_adapter_native( F _fn, const char * _tag )
			: method_adapter_helper<F>(_fn, _tag)
		{
		}

	protected:
		PyObject * call( void * _self, const class_type_scope_ptr & _scope, PyObject * _args, PyObject * _kwds ) override
		{
			size_t scopeId = this->getScopeId();
			size_t classId = this->getClassId();

			C * impl = (C*)detail::meta_cast_scope( _self, scopeId, classId, _scope );

            F fn = this->getFn();

			PyObject * ret = (impl->*fn)( _args, _kwds );

			return ret;
		}
	};
}

