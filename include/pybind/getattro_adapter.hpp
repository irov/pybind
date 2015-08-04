#	pragma once

#	include "pybind/adapter_interface.hpp"

#	include "pybind/method_call.hpp"
#	include "pybind/method_proxy_call.hpp"
#	include "pybind/function_proxy_call.hpp"

#   include "pybind/class_type_scope_interface.hpp"

#	include "pybind/detail.hpp"

namespace pybind
{
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
		virtual PyObject * call( kernel_interface * _kernel, void * _self, const class_type_scope_interface_ptr & _scope, PyObject * _key ) = 0;

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
		typedef typename function_parser<F>::result f_info;

	public:
		getattro_adapter( const char * _name, F _fn )
			: getattro_adapter_interface( _name )
			, m_fn(_fn)
		{
		}

	protected:
		PyObject * call( kernel_interface * _kernel, void * _impl, const class_type_scope_interface_ptr & _scope, PyObject * _key ) override
		{
			uint32_t class_id = _kernel->class_info<C*>();
			uint32_t scope_id = _kernel->class_info<C>();

			C * self = detail::meta_cast_scope_t<C *>( _impl, scope_id, class_id, _scope );
			
			typename f_info::ret_type result = (self->*m_fn)(_kernel
				, extract_throw<typename f_info::param1>( _key )
				);

			PyObject * py_result = pybind::ptr( result );

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
		PyObject * call( kernel_interface * _kernel, void * _impl, const class_type_scope_interface_ptr & _scope, PyObject * _key ) override
		{
			uint32_t scope_id = _kernel->class_info<C>();

			const char * scopeName = _kernel->get_class_type_info( scope_id );

			const char * name = this->getName();

			pybind::error_traceback( "method %s::%s depricated '%s'"
				, scopeName
				, name
				, m_doc
				);

			pybind::check_error();

			PyObject * ret = getattro_adapter<C, F>::call( _kernel, _impl, _scope, _key );

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
		typedef typename function_parser<F>::result f_info;

	public:
		getattro_adapter_proxy( const char * _name, F _fn, P * _proxy )
			: getattro_adapter_interface( _name )
			, m_fn(_fn)
			, m_proxy(_proxy)
		{
		}

	protected:
		PyObject * call( kernel_interface * _kernel, void * _impl, const class_type_scope_interface_ptr & _scope, PyObject * _key ) override
		{
			uint32_t class_id = _kernel->class_info<C*>();
			uint32_t scope_id = _kernel->class_info<C>();

			C * self = detail::meta_cast_scope_t<C *>( _impl, scope_id, class_id, _scope );
			
			typename f_info::ret_type result = (m_proxy->*m_fn)(_kernel, self
				, extract_throw<typename f_info::param2>( _key )
				);

			PyObject * py_result = pybind::ptr( result );

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
		typedef typename function_parser<F>::result f_info;

	public:
		getattro_adapter_proxy_function( const char * _name, F _fn )
			: getattro_adapter_interface( _name )
			, m_fn(_fn)
		{
		}

	protected:
		PyObject * call( kernel_interface * _kernel, void * _impl, const class_type_scope_interface_ptr & _scope, PyObject * _key ) override
		{
			uint32_t class_id = _kernel->class_info<C*>();
			uint32_t scope_id = _kernel->class_info<C>();

			C * self = detail::meta_cast_scope_t<C *>( _impl, scope_id, class_id, _scope );

			typename f_info::ret_type result = (*m_fn)(_kernel, self
				, extract_throw<typename f_info::param2>( _key )
				);

			PyObject * py_result = pybind::ptr( result );

			return py_result;
		}

	protected:
		F m_fn;
	};
}

