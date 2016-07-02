#	pragma once

#	include "pybind/method_interface.hpp"

#	include "pybind/method_call.hpp"
#	include "pybind/method_args_call.hpp"
#	include "pybind/method_proxy_call.hpp"
#	include "pybind/method_proxy_args_call.hpp"
#	include "pybind/function_proxy_call.hpp"

namespace pybind
{
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
		PyObject * call( void * _impl, const class_type_scope_ptr & _scope, PyObject * _args, PyObject * _kwds ) override
		{
            (void)_kwds;

			kernel_interface * kernel = pybind::get_kernel();

			C * self = kernel->meta_cast_class_t<C>( _impl, _scope );
			
			PyObject *ret = method_call<C, F>::call( self, m_fn, _args );

			return ret;
		}

	protected:
		F m_fn;
	};
	//////////////////////////////////////////////////////////////////////////
	template<class C, class F>
	class method_adapter_args
		: public method_adapter_interface
	{
	public:
		method_adapter_args( const char * _name, F _fn )
			: method_adapter_interface( _name )
			, m_fn( _fn )
		{
		}

	protected:
		PyObject * call( void * _impl, const class_type_scope_ptr & _scope, PyObject * _args, PyObject * _kwds ) override
		{
			(void)_kwds;

			C * self = detail::meta_cast_class_t<C>( _impl, _scope );

			PyObject *ret = method_args_call<C, F>::call( self, m_fn, _args );

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
		PyObject * call( void * _impl, const class_type_scope_ptr & _scope, PyObject * _args, PyObject * _kwds ) override
        {
			kernel_interface * kernel = pybind::get_kernel();
			
			const char * scopeName = kernel->get_class_type_info_t<C>();

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
		PyObject * call( void * _impl, const class_type_scope_ptr & _scope, PyObject * _args, PyObject * _kwds ) override
		{
            (void)_kwds;

			kernel_interface * kernel = pybind::get_kernel();

			C * self = kernel->meta_cast_class_t<C>( _impl, _scope );

			PyObject * ret = method_proxy_call<P, C, F>::call( m_proxy, self, m_fn, _args );

			return ret;
		}

	protected:
		F m_fn;
		P * m_proxy;
	};
	//////////////////////////////////////////////////////////////////////////
	template<class C, class P, class F>
	class method_adapter_proxy_args
		: public method_adapter_interface
	{
	public:
		method_adapter_proxy_args( const char * _name, F _fn, P * _proxy )
			: method_adapter_interface( _name )
			, m_fn( _fn )
			, m_proxy( _proxy )
		{
		}

	protected:
		PyObject * call( void * _impl, const class_type_scope_ptr & _scope, PyObject * _args, PyObject * _kwds ) override
		{
			(void)_kwds;

			kernel_interface * kernel = pybind::get_kernel();

			C * self = kernel->meta_cast_class_t<C>( _impl, _scope );

			PyObject * ret = method_proxy_args_call<P, C, F>::call( m_proxy, self, m_fn, _args );

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
		PyObject * call( void * _impl, const class_type_scope_ptr & _scope, PyObject * _args, PyObject * _kwds ) override
		{
			kernel_interface * kernel = pybind::get_kernel();

			C * self = kernel->meta_cast_class_t<C>( _impl, _scope );

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
		PyObject * call( void * _impl, const class_type_scope_ptr & _scope, PyObject * _args, PyObject * _kwds ) override
		{
            (void)_kwds;

			kernel_interface * kernel = pybind::get_kernel();

			C * self = kernel->meta_cast_class_t<C>( _impl, _scope );

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
		PyObject * call( void * _impl, const class_type_scope_ptr & _scope, PyObject * _args, PyObject * _kwds ) override
		{
			kernel_interface * kernel = pybind::get_kernel();

			C * self = kernel->meta_cast_class_t<C>( _impl, _scope );

			PyObject * ret = (self->*m_fn)(_args, _kwds);

			return ret;
		}

	protected:
		F m_fn;
	};
}

