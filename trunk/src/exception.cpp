#	include "pybind/exception.hpp"

namespace pybind
{
    //////////////////////////////////////////////////////////////////////////
    void throw_exception()
    {
        throw pybind_exception();
    }
}