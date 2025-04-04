#pragma once

#include "pybind/exports.hpp"
#include "pybind/types.hpp"
#include "pybind/exception.hpp"

#include "pybind/kernel_interface.hpp"

namespace pybind
{
    //////////////////////////////////////////////////////////////////////////
    typedef intrusive_ptr<class method_type_scope> method_type_scope_ptr;
    typedef intrusive_ptr<class member_type_scope> member_type_scope_ptr;
    typedef intrusive_ptr<class new_adapter_interface> new_adapter_interface_ptr;
    typedef intrusive_ptr<class destroy_adapter_interface> destroy_adapter_interface_ptr;
    typedef intrusive_ptr<class repr_adapter_interface> repr_adapter_interface_ptr;
    typedef intrusive_ptr<class method_adapter_interface> method_adapter_interface_ptr;
    typedef intrusive_ptr<class member_adapter_interface> member_adapter_interface_ptr;
    typedef intrusive_ptr<class convert_adapter_interface> convert_adapter_interface_ptr;
    typedef intrusive_ptr<class hash_adapter_interface> hash_adapter_interface_ptr;
    typedef intrusive_ptr<class compare_adapter_interface> compare_adapter_interface_ptr;
    typedef intrusive_ptr<class constructor_adapter_interface> constructor_adapter_interface_ptr;
    typedef intrusive_ptr<class getattro_adapter_interface> getattro_adapter_interface_ptr;
    typedef intrusive_ptr<class mapping_adapter_interface> mapping_adapter_interface_ptr;
    typedef intrusive_ptr<class sequence_get_adapter_interface> sequence_get_adapter_interface_ptr;
    typedef intrusive_ptr<class sequence_set_adapter_interface> sequence_set_adapter_interface_ptr;    
    typedef intrusive_ptr<class number_unary_adapter_interface> number_unary_adapter_interface_ptr;
    typedef intrusive_ptr<class number_inplace_adapter_interface> number_inplace_adapter_interface_ptr;
    typedef intrusive_ptr<class number_binary_adapter_interface> number_binary_adapter_interface_ptr;
    typedef intrusive_ptr<class smart_pointer_adapter_interface> smart_pointer_adapter_interface_ptr;
    typedef intrusive_ptr<class bindable_adapter_interface> bindable_adapter_interface_ptr;
    typedef intrusive_ptr<class proxy_adapter_interface> proxy_adapter_interface_ptr;
    //////////////////////////////////////////////////////////////////////////
    class class_type_scope_interface
        : public factorable
    {
    public:
        virtual bool initialize( PyObject * _module ) = 0;
        virtual void finalize() = 0;

    public:
        virtual kernel_interface * get_kernel() const = 0;

    public:
        virtual const char * get_name() const = 0;
        virtual typeid_t get_type_id() const = 0;
        virtual size_t get_pod_size() const = 0;
        virtual bool get_pod_hash() const = 0;

        virtual void * get_user() const = 0;

        template<class T>
        T * get_user_t() const
        {
            void * ptr = this->get_user();

            return static_cast<T *>(ptr);
        }

        virtual PyTypeObject * get_typeobject() const = 0;

        virtual void add_method( const method_adapter_interface_ptr & _imethod ) = 0;
        virtual const method_adapter_interface_ptr & find_method( const char * _name ) const = 0;
        virtual size_t get_methods_count() const = 0;
        virtual const method_adapter_interface_ptr & get_method( size_t _index ) const = 0;

        virtual void add_member( const member_adapter_interface_ptr & _imember ) = 0;
        virtual const member_adapter_interface_ptr & find_member( const char * _name ) const = 0;
        virtual size_t get_members_count() const = 0;
        virtual const member_adapter_interface_ptr & get_member( size_t _index ) const = 0;

        virtual void add_base( typeid_t _info, const class_type_scope_interface_ptr & _scope, pybind_metacast _cast ) = 0;

        template<class B>
        void add_base_t( pybind_metacast _cast )
        {
            kernel_interface * kernel = this->get_kernel();

            typeid_t tptrinfo = kernel->class_info<B *>();
            typeid_t tinfo = kernel->class_info<B>();

            if( kernel->has_class_type_scope( tinfo ) == false )
            {
                pybind::throw_exception( "class_type_scope_interface::add_base_t '%s' not bind base type '%s'"
                    , this->get_name()
                    , kernel->get_class_type_info( tinfo )
                );

                return;
            }

            const class_type_scope_interface_ptr & basescope = kernel->get_class_type_scope( tinfo );

            this->add_base( tptrinfo, basescope, _cast );
        }

        virtual void set_construct( const constructor_adapter_interface_ptr & _iconstruct ) = 0;
        virtual void set_convert( const convert_adapter_interface_ptr & _iconvert ) = 0;
        virtual void set_call( const method_adapter_interface_ptr & _icall ) = 0;
        virtual void set_repr( const repr_adapter_interface_ptr & _irepr ) = 0;
        virtual void set_hash( const hash_adapter_interface_ptr & _ihash ) = 0;
        virtual void set_compare( typeid_t _typeId, const compare_adapter_interface_ptr & _iadapter ) = 0;
        virtual void set_getattro( const getattro_adapter_interface_ptr & _igetattro ) = 0;
        virtual void set_mapping( const mapping_adapter_interface_ptr & _imapping ) = 0;
        virtual void set_sequence_get( const sequence_get_adapter_interface_ptr & _isequence ) = 0;
        virtual void set_sequence_set( const sequence_set_adapter_interface_ptr & _isequence ) = 0;
        virtual void set_number_neg( const number_unary_adapter_interface_ptr & _iadapter ) = 0;
        virtual void set_number_abs( const number_unary_adapter_interface_ptr & _iadapter ) = 0;
        virtual void set_number_add( typeid_t _typeId, const number_binary_adapter_interface_ptr & _iadapter ) = 0;
        virtual void set_number_sub( typeid_t _typeId, const number_binary_adapter_interface_ptr & _iadapter ) = 0;
        virtual void set_number_mul( typeid_t _typeId, const number_binary_adapter_interface_ptr & _iadapter ) = 0;
        virtual void set_number_div( typeid_t _typeId, const number_binary_adapter_interface_ptr & _iadapter ) = 0;
        virtual void set_number_inplace_add( typeid_t _typeId, const number_inplace_adapter_interface_ptr & _iadapter ) = 0;
        virtual void set_number_inplace_sub( typeid_t _typeId, const number_inplace_adapter_interface_ptr & _iadapter ) = 0;
        virtual void set_number_inplace_mul( typeid_t _typeId, const number_inplace_adapter_interface_ptr & _iadapter ) = 0;
        virtual void set_number_inplace_div( typeid_t _typeId, const number_inplace_adapter_interface_ptr & _iadapter ) = 0;
        virtual void set_smart_pointer( const smart_pointer_adapter_interface_ptr & _iadapter ) = 0;
        virtual void set_bindable( const bindable_adapter_interface_ptr & _iadapter ) = 0;
        virtual void add_proxy_interface( const class_type_scope_interface_ptr & _scope, const proxy_adapter_interface_ptr & _iproxy ) = 0;

        virtual PyObject * create_class( void * _impl ) = 0;
        virtual PyObject * create_holder( void * _impl ) = 0;
        virtual PyObject * create_weak( void * _impl ) = 0;
        virtual PyObject * create_pod( void ** _impl ) = 0;

        virtual void * meta_cast( typeid_t _typeId, void * _impl ) = 0;
        virtual void type_initialize( PyTypeObject * _type ) = 0;

        virtual bool is_instance( PyTypeObject * _type ) const = 0;

    public:
        virtual const new_adapter_interface_ptr & get_new() const = 0;
        virtual const destroy_adapter_interface_ptr & get_destroy() const = 0;
        virtual const constructor_adapter_interface_ptr & get_constructor() const = 0;
        virtual const convert_adapter_interface_ptr & get_convert() const = 0;
        virtual const method_adapter_interface_ptr & get_call() const = 0;
        virtual const repr_adapter_interface_ptr & get_repr() const = 0;
        virtual const hash_adapter_interface_ptr & get_hash() const = 0;
        virtual const compare_adapter_interface_ptr & get_compare( typeid_t _typeId ) const = 0;
        virtual const getattro_adapter_interface_ptr & get_getattro() const = 0;
        virtual const mapping_adapter_interface_ptr & get_mapping() const = 0;
        virtual const sequence_get_adapter_interface_ptr & get_sequence_get() const = 0;
        virtual const sequence_set_adapter_interface_ptr & get_sequence_set() const = 0;

        virtual const number_unary_adapter_interface_ptr & get_number_neg() const = 0;
        virtual const number_unary_adapter_interface_ptr & get_number_abs() const = 0;

        virtual const number_binary_adapter_interface_ptr & get_number_add( typeid_t _typeId ) const = 0;
        virtual const number_binary_adapter_interface_ptr & get_number_sub( typeid_t _typeId ) const = 0;
        virtual const number_binary_adapter_interface_ptr & get_number_mul( typeid_t _typeId ) const = 0;
        virtual const number_binary_adapter_interface_ptr & get_number_div( typeid_t _typeId ) const = 0;

        virtual const number_inplace_adapter_interface_ptr & get_number_inplace_add( typeid_t _typeId ) const = 0;
        virtual const number_inplace_adapter_interface_ptr & get_number_inplace_sub( typeid_t _typeId ) const = 0;
        virtual const number_inplace_adapter_interface_ptr & get_number_inplace_mul( typeid_t _typeId ) const = 0;
        virtual const number_inplace_adapter_interface_ptr & get_number_inplace_div( typeid_t _typeId ) const = 0;

        virtual const smart_pointer_adapter_interface_ptr & get_smart_pointer() const = 0;
        virtual const bindable_adapter_interface_ptr & get_bindable() const = 0;

    public:
        virtual void incref_smart_pointer( void * _impl ) = 0;
        virtual void decref_smart_pointer( void * _impl ) = 0;

    public:
        virtual void * call_new( PyObject * _obj, PyObject * _args, PyObject * _kwds ) = 0;
        virtual void call_destructor( PyObject * _obj, void * _impl ) = 0;
        virtual void clear_bindable( void * _impl ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef intrusive_ptr<class_type_scope_interface> class_type_scope_interface_ptr;
    //////////////////////////////////////////////////////////////////////////
}

