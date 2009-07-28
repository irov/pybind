#	pragma once

#	include "pybind/method_call.hpp"

namespace pybind
{
	class functor_proxy_interface
	{
	public:
		virtual ~functor_proxy_interface(){};

	public:
		virtual PyObject * call() = 0;
		virtual PyObject * call( PyObject * _args ) = 0;
	};

	namespace detail
	{
		PYBIND_API functor_proxy_interface * extract_from_py_functor( PyObject * _self );
	}

	template<class C, class F>
	class functor_proxy
		: public functor_proxy_interface
	{
		typedef typename method_parser<F>::result f_info;

	public:
		functor_proxy( const char * _name, C * _self, F f  )
			: m_fn_name( _name )
			, m_self(_self)
			, m_fn( f )			
		{
		}

	public:
		PyObject * call() override
		{
			PyObject *ret = method_call<C,F>::call( m_self, m_fn, 0 );
			return ret;

			return 0;			
		}

		PyObject * call( PyObject * _args ) override
		{
			PyObject *ret = method_call<C,F>::call( m_self, m_fn, _args );
			return ret;

			return 0;	
		}

	public:
		static PyObject * 
			method0( PyObject * _self )
		{
			functor_proxy_interface * f = detail::extract_from_py_functor( _self );

			PyObject * ret = f->call();

			return ret;
		}

		static PyObject * 
			method1( PyObject * _self, PyObject * _args )
		{
			functor_proxy_interface * f = detail::extract_from_py_functor( _self );

			PyObject * ret = f->call( _args );

			return ret;
		}

	protected:
		const char * m_fn_name;
		C * m_self;
		F m_fn;
	};
}

