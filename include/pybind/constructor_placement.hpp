#pragma once

#include "pybind/adapter/constructor_adapter.hpp"

#include <type_traits>

namespace pybind
{
    //////////////////////////////////////////////////////////////////////////
    template<class C, class P>
    struct call_constructor_placement_impl
    {
        template<uint32_t ... I>
        static C * call( kernel_interface * _kernel, void * _impl, PyObject * _args, std::integer_sequence<uint32_t, I...> )
        {
            return new (_impl)C(
                tuple_getitem_t( _kernel, _args, I ) ...
            );
        }
    };
    //////////////////////////////////////////////////////////////////////////
    template<class C, class P>
    class constructor_placement
        : public constructor_adapter_interface
    {
    public:
        constructor_placement()
            : constructor_adapter_interface( P::args_arity )
        {
        }

    public:
        void * call( kernel_interface * _kernel, PyObject * _obj, PyObject * _args ) override
        {
#if defined(PYBIND_DEBUG)
            if( this->valid( _kernel, _args ) == false )
            {
                return nullptr;
            }
#endif

            C * self = _kernel->get_class_impl_t<C>( _obj );

            if( self == nullptr )
            {
                return nullptr;
            }

            C * obj = call_constructor_placement_impl<C, P>::call( _kernel, self, _args, std::make_integer_sequence<uint32_t, P::args_arity>() );

            return obj;
        }
    };
}

