#pragma once

#include "pybind/class_type_scope_interface.hpp"

#include "pybind/adapter/new_adapter.hpp"
#include "pybind/adapter/destroy_adapter.hpp"
#include "pybind/adapter/method_adapter.hpp"
#include "pybind/adapter/member_adapter.hpp"
#include "pybind/adapter/convert_adapter.hpp"
#include "pybind/adapter/compare_adapter.hpp"
#include "pybind/adapter/repr_adapter.hpp"
#include "pybind/adapter/hash_adapter.hpp"
#include "pybind/adapter/mapping_adapter.hpp"
#include "pybind/adapter/getattro_adapter.hpp"
#include "pybind/adapter/sequence_get_adapter.hpp"
#include "pybind/adapter/sequence_set_adapter.hpp"
#include "pybind/adapter/number_binary_adapter.hpp"
#include "pybind/adapter/smart_pointer_adapter.hpp"
#include "pybind/adapter/bindable_adapter.hpp"

#include "pybind/helper/bases_helper.hpp"

#include "pybind/exception.hpp"

#include "config/stdex.hpp"

namespace pybind
{
    typedef bases<> no_bases;

    template<class C, class B = no_bases>
    class base_
    {
    protected:
        typedef B bases_type;

    public:
        base_( kernel_interface * _kernel, const char * _name, void * _user, const new_adapter_interface_ptr & _pynew, const destroy_adapter_interface_ptr & _pydestructor, uint32_t _size, bool _hash, PyObject * _module )
            : m_kernel( _kernel )
        {
            uint32_t pod = 0;

            if( _size <= PYBIND_OBJECT_POD_SIZE )
            {
                pod = _size;
            }

            const class_type_scope_interface_ptr & scope = m_kernel->create_new_scope<C>( _name, _user, _pynew, _pydestructor, pod, _hash );

            this->setup_bases( scope );

            if( scope->initialize( _module ) == false )
            {
                return;
            }

            m_scope = scope;
        }

    public:
        template<class F>
        base_ & def( const char * _name, F f )
        {
            method_adapter_interface_ptr iadapter( new method_adapter<C, F>( _name, f ) );
            m_scope->add_method( iadapter );

            return *this;
        }

        template<class R, class M, class ... Args>
        base_ & def_const( const char * _name, R( M:: * f )(Args ...) const )
        {
            typedef R( M:: * F )(Args ...) const;

            method_adapter_interface_ptr iadapter( new method_adapter<C, F>( _name, f ) );
            m_scope->add_method( iadapter );

            return *this;
        }

        template<class F>
        base_ & def_kernel( const char * _name, F f )
        {
            method_adapter_interface_ptr iadapter( new method_adapter_kernel<C, F>( _name, f ) );
            m_scope->add_method( iadapter );

            return *this;
        }

        template<class F>
        base_ & def_args( const char * _name, F f )
        {
            method_adapter_interface_ptr iadapter( new method_adapter_args<C, F>( _name, f ) );
            m_scope->add_method( iadapter );

            return *this;
        }

        template<class F>
        base_ & def_deprecated( const char * _name, F f, const char * _doc )
        {
            method_adapter_interface_ptr iadapter( new method_adapter_deprecated<C, F>( _name, f, _doc ) );
            m_scope->add_method( iadapter );

            return *this;
        }

        template<class F>
        base_ & def_native( const char * _name, F f )
        {
            method_adapter_interface_ptr iadapter( new method_adapter_native<C, F>( _name, f ) );
            m_scope->add_method( iadapter );

            return *this;
        }

        template<class F>
        base_ & def_native_kernel( const char * _name, F f )
        {
            method_adapter_interface_ptr iadapter( new method_adapter_native_kernel<C, F>( _name, f ) );
            m_scope->add_method( iadapter );

            return *this;
        }

        template<class F>
        base_ & def_static_native( const char * _name, F f )
        {
            method_adapter_interface_ptr iadapter( new method_adapter_static_native<C, F>( _name, f ) );
            m_scope->add_method( iadapter );

            return *this;
        }

        template<class F>
        base_ & def_static_native_kernel( const char * _name, F f )
        {
            method_adapter_interface_ptr iadapter( new method_adapter_static_native_kernel<C, F>( _name, f ) );
            m_scope->add_method( iadapter );

            return *this;
        }

        template<class F>
        base_ & def_convert( F f, void * _user )
        {
            convert_adapter_interface_ptr iadapter( new convert_adapter<C, F>( f, _user ) );
            m_scope->set_convert( iadapter );

            return *this;
        }

        template<class F>
        base_ & def_static( const char * _name, F f )
        {
            method_adapter_interface_ptr iadapter( new method_adapter_proxy_function<C, F>( _name, f ) );
            m_scope->add_method( iadapter );

            return *this;
        }

        template<class F>
        base_ & def_static_kernel( const char * _name, F f )
        {
            method_adapter_interface_ptr iadapter( new method_adapter_proxy_function_kernel<C, F>( _name, f ) );
            m_scope->add_method( iadapter );

            return *this;
        }

        template<class F>
        base_ & def_static_args( const char * _name, F f )
        {
            method_adapter_interface_ptr iadapter( new method_adapter_proxy_function_args<C, F>( _name, f ) );
            m_scope->add_method( iadapter );

            return *this;
        }

        template<class P, class F>
        base_ & def_proxy_static( const char * _name, P * _proxy, F f )
        {
            method_adapter_interface_ptr iadapter( new method_adapter_proxy<C, P, F>( _name, f, _proxy ) );
            m_scope->add_method( iadapter );

            return *this;
        }

        template<class P, class F>
        base_ & def_proxy_static( const char * _name, const stdex::intrusive_ptr<P> & _proxy, F f )
        {
            method_adapter_interface_ptr iadapter( new method_adapter_proxy<C, P, F>( _name, f, _proxy.get() ) );
            m_scope->add_method( iadapter );

            return *this;
        }

        template<class P, class F>
        base_ & def_proxy_static_deprecated( const char * _name, P * _proxy, F f, const char * _doc )
        {
            method_adapter_interface_ptr iadapter( new method_adapter_proxy_deprecated<C, P, F>( _name, f, _proxy, _doc ) );
            m_scope->add_method( iadapter );

            return *this;
        }

        template<class P, class F>
        base_ & def_proxy_static_deprecated( const char * _name, const stdex::intrusive_ptr<P> & _proxy, F f, const char * _doc )
        {
            method_adapter_interface_ptr iadapter( new method_adapter_proxy_deprecated<C, P, F>( _name, f, _proxy.get(), _doc ) );
            m_scope->add_method( iadapter );

            return *this;
        }

        template<class P, class F>
        base_ & def_proxy_static_kernel( const char * _name, P * _proxy, F f )
        {
            method_adapter_interface_ptr iadapter( new method_adapter_proxy_kernel<C, P, F>( _name, f, _proxy ) );
            m_scope->add_method( iadapter );

            return *this;
        }

        template<class P, class F>
        base_ & def_proxy_static_kernel( const char * _name, const stdex::intrusive_ptr<P> & _proxy, F f )
        {
            method_adapter_interface_ptr iadapter( new method_adapter_proxy_kernel<C, P, F>( _name, f, _proxy.get() ) );
            m_scope->add_method( iadapter );

            return *this;
        }

        template<class P, class F>
        base_ & def_proxy_static_kernel_args( const char * _name, P * _proxy, F f )
        {
            method_adapter_interface_ptr iadapter( new method_adapter_proxy_kernel_args<C, P, F>( _name, f, _proxy ) );
            m_scope->add_method( iadapter );

            return *this;
        }

        template<class P, class F>
        base_ & def_proxy_static_kernel_args( const char * _name, const stdex::intrusive_ptr<P> & _proxy, F f )
        {
            method_adapter_interface_ptr iadapter( new method_adapter_proxy_kernel_args<C, P, F>( _name, f, _proxy.get() ) );
            m_scope->add_method( iadapter );

            return *this;
        }

        template<class P, class F>
        base_ & def_proxy_static_args( const char * _name, P * _proxy, F f )
        {
            method_adapter_interface_ptr iadapter( new method_adapter_proxy_args<C, P, F>( _name, f, _proxy ) );
            m_scope->add_method( iadapter );

            return *this;
        }

        template<class P, class F>
        base_ & def_proxy_static_args( const char * _name, const stdex::intrusive_ptr<P> & _proxy, F f )
        {
            method_adapter_interface_ptr iadapter( new method_adapter_proxy_args<C, P, F>( _name, f, _proxy.get() ) );
            m_scope->add_method( iadapter );

            return *this;
        }

        template<class P, class F>
        base_ & def_proxy_native( const char * _name, P * _proxy, F f )
        {
            method_adapter_interface_ptr iadapter( new method_adapter_proxy_native<C, P, F>( _name, f, _proxy ) );
            m_scope->add_method( iadapter );

            return *this;
        }

        template<class P, class F>
        base_ & def_proxy_native( const char * _name, stdex::intrusive_ptr<P> & _proxy, F f )
        {
            method_adapter_interface_ptr iadapter( new method_adapter_proxy_native<C, P, F>( _name, f, _proxy.get() ) );
            m_scope->add_method( iadapter );

            return *this;
        }

        template<class P, class F>
        base_ & def_proxy_native_kernel( const char * _name, P * _proxy, F f )
        {
            method_adapter_interface_ptr iadapter( new method_adapter_proxy_native_kernel<C, P, F>( _name, f, _proxy ) );
            m_scope->add_method( iadapter );

            return *this;
        }

        template<class P, class F>
        base_ & def_proxy_native_kernel( const char * _name, const stdex::intrusive_ptr<P> & _proxy, F f )
        {
            method_adapter_interface_ptr iadapter( new method_adapter_proxy_native_kernel<C, P, F>( _name, f, _proxy.get() ) );
            m_scope->add_method( iadapter );

            return *this;
        }

        template<class A>
        base_ & def_member( const char * _name, A C:: * a )
        {
            member_adapter_interface_ptr iadapter( new member_adapter<C, A>( _name, a ) );
            m_scope->add_member( iadapter );

            return *this;
        }

        template<class FG, class FS>
        base_ & def_property( const char * _name, FG _get, FS _set )
        {
            member_adapter_interface_ptr iadapter( new member_adapter_property<C, FG, FS>( _name, _get, _set ) );
            m_scope->add_member( iadapter );

            return *this;
        }

        template<class FG, class FS>
        base_ & def_property_static( const char * _name, FG _get, FS _set )
        {
            member_adapter_interface_ptr iadapter( new member_adapter_property_static<C, FG, FS>( _name, _get, _set ) );

            m_scope->add_member( iadapter );

            return *this;
        }

        template<class F>
        base_ & def_call( F _fn )
        {
            method_adapter_interface_ptr iadapter( new method_adapter<C, F>( "__call__", _fn ) );
            m_scope->set_call( iadapter );

            return *this;
        }

        template<class F>
        base_ & def_repr( F _fn )
        {
            repr_adapter_interface_ptr iadapter( new repr_adapter<C, F>( _fn ) );
            m_scope->set_repr( iadapter );

            return *this;
        }

        template<class F>
        base_ & def_hash( F _fn )
        {
            hash_adapter_interface_ptr iadapter( new hash_adapter<C, F>( _fn ) );
            m_scope->set_hash( iadapter );

            return *this;
        }

        template<class F>
        base_ & def_compare( F _fn )
        {
            compare_adapter_interface_ptr iadapter( new compare_adapter<C, F>( _fn ) );
            m_scope->set_compare( iadapter );

            return *this;
        }

        template<class F>
        base_ & def_getattro( F _fn )
        {
            getattro_adapter_interface_ptr iadapter( new getattro_adapter<C, F>( "getattro", _fn ) );
            m_scope->set_getattro( iadapter );

            return *this;
        }

        template<class F>
        base_ & def_mapping( F _fn )
        {
            mapping_adapter_interface_ptr iadapter( new mapping_adapter<C, F>( "getmap", _fn ) );
            m_scope->set_mapping( iadapter );

            return *this;
        }

        template<class F>
        base_ & def_sequence_get( F _fn )
        {
            sequence_get_adapter_interface_ptr iadapter( new sequence_get_adapter<C, F>( "sequence_get", _fn ) );
            m_scope->set_sequence_get( iadapter );

            return *this;
        }

        template<class F>
        base_ & def_sequence_set( F _fn )
        {
            sequence_set_adapter_interface_ptr iadapter( new sequence_set_adapter<C, F>( "sequence_set", _fn ) );
            m_scope->set_sequence_set( iadapter );

            return *this;
        }

        template<class F>
        base_ & def_static_sequence_get( F _fn )
        {
            sequence_get_adapter_interface_ptr iadapter( new sequence_get_adapter_proxy_function<C, F>( "static_sequence_get", _fn ) );
            m_scope->set_sequence_get( iadapter );

            return *this;
        }

        template<class F>
        base_ & def_static_sequence_set( F _fn )
        {
            sequence_set_adapter_interface_ptr iadapter( new sequence_set_adapter_proxy_function<C, F>( "static_sequence_set", _fn ) );
            m_scope->set_sequence_set( iadapter );

            return *this;
        }

        base_ & def_operator_getset()
        {
            sequence_set_adapter_interface_ptr iadapter_set( new sequence_set_adapter_operator<C>( "operator_set" ) );
            m_scope->set_sequence_set( iadapter_set );

            sequence_get_adapter_interface_ptr iadapter_get( new sequence_get_adapter_operator<C>( "operator_get" ) );
            m_scope->set_sequence_get( iadapter_get );

            return *this;
        }

        template<class F>
        base_ & def_add( F _fn )
        {
            number_binary_adapter_interface_ptr iadapter( new number_binary_adapter<C, F>( "add", _fn ) );
            m_scope->set_number_add( iadapter );

            return *this;
        }

        template<class F>
        base_ & def_static_add( F _fn )
        {
            number_binary_adapter_interface_ptr iadapter( new number_binary_adapter_proxy_function<C, F>( "static_add", _fn ) );
            m_scope->set_number_add( iadapter );

            return *this;
        }

        template<class P, class F>
        base_ & def_proxy_static_add( P * _proxy, F f )
        {
            number_binary_adapter_interface_ptr iadapter( new number_binary_adapter_proxy<C, P, F>( "proxy_static_add", f, _proxy ) );
            m_scope->set_number_add( iadapter );

            return *this;
        }

        template<class V, class VC>
        base_ & def_operator_add_t()
        {
            number_binary_adapter_interface_ptr iadapter( new number_binary_adapter_operator_add<C, V>( "operator_add" ) );
            uint32_t typeId = m_kernel->class_info<VC>();

            m_scope->add_number_add( typeId, iadapter );

            return *this;
        }

        template<class V>
        base_ & def_operator_add_t()
        {
            return def_operator_add_t<V, V>();
        }

        base_ & def_operator_add()
        {
            number_binary_adapter_interface_ptr iadapter( new number_binary_adapter_operator_add<C, C>( "operator_add" ) );
            m_scope->set_number_add( iadapter );

            return *this;
        }

        template<class F>
        base_ & def_sub( F _fn )
        {
            number_binary_adapter_interface_ptr iadapter( new number_binary_adapter<C, F>( "sub", _fn ) );
            m_scope->set_number_sub( iadapter );

            return *this;
        }

        template<class F>
        base_ & def_static_sub( F _fn )
        {
            number_binary_adapter_interface_ptr iadapter( new number_binary_adapter_proxy_function<C, F>( "static_sub", _fn ) );
            m_scope->set_number_sub( iadapter );

            return *this;
        }

        template<class V, class VC>
        base_ & def_operator_sub_t()
        {
            number_binary_adapter_interface_ptr iadapter( new number_binary_adapter_operator_sub<C, V>( "operator_sub" ) );
            uint32_t typeId = m_kernel->class_info<VC>();

            m_scope->add_number_sub( typeId, iadapter );

            return *this;
        }

        template<class V>
        base_ & def_operator_sub_t()
        {
            return def_operator_sub_t<V, V>();
        }

        base_ & def_operator_sub()
        {
            number_binary_adapter_interface_ptr iadapter( new number_binary_adapter_operator_sub<C, C>( "operator_sub" ) );
            m_scope->set_number_sub( iadapter );

            return *this;
        }

        template<class P, class F>
        base_ & def_proxy_static_sub( P * _proxy, F f )
        {
            number_binary_adapter_interface_ptr iadapter( new number_binary_adapter_proxy<C, P, F>( "proxy_static_sub", f, _proxy ) );
            m_scope->set_number_sub( iadapter );

            return *this;
        }

        template<class F>
        base_ & def_mul( F _fn )
        {
            number_binary_adapter_interface_ptr iadapter( new number_binary_adapter<C, F>( "mul", _fn ) );
            m_scope->set_number_mul( iadapter );

            return *this;
        }

        template<class F>
        base_ & def_static_mul( F _fn )
        {
            number_binary_adapter_interface_ptr iadapter( new number_binary_adapter_proxy_function<C, F>( "static_mul", _fn ) );
            m_scope->set_number_mul( iadapter );

            return *this;
        }

        template<class P, class F>
        base_ & def_proxy_static_mul( P * _proxy, F f )
        {
            number_binary_adapter_interface_ptr iadapter( new number_binary_adapter_proxy<C, P, F>( "proxy_static_mul", f, _proxy ) );
            m_scope->set_number_mul( iadapter );

            return *this;
        }

        template<class V, class VC>
        base_ & def_operator_mul_t()
        {
            number_binary_adapter_interface_ptr iadapter( new number_binary_adapter_operator_mul<C, V>( "operator_mul" ) );

            uint32_t typeId = m_kernel->class_info<VC>();

            m_scope->add_number_mul( typeId, iadapter );

            return *this;
        }

        template<class V>
        base_ & def_operator_mul_t()
        {
            return def_operator_mul_t<V, V>();
        }

        base_ & def_operator_mul()
        {
            number_binary_adapter_interface_ptr iadapter( new number_binary_adapter_operator_mul<C, C>( "operator_mul" ) );
            m_scope->set_number_mul( iadapter );

            return *this;
        }

        template<class F>
        base_ & def_div( F _fn )
        {
            number_binary_adapter_interface_ptr iadapter( new number_binary_adapter<C, F>( "div", _fn ) );
            m_scope->set_number_div( iadapter );

            return *this;
        }

        template<class F>
        base_ & def_static_div( F _fn )
        {
            number_binary_adapter_interface_ptr iadapter( new number_binary_adapter_proxy_function<C, F>( "static_div", _fn ) );
            m_scope->set_number_div( iadapter );

            return *this;
        }

        template<class P, class F>
        base_ & def_proxy_static_div( P * _proxy, F f )
        {
            number_binary_adapter_interface_ptr iadapter( new number_binary_adapter_proxy<C, P, F>( "proxy_static_div", f, _proxy ) );
            m_scope->set_number_div( iadapter );

            return *this;
        }

        template<class V, class VC>
        base_ & def_operator_div_t()
        {
            number_binary_adapter_interface_ptr iadapter( new number_binary_adapter_operator_div_nr<C, V>( "operator_div_nr" ) );
            uint32_t typeId = m_kernel->class_info<VC>();

            m_scope->add_number_div( typeId, iadapter );

            return *this;
        }

        template<class V>
        base_ & def_operator_div_t()
        {
            return def_operator_div_t<V, V>();
        }

        base_ & def_operator_div()
        {
            number_binary_adapter_interface_ptr iadapter( new number_binary_adapter_operator_div<C, C>( "operator_div" ) );
            m_scope->set_number_div( iadapter );

            return *this;
        }

        template<class FI, class FD>
        base_ & def_static_proxy_smart_pointer( FI _incref, FD _decref )
        {
            smart_pointer_adapter_interface_ptr iadapter( new smart_pointer_adapter_proxy_function<C, FI, FD>( "smart_pointer", _incref, _decref ) );
            m_scope->set_smart_pointer( iadapter );

            return *this;
        }

        base_ & def_smart_pointer()
        {
            return this->def_static_proxy_smart_pointer( &C::intrusive_ptr_add_ref, &C::intrusive_ptr_dec_ref );
        }

        base_ & remove_smart_pointer()
        {
            m_scope->set_smart_pointer( nullptr );

            return *this;
        }

        base_ & def_bindable()
        {
            bindable_adapter_interface_ptr iadapter( new bindable_adapter<C>( "unwrap_bindable" ) );
            m_scope->set_bindable( iadapter );

            return *this;
        }

        const class_type_scope_interface_ptr & get_scope() const
        {
            return m_scope;
        }

    protected:
        template<class BB>
        static void * meta_cast( void * _ptr )
        {
            C * c = static_cast<C *>(_ptr);
            BB * bb = static_cast<BB *>(c);

            return bb;
        }

        void setup_extract( const type_cast_ptr & _type )
        {
            registration_type_cast<C>(m_kernel, _type);
        }

        template<class T>
        void add_base( const class_type_scope_interface_ptr & _scope )
        {
            _scope->add_base_t<T>( &base_::meta_cast<T> );
        }

        template<class bases_tuple, uint32_t ... I>
        void setup_bases_i( const class_type_scope_interface_ptr & _scope, std::integer_sequence<uint32_t, I...> )
        {
            (_scope->add_base_t<typename std::tuple_element<I, bases_tuple>::type>( &base_::meta_cast<typename std::tuple_element<I, bases_tuple>::type> ), ...);
        }

        void setup_bases( const class_type_scope_interface_ptr & _scope )
        {
            typedef typename bases_type::bases_tuple bases_tuple;

            this->setup_bases_i<bases_tuple>( _scope, std::make_integer_sequence<uint32_t, std::tuple_size<bases_tuple>::value>() );
        }

    protected:
        class_type_scope_interface_ptr m_scope;

        kernel_interface * m_kernel;
    };
}

