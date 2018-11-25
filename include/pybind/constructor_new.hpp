#pragma once

#include "pybind/adapter/constructor_adapter.hpp"

namespace pybind
{
    template<class C, class P, int i>
    struct call_constructor_new_impl
    {
        static C * call( kernel_interface * _kernel, PyObject * _obj, PyObject * _args );
    };

    template<class C, class P>
    struct call_constructor_new_impl<C, P, 0>
    {
        static C * call( kernel_interface * _kernel, PyObject * _args )
        {
            (void)_kernel;
            (void)_args;

            return new C();
        }
    };

    template<class C, class P>
    struct call_constructor_new_impl<C, P, 1>
    {
        static C * call( kernel_interface * _kernel, PyObject * _args )
        {
            return new C(
                tuple_getitem_t( _kernel, _args, 0 )
            );
        }
    };

    template<class C, class P>
    struct call_constructor_new_impl<C, P, 2>
    {
        static C * call( kernel_interface * _kernel, PyObject * _args )
        {
            return new C(
                tuple_getitem_t( _kernel, _args, 0 ),
                tuple_getitem_t( _kernel, _args, 1 )
            );
        }
    };

    template<class C, class P>
    struct call_constructor_new_impl<C, P, 3>
    {
        static C * call( kernel_interface * _kernel, PyObject * _args )
        {
            return new C(
                tuple_getitem_t( _kernel, _args, 0 ),
                tuple_getitem_t( _kernel, _args, 1 ),
                tuple_getitem_t( _kernel, _args, 2 )
            );
        }
    };

    template<class C, class P>
    struct call_constructor_new_impl<C, P, 4>
    {
        static C * call( kernel_interface * _kernel, PyObject * _args )
        {
            return new C(
                tuple_getitem_t( _kernel, _args, 0 ),
                tuple_getitem_t( _kernel, _args, 1 ),
                tuple_getitem_t( _kernel, _args, 2 ),
                tuple_getitem_t( _kernel, _args, 3 )
            );
        }
    };

    template<class C, class P>
    class constructor_new
        : public constructor_adapter_interface
    {
    public:
        constructor_new()
            : constructor_adapter_interface( P::args_arity )
        {
        }

    public:
        void * call( kernel_interface * _kernel, PyObject * _obj, PyObject * _args ) override
        {
            (void)_kernel;
            (void)_obj;

            if( this->valid( _kernel, _args ) == false )
            {
                return nullptr;
            }

            void * impl = call_constructor_new_impl<C, P, P::args_arity>::call( _kernel, _args );

            return impl;
        }
    };
}

