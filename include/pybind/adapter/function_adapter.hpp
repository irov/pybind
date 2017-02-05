#	pragma once

#	include "pybind/function_interface.hpp"

#	include "pybind/function_call.hpp"

#	include "pybind/types.hpp"

namespace pybind
{
	//////////////////////////////////////////////////////////////////////////
	template<class F>
	class function_adapter_helper
	{
	public:
		function_adapter_helper( F _fn )			
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
	template<class F>
	class function_adapter
		: public function_adapter_interface
		, public function_adapter_helper<F>
	{
	public:
		function_adapter( const char * _name, uint32_t _arity, F _fn )
			: function_adapter_interface(_name, _arity)
			, function_adapter_helper<F>(_fn)
		{
		}
		
	protected:
		PyObject * call( kernel_interface * _kernel, PyObject * _args, PyObject * _kwds ) override
		{
			(void)_kernel;
            (void)_kwds;

			F fn = this->getFn();

			PyObject *ret = function_call<F>::call( _kernel, fn, _args );

			return ret;
		}
	};
	//////////////////////////////////////////////////////////////////////////
	template<class F>
	class function_adapter_native
		: public function_adapter_interface
		, public function_adapter_helper<F>
	{
	public:
		function_adapter_native( const char * _name, uint32_t _arity, F _fn )
			: function_adapter_interface(_name, _arity)
			, function_adapter_helper<F>(_fn)
		{
		}

	protected:
		PyObject * call( kernel_interface * _kernel, PyObject * _args, PyObject * _kwds ) override
		{
			F fn = this->getFn();

			PyObject * ret = fn( _kernel, _args, _kwds );

			return ret;
		}
	};
}