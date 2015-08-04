#	pragma once

#	include "pybind/adapter_interface.hpp"
#   include "pybind/class_type_scope_interface.hpp"

#	include "pybind/method_call.hpp"
#	include "pybind/method_proxy_call.hpp"
#	include "pybind/function_proxy_call.hpp"

#   include "pybind/detail.hpp"

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
		virtual PyObject * call( kernel_interface * _kernel, void * _self, const class_type_scope_interface_ptr & scope, PyObject * _args, PyObject * _kwds ) = 0;

	protected:
		const char * m_name;
	};
    //////////////////////////////////////////////////////////////////////////
    typedef stdex::intrusive_ptr<method_adapter_interface> method_adapter_interface_ptr;
	//////////////////////////////////////////////////////////////////////////
	template<class C, class F>
	class method_adapter
		: public method_adapter_interface
	{
	public:
		method_adapter( const char * _name, F _fn )
			: method_adapter_interface(_name)
			, m_fn(_fn)
		{
		}

	protected:
		PyObject * call( kernel_interface * _kernel, void * _impl, const class_type_scope_interface_ptr & _scope, PyObject * _args, PyObject * _kwds ) override
		{
            (void)_kwds;

			uint32_t class_id = _kernel->class_info<C*>();
			uint32_t scope_id = _kernel->class_info<C>();

			C * self = detail::meta_cast_scope_t<C *>( _impl, scope_id, class_id, _scope );
			
			PyObject *ret = method_call<C, F>::call( self, m_fn, _args );

			return ret;
		}		

	protected:
		F m_fn;
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
		PyObject * call( kernel_interface * _kernel, void * _impl, const class_type_scope_interface_ptr & _scope, PyObject * _args, PyObject * _kwds ) override
        {
			uint32_t scope_id = _kernel->class_info<C>();

            const char * scopeName = _kernel->get_class_type_info( scope_id );

            const char * name = this->getName();

            pybind::error_traceback("method %s::%s depricated '%s'"
                , scopeName
                , name
                , m_doc
                );

            pybind::check_error();

			PyObject * ret = method_adapter<C, F>::call( _kernel, _impl, _scope, _args, _kwds );

            return ret;
        }

    protected:
        const char * m_doc;
    };
	//////////////////////////////////////////////////////////////////////////
	template<class C, class P, class F>
	class method_adapter_proxy
		: public method_adapter_interface
	{
	public:
		method_adapter_proxy( const char * _name, F _fn, P * _proxy )
			: method_adapter_interface(_name)
			, m_fn(_fn)
			, m_proxy(_proxy)
		{
		}

	protected:
		PyObject * call( kernel_interface * _kernel, void * _impl, const class_type_scope_interface_ptr & _scope, PyObject * _args, PyObject * _kwds ) override
		{
            (void)_kwds;

			uint32_t class_id = _kernel->class_info<C*>();
			uint32_t scope_id = _kernel->class_info<C>();

			C * self = detail::meta_cast_scope_t<C *>( _impl, scope_id, class_id, _scope );

			PyObject * ret = method_proxy_call<P, C, F>::call( m_proxy, self, m_fn, _args );

			return ret;
		}

	protected:
		F m_fn;
		P * m_proxy;
	};
	//////////////////////////////////////////////////////////////////////////
	template<class C, class P, class F>
	class method_adapter_proxy_native
		: public method_adapter_interface
	{
	public:
		method_adapter_proxy_native( const char * _name, F _fn, P * _proxy )
			: method_adapter_interface(_name)
			, m_fn(_fn)
			, m_proxy(_proxy)
		{
		}

	protected:
		PyObject * call( kernel_interface * _kernel, void * _impl, const class_type_scope_interface_ptr & _scope, PyObject * _args, PyObject * _kwds ) override
		{
			uint32_t class_id = _kernel->class_info<C*>();
			uint32_t scope_id = _kernel->class_info<C>();

			C * self = detail::meta_cast_scope_t<C *>( _impl, scope_id, class_id, _scope );

			PyObject * ret = (m_proxy->*m_fn)(self, _args, _kwds);

			return ret;
		}

	protected:
		F m_fn;
		P * m_proxy;		
	};
	//////////////////////////////////////////////////////////////////////////
	template<class C, class F>
	class method_adapter_proxy_function
		: public method_adapter_interface
	{
	public:
		method_adapter_proxy_function( const char * _name, F _fn )
			: method_adapter_interface(_name)
			, m_fn(_fn)
		{
		}

	protected:
		PyObject * call( kernel_interface * _kernel, void * _impl, const class_type_scope_interface_ptr & _scope, PyObject * _args, PyObject * _kwds ) override
		{
            (void)_kwds;

			uint32_t class_id = _kernel->class_info<C*>();
			uint32_t scope_id = _kernel->class_info<C>();

			C * self = detail::meta_cast_scope_t<C *>( _impl, scope_id, class_id, _scope );

			PyObject * ret = function_proxy_call<C, F>::call( self, m_fn, _args );

			return ret;
		}

	protected:
		F m_fn;
	};

	template<class C, class F>
	class method_adapter_native
		: public method_adapter_interface
	{
	public:
		method_adapter_native( const char * _name, F _fn )
			: method_adapter_interface(_name)
			, m_fn(_fn)
		{
		}

	protected:
		PyObject * call( kernel_interface * _kernel, void * _impl, const class_type_scope_interface_ptr & _scope, PyObject * _args, PyObject * _kwds ) override
		{
			uint32_t class_id = _kernel->class_info<C*>();
			uint32_t scope_id = _kernel->class_info<C>();

			C * self = detail::meta_cast_scope_t<C *>( _impl, scope_id, class_id, _scope );

			PyObject * ret = (self->*m_fn)(_args, _kwds);

			return ret;
		}

	protected:
		F m_fn;
	};
}

