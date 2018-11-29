#pragma once

#include "pybind/kernel.hpp"
#include "pybind/helper.hpp"
#include "pybind/tuple.hpp"

namespace pybind
{
    //////////////////////////////////////////////////////////////////////////
    typedef stdex::intrusive_ptr<class class_type_scope_interface> class_type_scope_interface_ptr;
    //////////////////////////////////////////////////////////////////////////
    template<class T, class ... A>
    detail::extract_operator_t internal_call_method( kernel_interface * _kernel, T * _self, const char * _name, const A & ... _a )
    {
        const class_type_scope_interface_ptr & scope = _kernel->class_scope<T>();

        tuple args = pybind::make_tuple_t( _kernel, _a ... );

        PyObject * py_result = _kernel->ask_adapter( _self, scope, _name, args );

        return detail::extract_operator_t( _kernel, py_result );
    }
}