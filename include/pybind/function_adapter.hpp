#	pragma once

#	include "pybind/adapter_interface.hpp"

#	include "pybind/function_call.hpp"
#	include "pybind/function_parser.hpp"

#	include "pybind/types.hpp"

namespace pybind
{
	class function_adapter_interface
		: public adapter_interface
	{
	public:
		virtual size_t getArity() const = 0;
		virtual const char * getName() const = 0;

	public:
		virtual PyObject * call( PyObject * _args, PyObject * _kwds ) = 0;
	};

    typedef stdex::intrusive_ptr<function_adapter_interface> function_adapter_interface_ptr;

	template<class F>
	class function_adapter_helper
		: public function_adapter_interface
	{
	public:
		function_adapter_helper( F _fn, const char * _name )
			: m_fn(_fn)
			, m_name(_name)
		{
		}

	protected:
		size_t getArity() const override
		{
			typedef typename function_parser<F>::result t_info;

			return t_info::arity;
		}

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

	template<class F>
	class function_adapter
		: public function_adapter_helper<F>
	{
	public:
		function_adapter( F _fn, const char * _tag )
			: function_adapter_helper<F>(_fn, _tag)
		{
		}
		
	protected:
		PyObject * call( PyObject * _args, PyObject * _kwds ) override
		{
            (void)_kwds;

			F fn = this->getFn();

			PyObject *ret = function_call<F>::call( fn, _args );

			return ret;
		}
	};

	template<class F>
	class function_adapter_native
		: public function_adapter_helper<F>
	{
	public:
		function_adapter_native( F _fn, const char * _tag )
			: function_adapter_helper<F>(_fn, _tag)
		{
		}

	protected:
		PyObject * call( PyObject * _args, PyObject * _kwds ) override
		{
			F fn = this->getFn();

			PyObject * ret = fn(_args, _kwds);

			return ret;
		}
	};
}