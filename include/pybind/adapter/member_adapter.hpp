#pragma once

#include "pybind/member_interface.hpp"

#include "pybind/extract.hpp"

#include "pybind/helper.hpp"

namespace pybind
{
    //////////////////////////////////////////////////////////////////////////
    template<class C, class A>
    class member_adapter
        : public member_adapter_interface
    {
    public:
        member_adapter( const char * _name, A C:: * _member )
            : member_adapter_interface( _name )
            , m_member( _member )
        {
        }

        ~member_adapter() override
        {
        }

    public:
        PyObject * get( kernel_interface * _kernel, void * _self, const class_type_scope_interface_ptr & _scope ) override
        {
            C * obj = _kernel->meta_cast_class_t<C>( _self, _scope );

            PyObject * py_value = pybind::ptr_throw( _kernel, obj->*m_member );

            return py_value;
        }

        int set( kernel_interface * _kernel, void * _self, PyObject * _args, const class_type_scope_interface_ptr & _scope ) override
        {
            C * obj = _kernel->meta_cast_class_t<C>( _self, _scope );

            obj->*m_member = pybind::extract_throw<A>( _kernel, _args );

            return 1;
        }

    protected:
        A C:: * m_member;
    };
    //////////////////////////////////////////////////////////////////////////
    template<class C, class FG, class FS>
    class member_adapter_property
        : public member_adapter_interface
    {
    public:
        member_adapter_property( const char * _name, FG _get, FS _set )
            : member_adapter_interface( _name )
            , m_get( _get )
            , m_set( _set )
        {
        }

        ~member_adapter_property() override
        {
        }

    public:
        PyObject * get( kernel_interface * _kernel, void * _self, const class_type_scope_interface_ptr & _scope ) override
        {
            C * obj = _kernel->meta_cast_class_t<C>( _self, _scope );

            PyObject * py_value = detail::return_operator_t( _kernel, (obj->*m_get)() );

            return py_value;
        }

        int set( kernel_interface * _kernel, void * _self, PyObject * _args, const class_type_scope_interface_ptr & _scope ) override
        {
            C * obj = _kernel->meta_cast_class_t<C>( _self, _scope );

            (obj->*m_set)(
                detail::extract_operator_t( _kernel, _args )
                );

            return 1;
        }

    protected:
        FG m_get;
        FS m_set;
    };
    //////////////////////////////////////////////////////////////////////////
    template<class C, class FG, class FS>
    class member_adapter_property_static
        : public member_adapter_interface
    {
    public:
        member_adapter_property_static( const char * _name, FG _get, FS _set )
            : member_adapter_interface( _name )
            , m_get( _get )
            , m_set( _set )
        {
        }

        ~member_adapter_property_static() override
        {
        }

    public:
        PyObject * get( kernel_interface * _kernel, void * _self, const class_type_scope_interface_ptr & _scope ) override
        {
            C * obj = _kernel->meta_cast_class_t<C>( _self, _scope );

            PyObject * py_value = detail::return_operator_t( _kernel, (*m_get)(obj) );

            return py_value;
        }

        int set( kernel_interface * _kernel, void * _self, PyObject * _args, const class_type_scope_interface_ptr & _scope ) override
        {
            C * obj = _kernel->meta_cast_class_t<C>( _self, _scope );

            (*m_set)(obj
                , detail::extract_operator_t( _kernel, _args )
                );

            return 1;
        }

    protected:
        FG m_get;
        FS m_set;
    };
    //////////////////////////////////////////////////////////////////////////
    template<class C, class FG>
    class member_adapter_property_static<C, FG, std::nullptr_t>
        : public member_adapter_interface
    {
    public:
        member_adapter_property_static( const char * _name, FG _get, std::nullptr_t )
            : member_adapter_interface( _name )
            , m_get( _get )
        {
        }

        ~member_adapter_property_static() override
        {
        }

    public:
        PyObject * get( kernel_interface * _kernel, void * _self, const class_type_scope_interface_ptr & _scope ) override
        {
            C * obj = _kernel->meta_cast_class_t<C>( _self, _scope );

            PyObject * py_value = detail::return_operator_t( _kernel, (*m_get)(obj) );

            return py_value;
        }

        int set( kernel_interface * _kernel, void * _self, PyObject * _args, const class_type_scope_interface_ptr & _scope ) override
        {
            (void)_kernel;
            (void)_self;
            (void)_args;
            (void)_scope;

            return 0;
        }

    protected:
        FG m_get;
    };
    //////////////////////////////////////////////////////////////////////////
}

