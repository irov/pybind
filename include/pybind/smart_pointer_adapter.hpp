#	pragma once

#	include "pybind/adapter_interface.hpp"

#	include "pybind/method_call.hpp"
#	include "pybind/method_proxy_call.hpp"
#	include "pybind/function_proxy_call.hpp"

namespace pybind
{
	//////////////////////////////////////////////////////////////////////////
	class smart_pointer_adapter_interface
		: public adapter_interface
	{
	public:
		smart_pointer_adapter_interface( const char * _name )
			: m_name(_name)
		{
		}

	public:
		inline const char * getName() const
		{
			return m_name;
		}

	public:
		virtual void incref( kernel_interface * _kernel, void * _self, const class_type_scope_ptr & scope ) = 0;
		virtual void decref( kernel_interface * _kernel, void * _self, const class_type_scope_ptr & scope ) = 0;

	protected:
		const char * m_name;
	};
    //////////////////////////////////////////////////////////////////////////
	typedef stdex::intrusive_ptr<smart_pointer_adapter_interface> smart_pointer_adapter_interface_ptr;
	//////////////////////////////////////////////////////////////////////////
	template<class C, class FI, class FD>
	class smart_pointer_adapter
		: public smart_pointer_adapter_interface
	{
	public:
		smart_pointer_adapter( const char * _name, FI _incref, FD _decref )
			: smart_pointer_adapter_interface( _name )
			, m_incref( _incref )
			, m_decref( _decref )
		{
		}

	protected:
		void incref( kernel_interface * _kernel, void * _impl, const class_type_scope_ptr & _scope ) override
		{            
			C * self = _kernel->meta_cast_class_t<C>( _impl, _scope );
			
			(self->*m_incref)();
		}		

		void decref( kernel_interface * _kernel, void * _impl, const class_type_scope_ptr & _scope ) override
		{
			C * self = _kernel->meta_cast_class_t<C>( _impl, _scope );

			(self->*m_decref)();
		}

	protected:
		FI m_incref;
		FD m_decref;
	};
	//////////////////////////////////////////////////////////////////////////
	template<class C, class P, class FI, class FD>
	class smart_pointer_adapter_proxy
		: public smart_pointer_adapter_interface
	{
	public:
		smart_pointer_adapter_proxy( const char * _name, FI _incref, FD _decref, P * _proxy )
			: smart_pointer_adapter_interface( _name )
			, m_incref( _incref )
			, m_decref( _decref )
			, m_proxy(_proxy)
		{
		}

	protected:
		void incref( kernel_interface * _kernel, void * _impl, const class_type_scope_ptr & _scope ) override
		{
			C * self = _kernel->meta_cast_class_t<C>( _impl, _scope );

			(m_proxy->*m_incref)(self);
		}

		void decref( kernel_interface * _kernel, void * _impl, const class_type_scope_ptr & _scope ) override
		{
			C * self = _kernel->meta_cast_class_t<C>( _impl, _scope );

			(m_proxy->*m_decref)(self);
		}

	protected:
		FI m_incref;
		FD m_decref;
		P * m_proxy;
	};
	//////////////////////////////////////////////////////////////////////////
	template<class C, class FI, class FD>
	class smart_pointer_adapter_proxy_function
		: public smart_pointer_adapter_interface
	{
	public:
		smart_pointer_adapter_proxy_function( const char * _name, FI _incref, FD _decref )
			: smart_pointer_adapter_interface( _name )
			, m_incref( _incref )
			, m_decref( _decref )
		{
		}

	protected:
		void incref( kernel_interface * _kernel, void * _impl, const class_type_scope_ptr & _scope ) override
		{			
			C * self = _kernel->meta_cast_class_t<C>( _impl, _scope );

			(*m_incref)(self);
		}

		void decref( kernel_interface * _kernel, void * _impl, const class_type_scope_ptr & _scope ) override
		{
			C * self = _kernel->meta_cast_class_t<C>( _impl, _scope );

			(*m_decref)(self);
		}

	protected:
		FI m_incref;
		FD m_decref;
	};
}

