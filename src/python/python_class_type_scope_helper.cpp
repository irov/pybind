#include "python_class_type_scope_helper.hpp"

#include "pybind/class_type_scope_interface.hpp"

#include "python_system.hpp"

namespace pybind
{
    namespace helper
    {
        bool is_object_bindable( kernel_interface * _kernel, PyObject * _obj )
        {
            if( _obj == nullptr )
            {
                return false;
            }

            PyTypeObject * objtype = pybind::object_type( _obj );

            const class_type_scope_interface_ptr & scope = _kernel->get_class_scope( objtype );

            if( scope == nullptr )
            {
                return false;
            }

            if( scope->get_bindable() == nullptr )
            {
                return false;
            }

            return true;
        }
    }
}