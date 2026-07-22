#include "tinypy_class_type_scope.hpp"

#include "tinypy_kernel.hpp"

#include "pybind/exception.hpp"

#include <cassert>
#include <cstring>

namespace pybind
{
    //////////////////////////////////////////////////////////////////////////
    namespace detail
    {
        struct method_holder_t
        {
            allocator_interface * allocator;
            tinypy_class_type_scope * scope;
            method_adapter_interface_ptr adapter;
            member_adapter_interface_ptr member;
            bool constructor;
            bool setter;
        };
        //////////////////////////////////////////////////////////////////////////
        static PyObject * cast_object( tinypy_value_t * _value )
        {
            return reinterpret_cast<PyObject *>(_value);
        }
        //////////////////////////////////////////////////////////////////////////
        static tinypy_value_t * cast_value( PyObject * _value )
        {
            return reinterpret_cast<tinypy_value_t *>(_value);
        }
        //////////////////////////////////////////////////////////////////////////
        static tinypy_value_t * tail_arguments( tinypy_kernel * _kernel, tinypy_value_t * _args )
        {
            size_t count = tinypy_tuple_size( _args );
            tinypy_vm_t * vm = _kernel->vm();

            if( count <= 1 )
            {
                return tinypy_tuple_new( vm, 0 );
            }

            tinypy_value_t * result = tinypy_tuple_new( vm, count - 1 );

            for( size_t index = 1; index != count; ++index )
            {
                tinypy_value_t * item = tinypy_tuple_get( _args, index );
                tinypy_tuple_set( result, index - 1, item );
            }

            return result;
        }
        //////////////////////////////////////////////////////////////////////////
        static void finalize_method_holder( void * _userData )
        {
            method_holder_t * holder = static_cast<method_holder_t *>(_userData);
            holder->allocator->deleteT( holder );
        }
        //////////////////////////////////////////////////////////////////////////
        static tinypy_value_t * call_method_holder( tinypy_value_t * _function, tinypy_value_t * _args, tinypy_value_t * _kwargs, void * _userData, tinypy_error_t ** _outError )
        {
            (void)_function;

            method_holder_t * holder = static_cast<method_holder_t *>(_userData);
            tinypy_class_type_scope * scope = holder->scope;
            kernel_interface * kernelInterface = pybind::get_kernel();
            tinypy_kernel * kernel = static_cast<tinypy_kernel *>(kernelInterface);
            tinypy_vm_t * vm = kernel->vm();
            size_t argumentCount = tinypy_tuple_size( _args );
            PyObject * kwargsObject = detail::cast_object( _kwargs );

            try
            {
                if( argumentCount == 0 )
                {
                    tinypy_vm_raise_error( vm, TINYPY_ERROR_TYPE, "native descriptor requires an instance" );
                    return nullptr;
                }

                tinypy_value_t * selfValue = tinypy_tuple_get( _args, 0 );
                PyObject * self = detail::cast_object( selfValue );
                tinypy_class_type_scope::payload_t * payload = scope->payload( self );

                if( holder->constructor == true )
                {
                    payload->scope = scope;
                    payload->flags = 0;
                    payload->hash = 0;

                    if( scope->get_pod_size() != 0 )
                    {
                        payload->impl = payload->pod;
                        payload->flags |= tinypy_class_type_scope::PayloadPod;
                    }

                    tinypy_value_t * callArgs = detail::tail_arguments( kernel, _args );
                    PyObject * callArgsObject = detail::cast_object( callArgs );
                    void * impl = scope->call_new( self, callArgsObject, kwargsObject );
                    tinypy_release( callArgs );

                    if( impl == nullptr )
                    {
                        return nullptr;
                    }

                    payload->scope = scope;
                    payload->impl = impl;
                    payload->flags |= tinypy_class_type_scope::PayloadConstructed;
                    scope->incref_smart_pointer( impl );

                    return tinypy_none_get( vm );
                }

                if( payload->impl == nullptr )
                {
                    tinypy_vm_raise_error( vm, TINYPY_ERROR_RUNTIME, "native object is no longer bound" );
                    return nullptr;
                }

                class_type_scope_interface_ptr scopePtr = class_type_scope_interface_ptr::from( payload->scope );

                if( holder->member != nullptr )
                {
                    if( holder->setter == false )
                    {
                        PyObject * result = holder->member->get( kernel, payload->impl, scopePtr );
                        return detail::cast_value( result );
                    }

                    if( argumentCount != 2 )
                    {
                        tinypy_vm_raise_error( vm, TINYPY_ERROR_TYPE, "native property setter expects one value" );
                        return nullptr;
                    }

                    tinypy_value_t * item = tinypy_tuple_get( _args, 1 );
                    PyObject * value = detail::cast_object( item );
                    holder->member->set( kernel, payload->impl, value, scopePtr );
                    return tinypy_none_get( vm );
                }

                tinypy_value_t * callArgs = detail::tail_arguments( kernel, _args );
                PyObject * callArgsObject = detail::cast_object( callArgs );
                PyObject * result = holder->adapter->call( kernel, payload->impl, scopePtr, callArgsObject, kwargsObject );
                tinypy_release( callArgs );
                return detail::cast_value( result );
            }
            catch( const pybind_exception & exception )
            {
                const char * message = exception.what();
                tinypy_vm_raise_error( vm, TINYPY_ERROR_RUNTIME, message );
                if( _outError != nullptr )
                {
                    *_outError = nullptr;
                }

                return nullptr;
            }
        }
        //////////////////////////////////////////////////////////////////////////
        static tinypy_value_t * make_method( tinypy_class_type_scope * _scope, const char * _name, const method_adapter_interface_ptr & _adapter, const member_adapter_interface_ptr & _member, bool _constructor, bool _setter )
        {
            tinypy_kernel * kernel = static_cast<tinypy_kernel *>(pybind::get_kernel());
            method_holder_t * holder = kernel->get_allocator()->newP<method_holder_t>();
            holder->allocator = kernel->get_allocator();
            holder->scope = _scope;
            holder->adapter = _adapter;
            holder->member = _member;
            holder->constructor = _constructor;
            holder->setter = _setter;

            tinypy_vm_t * vm = kernel->vm();
            size_t nameSize = ::strlen( _name );
            return tinypy_native_function_new( vm, _name, nameSize, &call_method_holder, holder, &finalize_method_holder );
        }
        //////////////////////////////////////////////////////////////////////////
        static int32_t native_construct( tinypy_value_t * _instance, void * _payload, tinypy_value_t * _args, tinypy_value_t * _kwargs, void * _userData, tinypy_error_t ** _outError )
        {
            tinypy_class_type_scope * scope = static_cast<tinypy_class_type_scope *>(_userData);
            kernel_interface * kernelInterface = pybind::get_kernel();
            tinypy_kernel * kernel = static_cast<tinypy_kernel *>(kernelInterface);
            tinypy_vm_t * vm = kernel->vm();
            tinypy_class_type_scope::payload_t * payload = static_cast<tinypy_class_type_scope::payload_t *>(_payload);

            try
            {
                payload->scope = scope;
                payload->flags = 0;
                payload->hash = 0;

                if( scope->get_pod_size() != 0 )
                {
                    payload->impl = payload->pod;
                    payload->flags |= tinypy_class_type_scope::PayloadPod;
                }

                PyObject * instance = detail::cast_object( _instance );
                PyObject * args = detail::cast_object( _args );
                PyObject * kwargs = detail::cast_object( _kwargs );
                void * impl = scope->call_new( instance, args, kwargs );

                if( impl == nullptr )
                {
                    return 0;
                }

                payload->impl = impl;
                payload->flags |= tinypy_class_type_scope::PayloadConstructed;
                scope->incref_smart_pointer( impl );
                return 1;
            }
            catch( const pybind_exception & exception )
            {
                const char * message = exception.what();
                tinypy_vm_raise_error( vm, TINYPY_ERROR_RUNTIME, message );
                if( _outError != nullptr )
                {
                    *_outError = nullptr;
                }

                return 0;
            }
        }
        //////////////////////////////////////////////////////////////////////////
        static void native_finalize( tinypy_value_t * _instance, void * _payload, void * _userData )
        {
            (void)_userData;

            tinypy_class_type_scope::payload_t * payload = static_cast<tinypy_class_type_scope::payload_t *>(_payload);
        //////////////////////////////////////////////////////////////////////////
            if( payload->scope == nullptr || payload->impl == nullptr || ( payload->flags & tinypy_class_type_scope::PayloadConstructed ) == 0 || ( payload->flags & tinypy_class_type_scope::PayloadWeak ) != 0 || ( payload->flags & tinypy_class_type_scope::PayloadUnwrapped ) != 0 )
            {
                return;
            }

            try
            {
                PyObject * instance = detail::cast_object( _instance );
                payload->scope->call_destructor( instance, payload->impl );
                payload->scope->clear_bindable( payload->impl );
                payload->scope->decref_smart_pointer( payload->impl );
            }
            catch( const pybind_exception & )
            {
            }

            payload->impl = nullptr;
        }
        //////////////////////////////////////////////////////////////////////////
        static tinypy_value_t * native_call( tinypy_value_t * _instance, void * _payload, tinypy_value_t * _args, tinypy_value_t * _kwargs, void * _userData, tinypy_error_t ** _outError )
        {
            (void)_instance;
            tinypy_class_type_scope * scope = static_cast<tinypy_class_type_scope *>(_userData);
            tinypy_class_type_scope::payload_t * payload = static_cast<tinypy_class_type_scope::payload_t *>(_payload);
            kernel_interface * kernelInterface = pybind::get_kernel();
            tinypy_kernel * kernel = static_cast<tinypy_kernel *>(kernelInterface);
            tinypy_vm_t * vm = kernel->vm();

            try
            {
                const method_adapter_interface_ptr & adapter = scope->get_call();
                class_type_scope_interface_ptr scopePtr = class_type_scope_interface_ptr::from( scope );
                PyObject * args = detail::cast_object( _args );
                PyObject * kwargs = detail::cast_object( _kwargs );
                PyObject * result = adapter->call( kernel, payload->impl, scopePtr, args, kwargs );

                return detail::cast_value( result );
            }
            catch( const pybind_exception & exception )
            {
                const char * message = exception.what();
                tinypy_vm_raise_error( vm, TINYPY_ERROR_RUNTIME, message );
                if( _outError != nullptr )
                {
                    *_outError = nullptr;
                }

                return nullptr;
            }
        }
        //////////////////////////////////////////////////////////////////////////
        static tinypy_value_t * native_repr( tinypy_value_t * _instance, void * _payload, void * _userData, tinypy_error_t ** _outError )
        {
            (void)_instance;
            tinypy_class_type_scope * scope = static_cast<tinypy_class_type_scope *>(_userData);
            tinypy_class_type_scope::payload_t * payload = static_cast<tinypy_class_type_scope::payload_t *>(_payload);
            kernel_interface * kernelInterface = pybind::get_kernel();
            tinypy_kernel * kernel = static_cast<tinypy_kernel *>(kernelInterface);
            tinypy_vm_t * vm = kernel->vm();

            try
            {
                const repr_adapter_interface_ptr & adapter = scope->get_repr();
                class_type_scope_interface_ptr scopePtr = class_type_scope_interface_ptr::from( scope );
                PyObject * result = adapter->repr( kernel, payload->impl, scopePtr );

                return detail::cast_value( result );
            }
            catch( const pybind_exception & exception )
            {
                const char * message = exception.what();
                tinypy_vm_raise_error( vm, TINYPY_ERROR_RUNTIME, message );
                if( _outError != nullptr )
                {
                    *_outError = nullptr;
                }

                return nullptr;
            }
        }
        //////////////////////////////////////////////////////////////////////////
        static tinypy_hash_t native_hash( tinypy_value_t * _instance, void * _payload, void * _userData, tinypy_error_t ** _outError )
        {
            (void)_instance;
            (void)_outError;
            tinypy_class_type_scope * scope = static_cast<tinypy_class_type_scope *>(_userData);
            tinypy_class_type_scope::payload_t * payload = static_cast<tinypy_class_type_scope::payload_t *>(_payload);
        //////////////////////////////////////////////////////////////////////////
            if( ( payload->flags & tinypy_class_type_scope::PayloadHashValid ) != 0 )
            {
                return payload->hash;
            }

            const hash_adapter_interface_ptr & adapter = scope->get_hash();
            kernel_interface * kernel = pybind::get_kernel();
            class_type_scope_interface_ptr scopePtr = class_type_scope_interface_ptr::from( scope );
            int64_t hash = adapter->hash( kernel, payload->impl, scopePtr );
            payload->hash = hash;
            payload->flags |= tinypy_class_type_scope::PayloadHashValid;
            return hash;
        }
        //////////////////////////////////////////////////////////////////////////
        static PybindOperatorCompare convert_compare( tinypy_compare_operation_e _operation )
        {
            switch( _operation )
            {
            case TINYPY_COMPARE_LESS:
                return POC_Less;
            case TINYPY_COMPARE_LESS_EQUAL:
                return POC_Lessequal;
            case TINYPY_COMPARE_NOT_EQUAL:
                return POC_Notequal;
            case TINYPY_COMPARE_GREATER:
                return POC_Great;
            case TINYPY_COMPARE_GREATER_EQUAL:
                return POC_Greatequal;
            default:
                return POC_Equal;
            }
        }
        //////////////////////////////////////////////////////////////////////////
        static tinypy_value_t * native_compare( tinypy_value_t * _instance, void * _payload, tinypy_value_t * _other, tinypy_compare_operation_e _operation, void * _userData, tinypy_error_t ** _outError )
        {
            tinypy_class_type_scope * scope = static_cast<tinypy_class_type_scope *>(_userData);
            tinypy_class_type_scope::payload_t * payload = static_cast<tinypy_class_type_scope::payload_t *>(_payload);
            kernel_interface * kernelInterface = pybind::get_kernel();
            tinypy_kernel * kernel = static_cast<tinypy_kernel *>(kernelInterface);
            tinypy_vm_t * vm = kernel->vm();
            PyObject * other = detail::cast_object( _other );
            typeid_t typeId = kernel->get_object_type_id( other );
            const compare_adapter_interface_ptr & adapter = scope->get_compare( typeId );

            if( adapter == nullptr )
            {
                return tinypy_not_implemented_get( vm );
            }

            bool result = false;

            try
            {
                PyObject * instance = detail::cast_object( _instance );
                class_type_scope_interface_ptr scopePtr = class_type_scope_interface_ptr::from( scope );
                PybindOperatorCompare operation = detail::convert_compare( _operation );

                if( adapter->compare( kernel, instance, payload->impl, scopePtr, other, operation, &result ) == false )
                {
                    return tinypy_not_implemented_get( vm );
                }

                int32_t value = result == true ? 1 : 0;
                return tinypy_bool_from_i32( vm, value );
            }
            catch( const pybind_exception & exception )
            {
                const char * message = exception.what();
                tinypy_vm_raise_error( vm, TINYPY_ERROR_RUNTIME, message );
                if( _outError != nullptr )
                {
                    *_outError = nullptr;
                }

                return nullptr;
            }
        }
        //////////////////////////////////////////////////////////////////////////
        static tinypy_value_t * native_get_attribute( tinypy_value_t * _instance, void * _payload, tinypy_value_t * _name, void * _userData, tinypy_error_t ** _outError )
        {
            (void)_instance;
            tinypy_class_type_scope * scope = static_cast<tinypy_class_type_scope *>(_userData);
            tinypy_class_type_scope::payload_t * payload = static_cast<tinypy_class_type_scope::payload_t *>(_payload);
            tinypy_kernel * kernel = static_cast<tinypy_kernel *>(pybind::get_kernel());
            tinypy_vm_t * vm = kernel->vm();

            try
            {
                const getattro_adapter_interface_ptr & adapter = scope->get_getattro();
                class_type_scope_interface_ptr scopePtr = class_type_scope_interface_ptr::from( scope );
                PyObject * name = detail::cast_object( _name );
                PyObject * result = adapter->call( kernel, payload->impl, scopePtr, name );

                return detail::cast_value( result );
            }
            catch( const pybind_exception & exception )
            {
                const char * message = exception.what();
                tinypy_vm_raise_error( vm, TINYPY_ERROR_RUNTIME, message );
                if( _outError != nullptr )
                {
                    *_outError = nullptr;
                }

                return nullptr;
            }
        }
        //////////////////////////////////////////////////////////////////////////
        static tinypy_value_t * native_mapping_get( tinypy_value_t * _instance, void * _payload, tinypy_value_t * _key, void * _userData, tinypy_error_t ** _outError )
        {
            (void)_instance;
            tinypy_class_type_scope * scope = static_cast<tinypy_class_type_scope *>(_userData);
            tinypy_class_type_scope::payload_t * payload = static_cast<tinypy_class_type_scope::payload_t *>(_payload);
            tinypy_kernel * kernel = static_cast<tinypy_kernel *>(pybind::get_kernel());
            tinypy_vm_t * vm = kernel->vm();

            try
            {
                const mapping_adapter_interface_ptr & adapter = scope->get_mapping();
                class_type_scope_interface_ptr scopePtr = class_type_scope_interface_ptr::from( scope );
                PyObject * key = detail::cast_object( _key );
                PyObject * result = adapter->call( kernel, payload->impl, scopePtr, key );

                return detail::cast_value( result );
            }
            catch( const pybind_exception & exception )
            {
                const char * message = exception.what();
                tinypy_vm_raise_error( vm, TINYPY_ERROR_RUNTIME, message );
                if( _outError != nullptr )
                {
                    *_outError = nullptr;
                }

                return nullptr;
            }
        }
        //////////////////////////////////////////////////////////////////////////
        static tinypy_value_t * native_sequence_get( tinypy_value_t * _instance, void * _payload, tinypy_value_t * _key, void * _userData, tinypy_error_t ** _outError )
        {
            (void)_instance;
            tinypy_class_type_scope * scope = static_cast<tinypy_class_type_scope *>(_userData);
            tinypy_class_type_scope::payload_t * payload = static_cast<tinypy_class_type_scope::payload_t *>(_payload);
            tinypy_kernel * kernel = static_cast<tinypy_kernel *>(pybind::get_kernel());
            tinypy_vm_t * vm = kernel->vm();
            int64_t index;

            if( kernel->extract_int64( detail::cast_object( _key ), index ) == false || index < 0 )
            {
                tinypy_vm_raise_error( kernel->vm(), TINYPY_ERROR_TYPE, "native sequence index must be a non-negative integer" );
                return nullptr;
            }

            try
            {
                return detail::cast_value( scope->get_sequence_get()->call( kernel, payload->impl, class_type_scope_interface_ptr::from( scope ), static_cast<size_t>(index) ) );
            }
            catch( const pybind_exception & exception )
            {
                const char * message = exception.what();
                tinypy_vm_raise_error( vm, TINYPY_ERROR_RUNTIME, message );
                if( _outError != nullptr )
                {
                    *_outError = nullptr;
                }

                return nullptr;
            }
        }
        //////////////////////////////////////////////////////////////////////////
        static int32_t native_sequence_set( tinypy_value_t * _instance, void * _payload, tinypy_value_t * _key, tinypy_value_t * _value, void * _userData, tinypy_error_t ** _outError )
        {
            (void)_instance;
            tinypy_class_type_scope * scope = static_cast<tinypy_class_type_scope *>(_userData);
            tinypy_class_type_scope::payload_t * payload = static_cast<tinypy_class_type_scope::payload_t *>(_payload);
            tinypy_kernel * kernel = static_cast<tinypy_kernel *>(pybind::get_kernel());
            tinypy_vm_t * vm = kernel->vm();
            int64_t index;

            if( kernel->extract_int64( detail::cast_object( _key ), index ) == false || index < 0 )
            {
                tinypy_vm_raise_error( kernel->vm(), TINYPY_ERROR_TYPE, "native sequence index must be a non-negative integer" );
                return 0;
            }

            try
            {
                scope->get_sequence_set()->call( kernel, payload->impl, class_type_scope_interface_ptr::from( scope ), static_cast<size_t>(index), detail::cast_object( _value ) );
                return 1;
            }
            catch( const pybind_exception & exception )
            {
                const char * message = exception.what();
                tinypy_vm_raise_error( vm, TINYPY_ERROR_RUNTIME, message );
                if( _outError != nullptr )
                {
                    *_outError = nullptr;
                }

                return 0;
            }
        }
        //////////////////////////////////////////////////////////////////////////
        static tinypy_value_t * native_unary( tinypy_value_t * _instance, void * _payload, void * _userData, tinypy_error_t ** _outError, bool _absolute )
        {
            (void)_instance;
            tinypy_class_type_scope * scope = static_cast<tinypy_class_type_scope *>(_userData);
            tinypy_class_type_scope::payload_t * payload = static_cast<tinypy_class_type_scope::payload_t *>(_payload);
            tinypy_kernel * kernel = static_cast<tinypy_kernel *>(pybind::get_kernel());
            tinypy_vm_t * vm = kernel->vm();
            const number_unary_adapter_interface_ptr & adapter = _absolute == true ? scope->get_number_abs() : scope->get_number_neg();

            try
            {
                class_type_scope_interface_ptr scopePtr = class_type_scope_interface_ptr::from( scope );
                PyObject * result = adapter->call( kernel, payload->impl, scopePtr );
                return detail::cast_value( result );
            }
            catch( const pybind_exception & exception )
            {
                const char * message = exception.what();
                tinypy_vm_raise_error( vm, TINYPY_ERROR_RUNTIME, message );
                if( _outError != nullptr )
                {
                    *_outError = nullptr;
                }

                return nullptr;
            }
        }
        //////////////////////////////////////////////////////////////////////////
        static tinypy_value_t * native_negative( tinypy_value_t * _instance, void * _payload, void * _userData, tinypy_error_t ** _outError )
        {
            return detail::native_unary( _instance, _payload, _userData, _outError, false );
        }
        //////////////////////////////////////////////////////////////////////////
        static tinypy_value_t * native_absolute( tinypy_value_t * _instance, void * _payload, void * _userData, tinypy_error_t ** _outError )
        {
            return detail::native_unary( _instance, _payload, _userData, _outError, true );
        }

        enum binary_operation_e
        {
            BinaryAdd,
            BinarySubtract,
            BinaryMultiply,
            BinaryDivide,
            BinaryInplaceAdd,
            BinaryInplaceSubtract,
            BinaryInplaceMultiply,
            BinaryInplaceDivide
        };
        //////////////////////////////////////////////////////////////////////////
        static tinypy_value_t * native_binary( tinypy_value_t * _instance, void * _payload, tinypy_value_t * _other, void * _userData, tinypy_error_t ** _outError, binary_operation_e _operation )
        {
            tinypy_class_type_scope * scope = static_cast<tinypy_class_type_scope *>(_userData);
            tinypy_class_type_scope::payload_t * payload = static_cast<tinypy_class_type_scope::payload_t *>(_payload);
            tinypy_kernel * kernel = static_cast<tinypy_kernel *>(pybind::get_kernel());
            tinypy_vm_t * vm = kernel->vm();
            typeid_t typeId = kernel->get_object_type_id( detail::cast_object( _other ) );

            try
            {
                const number_binary_adapter_interface_ptr * binary = nullptr;
                const number_inplace_adapter_interface_ptr * inplace = nullptr;

                switch( _operation )
                {
                case BinaryAdd:
                    binary = &scope->get_number_add( typeId );
                    break;
                case BinarySubtract:
                    binary = &scope->get_number_sub( typeId );
                    break;
                case BinaryMultiply:
                    binary = &scope->get_number_mul( typeId );
                    break;
                case BinaryDivide:
                    binary = &scope->get_number_div( typeId );
                    break;
                case BinaryInplaceAdd:
                    inplace = &scope->get_number_inplace_add( typeId );
                    break;
                case BinaryInplaceSubtract:
                    inplace = &scope->get_number_inplace_sub( typeId );
                    break;
                case BinaryInplaceMultiply:
                    inplace = &scope->get_number_inplace_mul( typeId );
                    break;
                case BinaryInplaceDivide:
                    inplace = &scope->get_number_inplace_div( typeId );
                    break;
                }

                if( binary != nullptr && *binary != nullptr )
                {
                    return detail::cast_value( ( *binary )->call( kernel, payload->impl, class_type_scope_interface_ptr::from( scope ), detail::cast_object( _other ), false ) );
                }

                if( inplace != nullptr && *inplace != nullptr )
                {
                    ( *inplace )->call( kernel, payload->impl, class_type_scope_interface_ptr::from( scope ), detail::cast_object( _other ) );
                    tinypy_retain( _instance );
                    return _instance;
                }

                return tinypy_not_implemented_get( vm );
            }
            catch( const pybind_exception & exception )
            {
                const char * message = exception.what();
                tinypy_vm_raise_error( vm, TINYPY_ERROR_RUNTIME, message );
                if( _outError != nullptr )
                {
                    *_outError = nullptr;
                }

                return nullptr;
            }
        }
        //////////////////////////////////////////////////////////////////////////
#define PYBIND_TINYPY_BINARY_CALLBACK(Name, Operation) \
        static tinypy_value_t * Name( tinypy_value_t * instance, void * payload, tinypy_value_t * other, void * userData, tinypy_error_t ** outError ) \
        { \
            return detail::native_binary( instance, payload, other, userData, outError, Operation ); \
    }
        //////////////////////////////////////////////////////////////////////////
        PYBIND_TINYPY_BINARY_CALLBACK( native_add, BinaryAdd )
        //////////////////////////////////////////////////////////////////////////
        PYBIND_TINYPY_BINARY_CALLBACK( native_subtract, BinarySubtract )
        //////////////////////////////////////////////////////////////////////////
        PYBIND_TINYPY_BINARY_CALLBACK( native_multiply, BinaryMultiply )
        //////////////////////////////////////////////////////////////////////////
        PYBIND_TINYPY_BINARY_CALLBACK( native_divide, BinaryDivide )
        //////////////////////////////////////////////////////////////////////////
        PYBIND_TINYPY_BINARY_CALLBACK( native_inplace_add, BinaryInplaceAdd )
        //////////////////////////////////////////////////////////////////////////
        PYBIND_TINYPY_BINARY_CALLBACK( native_inplace_subtract, BinaryInplaceSubtract )
        //////////////////////////////////////////////////////////////////////////
        PYBIND_TINYPY_BINARY_CALLBACK( native_inplace_multiply, BinaryInplaceMultiply )
        //////////////////////////////////////////////////////////////////////////
        PYBIND_TINYPY_BINARY_CALLBACK( native_inplace_divide, BinaryInplaceDivide )
        //////////////////////////////////////////////////////////////////////////
#undef PYBIND_TINYPY_BINARY_CALLBACK
        //////////////////////////////////////////////////////////////////////////
    }
    //////////////////////////////////////////////////////////////////////////
    tinypy_class_type_scope::tinypy_class_type_scope( tinypy_kernel * _kernel, const char * _name, typeid_t _typeId, void * _user, const new_adapter_interface_ptr & _pynew, const destroy_adapter_interface_ptr & _pydestructor, size_t _pod, bool _hash )
        : m_kernel( _kernel )
        , m_typeId( _typeId )
        , m_user( _user )
        , m_pod_size( _pod )
        , m_pod_hash( _hash )
        , m_module( nullptr )
        , m_type( nullptr )
        , m_basesCount( 0 )
        , m_methodCount( 0 )
        , m_memberCount( 0 )
        , m_new( _pynew )
        , m_destructor( _pydestructor )
        , m_compare_adapters( nullptr )
        , m_number_adapters( nullptr )
    {
        ::strncpy( m_name, _name, PYBIND_CLASS_TYPE_MAX_NAME );
        m_name[PYBIND_CLASS_TYPE_MAX_NAME] = '\0';

        if( m_pod_size > sizeof( static_cast<payload_t *>(nullptr)->pod ) )
        {
            pybind::throw_exception( "tinypy native POD '%s' is too large: %zu", _name, _pod );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    tinypy_class_type_scope::~tinypy_class_type_scope()
    {
        allocator_interface * allocator = m_kernel->get_allocator();

        if( m_compare_adapters != nullptr )
        {
            allocator->deleteT( m_compare_adapters );
        }

        if( m_number_adapters != nullptr )
        {
            m_kernel->get_allocator()->deleteT( m_number_adapters );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    bool tinypy_class_type_scope::initialize( PyObject * _module )
    {
        PyObject * moduleObject = _module != nullptr ? _module : m_kernel->get_current_module();

        if( moduleObject == nullptr )
        {
            pybind::throw_exception( "scope '%s' initialize not setup python module", m_name );

            return false;
        }

        m_module = moduleObject;
        tinypy_vm_t * vm = m_kernel->vm();
        tinypy_value_t * namespaceDict = tinypy_dict_new( vm );

        tinypy_value_t * initializer = detail::make_method( this, "__init__", nullptr, nullptr, true, false );
        tinypy_value_t * initializerKey = tinypy_string_from_bytes( vm, "__init__", 8 );
        tinypy_dict_set( namespaceDict, initializerKey, initializer );
        tinypy_release( initializerKey );
        tinypy_release( initializer );

        for( size_t index = 0; index != m_methodCount; ++index )
        {
            const method_adapter_interface_ptr & method = m_methods[index];
            const char * methodName = method->getName();
            size_t methodNameSize = ::strlen( methodName );
            tinypy_value_t * function = detail::make_method( this, methodName, method, nullptr, false, false );
            tinypy_value_t * key = tinypy_string_from_bytes( vm, methodName, methodNameSize );
            tinypy_dict_set( namespaceDict, key, function );
            tinypy_release( key );
            tinypy_release( function );
        }

        for( size_t index = 0; index != m_memberCount; ++index )
        {
            const member_adapter_interface_ptr & member = m_members[index];
            const char * memberName = member->getName();
            size_t memberNameSize = ::strlen( memberName );
            tinypy_value_t * getter = detail::make_method( this, memberName, nullptr, member, false, false );
            tinypy_value_t * setter = detail::make_method( this, memberName, nullptr, member, false, true );
            tinypy_value_t * property = tinypy_property_new( vm, getter, setter, nullptr, nullptr );
            tinypy_value_t * key = tinypy_string_from_bytes( vm, memberName, memberNameSize );
            tinypy_dict_set( namespaceDict, key, property );
            tinypy_release( key );
            tinypy_release( property );
            tinypy_release( setter );
            tinypy_release( getter );
        }

        const tinypy_type_t * bases[PYBIND_BASES_COUNT];
        size_t baseCount = 0;

        for( size_t index = 0; index != m_basesCount; ++index )
        {
            const base_t & base = m_bases[index];
            bases[baseCount++] = reinterpret_cast<const tinypy_type_t *>(base.scope->get_typeobject());

            const smart_pointer_adapter_interface_ptr & baseSmartPointer = base.scope->get_smart_pointer();

            if( m_smart_pointer == nullptr && baseSmartPointer != nullptr )
            {
                m_smart_pointer = baseSmartPointer;
            }

            const bindable_adapter_interface_ptr & baseBindable = base.scope->get_bindable();

            if( m_bindable == nullptr && baseBindable != nullptr )
            {
                m_bindable = baseBindable;
            }
        }

        tinypy_native_type_spec_t spec;
        tinypy_native_type_spec_init( &spec );
        spec.payload_size = sizeof( payload_t );
        spec.payload_alignment = alignof( payload_t );
        spec.user_data = this;
        spec.construct = &detail::native_construct;
        spec.finalize = &detail::native_finalize;
        spec.call = m_call != nullptr ? &detail::native_call : nullptr;
        spec.repr = m_repr != nullptr ? &detail::native_repr : nullptr;
        spec.hash = m_hash != nullptr ? &detail::native_hash : nullptr;
        spec.compare = m_compare_adapters != nullptr ? &detail::native_compare : nullptr;
        spec.get_attribute = m_getattro != nullptr ? &detail::native_get_attribute : nullptr;
        spec.mapping_get = m_mapping != nullptr ? &detail::native_mapping_get : nullptr;
        spec.sequence_get = m_sequence_get != nullptr ? &detail::native_sequence_get : nullptr;
        spec.sequence_set = m_sequence_set != nullptr ? &detail::native_sequence_set : nullptr;
        spec.negative = m_number_neg != nullptr ? &detail::native_negative : nullptr;
        spec.absolute = m_number_abs != nullptr ? &detail::native_absolute : nullptr;
        spec.add = m_number_adapters != nullptr ? &detail::native_add : nullptr;
        spec.subtract = m_number_adapters != nullptr ? &detail::native_subtract : nullptr;
        spec.multiply = m_number_adapters != nullptr ? &detail::native_multiply : nullptr;
        spec.divide = m_number_adapters != nullptr ? &detail::native_divide : nullptr;
        spec.inplace_add = m_number_adapters != nullptr ? &detail::native_inplace_add : nullptr;
        spec.inplace_subtract = m_number_adapters != nullptr ? &detail::native_inplace_subtract : nullptr;
        spec.inplace_multiply = m_number_adapters != nullptr ? &detail::native_inplace_multiply : nullptr;
        spec.inplace_divide = m_number_adapters != nullptr ? &detail::native_inplace_divide : nullptr;

        tinypy_error_t * error = nullptr;
        size_t nameSize = ::strlen( m_name );
        const tinypy_type_t * const * typeBases = baseCount == 0 ? nullptr : bases;
        m_type = tinypy_native_type_new( vm, m_name, nameSize, typeBases, baseCount, namespaceDict, &spec, &error );
        tinypy_release( namespaceDict );

        if( m_type == nullptr )
        {
            if( error != nullptr )
            {
                tinypy_error_release( error );
            }

            return false;
        }

        tinypy_value_t * module = detail::cast_value( moduleObject );
        tinypy_value_t * typeValue = tinypy_type_as_value( m_type );
        tinypy_module_add_value( module, m_name, nameSize, typeValue );

        class_type_scope_interface_ptr scope = class_type_scope_interface_ptr::from( this );
        m_kernel->cache_class_scope_type( scope );
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void tinypy_class_type_scope::finalize()
    {
        if( m_type != nullptr )
        {
            class_type_scope_interface_ptr scope = class_type_scope_interface_ptr::from( this );
            m_kernel->remove_class_scope_type( scope );

            tinypy_value_t * typeValue = tinypy_type_as_value( m_type );
            tinypy_release( typeValue );
            m_type = nullptr;
        }

        m_module = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    kernel_interface * tinypy_class_type_scope::get_kernel() const
    {
        return m_kernel;
    }
    //////////////////////////////////////////////////////////////////////////
    const char * tinypy_class_type_scope::get_name() const
    {
        return m_name;
    }
    //////////////////////////////////////////////////////////////////////////
    typeid_t tinypy_class_type_scope::get_type_id() const
    {
        return m_typeId;
    }
    //////////////////////////////////////////////////////////////////////////
    size_t tinypy_class_type_scope::get_pod_size() const
    {
        return m_pod_size;
    }
    //////////////////////////////////////////////////////////////////////////
    bool tinypy_class_type_scope::get_pod_hash() const
    {
        return m_pod_hash;
    }
    //////////////////////////////////////////////////////////////////////////
    void * tinypy_class_type_scope::get_user() const
    {
        return m_user;
    }
    //////////////////////////////////////////////////////////////////////////
    PyTypeObject * tinypy_class_type_scope::get_typeobject() const
    {
        return reinterpret_cast<PyTypeObject *>(m_type);
    }
    //////////////////////////////////////////////////////////////////////////
    void tinypy_class_type_scope::add_method( const method_adapter_interface_ptr & _imethod )
    {
        assert( m_methodCount != PYBIND_METHOD_COUNT );
        m_methods[m_methodCount++] = _imethod;

        if( m_type != nullptr )
        {
            const char * name = _imethod->getName();
            size_t nameSize = ::strlen( name );
            tinypy_value_t * function = detail::make_method( this, name, _imethod, nullptr, false, false );
            tinypy_type_set_attr( m_type, name, nameSize, function );
            tinypy_release( function );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void tinypy_class_type_scope::add_member( const member_adapter_interface_ptr & _imember )
    {
        assert( m_memberCount != PYBIND_MEMBER_COUNT );
        m_members[m_memberCount++] = _imember;

        if( m_type != nullptr )
        {
            const char * name = _imember->getName();
            size_t nameSize = ::strlen( name );
            tinypy_vm_t * vm = m_kernel->vm();
            tinypy_value_t * getter = detail::make_method( this, name, nullptr, _imember, false, false );
            tinypy_value_t * setter = detail::make_method( this, name, nullptr, _imember, false, true );
            tinypy_value_t * property = tinypy_property_new( vm, getter, setter, nullptr, nullptr );
            tinypy_type_set_attr( m_type, name, nameSize, property );
            tinypy_release( property );
            tinypy_release( setter );
            tinypy_release( getter );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    size_t tinypy_class_type_scope::get_methods_count() const
    {
        return m_methodCount;
    }
    //////////////////////////////////////////////////////////////////////////
    size_t tinypy_class_type_scope::get_members_count() const
    {
        return m_memberCount;
    }
    //////////////////////////////////////////////////////////////////////////
    const method_adapter_interface_ptr & tinypy_class_type_scope::get_method( size_t _index ) const
    {
        return m_methods[_index];
    }
    //////////////////////////////////////////////////////////////////////////
    const member_adapter_interface_ptr & tinypy_class_type_scope::get_member( size_t _index ) const
    {
        return m_members[_index];
    }
    //////////////////////////////////////////////////////////////////////////
    const method_adapter_interface_ptr & tinypy_class_type_scope::find_method( const char * _name ) const
    {
        for( size_t index = 0; index != m_methodCount; ++index )
        {
            if( std::strcmp( m_methods[index]->getName(), _name ) == 0 )
            {
                return m_methods[index];
            }
        }

        static method_adapter_interface_ptr none;
        return none;
    }
    //////////////////////////////////////////////////////////////////////////
    const member_adapter_interface_ptr & tinypy_class_type_scope::find_member( const char * _name ) const
    {
        for( size_t index = 0; index != m_memberCount; ++index )
        {
            if( std::strcmp( m_members[index]->getName(), _name ) == 0 )
            {
                return m_members[index];
            }
        }

        static member_adapter_interface_ptr none;
        return none;
    }
    //////////////////////////////////////////////////////////////////////////
    void tinypy_class_type_scope::add_base( typeid_t _info, const class_type_scope_interface_ptr & _scope, pybind_metacast _cast )
    {
        assert( m_basesCount != PYBIND_BASES_COUNT );
        m_bases[m_basesCount++] = base_t{ _info, _scope, _cast };
    }
    //////////////////////////////////////////////////////////////////////////
    void tinypy_class_type_scope::set_construct( const constructor_adapter_interface_ptr & _iconstruct )
    {
        m_constructor = _iconstruct;
    }
    //////////////////////////////////////////////////////////////////////////
    void tinypy_class_type_scope::set_convert( const convert_adapter_interface_ptr & _iconvert )
    {
        m_convert = _iconvert;
    }
    //////////////////////////////////////////////////////////////////////////
    void tinypy_class_type_scope::set_call( const method_adapter_interface_ptr & _icall )
    {
        m_call = _icall;
    }
    //////////////////////////////////////////////////////////////////////////
    void tinypy_class_type_scope::set_repr( const repr_adapter_interface_ptr & _irepr )
    {
        m_repr = _irepr;
    }
    //////////////////////////////////////////////////////////////////////////
    void tinypy_class_type_scope::set_hash( const hash_adapter_interface_ptr & _ihash )
    {
        m_hash = _ihash;
    }
    //////////////////////////////////////////////////////////////////////////
    void tinypy_class_type_scope::set_compare( typeid_t _typeId, const compare_adapter_interface_ptr & _iadapter )
    {
        assert( _typeId < PYBIND_TYPE_COUNT );
        this->get_compare_adapters_()->compares[_typeId] = _iadapter;
    }
    //////////////////////////////////////////////////////////////////////////
    void tinypy_class_type_scope::set_getattro( const getattro_adapter_interface_ptr & _igetattro )
    {
        m_getattro = _igetattro;
    }
    //////////////////////////////////////////////////////////////////////////
    void tinypy_class_type_scope::set_mapping( const mapping_adapter_interface_ptr & _imapping )
    {
        m_mapping = _imapping;
    }
    //////////////////////////////////////////////////////////////////////////
    void tinypy_class_type_scope::set_sequence_get( const sequence_get_adapter_interface_ptr & _isequence )
    {
        m_sequence_get = _isequence;
    }
    //////////////////////////////////////////////////////////////////////////
    void tinypy_class_type_scope::set_sequence_set( const sequence_set_adapter_interface_ptr & _isequence )
    {
        m_sequence_set = _isequence;
    }
    //////////////////////////////////////////////////////////////////////////
    void tinypy_class_type_scope::set_number_neg( const number_unary_adapter_interface_ptr & _iadapter )
    {
        m_number_neg = _iadapter;
    }
    //////////////////////////////////////////////////////////////////////////
    void tinypy_class_type_scope::set_number_abs( const number_unary_adapter_interface_ptr & _iadapter )
    {
        m_number_abs = _iadapter;
    }
    //////////////////////////////////////////////////////////////////////////
    void tinypy_class_type_scope::set_number_add( typeid_t _typeId, const number_binary_adapter_interface_ptr & _iadapter )
    {
        assert( _typeId < PYBIND_TYPE_COUNT );
        this->get_number_adapters_()->adds[_typeId] = _iadapter;
    }
    //////////////////////////////////////////////////////////////////////////
    void tinypy_class_type_scope::set_number_sub( typeid_t _typeId, const number_binary_adapter_interface_ptr & _iadapter )
    {
        assert( _typeId < PYBIND_TYPE_COUNT );
        this->get_number_adapters_()->subs[_typeId] = _iadapter;
    }
    //////////////////////////////////////////////////////////////////////////
    void tinypy_class_type_scope::set_number_mul( typeid_t _typeId, const number_binary_adapter_interface_ptr & _iadapter )
    {
        assert( _typeId < PYBIND_TYPE_COUNT );
        this->get_number_adapters_()->muls[_typeId] = _iadapter;
    }
    //////////////////////////////////////////////////////////////////////////
    void tinypy_class_type_scope::set_number_div( typeid_t _typeId, const number_binary_adapter_interface_ptr & _iadapter )
    {
        assert( _typeId < PYBIND_TYPE_COUNT );
        this->get_number_adapters_()->divs[_typeId] = _iadapter;
    }
    //////////////////////////////////////////////////////////////////////////
    void tinypy_class_type_scope::set_number_inplace_add( typeid_t _typeId, const number_inplace_adapter_interface_ptr & _iadapter )
    {
        assert( _typeId < PYBIND_TYPE_COUNT );
        this->get_number_adapters_()->iadds[_typeId] = _iadapter;
    }
    //////////////////////////////////////////////////////////////////////////
    void tinypy_class_type_scope::set_number_inplace_sub( typeid_t _typeId, const number_inplace_adapter_interface_ptr & _iadapter )
    {
        assert( _typeId < PYBIND_TYPE_COUNT );
        this->get_number_adapters_()->isubs[_typeId] = _iadapter;
    }
    //////////////////////////////////////////////////////////////////////////
    void tinypy_class_type_scope::set_number_inplace_mul( typeid_t _typeId, const number_inplace_adapter_interface_ptr & _iadapter )
    {
        assert( _typeId < PYBIND_TYPE_COUNT );
        this->get_number_adapters_()->imuls[_typeId] = _iadapter;
    }
    //////////////////////////////////////////////////////////////////////////
    void tinypy_class_type_scope::set_number_inplace_div( typeid_t _typeId, const number_inplace_adapter_interface_ptr & _iadapter )
    {
        assert( _typeId < PYBIND_TYPE_COUNT );
        this->get_number_adapters_()->idivs[_typeId] = _iadapter;
    }
    //////////////////////////////////////////////////////////////////////////
    void tinypy_class_type_scope::set_smart_pointer( const smart_pointer_adapter_interface_ptr & _iadapter )
    {
        m_smart_pointer = _iadapter;
    }
    //////////////////////////////////////////////////////////////////////////
    void tinypy_class_type_scope::set_bindable( const bindable_adapter_interface_ptr & _iadapter )
    {
        m_bindable = _iadapter;
    }
    //////////////////////////////////////////////////////////////////////////
    void tinypy_class_type_scope::add_proxy_interface( const class_type_scope_interface_ptr & _scope, const proxy_adapter_interface_ptr & _iproxy )
    {
        allocator_interface * allocator = m_kernel->get_allocator();
        size_t methodCount = _scope->get_methods_count();

        for( size_t index = 0; index != methodCount; ++index )
        {
            const method_adapter_interface_ptr & method = _scope->get_method( index );
            const char * methodName = method->getName();
            method_adapter_interface_ptr adapter = allocator->newT<proxy_method_adapter>( methodName, _iproxy, method, _scope );

            this->add_method( adapter );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * tinypy_class_type_scope::create_object_( void * _impl, uint32_t _flags )
    {
        tinypy_value_t * object = tinypy_native_instance_new( m_type );
        payload_t * value = static_cast<payload_t *>(tinypy_native_instance_payload( object));
        value->scope = this;
        value->impl = ( _flags & PayloadPod ) != 0 ? value->pod : _impl;
        value->flags = _flags | PayloadConstructed;
        value->hash = 0;

        if( ( _flags & ( PayloadWeak | PayloadPod ) ) == 0 )
        {
            assert( _impl != nullptr );

            this->incref_smart_pointer( _impl );
        }

        return detail::cast_object( object );
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * tinypy_class_type_scope::create_class( void * _impl )
    {
        return this->create_object_( _impl, 0 );
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * tinypy_class_type_scope::create_holder( void * _impl )
    {
        return this->create_object_( _impl, PayloadHolder );
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * tinypy_class_type_scope::create_weak( void * _impl )
    {
        return this->create_object_( _impl, PayloadWeak );
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * tinypy_class_type_scope::create_pod( void ** _impl )
    {
        PyObject * object = this->create_object_( nullptr, PayloadPod );
        payload_t * value = this->payload( object );
        *_impl = value->impl;
        return object;
    }
    //////////////////////////////////////////////////////////////////////////
    void * tinypy_class_type_scope::meta_cast( typeid_t _typeId, void * _impl )
    {
        if( _typeId == m_typeId )
        {
            return _impl;
        }

        for( size_t index = 0; index != m_basesCount; ++index )
        {
            const base_t & base = m_bases[index];

            if( base.info == _typeId )
            {
                return base.cast( _impl );
            }
        }

        for( size_t index = 0; index != m_basesCount; ++index )
        {
            const base_t & base = m_bases[index];
            void * baseImpl = base.cast( _impl );
            void * castImpl = base.scope->meta_cast( _typeId, baseImpl );

            if( castImpl != nullptr )
            {
                return castImpl;
            }
        }

        return nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    void tinypy_class_type_scope::type_initialize( PyTypeObject * _type )
    {
        (void)_type;
    }
    //////////////////////////////////////////////////////////////////////////
    bool tinypy_class_type_scope::is_instance( PyTypeObject * _type ) const
    {
        return tinypy_type_is_subtype( reinterpret_cast<const tinypy_type_t *>(_type), m_type ) != 0;
    }
    //////////////////////////////////////////////////////////////////////////
    const new_adapter_interface_ptr & tinypy_class_type_scope::get_new() const
    {
        return m_new;
    }
    //////////////////////////////////////////////////////////////////////////
    const destroy_adapter_interface_ptr & tinypy_class_type_scope::get_destroy() const
    {
        return m_destructor;
    }
    //////////////////////////////////////////////////////////////////////////
    const constructor_adapter_interface_ptr & tinypy_class_type_scope::get_constructor() const
    {
        return m_constructor;
    }
    //////////////////////////////////////////////////////////////////////////
    const convert_adapter_interface_ptr & tinypy_class_type_scope::get_convert() const
    {
        return m_convert;
    }
    //////////////////////////////////////////////////////////////////////////
    const method_adapter_interface_ptr & tinypy_class_type_scope::get_call() const
    {
        return m_call;
    }
    //////////////////////////////////////////////////////////////////////////
    const repr_adapter_interface_ptr & tinypy_class_type_scope::get_repr() const
    {
        return m_repr;
    }
    //////////////////////////////////////////////////////////////////////////
    const hash_adapter_interface_ptr & tinypy_class_type_scope::get_hash() const
    {
        return m_hash;
    }
    //////////////////////////////////////////////////////////////////////////
    const getattro_adapter_interface_ptr & tinypy_class_type_scope::get_getattro() const
    {
        return m_getattro;
    }
    //////////////////////////////////////////////////////////////////////////
    const mapping_adapter_interface_ptr & tinypy_class_type_scope::get_mapping() const
    {
        return m_mapping;
    }
    //////////////////////////////////////////////////////////////////////////
    const sequence_get_adapter_interface_ptr & tinypy_class_type_scope::get_sequence_get() const
    {
        return m_sequence_get;
    }
    //////////////////////////////////////////////////////////////////////////
    const sequence_set_adapter_interface_ptr & tinypy_class_type_scope::get_sequence_set() const
    {
        return m_sequence_set;
    }
    //////////////////////////////////////////////////////////////////////////
    const number_unary_adapter_interface_ptr & tinypy_class_type_scope::get_number_neg() const
    {
        return m_number_neg;
    }
    //////////////////////////////////////////////////////////////////////////
    const number_unary_adapter_interface_ptr & tinypy_class_type_scope::get_number_abs() const
    {
        return m_number_abs;
    }
    //////////////////////////////////////////////////////////////////////////
    const smart_pointer_adapter_interface_ptr & tinypy_class_type_scope::get_smart_pointer() const
    {
        return m_smart_pointer;
    }
    //////////////////////////////////////////////////////////////////////////
    const bindable_adapter_interface_ptr & tinypy_class_type_scope::get_bindable() const
    {
        return m_bindable;
    }
    //////////////////////////////////////////////////////////////////////////
    const compare_adapter_interface_ptr & tinypy_class_type_scope::get_compare( typeid_t _type ) const
    {
        if( m_compare_adapters != nullptr && _type < PYBIND_TYPE_COUNT )
        {
            return m_compare_adapters->compares[_type];
        }

        static compare_adapter_interface_ptr none;
        return none;
    }
    //////////////////////////////////////////////////////////////////////////
    const number_binary_adapter_interface_ptr & tinypy_class_type_scope::get_number_add( typeid_t _type ) const
    {
        if( m_number_adapters != nullptr && _type < PYBIND_TYPE_COUNT )
        {
            return m_number_adapters->adds[_type];
        }

        static number_binary_adapter_interface_ptr none;
        return none;
    }
    //////////////////////////////////////////////////////////////////////////
    const number_binary_adapter_interface_ptr & tinypy_class_type_scope::get_number_sub( typeid_t _type ) const
    {
        if( m_number_adapters != nullptr && _type < PYBIND_TYPE_COUNT )
        {
            return m_number_adapters->subs[_type];
        }

        static number_binary_adapter_interface_ptr none;
        return none;
    }
    //////////////////////////////////////////////////////////////////////////
    const number_binary_adapter_interface_ptr & tinypy_class_type_scope::get_number_mul( typeid_t _type ) const
    {
        if( m_number_adapters != nullptr && _type < PYBIND_TYPE_COUNT )
        {
            return m_number_adapters->muls[_type];
        }

        static number_binary_adapter_interface_ptr none;
        return none;
    }
    //////////////////////////////////////////////////////////////////////////
    const number_binary_adapter_interface_ptr & tinypy_class_type_scope::get_number_div( typeid_t _type ) const
    {
        if( m_number_adapters != nullptr && _type < PYBIND_TYPE_COUNT )
        {
            return m_number_adapters->divs[_type];
        }

        static number_binary_adapter_interface_ptr none;
        return none;
    }
    //////////////////////////////////////////////////////////////////////////
    const number_inplace_adapter_interface_ptr & tinypy_class_type_scope::get_number_inplace_add( typeid_t _type ) const
    {
        if( m_number_adapters != nullptr && _type < PYBIND_TYPE_COUNT )
        {
            return m_number_adapters->iadds[_type];
        }

        static number_inplace_adapter_interface_ptr none;
        return none;
    }
    //////////////////////////////////////////////////////////////////////////
    const number_inplace_adapter_interface_ptr & tinypy_class_type_scope::get_number_inplace_sub( typeid_t _type ) const
    {
        if( m_number_adapters != nullptr && _type < PYBIND_TYPE_COUNT )
        {
            return m_number_adapters->isubs[_type];
        }

        static number_inplace_adapter_interface_ptr none;
        return none;
    }
    //////////////////////////////////////////////////////////////////////////
    const number_inplace_adapter_interface_ptr & tinypy_class_type_scope::get_number_inplace_mul( typeid_t _type ) const
    {
        if( m_number_adapters != nullptr && _type < PYBIND_TYPE_COUNT )
        {
            return m_number_adapters->imuls[_type];
        }

        static number_inplace_adapter_interface_ptr none;
        return none;
    }
    //////////////////////////////////////////////////////////////////////////
    const number_inplace_adapter_interface_ptr & tinypy_class_type_scope::get_number_inplace_div( typeid_t _type ) const
    {
        if( m_number_adapters != nullptr && _type < PYBIND_TYPE_COUNT )
        {
            return m_number_adapters->idivs[_type];
        }

        static number_inplace_adapter_interface_ptr none;
        return none;
    }
    //////////////////////////////////////////////////////////////////////////
    tinypy_class_type_scope::compare_adapter_t * tinypy_class_type_scope::get_compare_adapters_()
    {
        if( m_compare_adapters == nullptr )
        {
            m_compare_adapters = m_kernel->get_allocator()->newP<compare_adapter_t>();
        }

        return m_compare_adapters;
    }
    //////////////////////////////////////////////////////////////////////////
    tinypy_class_type_scope::number_adapter_t * tinypy_class_type_scope::get_number_adapters_()
    {
        if( m_number_adapters == nullptr )
        {
            m_number_adapters = m_kernel->get_allocator()->newP<number_adapter_t>();
        }

        return m_number_adapters;
    }
    //////////////////////////////////////////////////////////////////////////
    void tinypy_class_type_scope::incref_smart_pointer( void * _impl )
    {
        if( m_smart_pointer != nullptr )
        {
            m_smart_pointer->incref_smart_pointer( m_kernel, _impl, class_type_scope_interface_ptr::from( this ) );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void tinypy_class_type_scope::decref_smart_pointer( void * _impl )
    {
        if( m_smart_pointer != nullptr )
        {
            m_smart_pointer->decref_smart_pointer( m_kernel, _impl, class_type_scope_interface_ptr::from( this ) );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void * tinypy_class_type_scope::call_new( PyObject * _obj, PyObject * _args, PyObject * _kwds )
    {
        if( m_new != nullptr )
        {
            return m_new->call( m_kernel, class_type_scope_interface_ptr::from( this ), _obj, _args, _kwds );
        }

        if( m_constructor != nullptr )
        {
            return m_constructor->call( m_kernel, _obj, _args );
        }

        return nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    void tinypy_class_type_scope::call_destructor( PyObject * _obj, void * _impl )
    {
        (void)_obj;
        if( m_destructor != nullptr )
        {
            m_destructor->call( m_kernel, class_type_scope_interface_ptr::from( this ), _impl );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void tinypy_class_type_scope::clear_bindable( void * _impl )
    {
        if( m_bindable != nullptr )
        {
            m_bindable->clear( m_kernel, _impl, class_type_scope_interface_ptr::from( this ) );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    tinypy_class_type_scope::payload_t * tinypy_class_type_scope::payload( PyObject * _object ) const
    {
        return static_cast<payload_t *>(tinypy_native_instance_payload( detail::cast_value( _object )));
    }
    //////////////////////////////////////////////////////////////////////////
}
