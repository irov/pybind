#pragma once

#include "pybind/adapter/constructor_adapter.hpp"

#include <type_traits>

namespace pybind
{
    //////////////////////////////////////////////////////////////////////////
    template<class C, class P>
    struct call_constructor_new_impl
    {
        template<uint32_t ... I>
        static C * call( kernel_interface * _kernel, PyObject * _args, std::integer_sequence<uint32_t, I...> )
        {
            return new C(
                tuple_getitem_t( _kernel, _args, I ) ...
            );
        }
    };
    //////////////////////////////////////////////////////////////////////////
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

#ifndef NDEBUG
            if( this->valid( _kernel, _args ) == false )
            {
                return nullptr;
            }
#endif

            void * impl = call_constructor_new_impl<C, P>::call( _kernel, _args, std::make_integer_sequence<uint32_t, P::args_arity>() );

            return impl;
        }
    };
}

