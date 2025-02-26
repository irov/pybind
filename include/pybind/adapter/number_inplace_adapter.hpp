#pragma once

#include "pybind/adapter/base_adapter.hpp"

#include "pybind/call/method_call.hpp"
#include "pybind/call/method_proxy_call.hpp"

namespace pybind
{
    //////////////////////////////////////////////////////////////////////////
    typedef intrusive_ptr<class class_type_scope_interface> class_type_scope_interface_ptr;
    //////////////////////////////////////////////////////////////////////////
    class number_inplace_adapter_interface
        : public base_adapter_interface
    {
    public:
        number_inplace_adapter_interface( const char * _name )
            : base_adapter_interface( _name )
        {
        }

        ~number_inplace_adapter_interface() override
        {
        }

    public:
        virtual void call( kernel_interface * _kernel, void * _self, const class_type_scope_interface_ptr & _scope, PyObject * _value ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef intrusive_ptr<number_inplace_adapter_interface> number_inplace_adapter_interface_ptr;
    //////////////////////////////////////////////////////////////////////////
    template<class C, class F>
    class number_inplace_adapter
        : public number_inplace_adapter_interface
    {
    public:
        number_inplace_adapter( const char * _name, F _fn )
            : number_inplace_adapter_interface( _name )
            , m_fn( _fn )
        {
        }

    protected:
        void call( kernel_interface * _kernel, void * _impl, const class_type_scope_interface_ptr & _scope, PyObject * _value ) override
        {
            C * self = _kernel->meta_cast_class_t<C>( _impl, _scope );

            (self->*m_fn)(detail::extract_operator_t( _kernel, _value ));
        }

    protected:
        F m_fn;
    };
    //////////////////////////////////////////////////////////////////////////
    template<class C, class P, class F>
    class number_inplace_adapter_proxy
        : public number_inplace_adapter_interface
    {
    public:
        number_inplace_adapter_proxy( const char * _name, F _fn, P * _proxy )
            : number_inplace_adapter_interface( _name )
            , m_fn( _fn )
            , m_proxy( _proxy )
        {
        }

    protected:
        void call( kernel_interface * _kernel, void * _impl, const class_type_scope_interface_ptr & _scope, PyObject * _value ) override
        {
            C * self = _kernel->meta_cast_class_t<C>( _impl, _scope );

            (m_proxy->*m_fn)(self, detail::extract_operator_t( _kernel, _value ));
        }

    protected:
        F m_fn;
        P * m_proxy;
    };
    //////////////////////////////////////////////////////////////////////////
    template<class C, class F>
    class number_inplace_adapter_proxy_function
        : public number_inplace_adapter_interface
    {
    public:
        number_inplace_adapter_proxy_function( const char * _name, F _fn )
            : number_inplace_adapter_interface( _name )
            , m_fn( _fn )
        {
        }

    protected:
        void call( kernel_interface * _kernel, void * _impl, const class_type_scope_interface_ptr & _scope, PyObject * _value ) override
        {
            C * self = _kernel->meta_cast_class_t<C>( _impl, _scope );

            (*m_fn)(self, detail::extract_operator_t( _kernel, _value ));
        }

    protected:
        F m_fn;
    };
    //////////////////////////////////////////////////////////////////////////
    template<class C, class V>
    class number_inplace_adapter_operator_add
        : public number_inplace_adapter_interface
    {
    public:
        number_inplace_adapter_operator_add( const char * _name )
            : number_inplace_adapter_interface( _name )
        {
        }

    protected:
        void call( kernel_interface * _kernel, void * _impl, const class_type_scope_interface_ptr & _scope, PyObject * _value ) override
        {
            C * self = _kernel->meta_cast_class_t<C>( _impl, _scope );

            V value;
            if( pybind::extract_value( _kernel, _value, value, true ) == false )
            {
                _kernel->throw_message( "number inplace add invalid extract value '%s'"
                    , _kernel->object_str( _value ).c_str()
                );

                return;
            }

            *self += value;
        }
    };
    //////////////////////////////////////////////////////////////////////////
    template<class C, class V>
    class number_inplace_adapter_operator_sub
        : public number_inplace_adapter_interface
    {
    public:
        number_inplace_adapter_operator_sub( const char * _name )
            : number_inplace_adapter_interface( _name )
        {
        }

    protected:
        void call( kernel_interface * _kernel, void * _impl, const class_type_scope_interface_ptr & _scope, PyObject * _value ) override
        {
            C * self = _kernel->meta_cast_class_t<C>( _impl, _scope );

            V value;
            if( pybind::extract_value( _kernel, _value, value, true ) == false )
            {
                _kernel->throw_message( "number inplace sub invalid extract value '%s'"
                    , _kernel->object_str( _value ).c_str()
                );

                return;
            }

            *self -= value;
        }
    };
    //////////////////////////////////////////////////////////////////////////
    template<class C, class V>
    class number_inplace_adapter_operator_mul
        : public number_inplace_adapter_interface
    {
    public:
        number_inplace_adapter_operator_mul( const char * _name )
            : number_inplace_adapter_interface( _name )
        {
        }

    protected:
        void call( kernel_interface * _kernel, void * _impl, const class_type_scope_interface_ptr & _scope, PyObject * _value ) override
        {
            C * self = _kernel->meta_cast_class_t<C>( _impl, _scope );

            V value;
            if( pybind::extract_value( _kernel, _value, value, true ) == false )
            {
                _kernel->throw_message( "number inplace mul invalid extract value '%s'"
                    , _kernel->object_str( _value ).c_str()
                );

                return;
            }

            *self *= value;
        }
    };
    //////////////////////////////////////////////////////////////////////////
    template<class C, class V>
    class number_inplace_adapter_operator_div
        : public number_inplace_adapter_interface
    {
    public:
        number_inplace_adapter_operator_div( const char * _name )
            : number_inplace_adapter_interface( _name )
        {
        }

    protected:
        void call( kernel_interface * _kernel, void * _impl, const class_type_scope_interface_ptr & _scope, PyObject * _value ) override
        {
            C * self = _kernel->meta_cast_class_t<C>( _impl, _scope );

            V value;
            if( pybind::extract_value( _kernel, _value, value, true ) == false )
            {
                _kernel->throw_message( "number inplace div invalid extract value '%s'"
                    , _kernel->object_str( _value ).c_str()
                );

                return;
            }

            *self /= value;
        }
    };
    //////////////////////////////////////////////////////////////////////////
}

