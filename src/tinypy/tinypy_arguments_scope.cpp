#include "tinypy_arguments_scope.hpp"

namespace pybind
{
    //////////////////////////////////////////////////////////////////////////
    tinypy_arguments_scope::tinypy_arguments_scope( tinypy_vm_t * _vm, tinypy_value_t * const * _items, size_t _count )
        : m_args( tinypy_native_arguments_acquire( _vm, nullptr, _items, _count, nullptr ) )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    tinypy_arguments_scope::~tinypy_arguments_scope()
    {
        tinypy_native_arguments_release( m_args );
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * tinypy_arguments_scope::get() const
    {
        PyObject * args = reinterpret_cast<PyObject *>(m_args);

        return args;
    }
    //////////////////////////////////////////////////////////////////////////
}
