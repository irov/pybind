#pragma once

#include "pybind/generator/base_.hpp"
#include "pybind/generator/helper_.hpp"

namespace pybind
{
    template<class C, class B = no_bases>
    class superclass_
        : public base_<C, B>
    {
    public:
        typedef extract_holder_type_ptr<C> extract_type_ptr;

    public:
        superclass_( kernel_interface * _kernel, const char * _name, void * _user, const new_adapter_interface_ptr & _pynew, const destroy_adapter_interface_ptr & _pydestructor, bool external_extract = true, PyObject * _module = 0 )
            : base_<C, B>( _kernel, _name, _user, _pynew, _pydestructor, 0, false, _module )
        {
            if( external_extract == true )
            {
                this->setup_extract( _kernel->get_allocator()->newT<extract_type_ptr>() );
            }
        }

    public:
        template<class ... Args>
        superclass_ & def_constructor( const init<Args...> & _init )
        {
            (void)_init;

            kernel_interface * kernel = this->get_kernel();

            allocator_interface * allocator = kernel->get_allocator();

            constructor_adapter_interface_ptr ctr = allocator->newT<constructor_new<C, init<Args...>>>();

            const class_type_scope_interface_ptr & scope = base_<C, B>::get_scope();

            scope->set_construct( ctr );

            return *this;
        }
    };
}

