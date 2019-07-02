#include "pybind/dictobject.hpp"

namespace pybind
{
    //////////////////////////////////////////////////////////////////////////
    pybind::object dictobject_new( kernel_interface * _kernel, const pybind::dict & _dict )
    {
        PyObject * ob = _kernel->dictobject_new( _dict.ptr() );

        return pybind::object( _kernel, ob );
    }
}
