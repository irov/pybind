#	pragma once

#	include "pybind/method_call.hpp"

namespace pybind
{
	struct py_class_type;

	namespace detail
	{
		PYBIND_API void * class_type_impl( py_class_type * _class_type );
	}

	class method_proxy_interface
	{
	public:
		virtual ~method_proxy_interface(){};

	public:
		virtual PyObject * call( void * _self ) = 0;
		virtual PyObject * call( void * _self, PyObject * _args ) = 0;
	};

	template<class C, class F>
	class method_proxy
		: public method_proxy_interface
	{
	public:
		method_proxy( const char * _name, F f )
		: m_name( _name )
		, fn( f )
		{
		}

	public:
		PyObject * call( void * _self ) override
		{
			C* impl = (C*)_self;
			PyObject *ret = method_call<C,F>::call( impl, fn, 0 );
			return ret;
		}

		PyObject * call( void * _self, PyObject * _args ) override
		{
			C* impl = (C*)_self;
			PyObject *ret = method_call<C,F>::call( impl, fn, _args );
			return ret;
		}

	protected:
		const char * m_name;
		F fn;
	};
}