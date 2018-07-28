#pragma once

#include "pybind/generator/base_.hpp"
#include "pybind/generator/helper_.hpp"

namespace pybind
{
    template<class C, class B = no_bases>
    class structhash_
        : public base_<C, B>
    {
    public:
        typedef extract_struct_type_ref<C> extract_type_ref;

    public:
        structhash_( kernel_interface * _kernel, const char * _name, bool external_extract = true, PyObject * _module = 0 )
            : base_<C, B>( _kernel, _name, 0, nullptr, nullptr, sizeof( C ), true, _module )
        {
            if( external_extract == true )
            {
                this->setup_extract( new extract_type_ref );
            }
        }

    public:
        template<class C0, class C1, class C2, class C3, class C4, class C5>
        structhash_ & def_constructor( const init<C0, C1, C2, C3, C4, C5> & _init )
        {
            (void)_init;

            constructor_adapter_interface_ptr ctr =
                new constructor_placement<C, init<C0, C1, C2, C3, C4, C5> >();

            const class_type_scope_interface_ptr & scope = base_<C, B>::get_scope();

            scope->set_construct( ctr );

            return *this;
        }
    };
}

