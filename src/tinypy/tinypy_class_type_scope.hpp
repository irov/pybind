#pragma once

#include "pybind/class_type_scope_interface.hpp"
#include "pybind/extract.hpp"

#include "pybind/adapter/bindable_adapter.hpp"
#include "pybind/adapter/compare_adapter.hpp"
#include "pybind/adapter/constructor_adapter.hpp"
#include "pybind/adapter/convert_adapter.hpp"
#include "pybind/adapter/getattro_adapter.hpp"
#include "pybind/adapter/hash_adapter.hpp"
#include "pybind/adapter/mapping_adapter.hpp"
#include "pybind/adapter/number_binary_adapter.hpp"
#include "pybind/adapter/number_inplace_adapter.hpp"
#include "pybind/adapter/number_unary_adapter.hpp"
#include "pybind/adapter/proxy_adapter.hpp"
#include "pybind/adapter/repr_adapter.hpp"
#include "pybind/adapter/sequence_get_adapter.hpp"
#include "pybind/adapter/sequence_set_adapter.hpp"
#include "pybind/adapter/smart_pointer_adapter.hpp"
#include "pybind/adapter/new_adapter.hpp"
#include "pybind/adapter/destroy_adapter.hpp"

#include "tinypy/tinypy.hpp"

#include <cstddef>

namespace pybind
{
    //////////////////////////////////////////////////////////////////////////
    class tinypy_kernel;
    //////////////////////////////////////////////////////////////////////////
    class tinypy_class_type_scope
        : public class_type_scope_interface
    {
    public:
        struct payload_t
        {
            tinypy_class_type_scope * scope;
            void * impl;
            uint32_t flags;
            int64_t hash;
            alignas(max_align_t) uint8_t pod[256];
        };

        enum payload_flag_e : uint32_t
        {
            PayloadHolder = 1U << 0,
            PayloadWeak = 1U << 1,
            PayloadPod = 1U << 2,
            PayloadUnwrapped = 1U << 3,
            PayloadHashValid = 1U << 4,
            PayloadConstructed = 1U << 5
        };

    public:
        tinypy_class_type_scope( tinypy_kernel * _kernel, const char * _name, typeid_t _typeId, void * _user, const new_adapter_interface_ptr & _pynew, const destroy_adapter_interface_ptr & _pydestructor, size_t _pod, bool _hash );
        ~tinypy_class_type_scope() override;

    public:
        bool initialize( PyObject * _module ) override;
        void finalize() override;

    public:
        kernel_interface * get_kernel() const override;

    public:
        const char * get_name() const override;
        typeid_t get_type_id() const override;
        size_t get_pod_size() const override;
        bool get_pod_hash() const override;

        void * get_user() const override;

        PyTypeObject * get_typeobject() const override;

        void add_method( const method_adapter_interface_ptr & _imethod ) override;
        const method_adapter_interface_ptr & find_method( const char * _name ) const override;
        size_t get_methods_count() const override;
        const method_adapter_interface_ptr & get_method( size_t _index ) const override;

        void add_member( const member_adapter_interface_ptr & _imember ) override;
        const member_adapter_interface_ptr & find_member( const char * _name ) const override;
        size_t get_members_count() const override;
        const member_adapter_interface_ptr & get_member( size_t _index ) const override;

        void add_base( typeid_t _info, const class_type_scope_interface_ptr & _scope, pybind_metacast _cast ) override;

        void set_construct( const constructor_adapter_interface_ptr & _iconstruct ) override;
        void set_convert( const convert_adapter_interface_ptr & _iconvert ) override;
        void set_call( const method_adapter_interface_ptr & _icall ) override;
        void set_repr( const repr_adapter_interface_ptr & _irepr ) override;
        void set_hash( const hash_adapter_interface_ptr & _ihash ) override;
        void set_compare( typeid_t _typeId, const compare_adapter_interface_ptr & _iadapter ) override;
        void set_getattro( const getattro_adapter_interface_ptr & _igetattro ) override;
        void set_mapping( const mapping_adapter_interface_ptr & _imapping ) override;
        void set_sequence_get( const sequence_get_adapter_interface_ptr & _isequence ) override;
        void set_sequence_set( const sequence_set_adapter_interface_ptr & _isequence ) override;
        void set_number_neg( const number_unary_adapter_interface_ptr & _iadapter ) override;
        void set_number_abs( const number_unary_adapter_interface_ptr & _iadapter ) override;
        void set_number_add( typeid_t _typeId, const number_binary_adapter_interface_ptr & _iadapter ) override;
        void set_number_sub( typeid_t _typeId, const number_binary_adapter_interface_ptr & _iadapter ) override;
        void set_number_mul( typeid_t _typeId, const number_binary_adapter_interface_ptr & _iadapter ) override;
        void set_number_div( typeid_t _typeId, const number_binary_adapter_interface_ptr & _iadapter ) override;
        void set_number_inplace_add( typeid_t _typeId, const number_inplace_adapter_interface_ptr & _iadapter ) override;
        void set_number_inplace_sub( typeid_t _typeId, const number_inplace_adapter_interface_ptr & _iadapter ) override;
        void set_number_inplace_mul( typeid_t _typeId, const number_inplace_adapter_interface_ptr & _iadapter ) override;
        void set_number_inplace_div( typeid_t _typeId, const number_inplace_adapter_interface_ptr & _iadapter ) override;
        void set_smart_pointer( const smart_pointer_adapter_interface_ptr & _iadapter ) override;
        void set_bindable( const bindable_adapter_interface_ptr & _iadapter ) override;

        void add_proxy_interface( const class_type_scope_interface_ptr & _scope, const proxy_adapter_interface_ptr & _iproxy ) override;

        PyObject * create_class( void * _impl ) override;
        PyObject * create_holder( void * _impl ) override;
        PyObject * create_weak( void * _impl ) override;
        PyObject * create_pod( void ** _impl ) override;

        void * meta_cast( typeid_t _typeId, void * _impl ) override;
        void type_initialize( PyTypeObject * _type ) override;

        bool is_instance( PyTypeObject * _type ) const override;

    public:
        const new_adapter_interface_ptr & get_new() const override;
        const destroy_adapter_interface_ptr & get_destroy() const override;
        const constructor_adapter_interface_ptr & get_constructor() const override;
        const convert_adapter_interface_ptr & get_convert() const override;
        const method_adapter_interface_ptr & get_call() const override;
        const repr_adapter_interface_ptr & get_repr() const override;
        const hash_adapter_interface_ptr & get_hash() const override;
        const compare_adapter_interface_ptr & get_compare( typeid_t _typeId ) const override;
        const getattro_adapter_interface_ptr & get_getattro() const override;
        const mapping_adapter_interface_ptr & get_mapping() const override;
        const sequence_get_adapter_interface_ptr & get_sequence_get() const override;
        const sequence_set_adapter_interface_ptr & get_sequence_set() const override;

        const number_unary_adapter_interface_ptr & get_number_neg() const override;
        const number_unary_adapter_interface_ptr & get_number_abs() const override;

        const number_binary_adapter_interface_ptr & get_number_add( typeid_t _typeId ) const override;
        const number_binary_adapter_interface_ptr & get_number_sub( typeid_t _typeId ) const override;
        const number_binary_adapter_interface_ptr & get_number_mul( typeid_t _typeId ) const override;
        const number_binary_adapter_interface_ptr & get_number_div( typeid_t _typeId ) const override;

        const number_inplace_adapter_interface_ptr & get_number_inplace_add( typeid_t _typeId ) const override;
        const number_inplace_adapter_interface_ptr & get_number_inplace_sub( typeid_t _typeId ) const override;
        const number_inplace_adapter_interface_ptr & get_number_inplace_mul( typeid_t _typeId ) const override;
        const number_inplace_adapter_interface_ptr & get_number_inplace_div( typeid_t _typeId ) const override;

        const smart_pointer_adapter_interface_ptr & get_smart_pointer() const override;
        const bindable_adapter_interface_ptr & get_bindable() const override;

    public:
        void incref_smart_pointer( void * _impl ) override;
        void decref_smart_pointer( void * _impl ) override;

    public:
        void * call_new( PyObject * _obj, PyObject * _args, PyObject * _kwds ) override;
        void call_destructor( PyObject * _obj, void * _impl ) override;
        void clear_bindable( void * _impl ) override;

    public:
        payload_t * payload( PyObject * _object ) const;

    public:
        struct base_t
        {
            typeid_t info;
            class_type_scope_interface_ptr scope;
            pybind_metacast cast;
        };

        struct compare_adapter_t
        {
            compare_adapter_interface_ptr compares[PYBIND_TYPE_COUNT];
        };

        struct number_adapter_t
        {
            number_binary_adapter_interface_ptr adds[PYBIND_TYPE_COUNT];
            number_binary_adapter_interface_ptr subs[PYBIND_TYPE_COUNT];
            number_binary_adapter_interface_ptr muls[PYBIND_TYPE_COUNT];
            number_binary_adapter_interface_ptr divs[PYBIND_TYPE_COUNT];

            number_inplace_adapter_interface_ptr iadds[PYBIND_TYPE_COUNT];
            number_inplace_adapter_interface_ptr isubs[PYBIND_TYPE_COUNT];
            number_inplace_adapter_interface_ptr imuls[PYBIND_TYPE_COUNT];
            number_inplace_adapter_interface_ptr idivs[PYBIND_TYPE_COUNT];
        };

    private:
        PyObject * create_object_( void * _impl, uint32_t _flags );
        compare_adapter_t * get_compare_adapters_();
        number_adapter_t * get_number_adapters_();

    private:
        tinypy_kernel * m_kernel;

        char m_name[PYBIND_CLASS_TYPE_MAX_NAME + 1];
        typeid_t m_typeId;

        void * m_user;

        size_t m_pod_size;
        bool m_pod_hash;

        PyObject * m_module;
        tinypy_type_t * m_type;

        base_t m_bases[PYBIND_BASES_COUNT];
        size_t m_basesCount;

        method_adapter_interface_ptr m_methods[PYBIND_METHOD_COUNT];
        size_t m_methodCount;

        member_adapter_interface_ptr m_members[PYBIND_MEMBER_COUNT];
        size_t m_memberCount;

        new_adapter_interface_ptr m_new;
        destroy_adapter_interface_ptr m_destructor;
        constructor_adapter_interface_ptr m_constructor;
        convert_adapter_interface_ptr m_convert;
        method_adapter_interface_ptr m_call;
        repr_adapter_interface_ptr m_repr;
        hash_adapter_interface_ptr m_hash;
        getattro_adapter_interface_ptr m_getattro;
        mapping_adapter_interface_ptr m_mapping;
        sequence_get_adapter_interface_ptr m_sequence_get;
        sequence_set_adapter_interface_ptr m_sequence_set;

        number_unary_adapter_interface_ptr m_number_neg;
        number_unary_adapter_interface_ptr m_number_abs;

        compare_adapter_t * m_compare_adapters;
        number_adapter_t * m_number_adapters;

        smart_pointer_adapter_interface_ptr m_smart_pointer;
        bindable_adapter_interface_ptr m_bindable;
    };
    //////////////////////////////////////////////////////////////////////////
}
