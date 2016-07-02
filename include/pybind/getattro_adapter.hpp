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
	class getattro_adapter_interface
		: public adapter_interface
	{
	public:
		getattro_adapter_interface( const char * _name )
			: m_name(_name)
		{
		}

	public:
		inline const char * getName() const
		{
			return m_name;
		}

	public:
		virtual PyObject * call( void * _self, const class_type_scope_ptr & _scope, PyObject * _key ) = 0;

	protected:
		const char * m_name;
	};
    //////////////////////////////////////////////////////////////////////////
	typedef stdex::intrusive_ptr<getattro_adapter_interface> getattro_adapter_interface_ptr;
	//////////////////////////////////////////////////////////////////////////
	template<class C, class F>
	class getattro_adapter
		: public getattro_adapter_interface
	{
	public:
		getattro_adapter( const char * _name, F _fn )
			: getattro_adapter_interface( _name )
			, m_fn(_fn)
		{
		}

	protected:
		PyObject * call( void * _impl, const class_type_scope_ptr & _scope, PyObject * _key ) override
		{
			kernel_interface * kernel = pybind::get_kernel();

			C * self = kernel->meta_cast_class_t<C>( _impl, _scope );
			
			PyObject * py_result = (detail::return_operator_t)(self->*m_fn)(
				detail::extract_operator_t( _key )
				);

			return py_result;
		}		

	protected:
		F m_fn;
	};	
	//////////////////////////////////////////////////////////////////////////
	template<class C, class F>
	class getattro_adapter_depricated
		: public getattro_adapter<C, F>
	{
	public:
		getattro_adapter_depricated( const char * _name, F _fn, const char * _doc )
			: getattro_adapter<C, F>( _name, _fn )
			, m_doc( _doc )
		{
		}

	protected:
		PyObject * call( void * _impl, const class_type_scope_ptr & _scope, PyObject * _key ) override
		{
			kernel_interface * kernel = pybind::get_kernel();
			
			const char * scopeName = kernel->get_class_type_info_t<C>();

			const char * name = this->getName();

			pybind::error_traceback( "method %s::%s depricated '%s'"
				, scopeName
				, name
				, m_doc
				);

			pybind::check_error();

			PyObject * ret = getattro_adapter<C, F>::call( _impl, _scope, _key );

			return ret;
		}

	protected:
		const char * m_doc;
	};
	//////////////////////////////////////////////////////////////////////////
	template<class C, class P, class F>
	class getattro_adapter_proxy
		: public getattro_adapter_interface
	{
	public:
		getattro_adapter_proxy( const char * _name, F _fn, P * _proxy )
			: getattro_adapter_interface( _name )
			, m_fn(_fn)
			, m_proxy(_proxy)
		{
		}

	protected:
		PyObject * call( void * _impl, const class_type_scope_ptr & _scope, PyObject * _key ) override
		{
			kernel_interface * kernel = pybind::get_kernel();

			C * self = kernel->meta_cast_class_t<C>( _impl, _scope );
			
			PyObject * py_result = (detail::return_operator_t)(m_proxy->*m_fn)(self
				, detail::extract_operator_t( _key )
				);

			return py_result;
		}

	protected:
		F m_fn;
		P * m_proxy;
	};
	//////////////////////////////////////////////////////////////////////////
	template<class C, class F>
	class getattro_adapter_proxy_function
		: public getattro_adapter_interface
	{
	public:
		getattro_adapter_proxy_function( const char * _name, F _fn )
			: getattro_adapter_interface( _name )
			, m_fn(_fn)
		{
		}

	protected:
		PyObject * call( void * _impl, const class_type_scope_ptr & _scope, PyObject * _key ) override
		{
			kernel_interface * kernel = pybind::get_kernel();

			C * self = kernel->meta_cast_class_t<C>( _impl, _scope );

			PyObject * py_result = (detail::return_operator_t)(*m_fn)(self
				, detail::extract_operator_t( _key )
				);

			return py_result;
		}

	protected:
		F m_fn;
	};
}

