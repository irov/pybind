#	pragma once

#	include "pybind/adapter_interface.hpp"

#	include "pybind/method_call.hpp"
#	include "pybind/method_proxy_call.hpp"

namespace pybind
{
	//////////////////////////////////////////////////////////////////////////
	typedef stdex::intrusive_ptr<class class_type_scope_interface> class_type_scope_interface_ptr;
	//////////////////////////////////////////////////////////////////////////
	class sequence_set_adapter_interface
		: public adapter_interface
	{
	public:
		sequence_set_adapter_interface( const char * _name )
			: m_name(_name)
		{
		}

	public:
		inline const char * getName() const
		{
			return m_name;
		}

	public:
		virtual void call( kernel_interface * _kernel, void * _self, const class_type_scope_interface_ptr & scope, uint32_t _index, PyObject * _value ) = 0;

	protected:
		const char * m_name;
	};
    //////////////////////////////////////////////////////////////////////////
    typedef stdex::intrusive_ptr<sequence_set_adapter_interface> sequence_set_adapter_interface_ptr;
	//////////////////////////////////////////////////////////////////////////
	template<class C, class F>
	class sequence_set_adapter
		: public sequence_set_adapter_interface
	{
	public:
		sequence_set_adapter( const char * _name, F _fn )
			: sequence_set_adapter_interface(_name)
			, m_fn(_fn)
		{
		}

	protected:
		void call( kernel_interface * _kernel, void * _impl, const class_type_scope_interface_ptr & _scope, uint32_t _index, PyObject * _value ) override
		{
			C * self = _kernel->meta_cast_class_t<C>( _impl, _scope );

			(self->*m_fn)( _index
				, detail::extract_operator_t( _kernel, _value )
				);
		}		

	protected:
		F m_fn;
	};
	//////////////////////////////////////////////////////////////////////////
    template<class C, class F>
    class sequence_set_adapter_depricated
        : public sequence_set_adapter<C, F>
    {
    public:
        sequence_set_adapter_depricated( const char * _name, F _fn, const char * _doc )
            : sequence_set_adapter<C, F>(_name, _fn)
            , m_doc(_doc)
        {
        }

    protected:
		void call( kernel_interface * _kernel, void * _impl, const class_type_scope_interface_ptr & _scope, uint32_t _index, PyObject * _value ) override
        {
			const char * scopeName = _kernel->get_class_type_info_t<C>();

            const char * name = this->getName();

            pybind::error_traceback("method %s::%s depricated '%s'"
                , scopeName
                , name
                , m_doc
                );

            pybind::check_error();

			sequence_get_adapter<C, F>::call( _kernel, _impl, _scope
				, _index
				, _value 
				);
        }

    protected:
        const char * m_doc;
    };
	//////////////////////////////////////////////////////////////////////////
	template<class C, class P, class F>
	class sequence_set_adapter_proxy
		: public sequence_set_adapter_interface
	{
	public:
		sequence_set_adapter_proxy( const char * _name, F _fn, P * _proxy )
			: sequence_set_adapter_interface(_name)
			, m_fn(_fn)
			, m_proxy(_proxy)
		{
		}

	protected:
		void call( kernel_interface * _kernel, void * _impl, const class_type_scope_interface_ptr & _scope, uint32_t _index, PyObject * _value ) override
		{
			C * self = _kernel->meta_cast_class_t<C>( _impl, _scope );

			(m_proxy->*m_fn)(self
				, _index
				, detail::extract_operator_t( _kernel, _value )
				);
		}

	protected:
		F m_fn;
		P * m_proxy;
	};
	//////////////////////////////////////////////////////////////////////////
	template<class C, class F>
	class sequence_set_adapter_proxy_function
		: public sequence_set_adapter_interface
	{
	public:
		sequence_set_adapter_proxy_function( const char * _name, F _fn )
			: sequence_set_adapter_interface(_name)
			, m_fn(_fn)
		{
		}

	protected:
		void call( kernel_interface * _kernel, void * _impl, const class_type_scope_interface_ptr & _scope, uint32_t _index, PyObject * _value ) override
		{
			C * self = _kernel->meta_cast_class_t<C>( _impl, _scope );

			(*m_fn)( self
				, _index
				, detail::extract_operator_t( _kernel, _value )
				);
		}

	protected:
		F m_fn;
	};
	//////////////////////////////////////////////////////////////////////////
	template<class C>
	class sequence_set_adapter_operator
		: public sequence_set_adapter_interface
	{
	public:
		sequence_set_adapter_operator( const char * _name )
			: sequence_set_adapter_interface( _name )
		{
		}

	protected:
		void call( kernel_interface * _kernel, void * _impl, const class_type_scope_interface_ptr & _scope, uint32_t _index, PyObject * _value ) override
		{
			C * self = _kernel->meta_cast_class_t<C>( _impl, _scope );

			self->operator [] ( _index ) = detail::extract_operator_t( _kernel, _value );
		}
	};
}

