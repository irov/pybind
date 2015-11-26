#	pragma once

#	include "pybind/adapter_interface.hpp"

#	include "pybind/method_call.hpp"
#	include "pybind/method_proxy_call.hpp"
#	include "pybind/function_proxy_call.hpp"

namespace pybind
{
	//////////////////////////////////////////////////////////////////////////
	typedef stdex::intrusive_ptr<class class_type_scope> class_type_scope_ptr;
	//////////////////////////////////////////////////////////////////////////
	class number_binary_adapter_interface
		: public adapter_interface
	{
	public:
		number_binary_adapter_interface( const char * _name )
			: m_name(_name)
		{
		}

	public:
		inline const char * getName() const
		{
			return m_name;
		}

	public:
		virtual PyObject * call( void * _self, const class_type_scope_ptr & _scope, PyObject * _value ) = 0;

	protected:
		const char * m_name;
	};
    //////////////////////////////////////////////////////////////////////////
	typedef stdex::intrusive_ptr<number_binary_adapter_interface> number_binary_adapter_interface_ptr;
	//////////////////////////////////////////////////////////////////////////
	template<class C, class F>
	class number_binary_adapter
		: public number_binary_adapter_interface
	{
	public:
		number_binary_adapter( const char * _name, F _fn )
			: number_binary_adapter_interface( _name )
			, m_fn(_fn)
		{
		}

	protected:
		PyObject * call( void * _impl, const class_type_scope_ptr & _scope, PyObject * _value ) override
		{
			C * self = detail::meta_cast_class_t<C>( _impl, _scope );
			
			PyObject * py_result = (detail::return_operator_t)(self->*m_fn)(
				detail::extract_operator_t( _value )
				);

			return py_result;
		}		

	protected:
		F m_fn;
	};
	//////////////////////////////////////////////////////////////////////////
	template<class C, class P, class F>
	class number_binary_adapter_proxy
		: public number_binary_adapter_interface
	{
	public:
		number_binary_adapter_proxy( const char * _name, F _fn, P * _proxy )
			: number_binary_adapter_interface( _name )
			, m_fn(_fn)
			, m_proxy(_proxy)
		{
		}

	protected:
		PyObject * call( void * _impl, const class_type_scope_ptr & _scope, PyObject * _value ) override
		{
			C * self = detail::meta_cast_class_t<C>( _impl, _scope );

			PyObject * py_result = (detail::return_operator_t)(m_proxy->*m_fn)(self
				, detail::extract_operator_t( _value )
				);

			return py_result;
		}

	protected:
		F m_fn;
		P * m_proxy;
	};
	//////////////////////////////////////////////////////////////////////////
	template<class C, class F>
	class number_binary_adapter_proxy_function
		: public number_binary_adapter_interface
	{
	public:
		number_binary_adapter_proxy_function( const char * _name, F _fn )
			: number_binary_adapter_interface( _name )
			, m_fn(_fn)
		{
		}

	protected:
		PyObject * call( void * _impl, const class_type_scope_ptr & _scope, PyObject * _value ) override
		{
			C * self = detail::meta_cast_class_t<C>( _impl, _scope );

			PyObject * py_result = (detail::return_operator_t)(*m_fn)(self
				, detail::extract_operator_t( _value )
				);

			return py_result;
		}

	protected:
		F m_fn;
	};
}

