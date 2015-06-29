#	pragma once

#   include "pybind/adapter_interface.hpp"

#	include "pybind/method_call.hpp"

namespace pybind
{
	class functor_adapter_interface
        : public adapter_interface
	{
	public:
		functor_adapter_interface( const char * _name, uint32_t _arity )
			: m_name(_name)
			, m_arity(_arity)
		{
		}

	public:
		inline const char * getName() const
		{
			return m_name;
		}

    public:
		uint32_t getArity() const
		{
			return m_arity;
		}        

	public:
		virtual PyObject * call( PyObject * _args, PyObject * _kwds ) = 0;

	protected:
		const char * m_name;
		uint32_t m_arity;
	};
    //////////////////////////////////////////////////////////////////////////
    typedef stdex::intrusive_ptr<class functor_adapter_interface> functor_adapter_interface_ptr;
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
		PyObject * call( PyObject * _args, PyObject * _kwds ) override
		{
            (void)_kwds;

			PyObject * ret = method_call<C,M>::call( m_self, m_method, _args );
			
			return ret;	
		}

	protected:
		C * m_self;
		M m_method;		
	};
}

