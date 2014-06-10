#	pragma once

#	include "pybind/adapter_interface.hpp"

#	include "pybind/function_call.hpp"
#	include "pybind/function_parser.hpp"

#	include "pybind/types.hpp"

namespace pybind
{
	//////////////////////////////////////////////////////////////////////////
	class function_adapter_interface
		: public adapter_interface
	{
	public:
		function_adapter_interface( const char * _name, size_t _arity )
			: m_name(_name)
			, m_arity(_arity)
		{
		}

	public:
		const char * getName() const
		{
			return m_name;
		}

		size_t getArity() const
		{
			return m_arity;
		}

	public:
		virtual PyObject * call( PyObject * _args, PyObject * _kwds ) = 0;

	protected:
		const char * m_name;
		size_t m_arity;
	};
	//////////////////////////////////////////////////////////////////////////
    typedef stdex::intrusive_ptr<function_adapter_interface> function_adapter_interface_ptr;
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
		function_adapter( const char * _name, size_t _arity, F _fn )
			: function_adapter_interface(_name, _arity)
			, function_adapter_helper<F>(_fn)
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
	//////////////////////////////////////////////////////////////////////////
	template<class F>
	class function_adapter_native
		: public function_adapter_interface
		, public function_adapter_helper<F>
	{
	public:
		function_adapter_native( const char * _name, size_t _arity, F _fn )
			: function_adapter_interface(_name, _arity)
			, function_adapter_helper<F>(_fn)
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