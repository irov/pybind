#pragma once

#include "pybind/generator/base_.hpp"
#include "pybind/generator/helper_.hpp"

namespace pybind
{
    template<class C, class B = no_bases>
    class proxy_
        : public base_<C, B>
    {
    public:
        typedef extract_holder_type_ptr<C> extract_type_ptr;

    public:
        proxy_( kernel_interface * _kernel, const char * _name, bool external_extract = true, PyObject * _module = 0 )
            : base_<C, B>( _kernel, _name, 0, nullptr, nullptr, 0, false, _module )
        {
            if( external_extract == true )
            {
                this->setup_extract( _kernel->get_allocator()->newT<extract_type_ptr>() );
            }
        }

    public:
        template<class ... Args>
        proxy_ & def_constructor( const init<Args...> & _init )
        {
            allocator_interface * allocator = m_kernel->get_allocator();

            constructor_adapter_interface_ptr ctr = allocator->newT<constructor_new<C, init<Args...> >>();

            const class_type_scope_interface_ptr & scope = base_<C, B>::get_scope();

            scope->set_construct( ctr );

            return *this;
        }
    };
}

