#pragma once

#include "pybind/generator/base_.hpp"
#include "pybind/generator/helper_.hpp"

namespace pybind
{
    template<class C, class B = no_bases>
    class interface_
        : public base_<C, B>
    {
    public:
        typedef extract_holder_type_ptr<C> extract_type_ptr;

    public:
        interface_( kernel_interface * _kernel, const char * _name, bool external_extract = true, PyObject * _module = 0 )
            : base_<C, B>( _kernel, _name, 0, new class_new_invalid, nullptr, 0, false, _module )
        {
            if( external_extract == true )
            {
                this->setup_extract( new extract_type_ptr );
            }
        }
    };
}

