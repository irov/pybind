#pragma once

#include "pybind/type_cast.hpp"

#include "pybind/kernel_interface.hpp"

namespace pybind
{
    namespace detail
    {
        template<class T> struct type_down_cast;

        template<class T, class B>
        struct type_down_cast_find
        {
            static type_cast * find( kernel_interface * _kernel )
            {
                typeid_t id = _kernel->class_info<T>();

                type_cast * etype = _kernel->find_type_info_extract( id );

                if( etype == nullptr )
                {
                    return type_down_cast<B>::find( _kernel );
                }

                return etype;
            }
        };

        template<class B>
        struct type_down_cast_find<PyObject, B>
        {
            static type_cast * find( kernel_interface * _kernel )
            {
                (void)_kernel;
                return nullptr;
            }
        };

        template<class T>
        struct type_down_cast
        {
            static type_cast * find( kernel_interface * _kernel )
            {
                return type_down_cast_find<T, void>::find( _kernel );
            }
        };

        template<>
        struct type_down_cast<void>
        {
            static type_cast * find( kernel_interface * _kernel )
            {
                (void)_kernel;

                return nullptr;
            }
        };

        template<class T>
        struct type_down_cast<T *>
        {
            static type_cast * find( kernel_interface * _kernel )
            {
                return type_down_cast_find<T *, T>::find( _kernel );
            }
        };

        template<class T>
        struct type_down_cast<const T *>
        {
            static type_cast * find( kernel_interface * _kernel )
            {
                return type_down_cast_find<const T *, T>::find( _kernel );
            }
        };

        template<class T>
        struct type_down_cast<T &>
        {
            static type_cast * find( kernel_interface * _kernel )
            {
                return type_down_cast_find<T &, T>::find( _kernel );
            }
        };

        template<class T>
        struct type_down_cast<const T &>
        {
            static type_cast * find( kernel_interface * _kernel )
            {
                return type_down_cast_find<const T &, T>::find( _kernel );
            }
        };
    }

    template<class T>
    bool instance_of( kernel_interface * _kernel, PyObject * _obj )
    {
        typeid_t id = _kernel->class_info<T>();

        bool result = _kernel->instance_of_type( _obj, id );

        return result;
    }

    template<class T>
    void registration_type_cast( kernel_interface * _kernel, const type_cast_ptr & _type )
    {
        typeid_t id = _kernel->class_info<T>();

        _kernel->register_type_info_extract( id, _type );
    }

    template<class T>
    void unregistration_type_cast( kernel_interface * _kernel )
    {
        typeid_t id = _kernel->class_info<T>();

        _kernel->unregister_type_info_extract( id );
    }

    template<class T>
    class type_cast_result
        : public type_cast
    {
    public:
        typedef T value_type;

        typedef value_type & TCastValue;
        typedef const value_type & TCastRef;

    public:
        virtual bool apply( kernel_interface * _kernel, PyObject * _obj, TCastValue _value, bool _nothrow ) = 0;
        virtual PyObject * wrap( kernel_interface * _kernel, TCastRef t ) = 0;
    };
}