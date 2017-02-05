#	pragma once

#   include "pybind/functor_interface.hpp"

#	include "pybind/method_call.hpp"
#	include "pybind/method_args_call.hpp"

namespace pybind
{
    //////////////////////////////////////////////////////////////////////////
	template<class C, class M>
	class functor_proxy_adapter
		: public functor_adapter_interface
	{
	public:
		functor_proxy_adapter( const char * _name, uint32_t _arity, C * _self, M _method )
			: functor_adapter_interface(_name, _arity)
			, m_self(_self)
			, m_method(_method)			
		{
		}

	public:
		PyObject * call( kernel_interface * _kernel, PyObject * _args, PyObject * _kwds ) override
		{
			(void)_kernel;
            (void)_kwds;

			PyObject * ret = method_call<C,M>::call( _kernel, m_self, m_method, _args );
			
			return ret;	
		}

	protected:
		C * m_self;
		M m_method;		
	};
	//////////////////////////////////////////////////////////////////////////
	template<class C, class M>
	class functor_proxy_kernel_adapter
		: public functor_adapter_interface
	{
	public:
		functor_proxy_kernel_adapter( const char * _name, uint32_t _arity, C * _self, M _method )
			: functor_adapter_interface( _name, _arity )
			, m_self( _self )
			, m_method( _method )
		{
		}

	public:
		PyObject * call( kernel_interface * _kernel, PyObject * _args, PyObject * _kwds ) override
		{
			(void)_kernel;
			(void)_kwds;

			PyObject * ret = method_kernel_call<C, M>::call( _kernel, m_self, m_method, _args );

			return ret;
		}

	protected:
		C * m_self;
		M m_method;
	};
	//////////////////////////////////////////////////////////////////////////
	template<class C, class M>
	class functor_proxy_adapter_args
		: public functor_adapter_interface
	{
	public:
		functor_proxy_adapter_args( const char * _name, uint32_t _arity, C * _self, M _method )
			: functor_adapter_interface( _name, _arity )
			, m_self( _self )
			, m_method( _method )
		{
		}

	public:
		PyObject * call( kernel_interface * _kernel, PyObject * _args, PyObject * _kwds ) override
		{
			(void)_kernel;
			(void)_kwds;

			PyObject * ret = method_args_call<C, M>::call( _kernel, m_self, m_method, _args );

			return ret;
		}

	protected:
		C * m_self;
		M m_method;
	};
}

