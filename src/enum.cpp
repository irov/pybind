#include "pybind/enum.hpp"
#include "pybind/kernel.hpp"
#include "pybind/exception.hpp"
#include "pybind/extract.hpp"

namespace pybind
{
    namespace detail
    {
        void add_module_enum( kernel_interface * _kernel, PyObject * _module, const char * _id, uint32_t _value )
        {
            PyObject * py_module = _module;

            if( py_module == nullptr )
            {
                py_module = _kernel->get_current_module();

                if( py_module == nullptr )
                {
                    pybind::throw_exception( "scope %s initialize not setup python module"
                        , _id
                    );

                    return;
                }
            }

            PyObject * py_value = pybind::ptr( _kernel, _value );
            pybind::module_addobject( py_module, _id, py_value );
        }
    }
}