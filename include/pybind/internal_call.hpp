#pragma once

#include "pybind/kernel_interface.hpp"

#include "pybind/helper.hpp"
#include "pybind/tuple.hpp"

namespace pybind
{
    //////////////////////////////////////////////////////////////////////////
    typedef intrusive_ptr<class class_type_scope_interface> class_type_scope_interface_ptr;
    //////////////////////////////////////////////////////////////////////////
    template<class T, class ... A>
    detail::extract_operator_t internal_call_method( kernel_interface * _kernel, T * _self, const char * _name, const A & ... _a )
    {
        const class_type_scope_interface_ptr & scope = _kernel->get_class_type_scope_t<T>();

        tuple args = pybind::make_tuple_t( _kernel, _a ... );

        // As ask_tuple_t: the invalid arguments of a null item stand for an
        // error already raised, so nothing is called.
        if( args.is_invalid() == true )
        {
            return detail::extract_operator_t( _kernel, nullptr, pybind::borrowed );
        }

        PyObject * py_result = _kernel->ask_adapter( _self, scope, _name, args.ptr() );

        return detail::extract_operator_t( _kernel, py_result, pybind::borrowed );
    }
    //////////////////////////////////////////////////////////////////////////
}