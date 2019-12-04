#pragma once

#include "pybind/type_cast_result.hpp"

#include "pybind/class_type_scope_interface.hpp"

#include "pybind/helper/bases_helper.hpp"

#include "pybind/exception.hpp"

#include "pybind/constructor_new.hpp"
#include "pybind/constructor_placement.hpp"

namespace pybind
{
    class class_new_invalid
        : public new_adapter_interface
    {
    public:
        void * call( kernel_interface * _kernel, const class_type_scope_interface_ptr & _scope, PyObject * _obj, PyObject * _args, PyObject * _kwds ) override
        {
            (void)_kernel;
            (void)_scope;
            (void)_obj;
            (void)_args;
            (void)_kwds;

            pybind::throw_exception( "scope %s not support 'new_interface'"
                , _scope->get_name()
            );

            return nullptr;
        }
    };

    template<class C>
    class class_destroy_delete
        : public destroy_adapter_interface
    {
    public:
        void call( kernel_interface * _kernel, const class_type_scope_interface_ptr & _scope, void * _impl ) override
        {
            (void)_kernel;
            (void)_scope;

            C * obj = static_cast<C*>(_impl);

            delete obj;
        }
    };

    template<class C>
    class class_destroy_destructor
        : public destroy_adapter_interface
    {
    public:
        void call( kernel_interface * _kernel, const class_type_scope_interface_ptr & _scope, void * _impl ) override
        {
            (void)_kernel;
            (void)_scope;

            C * obj = static_cast<C*>(_impl);

            (void)obj;

            obj->~C();
        }
    };

    template<class C>
    struct extract_class_type_ptr
        : public type_cast_result<C *>
    {
        bool apply( kernel_interface * _kernel, PyObject * _obj, typename type_cast_result<C *>::TCastValue _value, bool _nothrow ) override
        {
            if( _kernel->is_none( _obj ) == true )
            {
                _value = nullptr;

                return true;
            }

            uint32_t tinfo = _kernel->class_info<C>();
            uint32_t tptrinfo = _kernel->class_info<C *>();

            void * impl;
            if( this->type_info_cast( _kernel, _obj, tinfo, tptrinfo, &impl ) == false )
            {
                if( _nothrow == false )
                {
                    _kernel->error_invalid_extract( _obj, tinfo );
                }

                _value = nullptr;

                return false;
            }

            _value = static_cast<C*>(impl);

            return true;
        }

        PyObject * wrap( kernel_interface * _kernel, typename type_cast_result<C *>::TCastRef _value ) override
        {
            if( _value == nullptr )
            {
                return _kernel->ret_none();
            }

            const class_type_scope_interface_ptr & scope = _kernel->get_class_type_scope_t<C>();

            PyObject * py_obj = scope->create_class( (void *)_value );

            return py_obj;
        }
    };

    template<class C>
    struct extract_holder_type_ptr
        : public type_cast_result<C *>
    {
        bool apply( kernel_interface * _kernel, PyObject * _obj, typename type_cast_result<C *>::TCastValue _value, bool _nothrow ) override
        {
            if( _kernel->is_none( _obj ) == true )
            {
                _value = nullptr;

                return true;
            }

            uint32_t tinfo = _kernel->class_info<C>();
            uint32_t tptrinfo = _kernel->class_info<C *>();

            void * impl;
            if( type_cast::type_info_cast( _kernel, _obj, tinfo, tptrinfo, &impl ) == false )
            {
                if( _nothrow == false )
                {
                    _kernel->error_invalid_extract( _obj, tinfo );
                }

                _value = nullptr;

                return false;
            }

            _value = static_cast<C*>(impl);

            return true;
        }

        PyObject * wrap( kernel_interface * _kernel, typename type_cast_result<C *>::TCastRef _value ) override
        {
            if( _value == nullptr )
            {
                return _kernel->ret_none();
            }

            PyObject * py_obj = _kernel->scope_create_holder_t( _value );

            return py_obj;
        }
    };

    template<class C>
    struct extract_struct_type_ref
        : public type_cast_result<C>
    {
        bool apply( kernel_interface * _kernel, PyObject * _obj, typename type_cast_result<C>::TCastValue _value, bool _nothrow ) override
        {
            uint32_t tinfo = _kernel->class_info<C>();
            uint32_t tptrinfo = _kernel->class_info<C *>();

            void * impl;
            if( type_cast::type_info_cast( _kernel, _obj, tinfo, tptrinfo, &impl ) == false )
            {
                const class_type_scope_interface_ptr & scope = _kernel->get_class_type_scope( tinfo );

                const convert_adapter_interface_ptr & convert = scope->get_convert_adapter();

                if( convert == nullptr )
                {
                    if( _nothrow == false )
                    {
                        _kernel->error_invalid_extract( _obj, tinfo );
                    }

                    return false;
                }

                if( convert->convert( _kernel, _obj, &_value ) == false )
                {
                    if( _nothrow == false )
                    {
                        _kernel->error_invalid_extract( _obj, tinfo );
                    }

                    return false;
                }

                return true;
            }

            _value = *static_cast<C*>(impl);

            return true;
        }

        PyObject * wrap( kernel_interface * _kernel, const C & _class ) override
        {
            uint32_t tinfo = _kernel->class_info<C>();

            void * obj_place = nullptr;

            const class_type_scope_interface_ptr & scope = _kernel->get_class_type_scope( tinfo );

            PyObject * py_obj = scope->create_pod( &obj_place );

            if( py_obj == nullptr )
            {
                return nullptr;
            }

            new (obj_place)C( _class );

            return py_obj;
        }
    };
}

