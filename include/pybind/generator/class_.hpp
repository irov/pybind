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
            : base_<C, B>( _kernel, _name, nullptr, nullptr, _kernel->get_allocator()->newT<class_destroy_delete<C>>(), 0, false, _module )
        {
            if( external_extract == true )
            {
                this->setup_extract( _kernel->get_allocator()->newT<extract_type_ptr>() );
            }
        }

    public:
        template<class ... Args>
        class_ & def_constructor( const init<Args...> & )
        {
            allocator_interface * allocator = m_kernel->get_allocator();

            constructor_adapter_interface_ptr ctr = allocator->newT<constructor_new<C, init<Args...>>>();

            const class_type_scope_interface_ptr & scope = base_<C, B>::get_scope();

            scope->set_construct( ctr );

            return *this;
        }
    };

    template<class T>
    PyTypeObject * get_typemodule( kernel_interface * _kernel )
    {
        const class_type_scope_interface_ptr & scope = _kernel->get_class_type_scope_t<T>();

        PyTypeObject * typemodule = scope->get_typeobject();

        return typemodule;
    }
}

