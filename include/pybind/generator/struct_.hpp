#pragma once

#include "pybind/generator/base_.hpp"
#include "pybind/generator/helper_.hpp"

namespace pybind
{
    template<class C, class B = no_bases>
    class struct_
        : public base_<C, B>
    {
    public:
        typedef extract_struct_type_ref<C> extract_type_ref;

    public:
        struct_( kernel_interface * _kernel, const char * _name, bool external_extract = true, PyObject * _module = 0 )
            : base_<C, B>( _kernel, _name, 0, nullptr, nullptr, sizeof( C ), false, _module )
        {
            if( external_extract == true )
            {
                type_cast_ptr cast( new extract_type_ref );

                this->setup_extract( cast );
            }
        }

    public:
        template<class ... Args>
        struct_ & def_constructor( const init<Args...> & _init )
        {
            (void)_init;

            const class_type_scope_interface_ptr & scope = base_<C, B>::get_scope();

            constructor_adapter_interface_ptr ctr( new constructor_placement<C, init<Args...> > );
            scope->set_construct( ctr );

            return *this;
        }
    };
}

