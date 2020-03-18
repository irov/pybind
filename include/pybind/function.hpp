#pragma once

#include "pybind/types.hpp"

#include "pybind/adapter/function_adapter.hpp"

#include "stdex/function_traits.h"

namespace pybind
{
    //////////////////////////////////////////////////////////////////////////
    template<class F>
    void def_function( kernel_interface * _kernel, const char * _name, F _function, PyObject * _module = 0 )
    {
        typedef typename stdex::function_traits<F>::result t_info;

        allocator_interface * allocator = _kernel->get_allocator();

        function_adapter_interface_ptr adapter = allocator->newT<function_adapter<F>>( _name, t_info::arity, _function );

        _kernel->def_function_adapter( adapter, false, _module );
    }
    //////////////////////////////////////////////////////////////////////////
    template<class F>
    void def_function_args( kernel_interface * _kernel, const char * _name, F _function, PyObject * _module = 0 )
    {
        typedef typename stdex::function_traits<F>::result t_info;

        allocator_interface * allocator = _kernel->get_allocator();

        function_adapter_interface_ptr adapter = allocator->newT<function_adapter_args<F>>( _name, t_info::arity, _function );

        _kernel->def_function_adapter( adapter, false, _module );
    }
    //////////////////////////////////////////////////////////////////////////
    template<class F>
    void def_function_deprecate( kernel_interface * _kernel, const char * _name, F _function, const char * _doc, PyObject * _module = 0 )
    {
        typedef typename stdex::function_traits<F>::result t_info;

        allocator_interface * allocator = _kernel->get_allocator();

        function_adapter_interface_ptr adapter = allocator->newT<function_adapter_deprecate<F>>( _name, t_info::arity, _function, _doc );

        _kernel->def_function_adapter( adapter, false, _module );
    }
    //////////////////////////////////////////////////////////////////////////
    template<class F>
    void def_function_kernel( kernel_interface * _kernel, const char * _name, F _function, PyObject * _module = 0 )
    {
        typedef typename stdex::function_traits<F>::result t_info;

        allocator_interface * allocator = _kernel->get_allocator();

        function_adapter_interface_ptr adapter = allocator->newT<function_kernel_adapter<F>>( _name, t_info::arity, _function );

        _kernel->def_function_adapter( adapter, false, _module );
    }
    //////////////////////////////////////////////////////////////////////////
    template<class F>
    void def_function_kernel_args( kernel_interface * _kernel, const char * _name, F _function, PyObject * _module = 0 )
    {
        typedef typename stdex::function_traits<F>::result t_info;

        allocator_interface * allocator = _kernel->get_allocator();

        function_adapter_interface_ptr adapter = allocator->newT<function_kernel_adapter_args<F>>( _name, t_info::arity, _function );

        _kernel->def_function_adapter( adapter, false, _module );
    }
    //////////////////////////////////////////////////////////////////////////
    template<class F, class P>
    void def_function_proxy( kernel_interface * _kernel, const char * _name, F _function, P * _proxy, PyObject * _module = 0 )
    {
        typedef typename stdex::function_traits<F>::result t_info;

        allocator_interface * allocator = _kernel->get_allocator();

        function_adapter_interface_ptr adapter = allocator->newT<function_proxy_adapter<F, P>>( _name, t_info::arity, _function, _proxy );

        _kernel->def_function_adapter( adapter, false, _module );
    }
    //////////////////////////////////////////////////////////////////////////
    template<class F, class P>
    void def_function_proxy_args( kernel_interface * _kernel, const char * _name, F _function, P * _proxy, PyObject * _module = 0 )
    {
        typedef typename stdex::function_traits<F>::result t_info;

        allocator_interface * allocator = _kernel->get_allocator();

        function_adapter_interface_ptr adapter = allocator->newT<function_proxy_adapter_args<F, P>>( _name, t_info::arity, _function, _proxy );

        _kernel->def_function_adapter( adapter, false, _module );
    }
    //////////////////////////////////////////////////////////////////////////
    template<class F>
    PyObject * create_function( kernel_interface * _kernel, const char * _name, F _function )
    {
        typedef typename stdex::function_traits<F>::result t_info;

        allocator_interface * allocator = _kernel->get_allocator();

        function_adapter_interface_ptr adapter = allocator->newT<function_adapter<F>>( _name, t_info::arity, _function );

        PyObject * py_function = _kernel->create_function_adapter( adapter, false );

        return py_function;
    }
    //////////////////////////////////////////////////////////////////////////
    template<class F>
    void def_function_native( kernel_interface * _kernel, const char * _name, F _function, PyObject * _module = 0 )
    {
        typedef typename stdex::function_traits<F>::result t_info;

        allocator_interface * allocator = _kernel->get_allocator();

        function_adapter_interface_ptr adapter = allocator->newT<function_adapter_native<F>>( _name, t_info::arity, _function );

        _kernel->def_function_adapter( adapter, true, _module );
    }
    //////////////////////////////////////////////////////////////////////////
    template<class F>
    PyObject * create_function_native( kernel_interface * _kernel, const char * _name, F _function )
    {
        typedef typename stdex::function_traits<F>::result t_info;

        allocator_interface * allocator = _kernel->get_allocator();

        function_adapter_interface_ptr adapter = allocator->newT<function_adapter_native<F>>( _name, t_info::arity, _function );

        PyObject * py_function = _kernel->create_function_adapter( adapter, true );

        return py_function;
    }
}