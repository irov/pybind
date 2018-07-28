#pragma once

#include "pybind/generator/base_.hpp"
#include "pybind/generator/helper_.hpp"

namespace pybind
{
    template<class C, class B = no_bases>
    class class_
        : public base_<C, B>
    {
    protected:
        typedef extract_class_type_ptr<C> extract_type_ptr;

    public:
        class_( kernel_interface * _kernel, const char * _name, bool external_extract = true, PyObject * _module = 0 )
            : base_<C, B>( _kernel, _name, 0, nullptr, new class_destroy_delete<C>, 0, false, _module )
        {
            if( external_extract == true )
            {
                this->setup_extract( new extract_type_ptr );
            }
        }

    public:
        template<class C0, class C1, class C2, class C3, class C4, class C5>
        class_ & def_constructor( const init<C0, C1, C2, C3, C4, C5> & _init )
        {
            constructor_adapter_interface_ptr ctr =
                new constructor_new<C, init<C0, C1, C2, C3, C4, C5> >();

            const class_type_scope_interface_ptr & scope = base_<C, B>::get_scope();

            scope->set_construct( ctr );

            return *this;
        }
    };

    template<class T>
    PyTypeObject * get_typemodule( kernel_interface * _kernel )
    {
        const class_type_scope_interface_ptr & scope = _kernel->class_scope<T>();

        PyTypeObject * typemodule = scope->get_typeobject();

        return typemodule;
    }
}

