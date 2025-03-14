#pragma once

#include "pybind/adapter/base_adapter.hpp"

#include "pybind/call/method_call.hpp"
#include "pybind/call/method_proxy_call.hpp"

namespace pybind
{
    //////////////////////////////////////////////////////////////////////////
    typedef intrusive_ptr<class class_type_scope_interface> class_type_scope_interface_ptr;
    //////////////////////////////////////////////////////////////////////////
    class sequence_get_adapter_interface
        : public base_adapter_interface
    {
    public:
        sequence_get_adapter_interface( const char * _name )
            : base_adapter_interface( _name )
        {
        }

        ~sequence_get_adapter_interface() override
        {
        }

    public:
        virtual PyObject * call( kernel_interface * _kernel, void * _self, const class_type_scope_interface_ptr & scope, size_t _index ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef intrusive_ptr<sequence_get_adapter_interface> sequence_get_adapter_interface_ptr;
    //////////////////////////////////////////////////////////////////////////
    template<class C, class F>
    class sequence_get_adapter
        : public sequence_get_adapter_interface
    {
    public:
        sequence_get_adapter( const char * _name, F _fn )
            : sequence_get_adapter_interface( _name )
            , m_fn( _fn )
        {
        }

        ~sequence_get_adapter() override
        {
        }

    protected:
        PyObject * call( kernel_interface * _kernel, void * _impl, const class_type_scope_interface_ptr & _scope, size_t _index ) override
        {
            C * self = _kernel->meta_cast_class_t<C>( _impl, _scope );

            PyObject * py_result = detail::return_operator_t( _kernel, (self->*m_fn)(
                _index
                ) );

            return py_result;
        }

    protected:
        F m_fn;
    };
    //////////////////////////////////////////////////////////////////////////
    template<class C, class F>
    class sequence_get_adapter_depricated
        : public sequence_get_adapter<C, F>
    {
    public:
        sequence_get_adapter_depricated( const char * _name, F _fn, const char * _doc )
            : sequence_get_adapter<C, F>( _name, _fn )
            , m_doc( _doc )
        {
        }

        ~sequence_get_adapter_depricated() override
        {
        }

    protected:
        PyObject * call( kernel_interface * _kernel, void * _impl, const class_type_scope_interface_ptr & _scope, size_t _index ) override
        {
            const char * scopeName = _kernel->get_class_type_info_t<C>();

            const char * name = this->getName();

            _kernel->error_traceback( "method '%s::%s' depricated '%s'"
                , scopeName
                , name
                , m_doc
            );

            PyObject * ret = sequence_get_adapter<C, F>::call( _kernel, _impl, _scope
                , _index
            );

            return ret;
        }

    protected:
        const char * m_doc;
    };
    //////////////////////////////////////////////////////////////////////////
    template<class C, class P, class F>
    class sequence_get_adapter_proxy
        : public sequence_get_adapter_interface
    {
    public:
        sequence_get_adapter_proxy( const char * _name, F _fn, P * _proxy )
            : sequence_get_adapter_interface( _name )
            , m_fn( _fn )
            , m_proxy( _proxy )
        {
        }

        ~sequence_get_adapter_proxy() override
        {
        }

    protected:
        PyObject * call( kernel_interface * _kernel, void * _impl, const class_type_scope_interface_ptr & _scope, size_t _index ) override
        {
            C * self = _kernel->meta_cast_class_t<C>( _impl, _scope );

            PyObject * py_result = detail::return_operator_t( _kernel, (m_proxy->*m_fn)(self
                , _index
                ) );

            return py_result;
        }

    protected:
        F m_fn;
        P * m_proxy;
    };
    //////////////////////////////////////////////////////////////////////////
    template<class C, class F>
    class sequence_get_adapter_proxy_function
        : public sequence_get_adapter_interface
    {
    public:
        sequence_get_adapter_proxy_function( const char * _name, F _fn )
            : sequence_get_adapter_interface( _name )
            , m_fn( _fn )
        {
        }

        ~sequence_get_adapter_proxy_function() override
        {
        }

    protected:
        PyObject * call( kernel_interface * _kernel, void * _impl, const class_type_scope_interface_ptr & _scope, size_t _index ) override
        {
            C * self = _kernel->meta_cast_class_t<C>( _impl, _scope );

            PyObject * py_result = detail::return_operator_t( _kernel, (*m_fn)(self
                , _index
                ) );

            return py_result;
        }

    protected:
        F m_fn;
    };
    //////////////////////////////////////////////////////////////////////////
    template<class C>
    class sequence_get_adapter_operator
        : public sequence_get_adapter_interface
    {
    public:
        sequence_get_adapter_operator( const char * _name )
            : sequence_get_adapter_interface( _name )
        {
        }

        ~sequence_get_adapter_operator() override
        {
        }

    protected:
        PyObject * call( kernel_interface * _kernel, void * _impl, const class_type_scope_interface_ptr & _scope, size_t _index ) override
        {
            C * self = _kernel->meta_cast_class_t<C>( _impl, _scope );

            PyObject * py_result = detail::return_operator_t( _kernel, self->operator [] ( _index ) );

            return py_result;
        }
    };
}

