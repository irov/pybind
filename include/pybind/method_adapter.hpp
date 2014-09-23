#	pragma once

#	include "pybind/adapter_interface.hpp"

#	include "pybind/method_call.hpp"
#	include "pybind/method_proxy_call.hpp"
#	include "pybind/function_proxy_call.hpp"

#   include "pybind/class_info.hpp"
#   include "pybind/class_type_scope.hpp"

namespace pybind
{
	//////////////////////////////////////////////////////////////////////////
	class method_adapter_interface
		: public adapter_interface
	{
	public:
		method_adapter_interface( const char * _name )
			: m_name(_name)
		{
		}

	public:
		inline const char * getName() const
		{
			return m_name;
		}

	public:
		virtual PyObject * call( void * _self, const class_type_scope_ptr & scope, PyObject * _args, PyObject * _kwds ) = 0;

	protected:
		const char * m_name;
	};
    //////////////////////////////////////////////////////////////////////////
    typedef stdex::intrusive_ptr<method_adapter_interface> method_adapter_interface_ptr;
    //////////////////////////////////////////////////////////////////////////
	template<class F>
	class method_adapter_helper
	{
	public:
		method_adapter_helper( F _fn )
			: m_fn(_fn)
		{
		}

	protected:
		F getFn() const
		{
			return m_fn;
		}

	protected:
		F m_fn;
	};
	//////////////////////////////////////////////////////////////////////////
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
		uint32_t getClassId() const
		{
			return m_class_id;
		}

		uint32_t getScopeId() const
		{
			return m_scope_id;
		}

	protected:
		uint32_t m_class_id;
		uint32_t m_scope_id;
	};
	//////////////////////////////////////////////////////////////////////////
	template<class C, class F>
	class method_adapter
		: public method_adapter_interface
		, public method_adapter_helper<F>
		, public class_adapter_helper<C>
	{
	public:
		method_adapter( const char * _name, F _fn )
			: method_adapter_interface(_name)
			, method_adapter_helper<F>(_fn)
		{
		}

	protected:
		PyObject * call( void * _impl, const class_type_scope_ptr & _scope, PyObject * _args, PyObject * _kwds ) override
		{
            (void)_kwds;

			uint32_t scopeId = this->getScopeId();
			uint32_t classId = this->getClassId();

			void * meta_impl = detail::meta_cast_scope( _impl, scopeId, classId, _scope );

			C * self = static_cast<C *>(meta_impl);

			F fn = this->getFn();

			PyObject *ret = method_call<C,F>::call( self, fn, _args );

			return ret;
		}		
	};
	//////////////////////////////////////////////////////////////////////////
    template<class C, class F>
    class method_adapter_depricated
        : public method_adapter<C, F>
    {
    public:
        method_adapter_depricated( const char * _name, F _fn, const char * _doc )
            : method_adapter<C, F>(_name, _fn)
            , m_doc(_doc)
        {
        }

    protected:
        PyObject * call( void * _impl, const class_type_scope_ptr & _scope, PyObject * _args, PyObject * _kwds ) override
        {
            uint32_t scopeId = this->getScopeId();
            const char * scopeName = detail::get_class_type_info( scopeId );

            const char * name = this->getName();

            pybind::error_traceback("method %s::%s depricated '%s'"
                , scopeName
                , name
                , m_doc
                );

            pybind::check_error();

            PyObject * ret = method_adapter<C, F>::call( _impl, _scope, _args, _kwds );

            return ret;
        }

    protected:
        const char * m_doc;
    };
	//////////////////////////////////////////////////////////////////////////
	template<class C, class P, class F>
	class method_adapter_proxy_member
		: public method_adapter_interface
		, public method_adapter_helper<F>
		, public class_adapter_helper<C>
	{
	public:
		method_adapter_proxy_member( const char * _name, F _fn, P * _proxy )
			: method_adapter_interface(_name)
			, method_adapter_helper<F>(_fn)
			, m_proxy(_proxy)
		{
		}

	protected:
		PyObject * call( void * _impl, const class_type_scope_ptr & _scope, PyObject * _args, PyObject * _kwds ) override
		{
            (void)_kwds;

			uint32_t scopeId = this->getScopeId();
			uint32_t classId = this->getClassId();

			void * meta_impl = detail::meta_cast_scope( _impl, scopeId, classId, _scope );

			C * self = static_cast<C *>(meta_impl);

            F fn = this->getFn();

			PyObject * ret = method_proxy_call<P,C,F>::call( m_proxy, self, fn, _args );

			return ret;
		}

	protected:
		P * m_proxy;
	};
	//////////////////////////////////////////////////////////////////////////
	template<class C, class P, class F>
	class method_adapter_proxy_native
		: public method_adapter_interface
		, public method_adapter_helper<F>
		, public class_adapter_helper<C>
	{
	public:
		method_adapter_proxy_native( const char * _name, F _fn, P * _proxy )
			: method_adapter_interface(_name)
			, method_adapter_helper<F>(_fn)
			, m_proxy(_proxy)
		{
		}

	protected:
		PyObject * call( void * _impl, const class_type_scope_ptr & _scope, PyObject * _args, PyObject * _kwds ) override
		{
			uint32_t scopeId = this->getScopeId();
			uint32_t classId = this->getClassId();

			void * meta_impl = detail::meta_cast_scope( _impl, scopeId, classId, _scope );

			C * self = static_cast<C *>(meta_impl);

			F fn = this->getFn();

			PyObject * ret = (m_proxy->*fn)( self, _args, _kwds );

			return ret;
		}

	protected:
		P * m_proxy;
	};
	//////////////////////////////////////////////////////////////////////////
	template<class C, class F>
	class method_adapter_proxy_function
		: public method_adapter_interface
		, public method_adapter_helper<F>
		, public class_adapter_helper<C>
	{
	public:
		method_adapter_proxy_function( const char * _name, F _fn )
			: method_adapter_interface(_name)
			, method_adapter_helper<F>(_fn)
		{
		}

	protected:
		PyObject * call( void * _impl, const class_type_scope_ptr & _scope, PyObject * _args, PyObject * _kwds ) override
		{
            (void)_kwds;

			uint32_t scopeId = this->getScopeId();
			uint32_t classId = this->getClassId();

			void * meta_impl = detail::meta_cast_scope( _impl, scopeId, classId, _scope );

			C * self = static_cast<C *>(meta_impl);

			F fn = this->getFn();

			PyObject * ret = function_proxy_call<C,F>::call( self, fn, _args );

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
		method_adapter_native( const char * _name, F _fn )
			: method_adapter_interface(_name)
			, method_adapter_helper<F>(_fn)
		{
		}

	protected:
		PyObject * call( void * _impl, const class_type_scope_ptr & _scope, PyObject * _args, PyObject * _kwds ) override
		{
			uint32_t scopeId = this->getScopeId();
			uint32_t classId = this->getClassId();

			void * meta_impl = detail::meta_cast_scope( _impl, scopeId, classId, _scope );

			C * self = static_cast<C *>(meta_impl);

            F fn = this->getFn();

			PyObject * ret = (self->*fn)( _args, _kwds );

			return ret;
		}
	};
}

