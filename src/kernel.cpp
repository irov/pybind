#include "pybind/kernel.hpp"

#include "python/python_system.hpp"

namespace pybind
{
    //////////////////////////////////////////////////////////////////////////
    struct kernel_scope
    {
        kernel_interface * current_kernel;
    };
    //////////////////////////////////////////////////////////////////////////
    static kernel_scope & get_scope()
    {
        static kernel_scope k;

        return k;
    }
    //////////////////////////////////////////////////////////////////////////
    kernel_interface * get_kernel()
    {
        kernel_scope & scope = get_scope();

        return scope.current_kernel;
    }
    //////////////////////////////////////////////////////////////////////////
    void set_kernel( kernel_interface * _kernel )
    {
        kernel_scope & scope = get_scope();

        scope.current_kernel = _kernel;
    }
}