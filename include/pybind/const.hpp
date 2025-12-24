#pragma once

#include "pybind/exports.hpp"
#include "pybind/types.hpp"
#include "pybind/extract.hpp"
#include "pybind/exception.hpp"

namespace pybind
{
    //////////////////////////////////////////////////////////////////////////
    template<class T>
    void const_( kernel_interface * _kernel, const char * _name, const T & _value, PyObject * _module = nullptr )
    {
        PyObject * py_module = _module;

        if( py_module == nullptr )
        {
            py_module = _kernel->get_current_module();

            if( py_module == nullptr )
            {
                pybind::throw_exception( "const_ '%s' initialize not setup python module"
                    , _name
                );

                return;
            }
        }

        PyObject * py_value = pybind::ptr( _kernel, _value );
        
        if( py_value == nullptr )
        {
            pybind::throw_exception( "const_ '%s' failed to convert value to PyObject"
                , _name
            );

            return;
        }

        _kernel->module_addobject( py_module, _name, py_value );
    }
    //////////////////////////////////////////////////////////////////////////
}

