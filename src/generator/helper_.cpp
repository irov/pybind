#include "pybind/generator/helper_.hpp"

namespace pybind
{
    //////////////////////////////////////////////////////////////////////////
    void * class_new_invalid::call( kernel_interface * _kernel, const class_type_scope_interface_ptr & _scope, PyObject * _obj, PyObject * _args, PyObject * _kwds )
    {
        (void)_kernel;
        (void)_scope;
        (void)_obj;
        (void)_args;
        (void)_kwds;

        pybind::throw_exception( "scope %s not support 'new_interface'"
            , _scope->get_name()
        );

        return nullptr;
    }
}

