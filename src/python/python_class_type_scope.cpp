#include "python_class_type_scope.hpp"

#include "python_pod.hpp"
#include "python_system.hpp"

#include "pybind/exception.hpp"

#include "config/python.hpp"

#include "pybind/debug.hpp"

#include <initializer_list>

namespace pybind
{
    //////////////////////////////////////////////////////////////////////////
    namespace detail
    {
        //////////////////////////////////////////////////////////////////////////
        static PyObject * py_alloc_class( PyTypeObject * _type, PyObject * _args, PyObject * _kwds )
        {
            (void)_args;
            (void)_kwds;

            PyObject * py_self = PyType_GenericAlloc( _type, 0 );

            if( py_self == nullptr )
            {
                pybind::check_error();

                return nullptr;
            }

            return py_self;
        }
        //////////////////////////////////////////////////////////////////////////
        static PyObject * py_callfunc( PyObject * _obj, PyObject * _args, PyObject * _kwds )
        {
            kernel_interface * kernel = pybind::get_kernel();

            void * impl = kernel->get_class_impl( _obj );

#if defined(PYBIND_DEBUG)
            if( impl == nullptr )
            {
                pybind::error_message( "callfunc unbind object '%s'"
                    , kernel->object_str( _obj ).c_str()
                );

                return nullptr;
            }
#endif

            PyTypeObject * objtype = pybind::object_type( _obj );

            const class_type_scope_interface_ptr & scope = kernel->get_class_scope( objtype );

            if( scope == nullptr )
            {
                pybind::error_message( "obj '%s' type '%s' py_callfunc scope is null"
                    , kernel->object_str( _obj ).c_str()
                    , kernel->type_name( objtype )
                );

                return nullptr;
            }

            const method_adapter_interface_ptr & adapter = scope->get_call();

            try
            {
                DEBUG_PYBIND_NOTIFY_BIND_CALL_SCOPE( kernel, scope->get_name(), adapter->getName(), adapter->getCallDebugSilent(), _args, _kwds );

                PyObject * ret = adapter->call( kernel, impl, scope, _args, _kwds );

                return ret;
            }
            catch( const pybind::pybind_exception & _ex )
            {
                pybind::error_message( "obj '%s' py_callfunc invalid call '%s' error: %s"
                    , kernel->object_str( _obj ).c_str()
                    , adapter->getName()
                    , _ex.what()
                );
            }

            return nullptr;
        }
        //////////////////////////////////////////////////////////////////////////
        static PyObject * py_reprfunc( PyObject * _obj )
        {
            kernel_interface * kernel = pybind::get_kernel();

            void * impl = kernel->get_class_impl( _obj );

#if defined(PYBIND_DEBUG)
            if( impl == nullptr )
            {
                pybind::error_message( "reprfunc unbind object '%s'"
                    , kernel->object_str( _obj ).c_str()
                );

                return nullptr;
            }
#endif

            PyTypeObject * objtype = pybind::object_type( _obj );

            const class_type_scope_interface_ptr & scope = kernel->get_class_scope( objtype );

            if( scope == nullptr )
            {
                PyObject * ret = pybind::string_from_char( "[pybind scope is nullptr]" );

                return ret;
            }

            const repr_adapter_interface_ptr & adapter = scope->get_repr();

            try
            {
                PyObject * ret = adapter->repr( kernel, impl, scope );

                return ret;
            }
            catch( const pybind::pybind_exception & _ex )
            {
                pybind::error_message( "obj '%s' py_reprfunc invalid call exception: %s"
                    , kernel->object_str( _obj ).c_str()
                    , _ex.what()
                );
            }

            return nullptr;
        }
        //////////////////////////////////////////////////////////////////////////
        static long py_hash( PyObject * _obj )
        {
            const bool has_pod_hash = helper::has_pod_hash( _obj );

            if( has_pod_hash == true )
            {
                int64_t pod_hash = helper::get_pod_hash( _obj );

                if( pod_hash != -1 )
                {
                    long py_pod_hash = (long)pod_hash;

                    if( py_pod_hash == -1 )
                    {
                        py_pod_hash = -2;
                    }

                    return py_pod_hash;
                }
            }

            kernel_interface * kernel = pybind::get_kernel();

            void * impl = kernel->get_class_impl( _obj );

#if defined(PYBIND_DEBUG)
            if( impl == nullptr )
            {
                pybind::error_message( "hash unbind object '%s'"
                    , kernel->object_str( _obj ).c_str()
                );

                return -1;
            }
#endif

            PyTypeObject * objtype = pybind::object_type( _obj );

            const class_type_scope_interface_ptr & scope = kernel->get_class_scope( objtype );

            if( scope == nullptr )
            {
                pybind::error_message( "obj '%s' type '%s' py_hash scope is null"
                    , kernel->object_str( _obj ).c_str()
                    , kernel->type_name( objtype )
                );

                return -1;
            }

            const hash_adapter_interface_ptr & adapter = scope->get_hash();

            try
            {
                int64_t hash = adapter->hash( kernel, impl, scope );
                long py_hash = (long)hash;

                if( py_hash == -1 )
                {
                    py_hash = -2;
                }

                if( has_pod_hash == true )
                {
                    helper::set_pod_hash( _obj, (int64_t)py_hash );
                }

                return py_hash;
            }
            catch( const pybind::pybind_exception & _ex )
            {
                pybind::error_message( "obj '%s' py_hash invalid call exception: %s"
                    , kernel->object_str( _obj ).c_str()
                    , _ex.what()
                );
            }

            return -1;
        }
        //////////////////////////////////////////////////////////////////////////
        static PyObject * py_richcompare( PyObject * _obj, PyObject * _compare, int _op )
        {
            kernel_interface * kernel = pybind::get_kernel();

            void * impl = kernel->get_class_impl( _obj );

#if defined(PYBIND_DEBUG)
            if( impl == nullptr )
            {
                pybind::error_message( "py_richcompare '%s' compare unbind object"
                    , kernel->object_str( _obj ).c_str()
                );

                return nullptr;
            }
#endif

            PybindOperatorCompare pybind_op = POC_Less;

            switch( _op )
            {
            case Py_LT:
                {
                    pybind_op = POC_Less;
                }break;
            case Py_LE:
                {
                    pybind_op = POC_Lessequal;
                }break;
            case Py_EQ:
                {
                    pybind_op = POC_Equal;
                }break;
            case Py_NE:
                {
                    pybind_op = POC_Notequal;
                }break;
            case Py_GT:
                {
                    pybind_op = POC_Great;
                }break;
            case Py_GE:
                {
                    pybind_op = POC_Greatequal;
                }break;
            default:
                {
                    pybind::error_message( "invalid compare op '%d'"
                        , _op
                    );

                    return nullptr;
                }break;
            }

            PyTypeObject * objtype = pybind::object_type( _obj );

            const class_type_scope_interface_ptr & scope = kernel->get_class_scope( objtype );

            if( scope == nullptr )
            {
                pybind::error_message( "obj '%s' type '%s' py_richcompare scope is null"
                    , kernel->object_str( _obj ).c_str()
                    , kernel->type_name( objtype )
                );

                return nullptr;
            }

            typeid_t compare_type_id = kernel->get_object_type_id( _compare );

            const compare_adapter_interface_ptr & adapter = scope->get_compare( compare_type_id );

            if( adapter != nullptr )
            {
                try
                {
                    bool test_result;
                    if( adapter->compare( kernel, _obj, impl, scope, _compare, pybind_op, &test_result ) == false )
                    {
                        return kernel->ret_not_implemented();
                    }

                    PyObject * py_result = pybind::ret_bool( test_result );

                    return py_result;
                }
                catch( const pybind::pybind_exception & _ex )
                {
                    pybind::error_message( "obj '%s' py_richcompare '%s' invalid compare exception: %s"
                        , kernel->object_str( _obj ).c_str()
                        , kernel->object_str( _compare ).c_str()
                        , _ex.what()
                    );

                    return nullptr;
                }
            }

            typeid_t base_type_id = kernel->get_object_type_id( _obj );

            const compare_adapter_interface_ptr & base_adapter = scope->get_compare( base_type_id );

            if( base_adapter != nullptr )
            {
                try
                {
                    bool test_result;
                    if( base_adapter->compare( kernel, _obj, impl, scope, _compare, pybind_op, &test_result ) == false )
                    {
                        return kernel->ret_not_implemented();
                    }

                    PyObject * py_result = pybind::ret_bool( test_result );

                    return py_result;
                }
                catch( const pybind::pybind_exception & _ex )
                {
                    pybind::error_message( "obj '%s' py_richcompare '%s' invalid base compare exception: %s"
                        , kernel->object_str( _obj ).c_str()
                        , kernel->object_str( _compare ).c_str()
                        , _ex.what()
                    );

                    return nullptr;
                }
            }

            return kernel->ret_not_implemented();
        }
        //////////////////////////////////////////////////////////////////////////
        static PyObject * py_getattro( PyObject * _obj, PyObject * _key )
        {
            kernel_interface * kernel = pybind::get_kernel();

            void * impl = kernel->get_class_impl( _obj );

#if defined(PYBIND_DEBUG)
            if( impl == nullptr )
            {
                pybind::error_message( "getattro '%s' unbind object"
                    , kernel->object_str( _obj ).c_str()
                );

                return nullptr;
            }
#endif

            PyTypeObject * objtype = pybind::object_type( _obj );

            const class_type_scope_interface_ptr & scope = kernel->get_class_scope( objtype );

            if( scope == nullptr )
            {
                pybind::error_message( "obj '%s' type '%s' py_getattro scope is null"
                    , kernel->object_str( _obj ).c_str()
                    , kernel->type_name( objtype )
                );

                return nullptr;
            }

            const getattro_adapter_interface_ptr & adapter = scope->get_getattro();

            try
            {
                DEBUG_PYBIND_NOTIFY_BIND_CALL_SCOPE( kernel, scope->get_name(), adapter->getName(), adapter->getCallDebugSilent(), _key, nullptr );

                PyObject * res = adapter->call( kernel, impl, scope, _key );

                return res;
            }
            catch( const pybind::pybind_exception & _ex )
            {
                pybind::error_message( "obj '%s' py_getattro invalid call '%s' exception: %s"
                    , kernel->object_str( _obj ).c_str()
                    , adapter->getName()
                    , _ex.what()
                );
            }

            return nullptr;
        }
        //////////////////////////////////////////////////////////////////////////
        static PyObject * py_subscript( PyObject * _obj, PyObject * _key )
        {
            kernel_interface * kernel = pybind::get_kernel();

            void * impl = kernel->get_class_impl( _obj );

#if defined(PYBIND_DEBUG)
            if( impl == nullptr )
            {
                pybind::error_message( "subscript unbind object '%s'"
                    , kernel->object_str( _obj ).c_str()
                );

                return nullptr;
            }
#endif

            PyTypeObject * objtype = pybind::object_type( _obj );

            const class_type_scope_interface_ptr & scope = kernel->get_class_scope( objtype );

            if( scope == nullptr )
            {
                pybind::error_message( "obj '%s' type '%s' py_subscript scope is null"
                    , kernel->object_str( _obj ).c_str()
                    , kernel->type_name( objtype )
                );

                return nullptr;
            }

            const mapping_adapter_interface_ptr & adapter = scope->get_mapping();

            try
            {
                DEBUG_PYBIND_NOTIFY_BIND_CALL_SCOPE( kernel, scope->get_name(), adapter->getName(), adapter->getCallDebugSilent(), _key, nullptr );

                PyObject * res = adapter->call( kernel, impl, scope, _key );

                return res;
            }
            catch( const pybind::pybind_exception & _ex )
            {
                pybind::error_message( "obj '%s' py_subscript invalid call '%s' exception: %s"
                    , kernel->object_str( _obj ).c_str()
                    , adapter->getName()
                    , _ex.what()
                );
            }

            return nullptr;
        }
        //////////////////////////////////////////////////////////////////////////
        static PyMappingMethods py_as_mapping = {
            (lenfunc)0,		/* mp_length */
            (binaryfunc)&detail::py_subscript,		/* mp_subscript */
            (objobjargproc)0,	/* mp_ass_subscript */
        };
        //////////////////////////////////////////////////////////////////////////
        static PyObject * py_item_get( PyObject * _obj, Py_ssize_t _index )
        {
            kernel_interface * kernel = pybind::get_kernel();

            void * impl = kernel->get_class_impl( _obj );

#if defined(PYBIND_DEBUG)
            if( impl == nullptr )
            {
                pybind::error_message( "item get unbind object '%s"
                    , kernel->object_str( _obj ).c_str()
                );

                return nullptr;
            }
#endif

            PyTypeObject * objtype = pybind::object_type( _obj );

            const class_type_scope_interface_ptr & scope = kernel->get_class_scope( objtype );

            if( scope == nullptr )
            {
                pybind::error_message( "obj '%s' type '%s' py_item_get scope is null"
                    , kernel->object_str( _obj ).c_str()
                    , kernel->type_name( objtype )
                );

                return nullptr;
            }

            // Without sq_length the index arrives unnormalized; as the tinypy backend.
            if( _index < 0 )
            {
                PyErr_SetString( PyExc_IndexError, "index out of range" );

                return nullptr;
            }

            const sequence_get_adapter_interface_ptr & adapter = scope->get_sequence_get();

            try
            {
                DEBUG_PYBIND_NOTIFY_BIND_CALL_SCOPE( kernel, scope->get_name(), adapter->getName(), adapter->getCallDebugSilent(), nullptr, nullptr );

                PyObject * res = adapter->call( kernel, impl, scope, (size_t)_index );

                return res;
            }
            catch( const pybind::pybind_exception & _ex )
            {
                pybind::error_message( "obj '%s' py_item_get invalid call '%s' exception: %s"
                    , kernel->object_str( _obj ).c_str()
                    , adapter->getName()
                    , _ex.what()
                );
            }

            return nullptr;
        }
        //////////////////////////////////////////////////////////////////////////
        static int py_item_set( PyObject * _obj, Py_ssize_t _index, PyObject * _value )
        {
            kernel_interface * kernel = pybind::get_kernel();

            void * impl = kernel->get_class_impl( _obj );

#if defined(PYBIND_DEBUG)
            if( impl == nullptr )
            {
                pybind::error_message( "item set unbind object '%s'"
                    , kernel->object_str( _obj ).c_str()
                );

                return -1;
            }
#endif

            PyTypeObject * objtype = pybind::object_type( _obj );

            const class_type_scope_interface_ptr & scope = kernel->get_class_scope( objtype );

            if( scope == nullptr )
            {
                pybind::error_message( "obj '%s' type '%s' py_item_set scope is null"
                    , kernel->object_str( _obj ).c_str()
                    , kernel->type_name( objtype )
                );

                return -1;
            }

            if( _index < 0 )
            {
                PyErr_SetString( PyExc_IndexError, "index out of range" );

                return -1;
            }

            const sequence_set_adapter_interface_ptr & adapter = scope->get_sequence_set();

            try
            {
                DEBUG_PYBIND_NOTIFY_BIND_CALL_SCOPE( kernel, scope->get_name(), adapter->getName(), adapter->getCallDebugSilent(), nullptr, nullptr );

                adapter->call( kernel, impl, scope, (size_t)_index, _value );

                return 0;
            }
            catch( const pybind::pybind_exception & _ex )
            {
                pybind::error_message( "obj '%s' py_item_set invalid call '%s' exception: %s"
                    , kernel->object_str( _obj ).c_str()
                    , adapter->getName()
                    , _ex.what()
                );
            }

            return -1;
        }
        //////////////////////////////////////////////////////////////////////////
        static void raise_cannot_create( const class_type_scope_interface_ptr & _scope )
        {
            // As a type without tp_new and the tinypy backend; an error a
            // constructor raised stays.
            if( PyErr_Occurred() != nullptr )
            {
                return;
            }

            PyErr_Format( PyExc_TypeError, "cannot create '%s' instances"
                , _scope->get_name()
            );
        }
        //////////////////////////////////////////////////////////////////////////
        static PyObject * py_new_class( PyTypeObject * _type, PyObject * _args, PyObject * _kwds )
        {
            kernel_interface * kernel = pybind::get_kernel();

            const class_type_scope_interface_ptr & scope = kernel->get_class_scope( _type );

            if( scope == nullptr )
            {
                pybind::error_message( "obj '%s' type '%s' py_new_class scope is null"
                    , kernel->object_str( (PyObject *)_type ).c_str()
                    , kernel->type_name( _type )
                );

                return nullptr;
            }

            PyObject * py_self = py_alloc_class( _type, _args, _kwds );

            if( py_self == nullptr )
            {
                return nullptr;
            }

            try
            {
                void * impl = scope->call_new( py_self, _args, _kwds );

                if( impl == nullptr )
                {
                    pybind::decref( py_self );

                    detail::raise_cannot_create( scope );

                    return nullptr;
                }

                helper::wrap_pod_ptr( py_self, impl, false );

                scope->incref_smart_pointer( impl );

                return py_self;
            }
            catch( const pybind::pybind_exception & _ex )
            {
                pybind::decref( py_self );

                pybind::error_message( "obj '%s' py_new_class scope '%s' exception: %s"
                    , kernel->object_str( (PyObject *)_type ).c_str()
                    , scope->get_name()
                    , _ex.what()
                );
            }

            return nullptr;
        }
        //////////////////////////////////////////////////////////////////////////
        static PyObject * py_new_pod( PyTypeObject * _type, PyObject * _args, PyObject * _kwds )
        {
            kernel_interface * kernel = pybind::get_kernel();

            const class_type_scope_interface_ptr & scope = kernel->get_class_scope( _type );

            if( scope == nullptr )
            {
                pybind::error_message( "obj '%s' type '%s' py_new_pod scope is null"
                    , kernel->object_str( (PyObject *)_type ).c_str()
                    , kernel->type_name( _type )
                );

                return nullptr;
            }

            PyObject * py_self = detail::py_alloc_class( _type, _args, _kwds );

            if( py_self == nullptr )
            {
                return nullptr;
            }

            void * buff = nullptr;
            size_t pod_size = scope->get_pod_size();
            bool pod_hash = scope->get_pod_hash();

            helper::wrap_pod( py_self, &buff, pod_size, pod_hash );

            try
            {
                void * impl = scope->call_new( py_self, _args, _kwds );

                if( impl == nullptr )
                {
                    helper::unwrap_pod( py_self );

                    pybind::decref( py_self );

                    detail::raise_cannot_create( scope );

                    return nullptr;
                }

                scope->incref_smart_pointer( impl );

                return py_self;
            }
            catch( const pybind::pybind_exception & _ex )
            {
                helper::unwrap_pod( py_self );

                pybind::decref( py_self );

                pybind::error_message( "obj '%s' py_new_pod scope '%s' exception: %s"
                    , kernel->object_str( (PyObject *)_type ).c_str()
                    , scope->get_name()
                    , _ex.what()
                );
            }

            return nullptr;
        }
        //////////////////////////////////////////////////////////////////////////
        static PySequenceMethods py_as_sequence = {
            (lenfunc)0,                       /* sq_length */
            (binaryfunc)0,                    /* sq_concat */
            (ssizeargfunc)0,                  /* sq_repeat */
            (ssizeargfunc)&detail::py_item_get,                    /* sq_item */
            (ssizessizeargfunc)0,              /* sq_slice */
            (ssizeobjargproc)&detail::py_item_set,             /* sq_ass_item */
            (ssizessizeobjargproc)0,       /* sq_ass_slice */
            (objobjproc)0,                  /* sq_contains */
            (binaryfunc)0,            /* sq_inplace_concat */
            (ssizeargfunc)0,          /* sq_inplace_repeat */
        };
        //////////////////////////////////////////////////////////////////////////
        static PyObject * py_nb_unary_method( kernel_interface * _kernel, PyObject * _obj, const class_type_scope_interface_ptr & _scope, const number_unary_adapter_interface_ptr & _adapter )
        {
            void * impl = _kernel->get_class_impl( _obj );

#if defined(PYBIND_DEBUG)
            if( impl == nullptr )
            {
                pybind::error_message( "nb unary unbind object '%s' scope '%s'"
                    , _kernel->object_str( _obj ).c_str()
                    , _scope->get_name()
                );

                return nullptr;
            }
#endif

            DEBUG_PYBIND_NOTIFY_BIND_CALL_SCOPE( _kernel, _scope->get_name(), _adapter->getName(), _adapter->getCallDebugSilent(), nullptr, nullptr );

            PyObject * res = _adapter->call( _kernel, impl, _scope );

            return res;
        }
        //////////////////////////////////////////////////////////////////////////
        static PyObject * py_nb_binary_method( kernel_interface * _kernel, PyObject * _obj, PyObject * _value, const class_type_scope_interface_ptr & _scope, const number_binary_adapter_interface_ptr & _adapter, bool _rotate )
        {
            void * impl = _kernel->get_class_impl( _obj );

#if defined(PYBIND_DEBUG)
            if( impl == nullptr )
            {
                pybind::error_message( "nb binary unbind object '%s' scope '%s'"
                    , _kernel->object_str( _obj ).c_str()
                    , _scope->get_name()
                );

                return nullptr;
            }
#endif

            DEBUG_PYBIND_NOTIFY_BIND_CALL_SCOPE( _kernel, _scope->get_name(), _adapter->getName(), _adapter->getCallDebugSilent(), nullptr, nullptr );

            PyObject * res = _adapter->call( _kernel, impl, _scope, _value, _rotate );

            return res;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool py_nb_inplace_method( kernel_interface * _kernel, PyObject * _obj, PyObject * _value, const class_type_scope_interface_ptr & _scope, const number_inplace_adapter_interface_ptr & _adapter )
        {
            void * impl = _kernel->get_class_impl( _obj );

#if defined(PYBIND_DEBUG)
            if( impl == nullptr )
            {
                pybind::error_message( "nb inplace unbind object '%s' scope '%s'"
                    , _kernel->object_str( _obj ).c_str()
                    , _scope->get_name()
                );

                return false;
            }
#endif

            DEBUG_PYBIND_NOTIFY_BIND_CALL_SCOPE( _kernel, _scope->get_name(), _adapter->getName(), _adapter->getCallDebugSilent(), nullptr, nullptr );

            _adapter->call( _kernel, impl, _scope, _value );

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        static PyObject * py_nb_neg( PyObject * _obj )
        {
            kernel_interface * kernel = pybind::get_kernel();

            PyTypeObject * objtype = pybind::object_type( _obj );

            const class_type_scope_interface_ptr & scope = kernel->get_class_scope( objtype );

            const number_unary_adapter_interface_ptr & adapter = scope->get_number_neg();

            if( adapter != nullptr )
            {
                try
                {
                    PyObject * result = detail::py_nb_unary_method( kernel, _obj, scope, adapter );

                    return result;
                }
                catch( const pybind_exception & )
                {
                    pybind::error_message( "obj '%s' nb neg invalid call '%s'"
                        , kernel->object_str( _obj ).c_str()
                        , adapter->getName()
                    );

                    return nullptr;
                }
            }

            // A unary operator has no reflected operand to fall back to.
            PyErr_Format( PyExc_TypeError, "bad operand type for unary -: '%s'"
                , objtype->tp_name
            );

            return nullptr;
        }
        //////////////////////////////////////////////////////////////////////////
        static PyObject * py_nb_abs( PyObject * _obj )
        {
            kernel_interface * kernel = pybind::get_kernel();

            PyTypeObject * objtype = pybind::object_type( _obj );

            const class_type_scope_interface_ptr & scope = kernel->get_class_scope( objtype );

            const number_unary_adapter_interface_ptr & adapter = scope->get_number_abs();

            if( adapter != nullptr )
            {
                try
                {
                    PyObject * result = detail::py_nb_unary_method( kernel, _obj, scope, adapter );

                    return result;
                }
                catch( const pybind_exception & )
                {
                    pybind::error_message( "obj '%s' nb abs invalid call '%s'"
                        , kernel->object_str( _obj ).c_str()
                        , adapter->getName()
                    );

                    return nullptr;
                }
            }

            // A unary operator has no reflected operand to fall back to.
            PyErr_Format( PyExc_TypeError, "bad operand type for abs(): '%s'"
                , objtype->tp_name
            );

            return nullptr;
        }
        //////////////////////////////////////////////////////////////////////////
        static PyObject * py_nb_add2( kernel_interface * _kernel, PyObject * _obj, PyObject * _value, bool _rotate )
        {
            PyTypeObject * objtype = pybind::object_type( _obj );

            const class_type_scope_interface_ptr & scope = _kernel->get_class_scope( objtype );

            typeid_t value_type_id = _kernel->get_object_type_id( _value );

            const number_binary_adapter_interface_ptr & adapter = scope->get_number_add( value_type_id );

            if( adapter != nullptr )
            {
                try
                {
                    PyObject * result = detail::py_nb_binary_method( _kernel, _obj, _value, scope, adapter, _rotate );

                    if( result == nullptr )
                    {
                        return _kernel->ret_not_implemented();
                    }

                    return result;
                }
                catch( const pybind_exception & )
                {
                    pybind::error_message( "obj '%s' nb add invalid call '%s'"
                        , _kernel->object_str( _obj ).c_str()
                        , adapter->getName()
                    );

                    return nullptr;
                }
            }

            typeid_t base_type_id = _kernel->get_object_type_id( _obj );

            const number_binary_adapter_interface_ptr & base_adapter = scope->get_number_add( base_type_id );

            if( base_adapter != nullptr )
            {
                try
                {
                    PyObject * result = detail::py_nb_binary_method( _kernel, _obj, _value, scope, base_adapter, _rotate );

                    if( result == nullptr )
                    {
                        return _kernel->ret_not_implemented();
                    }

                    return result;
                }
                catch( const pybind_exception & )
                {
                    pybind::error_message( "obj '%s' nb base add invalid call '%s'"
                        , _kernel->object_str( _obj ).c_str()
                        , base_adapter->getName()
                    );

                    return nullptr;
                }
            }

            return _kernel->ret_not_implemented();
        }
        //////////////////////////////////////////////////////////////////////////
        static PyObject * py_nb_add( PyObject * _obj, PyObject * _value )
        {
            kernel_interface * kernel = pybind::get_kernel();

            if( kernel->is_class( _obj ) == false )
            {
                return detail::py_nb_add2( kernel, _value, _obj, true );
            }

            PyObject * result = detail::py_nb_add2( kernel, _obj, _value, false );

            // Bound types share this slot, so CPython does not try the right
            // operand itself; as the tinypy backend, its reflected adapter does.
            if( result != Py_NotImplemented || kernel->is_class( _value ) == false )
            {
                return result;
            }

            Py_DECREF( result );

            return detail::py_nb_add2( kernel, _value, _obj, true );
        }
        //////////////////////////////////////////////////////////////////////////
        static PyObject * py_nb_sub2( kernel_interface * _kernel, PyObject * _obj, PyObject * _value, bool _rotate )
        {
            PyTypeObject * objtype = pybind::object_type( _obj );

            const class_type_scope_interface_ptr & scope = _kernel->get_class_scope( objtype );

            typeid_t value_type_id = _kernel->get_object_type_id( _value );

            const number_binary_adapter_interface_ptr & adapter = scope->get_number_sub( value_type_id );

            if( adapter != nullptr )
            {
                try
                {
                    PyObject * result = detail::py_nb_binary_method( _kernel, _obj, _value, scope, adapter, _rotate );

                    if( result == nullptr )
                    {
                        return _kernel->ret_not_implemented();
                    }

                    return result;
                }
                catch( const pybind_exception & )
                {
                    pybind::error_message( "obj '%s' nb sub invalid call '%s'"
                        , _kernel->object_str( _obj ).c_str()
                        , adapter->getName()
                    );

                    return nullptr;
                }
            }

            typeid_t base_type_id = _kernel->get_object_type_id( _obj );

            const number_binary_adapter_interface_ptr & base_adapter = scope->get_number_sub( base_type_id );

            if( base_adapter != nullptr )
            {
                try
                {
                    PyObject * result = detail::py_nb_binary_method( _kernel, _obj, _value, scope, base_adapter, _rotate );

                    if( result == nullptr )
                    {
                        return _kernel->ret_not_implemented();
                    }

                    return result;
                }
                catch( const pybind_exception & )
                {
                    pybind::error_message( "obj '%s' nb base sub invalid call '%s'"
                        , _kernel->object_str( _obj ).c_str()
                        , base_adapter->getName()
                    );

                    return nullptr;
                }
            }

            return _kernel->ret_not_implemented();
        }
        //////////////////////////////////////////////////////////////////////////
        static PyObject * py_nb_sub( PyObject * _obj, PyObject * _value )
        {
            kernel_interface * kernel = pybind::get_kernel();

            if( kernel->is_class( _obj ) == false )
            {
                return detail::py_nb_sub2( kernel, _value, _obj, true );
            }

            PyObject * result = detail::py_nb_sub2( kernel, _obj, _value, false );

            // Bound types share this slot, so CPython does not try the right
            // operand itself; as the tinypy backend, its reflected adapter does.
            if( result != Py_NotImplemented || kernel->is_class( _value ) == false )
            {
                return result;
            }

            Py_DECREF( result );

            return detail::py_nb_sub2( kernel, _value, _obj, true );
        }
        //////////////////////////////////////////////////////////////////////////
        static PyObject * py_nb_mul2( kernel_interface * _kernel, PyObject * _obj, PyObject * _value, bool _rotate )
        {
            PyTypeObject * objtype = pybind::object_type( _obj );

            const class_type_scope_interface_ptr & scope = _kernel->get_class_scope( objtype );

            typeid_t value_type_id = _kernel->get_object_type_id( _value );

            const number_binary_adapter_interface_ptr & adapter = scope->get_number_mul( value_type_id );

            if( adapter != nullptr )
            {
                try
                {
                    PyObject * result = detail::py_nb_binary_method( _kernel, _obj, _value, scope, adapter, _rotate );

                    if( result == nullptr )
                    {
                        return _kernel->ret_not_implemented();
                    }

                    return result;
                }
                catch( const pybind_exception & )
                {
                    pybind::error_message( "obj '%s' nb mul invalid call '%s'"
                        , _kernel->object_str( _obj ).c_str()
                        , adapter->getName()
                    );

                    return nullptr;
                }
            }

            typeid_t base_type_id = _kernel->get_object_type_id( _obj );

            const number_binary_adapter_interface_ptr & base_adapter = scope->get_number_mul( base_type_id );

            if( base_adapter != nullptr )
            {
                try
                {
                    PyObject * result = detail::py_nb_binary_method( _kernel, _obj, _value, scope, base_adapter, _rotate );

                    if( result == nullptr )
                    {
                        return _kernel->ret_not_implemented();
                    }

                    return result;
                }
                catch( const pybind_exception & )
                {
                    pybind::error_message( "obj '%s' nb base mul invalid call '%s'"
                        , _kernel->object_str( _obj ).c_str()
                        , base_adapter->getName()
                    );

                    return nullptr;
                }
            }

            return _kernel->ret_not_implemented();
        }
        //////////////////////////////////////////////////////////////////////////
        static PyObject * py_nb_mul( PyObject * _obj, PyObject * _value )
        {
            kernel_interface * kernel = pybind::get_kernel();

            if( kernel->is_class( _obj ) == false )
            {
                return detail::py_nb_mul2( kernel, _value, _obj, true );
            }

            PyObject * result = detail::py_nb_mul2( kernel, _obj, _value, false );

            // Bound types share this slot, so CPython does not try the right
            // operand itself; as the tinypy backend, its reflected adapter does.
            if( result != Py_NotImplemented || kernel->is_class( _value ) == false )
            {
                return result;
            }

            Py_DECREF( result );

            return detail::py_nb_mul2( kernel, _value, _obj, true );
        }
        //////////////////////////////////////////////////////////////////////////
        static PyObject * py_nb_div2( kernel_interface * _kernel, PyObject * _obj, PyObject * _value, bool _rotate )
        {
            PyTypeObject * objtype = pybind::object_type( _obj );

            const class_type_scope_interface_ptr & scope = _kernel->get_class_scope( objtype );

            typeid_t value_type_id = _kernel->get_object_type_id( _value );

            const number_binary_adapter_interface_ptr & adapter = scope->get_number_div( value_type_id );

            if( adapter != nullptr )
            {
                try
                {
                    PyObject * result = detail::py_nb_binary_method( _kernel, _obj, _value, scope, adapter, _rotate );

                    if( result == nullptr )
                    {
                        return _kernel->ret_not_implemented();
                    }

                    return result;
                }
                catch( const pybind_exception & )
                {
                    pybind::error_message( "obj '%s' nb div invalid call '%s'"
                        , _kernel->object_str( _obj ).c_str()
                        , adapter->getName()
                    );

                    return nullptr;
                }
            }

            typeid_t base_type_id = _kernel->get_object_type_id( _obj );

            const number_binary_adapter_interface_ptr & base_adapter = scope->get_number_div( base_type_id );

            if( base_adapter != nullptr )
            {
                try
                {
                    PyObject * result = detail::py_nb_binary_method( _kernel, _obj, _value, scope, base_adapter, _rotate );

                    if( result == nullptr )
                    {
                        return _kernel->ret_not_implemented();
                    }

                    return result;
                }
                catch( const pybind_exception & )
                {
                    pybind::error_message( "obj '%s' nb base div invalid call '%s'"
                        , _kernel->object_str( _obj ).c_str()
                        , base_adapter->getName()
                    );

                    return nullptr;
                }
            }

            return _kernel->ret_not_implemented();
        }
        //////////////////////////////////////////////////////////////////////////
        static PyObject * py_nb_div( PyObject * _obj, PyObject * _value )
        {
            kernel_interface * kernel = pybind::get_kernel();

            if( kernel->is_class( _obj ) == false )
            {
                return detail::py_nb_div2( kernel, _value, _obj, true );
            }

            PyObject * result = detail::py_nb_div2( kernel, _obj, _value, false );

            // Bound types share this slot, so CPython does not try the right
            // operand itself; as the tinypy backend, its reflected adapter does.
            if( result != Py_NotImplemented || kernel->is_class( _value ) == false )
            {
                return result;
            }

            Py_DECREF( result );

            return detail::py_nb_div2( kernel, _value, _obj, true );
        }
        //////////////////////////////////////////////////////////////////////////
        static PyObject * py_nb_inplace_add( PyObject * _obj, PyObject * _value )
        {
            kernel_interface * kernel = pybind::get_kernel();

            PyTypeObject * objtype = pybind::object_type( _obj );

            const class_type_scope_interface_ptr & scope = kernel->get_class_scope( objtype );

            typeid_t value_type_id = kernel->get_object_type_id( _value );

            const number_inplace_adapter_interface_ptr & adapter = scope->get_number_inplace_add( value_type_id );

            if( adapter != nullptr )
            {
                try
                {
                    if( detail::py_nb_inplace_method( kernel, _obj, _value, scope, adapter ) == false )
                    {
                        return nullptr;
                    }

                    pybind::incref( _obj );

                    return _obj;
                }
                catch( const pybind_exception & )
                {
                    pybind::error_message( "obj '%s' nb inplace add invalid call '%s'"
                        , kernel->object_str( _obj ).c_str()
                        , adapter->getName()
                    );

                    return nullptr;
                }
            }

            typeid_t base_type_id = kernel->get_object_type_id( _obj );

            const number_inplace_adapter_interface_ptr & base_adapter = scope->get_number_inplace_add( base_type_id );

            if( base_adapter != nullptr )
            {
                try
                {
                    if( detail::py_nb_inplace_method( kernel, _obj, _value, scope, base_adapter ) == false )
                    {
                        return nullptr;
                    }

                    pybind::incref( _obj );

                    return _obj;
                }
                catch( const pybind_exception & )
                {
                    pybind::error_message( "obj '%s' nb inplace base add invalid call '%s'"
                        , kernel->object_str( _obj ).c_str()
                        , base_adapter->getName()
                    );

                    return nullptr;
                }
            }

            return kernel->ret_not_implemented();
        }
        //////////////////////////////////////////////////////////////////////////
        static PyObject * py_nb_inplace_sub( PyObject * _obj, PyObject * _value )
        {
            kernel_interface * kernel = pybind::get_kernel();

            PyTypeObject * objtype = pybind::object_type( _obj );

            const class_type_scope_interface_ptr & scope = kernel->get_class_scope( objtype );

            typeid_t value_type_id = kernel->get_object_type_id( _value );

            const number_inplace_adapter_interface_ptr & adapter = scope->get_number_inplace_sub( value_type_id );

            if( adapter != nullptr )
            {
                try
                {
                    if( detail::py_nb_inplace_method( kernel, _obj, _value, scope, adapter ) == false )
                    {
                        return nullptr;
                    }

                    pybind::incref( _obj );

                    return _obj;
                }
                catch( const pybind_exception & )
                {
                    pybind::error_message( "obj '%s' nb inplace sub invalid call '%s'"
                        , kernel->object_str( _obj ).c_str()
                        , adapter->getName()
                    );

                    return nullptr;
                }
            }

            typeid_t base_type_id = kernel->get_object_type_id( _obj );

            const number_inplace_adapter_interface_ptr & base_adapter = scope->get_number_inplace_sub( base_type_id );

            if( base_adapter != nullptr )
            {
                try
                {
                    if( detail::py_nb_inplace_method( kernel, _obj, _value, scope, base_adapter ) == false )
                    {
                        return nullptr;
                    }

                    pybind::incref( _obj );

                    return _obj;
                }
                catch( const pybind_exception & )
                {
                    pybind::error_message( "obj '%s' nb inplace base sub invalid call '%s'"
                        , kernel->object_str( _obj ).c_str()
                        , base_adapter->getName()
                    );

                    return nullptr;
                }
            }

            return kernel->ret_not_implemented();
        }
        //////////////////////////////////////////////////////////////////////////
        static PyObject * py_nb_inplace_mul( PyObject * _obj, PyObject * _value )
        {
            kernel_interface * kernel = pybind::get_kernel();

            PyTypeObject * objtype = pybind::object_type( _obj );

            const class_type_scope_interface_ptr & scope = kernel->get_class_scope( objtype );

            typeid_t value_type_id = kernel->get_object_type_id( _value );

            const number_inplace_adapter_interface_ptr & adapter = scope->get_number_inplace_mul( value_type_id );

            if( adapter != nullptr )
            {
                try
                {
                    if( detail::py_nb_inplace_method( kernel, _obj, _value, scope, adapter ) == false )
                    {
                        return nullptr;
                    }

                    pybind::incref( _obj );

                    return _obj;
                }
                catch( const pybind_exception & )
                {
                    pybind::error_message( "obj '%s' nb inplace mul invalid call '%s'"
                        , kernel->object_str( _obj ).c_str()
                        , adapter->getName()
                    );

                    return nullptr;
                }
            }

            typeid_t base_type_id = kernel->get_object_type_id( _obj );

            const number_inplace_adapter_interface_ptr & base_adapter = scope->get_number_inplace_mul( base_type_id );

            if( base_adapter != nullptr )
            {
                try
                {
                    if( detail::py_nb_inplace_method( kernel, _obj, _value, scope, base_adapter ) == false )
                    {
                        return nullptr;
                    }

                    pybind::incref( _obj );

                    return _obj;
                }
                catch( const pybind_exception & )
                {
                    pybind::error_message( "obj '%s' nb inplace base mul invalid call '%s'"
                        , kernel->object_str( _obj ).c_str()
                        , base_adapter->getName()
                    );

                    return nullptr;
                }
            }

            return kernel->ret_not_implemented();
        }
        //////////////////////////////////////////////////////////////////////////
        static PyObject * py_nb_inplace_div( PyObject * _obj, PyObject * _value )
        {
            kernel_interface * kernel = pybind::get_kernel();

            PyTypeObject * objtype = pybind::object_type( _obj );

            const class_type_scope_interface_ptr & scope = kernel->get_class_scope( objtype );

            typeid_t value_type_id = kernel->get_object_type_id( _value );

            const number_inplace_adapter_interface_ptr & adapter = scope->get_number_inplace_div( value_type_id );

            if( adapter != nullptr )
            {
                try
                {
                    if( detail::py_nb_inplace_method( kernel, _obj, _value, scope, adapter ) == false )
                    {
                        return nullptr;
                    }

                    pybind::incref( _obj );

                    return _obj;
                }
                catch( const pybind_exception & )
                {
                    pybind::error_message( "obj '%s' nb inplace div invalid call '%s'"
                        , kernel->object_str( _obj ).c_str()
                        , adapter->getName()
                    );

                    return nullptr;
                }
            }

            typeid_t base_type_id = kernel->get_object_type_id( _obj );

            const number_inplace_adapter_interface_ptr & base_adapter = scope->get_number_inplace_div( base_type_id );

            if( base_adapter != nullptr )
            {
                try
                {
                    if( detail::py_nb_inplace_method( kernel, _obj, _value, scope, base_adapter ) == false )
                    {
                        return nullptr;
                    }

                    pybind::incref( _obj );

                    return _obj;
                }
                catch( const pybind_exception & )
                {
                    pybind::error_message( "obj '%s' nb inplace base div invalid call '%s'"
                        , kernel->object_str( _obj ).c_str()
                        , base_adapter->getName()
                    );

                    return nullptr;
                }
            }

            return kernel->ret_not_implemented();
        }
        //////////////////////////////////////////////////////////////////////////
#if PYBIND_PYTHON_VERSION < 300
        //////////////////////////////////////////////////////////////////////////
        static PyNumberMethods py_as_number = {
            (binaryfunc)&detail::py_nb_add,
            (binaryfunc)&detail::py_nb_sub,
            (binaryfunc)&detail::py_nb_mul,
            (binaryfunc)&detail::py_nb_div,
            (binaryfunc)0, /*nb_remainder*/
            (binaryfunc)0, /*nb_divmod*/
            (ternaryfunc)0, /*nb_power*/
            (unaryfunc)&detail::py_nb_neg, /*nb_negative*/
            (unaryfunc)0, /*nb_positive*/
            (unaryfunc)&detail::py_nb_abs, /*nb_absolute*/
            (inquiry)0, /*nb_nonzero*/
            (unaryfunc)0, /*nb_invert*/
            (binaryfunc)0, /*nb_lshift*/
            (binaryfunc)0, /*nb_rshift*/
            (binaryfunc)0, /*nb_and*/
            (binaryfunc)0, /*nb_xor*/
            (binaryfunc)0, /*nb_or*/
            (coercion)0, /*nb_coerce*/
            (unaryfunc)0, /*nb_int*/
            (unaryfunc)0, /*nb_long*/
            (unaryfunc)0, /*nb_float*/
            (unaryfunc)0, /*nb_oct*/
            (unaryfunc)0, /*nb_hex*/
            /* Added in release 2.0 */
            (binaryfunc)&detail::py_nb_inplace_add, /*nb_inplace_add*/
            (binaryfunc)&detail::py_nb_inplace_sub, /*nb_inplace_subtract*/
            (binaryfunc)&detail::py_nb_inplace_mul, /*nb_inplace_multiply*/
            (binaryfunc)&detail::py_nb_inplace_div, /*nb_inplace_divide*/
            (binaryfunc)0, /*nb_inplace_remainder*/
            (ternaryfunc)0, /*nb_inplace_power*/
            (binaryfunc)0, /*nb_inplace_lshift*/
            (binaryfunc)0, /*nb_inplace_rshift*/
            (binaryfunc)0, /*nb_inplace_and*/
            (binaryfunc)0, /*nb_inplace_xor*/
            (binaryfunc)0, /*nb_inplace_or*/

            /* Added in release 2.2 */
            /* The following require the Py_TPFLAGS_HAVE_CLASS flag */
            (binaryfunc)0, /*nb_floor_divide*/
            (binaryfunc)0, /*nb_true_divide*/
            (binaryfunc)0, /*nb_inplace_floor_divide*/
            (binaryfunc)0, /*nb_inplace_true_divide*/

            /* Added in release 2.5 */
            (unaryfunc)0, /*nb_index*/
        };
#	else
        static PyNumberMethods py_as_number = {
            /* Number implementations must check *both*
            arguments for proper type and implement the necessary conversions
            in the slot functions themselves. */

            (binaryfunc)&detail::py_nb_add,
            (binaryfunc)&detail::py_nb_sub,
            (binaryfunc)&detail::py_nb_mul,
            (binaryfunc)&detail::py_nb_div,
            (binaryfunc)0, /*nb_divmod*/
            (ternaryfunc)0, /*nb_power*/
            (unaryfunc)&detail::py_nb_neg, /*nb_negative*/
            (unaryfunc)0, /*nb_positive*/
            (unaryfunc)&detail::py_nb_abs, /*nb_absolute*/
            (inquiry)0, /*nb_bool*/
            (unaryfunc)0, /*nb_invert*/
            (binaryfunc)0, /*nb_lshift*/
            (binaryfunc)0, /*nb_rshift*/
            (binaryfunc)0, /*nb_and*/
            (binaryfunc)0, /*nb_xor*/
            (binaryfunc)0, /*nb_or*/
            (unaryfunc)0, /*nb_int*/
            0,
            (unaryfunc)0, /*nb_float*/

            (binaryfunc)&detail::py_nb_inplace_add, /*nb_inplace_add*/
            (binaryfunc)&detail::py_nb_inplace_sub, /*nb_inplace_subtract*/
            (binaryfunc)&detail::py_nb_inplace_mul, /*nb_inplace_multiply*/
            (binaryfunc)&detail::py_nb_inplace_div, /*nb_inplace_remainder*/
            (ternaryfunc)0, /*nb_inplace_power*/
            (binaryfunc)0, /*nb_inplace_lshift*/
            (binaryfunc)0, /*nb_inplace_rshift*/
            (binaryfunc)0, /*nb_inplace_and*/
            (binaryfunc)0, /*nb_inplace_xor*/
            (binaryfunc)0, /*nb_inplace_or*/

            (binaryfunc)0, /*nb_floor_divide*/
            (binaryfunc)0, /*nb_true_divide*/
            (binaryfunc)0, /*nb_inplace_floor_divide*/
            (binaryfunc)0, /*nb_inplace_true_divide*/

            (unaryfunc)0, /*nb_index*/

            (binaryfunc)0, /*nb_matrix_multiply*/
            (binaryfunc)0, /*nb_inplace_matrix_multiply*/
        };
        //////////////////////////////////////////////////////////////////////////
#	endif
        //////////////////////////////////////////////////////////////////////////
        static bool defines_special( PyTypeObject * _type, PyTypeObject * _base, std::initializer_list<const char *> _names )
        {
            PyObject * py_mro = _type->tp_mro;
            Py_ssize_t mro_count = PyTuple_GET_SIZE( py_mro );

            for( Py_ssize_t mro_index = 0; mro_index != mro_count; ++mro_index )
            {
                PyTypeObject * py_mro_type = reinterpret_cast<PyTypeObject *>(PyTuple_GET_ITEM( py_mro, mro_index ));

                if( py_mro_type == _base )
                {
                    return false;
                }

                for( const char * name : _names )
                {
                    if( PyDict_GetItemString( py_mro_type->tp_dict, name ) != nullptr )
                    {
                        return true;
                    }
                }
            }

            return false;
        }
        //////////////////////////////////////////////////////////////////////////
        template<class T>
        static void inherit_bound_slot( PyTypeObject * _type, PyTypeObject * _base, T * _slot, T _baseSlot, std::initializer_list<const char *> _names )
        {
            if( _baseSlot == nullptr || detail::defines_special( _type, _base, _names ) == true )
            {
                return;
            }

            *_slot = _baseSlot;
        }
        //////////////////////////////////////////////////////////////////////////
        static void inherit_bound_slots( PyTypeObject * _type, PyTypeObject * _base )
        {
            // type_new resets a slot that no special method of the MRO names, and
            // a bound type sets its slots once created, so a Python subclass would
            // lose them.
            detail::inherit_bound_slot( _type, _base, &_type->tp_call, _base->tp_call, {"__call__"} );
            detail::inherit_bound_slot( _type, _base, &_type->tp_repr, _base->tp_repr, {"__repr__"} );
            detail::inherit_bound_slot( _type, _base, &_type->tp_hash, _base->tp_hash, {"__hash__"} );
            detail::inherit_bound_slot( _type, _base, &_type->tp_richcompare, _base->tp_richcompare, {"__eq__", "__ne__", "__lt__", "__le__", "__gt__", "__ge__"} );
            detail::inherit_bound_slot( _type, _base, &_type->tp_getattro, _base->tp_getattro, {"__getattribute__", "__getattr__"} );

            PyMappingMethods * mapping = _type->tp_as_mapping;
            const PyMappingMethods * base_mapping = _base->tp_as_mapping;

            if( mapping != nullptr && base_mapping != nullptr )
            {
                detail::inherit_bound_slot( _type, _base, &mapping->mp_subscript, base_mapping->mp_subscript, {"__getitem__"} );
            }

            PySequenceMethods * sequence = _type->tp_as_sequence;
            const PySequenceMethods * base_sequence = _base->tp_as_sequence;

            if( sequence != nullptr && base_sequence != nullptr )
            {
                detail::inherit_bound_slot( _type, _base, &sequence->sq_item, base_sequence->sq_item, {"__getitem__"} );
                detail::inherit_bound_slot( _type, _base, &sequence->sq_ass_item, base_sequence->sq_ass_item, {"__setitem__", "__delitem__"} );
            }

            PyNumberMethods * number = _type->tp_as_number;
            const PyNumberMethods * base_number = _base->tp_as_number;

            if( number != nullptr && base_number != nullptr )
            {
                detail::inherit_bound_slot( _type, _base, &number->nb_add, base_number->nb_add, {"__add__", "__radd__"} );
                detail::inherit_bound_slot( _type, _base, &number->nb_subtract, base_number->nb_subtract, {"__sub__", "__rsub__"} );
                detail::inherit_bound_slot( _type, _base, &number->nb_multiply, base_number->nb_multiply, {"__mul__", "__rmul__"} );
                detail::inherit_bound_slot( _type, _base, &number->nb_true_divide, base_number->nb_true_divide, {"__truediv__", "__rtruediv__"} );
                detail::inherit_bound_slot( _type, _base, &number->nb_negative, base_number->nb_negative, {"__neg__"} );
                detail::inherit_bound_slot( _type, _base, &number->nb_absolute, base_number->nb_absolute, {"__abs__"} );
                detail::inherit_bound_slot( _type, _base, &number->nb_inplace_add, base_number->nb_inplace_add, {"__iadd__"} );
                detail::inherit_bound_slot( _type, _base, &number->nb_inplace_subtract, base_number->nb_inplace_subtract, {"__isub__"} );
                detail::inherit_bound_slot( _type, _base, &number->nb_inplace_multiply, base_number->nb_inplace_multiply, {"__imul__"} );
                detail::inherit_bound_slot( _type, _base, &number->nb_inplace_true_divide, base_number->nb_inplace_true_divide, {"__itruediv__"} );
#if PYBIND_PYTHON_VERSION < 300
                detail::inherit_bound_slot( _type, _base, &number->nb_divide, base_number->nb_divide, {"__div__", "__rdiv__"} );
                detail::inherit_bound_slot( _type, _base, &number->nb_inplace_divide, base_number->nb_inplace_divide, {"__idiv__"} );
#endif
            }

            PyType_Modified( _type );
        }
        //////////////////////////////////////////////////////////////////////////
        static bool s_bound_type_creating = false;
        //////////////////////////////////////////////////////////////////////////
        static PyObject * py_bound_type_new( PyTypeObject * _metatype, PyObject * _args, PyObject * _kwds )
        {
            PyObject * py_type = PyType_Type.tp_new( _metatype, _args, _kwds );

            // A bound type keeps the slots its own scope sets, as a host native
            // subtype of tinypy does.
            if( py_type == nullptr || s_bound_type_creating == true )
            {
                return py_type;
            }

            kernel_interface * kernel = pybind::get_kernel();

            if( kernel == nullptr )
            {
                return py_type;
            }

            PyTypeObject * type = reinterpret_cast<PyTypeObject *>(py_type);
            const class_type_scope_interface_ptr & scope = kernel->get_class_scope( type );

            if( scope != nullptr )
            {
                detail::inherit_bound_slots( type, scope->get_typeobject() );
            }

            return py_type;
        }
        //////////////////////////////////////////////////////////////////////////
        static PyTypeObject * get_bound_metatype()
        {
            // The metatype of bound types and of their Python subclasses.
            static PyTypeObject s_metatype;

            if( s_metatype.tp_new != nullptr )
            {
                return &s_metatype;
            }

            reinterpret_cast<PyObject *>(&s_metatype)->ob_refcnt = 1;
            s_metatype.tp_name = "pybind_type";
            s_metatype.tp_flags = Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE;
            s_metatype.tp_base = &PyType_Type;
            s_metatype.tp_new = &detail::py_bound_type_new;

            if( PyType_Ready( &s_metatype ) < 0 )
            {
                pybind::check_error();

                s_metatype.tp_new = nullptr;

                return &PyType_Type;
            }

            return &s_metatype;
        }
        //////////////////////////////////////////////////////////////////////////
    }
    //////////////////////////////////////////////////////////////////////////
    python_class_type_scope::python_class_type_scope( kernel_interface * _kernel, const char * _name, typeid_t _typeId, void * _user, const new_adapter_interface_ptr & _pynew, const destroy_adapter_interface_ptr & _pydestructor, size_t _pod, bool _hash )
        : m_kernel( _kernel )
        , m_module( nullptr )
        , m_typeId( _typeId )
        , m_basesCount( 0 )
        , m_user( _user )
        , m_methodCount( 0 )
        , m_memberCount( 0 )
        , m_new( _pynew )
        , m_destructor( _pydestructor )
        , m_compare_adapters( nullptr )
        , m_number_adapters( nullptr )
        , m_objectCount( 0 )
        , m_pytypeobject( nullptr )
        , m_pod_size( _pod )
        , m_pod_hash( _hash )
        , m_binable_base( false )
    {
        ::strncpy( m_name, _name, PYBIND_CLASS_TYPE_MAX_NAME );
        m_name[PYBIND_CLASS_TYPE_MAX_NAME] = '\0';
    }
    //////////////////////////////////////////////////////////////////////////
    python_class_type_scope::~python_class_type_scope()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool python_class_type_scope::initialize( PyObject * _module )
    {
        PyObject * py_module = _module;

        if( py_module == nullptr )
        {
            py_module = m_kernel->get_current_module();

            if( py_module == nullptr )
            {
                pybind::throw_exception( "scope '%s' initialize not setup python module"
                    , m_name
                );

                return false;
            }
        }

        m_module = py_module;
        pybind::incref( m_module );

#if PYBIND_PYTHON_VERSION < 300
        PyObject * py_name = PyString_InternFromString( m_name );
#else
        PyObject * py_name = PyUnicode_InternFromString( m_name );
#endif

        PyObject * py_bases;

        if( m_basesCount > 0 )
        {
            Py_ssize_t py_basesCount = (Py_ssize_t)m_basesCount;
            py_bases = PyTuple_New( py_basesCount );
            size_t index = 0;

            for( size_t i = 0; i != m_basesCount; ++i )
            {
                Metacast & mc = m_bases[i];

                PyTypeObject * py_base = mc.scope->get_typeobject();

                // A pod keeps the storage of the layout its base inherits.
                if( m_pod_size != 0 )
                {
                    PyTypeObject * py_pod_type = m_kernel->get_pod_type( m_pod_size, m_pod_hash );
                    int fits = py_pod_type != nullptr ? PyType_IsSubtype( py_base, py_pod_type ) : 0;
                    pybind::xdecref( (PyObject *)py_pod_type );

                    if( fits == 0 )
                    {
                        pybind::decref( py_bases );
                        pybind::decref( py_name );

                        pybind::throw_exception( "scope '%s' pod %zu does not fit the layout of base '%s'"
                            , m_name
                            , m_pod_size
                            , mc.scope->get_name()
                        );

                        return false;
                    }
                }

                pybind::tuple_setitem( py_bases, index++, (PyObject *)py_base );

                const smart_pointer_adapter_interface_ptr & base_smart_pointer = mc.scope->get_smart_pointer();

                if( m_smart_pointer == nullptr && base_smart_pointer != nullptr )
                {
                    m_smart_pointer = base_smart_pointer;
                }

                const bindable_adapter_interface_ptr & base_bindable = mc.scope->get_bindable();

                if( m_bindable == nullptr && base_bindable != nullptr )
                {
                    m_bindable = base_bindable;
                }
            }
        }
        else
        {
            py_bases = PyTuple_New( 1 );

            PyTypeObject * py_pybind_type = m_kernel->get_pod_type( m_pod_size, m_pod_hash );

            if( py_pybind_type == nullptr )
            {
                pybind::throw_exception( "scope '%s' initialize not pod %zu (hash %d)"
                    , m_name
                    , m_pod_size
                    , m_pod_hash
                );
            }

            pybind::tuple_setitem( py_bases, 0, (PyObject *)py_pybind_type );

            pybind::decref( (PyObject *)py_pybind_type );
        }

        PyObject * py_dict = PyDict_New();

        PyObject * py_args = PyTuple_Pack( 3, py_name, py_bases, py_dict );

        pybind::decref( py_name );
        pybind::decref( py_bases );
        pybind::decref( py_dict );

        PyTypeObject * py_metatype = detail::get_bound_metatype();

        detail::s_bound_type_creating = true;
        PyObject * new_pytypeobject = PyObject_Call( (PyObject *)py_metatype, py_args, nullptr );
        detail::s_bound_type_creating = false;

        m_pytypeobject = (PyTypeObject *)new_pytypeobject;

        pybind::decref( py_args );

        if( m_pytypeobject == nullptr )
        {
            pybind::check_error();

            pybind::throw_exception( "scope '%s' initialize invalide not create type"
                , m_name
            );

            return false;
        }

        // The pod base releases the bound object when an instance is deallocated.
        if( m_pod_size == 0 )
        {
            m_pytypeobject->tp_new = &detail::py_new_class;
        }
        else
        {
            m_pytypeobject->tp_new = &detail::py_new_pod;
        }

        PyType_Modified( m_pytypeobject );

#if defined(PYBIND_DEBUG)
        if( pybind::module_hasobject( m_module, m_pytypeobject->tp_name ) == true )
        {
            pybind::throw_exception( "scope '%s' module overriding object '%s'"
                , m_name
                , m_pytypeobject->tp_name
            );

            return false;
        }
#endif

        pybind::incref( (PyObject *)m_pytypeobject );

        pybind::module_addobject( m_module, m_pytypeobject->tp_name, (PyObject *)m_pytypeobject );

        m_kernel->cache_class_scope_type( python_class_type_scope_ptr( this ) );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void python_class_type_scope::finalize()
    {
        if( m_module != nullptr )
        {
            pybind::module_removeobject( m_module, m_name );

            pybind::decref( m_module );
            m_module = nullptr;
        }

        m_kernel->remove_class_scope_type( python_class_type_scope_ptr( this ) );

        for( size_t i = 0; i != m_basesCount; ++i )
        {
            Metacast & m = m_bases[i];

            m.info = 0;
            m.cast = nullptr;
            m.scope = nullptr;
        }

        for( size_t i = 0; i != m_methodCount; ++i )
        {
            method_adapter_interface_ptr & adapter = m_methods[i];

            const char * name = adapter->getName();

            pybind::module_removeobject( reinterpret_cast<PyObject *>(m_pytypeobject), name );

            adapter = nullptr;
        }

        for( size_t i = 0; i != m_memberCount; ++i )
        {
            member_adapter_interface_ptr & adapter = m_members[i];

            const char * name = adapter->getName();

            pybind::module_removeobject( reinterpret_cast<PyObject *>(m_pytypeobject), name );

            adapter = nullptr;
        }

        pybind::xdecref( (PyObject *)m_pytypeobject );

        m_new = nullptr;
        m_destructor = nullptr;
        m_constructor = nullptr;
        m_convert = nullptr;
        m_call = nullptr;
        m_repr = nullptr;
        m_hash = nullptr;
        m_getattro = nullptr;
        m_mapping = nullptr;
        m_sequence_get = nullptr;
        m_sequence_set = nullptr;

        if( m_compare_adapters != nullptr )
        {
            for( size_t index = 0; index != PYBIND_METHOD_COUNT; ++index )
            {
                m_compare_adapters->compares[index] = nullptr;
            }

            allocator_interface * allocator = m_kernel->get_allocator();
            allocator->deleteT( m_compare_adapters );

            m_compare_adapters = nullptr;
        }

        if( m_number_adapters != nullptr )
        {
            for( size_t index = 0; index != PYBIND_TYPE_COUNT; ++index )
            {
                m_number_adapters->adds[index] = nullptr;
                m_number_adapters->subs[index] = nullptr;
                m_number_adapters->muls[index] = nullptr;
                m_number_adapters->divs[index] = nullptr;

                m_number_adapters->iadds[index] = nullptr;
                m_number_adapters->isubs[index] = nullptr;
                m_number_adapters->imuls[index] = nullptr;
                m_number_adapters->idivs[index] = nullptr;
            }

            allocator_interface * allocator = m_kernel->get_allocator();
            allocator->deleteT( m_number_adapters );

            m_number_adapters = nullptr;
        }

        m_smart_pointer = nullptr;
        m_bindable = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    kernel_interface * python_class_type_scope::get_kernel() const
    {
        return m_kernel;
    }
    //////////////////////////////////////////////////////////////////////////
    void python_class_type_scope::type_initialize( PyTypeObject * _type )
    {
        // Python subclasses release their bound object through the pod base.
        (void)_type;
    }
    //////////////////////////////////////////////////////////////////////////
    bool python_class_type_scope::is_instance( PyTypeObject * _type ) const
    {
        if( PyType_IsSubtype( _type, m_pytypeobject ) == 0 )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    const char * python_class_type_scope::get_name() const
    {
        return m_name;
    }
    //////////////////////////////////////////////////////////////////////////
    typeid_t python_class_type_scope::get_type_id() const
    {
        return m_typeId;
    }
    //////////////////////////////////////////////////////////////////////////
    size_t python_class_type_scope::get_pod_size() const
    {
        return m_pod_size;
    }
    //////////////////////////////////////////////////////////////////////////
    bool python_class_type_scope::get_pod_hash() const
    {
        return m_pod_hash;
    }
    //////////////////////////////////////////////////////////////////////////
    void * python_class_type_scope::get_user() const
    {
        return m_user;
    }
    //////////////////////////////////////////////////////////////////////////
    PyTypeObject * python_class_type_scope::get_typeobject() const
    {
        return m_pytypeobject;
    }
    //////////////////////////////////////////////////////////////////////////
    void python_class_type_scope::add_method( const method_adapter_interface_ptr & _imethod )
    {
#if defined(PYBIND_DEBUG)
        if( m_methodCount == PYBIND_METHOD_COUNT )
        {
            pybind::throw_exception( "scope '%s' add_method '%s' max count"
                , this->get_name()
                , _imethod->getName()
            );

            return;
        }
#endif

        const char * name = _imethod->getName();

#if defined(PYBIND_DEBUG)
        if( pybind::object_hasattrstring( (PyObject *)m_pytypeobject, name ) == true )
        {
            pybind::throw_exception( "scope '%s' add_method '%s' already exist"
                , this->get_name()
                , name
            );

            return;
        }
#endif

        PyObject * py_type_method = m_kernel->create_method_adapter( _imethod, m_pytypeobject );

        if( PyObject_SetAttrString( reinterpret_cast<PyObject *>(m_pytypeobject), name, py_type_method ) < 0 )
        {
            pybind::decref( py_type_method );

            pybind::throw_exception( "scope '%s' add_method '%s' python error"
                , this->get_name()
                , name
            );

            return;
        }

        pybind::decref( py_type_method );

        m_methods[m_methodCount++] = _imethod;
        PyType_Modified( m_pytypeobject );
    }
    //////////////////////////////////////////////////////////////////////////
    const method_adapter_interface_ptr & python_class_type_scope::find_method( const char * _name ) const
    {
        for( size_t index = 0; index != m_methodCount; ++index )
        {
            const method_adapter_interface_ptr & method = m_methods[index];

            const char * methodName = method->getName();

            if( strcmp( methodName, _name ) != 0 )
            {
                continue;
            }

            return method;
        }

        pybind::throw_exception( "scope '%s' find_method '%s' python error"
            , this->get_name()
            , _name
        );

        return method_adapter_interface_ptr::none();
    }
    //////////////////////////////////////////////////////////////////////////
    size_t python_class_type_scope::get_methods_count() const
    {
        return m_methodCount;
    }
    //////////////////////////////////////////////////////////////////////////
    const method_adapter_interface_ptr & python_class_type_scope::get_method( size_t _index ) const
    {
        return m_methods[_index];
    }
    //////////////////////////////////////////////////////////////////////////
    const member_adapter_interface_ptr & python_class_type_scope::find_member( const char * _name ) const
    {
        for( size_t index = 0; index != m_memberCount; ++index )
        {
            const member_adapter_interface_ptr & method = m_members[index];

            const char * memberName = method->getName();

            if( strcmp( memberName, _name ) != 0 )
            {
                continue;
            }

            return method;
        }

        pybind::throw_exception( "scope '%s' find_member '%s' python error"
            , this->get_name()
            , _name
        );

        return member_adapter_interface_ptr::none();
    }
    //////////////////////////////////////////////////////////////////////////
    size_t python_class_type_scope::get_members_count() const
    {
        return m_memberCount;
    }
    //////////////////////////////////////////////////////////////////////////
    const member_adapter_interface_ptr & python_class_type_scope::get_member( size_t _index ) const
    {
        return m_members[_index];
    }
    //////////////////////////////////////////////////////////////////////////
    void python_class_type_scope::add_member( const member_adapter_interface_ptr & _imember )
    {
#if defined(PYBIND_DEBUG)
        if( m_memberCount == PYBIND_MEMBER_COUNT )
        {
            pybind::throw_exception( "scope '%s' add_member '%s' max count"
                , this->get_name()
                , _imember->getName()
            );

            return;
        }
#endif

        const char * name = _imember->getName();

#if defined(PYBIND_DEBUG)
        if( pybind::object_hasattrstring( (PyObject *)m_pytypeobject, name ) == true )
        {
            pybind::throw_exception( "scope '%s' add_member '%s' already exist"
                , this->get_name()
                , name
            );

            return;
        }
#endif

        PyObject * py_member = m_kernel->create_member_adapter( _imember, m_pytypeobject );

        if( PyObject_SetAttrString( reinterpret_cast<PyObject *>(m_pytypeobject), name, py_member ) < 0 )
        {
            pybind::decref( py_member );

            pybind::throw_exception( "scope '%s' add_member '%s' python error"
                , this->get_name()
                , name
            );

            return;
        }

        pybind::decref( py_member );

        m_members[m_memberCount++] = _imember;
        PyType_Modified( m_pytypeobject );
    }
    //////////////////////////////////////////////////////////////////////////
    void python_class_type_scope::set_convert( const convert_adapter_interface_ptr & _iconvert )
    {
        m_convert = _iconvert;
    }
    //////////////////////////////////////////////////////////////////////////
    void python_class_type_scope::set_call( const method_adapter_interface_ptr & _icall )
    {
        m_call = _icall;

        m_pytypeobject->tp_call = &detail::py_callfunc;
        PyType_Modified( m_pytypeobject );
    }
    //////////////////////////////////////////////////////////////////////////
    void python_class_type_scope::set_repr( const repr_adapter_interface_ptr & _irepr )
    {
        m_repr = _irepr;

        m_pytypeobject->tp_repr = &detail::py_reprfunc;
        PyType_Modified( m_pytypeobject );
    }
    //////////////////////////////////////////////////////////////////////////
    void python_class_type_scope::set_hash( const hash_adapter_interface_ptr & _ihash )
    {
        m_hash = _ihash;

        m_pytypeobject->tp_hash = (hashfunc)&detail::py_hash;
        PyType_Modified( m_pytypeobject );
    }
    //////////////////////////////////////////////////////////////////////////
    void python_class_type_scope::set_compare( typeid_t _typeId, const compare_adapter_interface_ptr & _icompare )
    {
        compare_adapter_t * compare_adapters = this->get_compare_adapters();
        compare_adapters->compares[_typeId] = _icompare;

        m_pytypeobject->tp_richcompare = &detail::py_richcompare;
        PyType_Modified( m_pytypeobject );
    }
    //////////////////////////////////////////////////////////////////////////
    void python_class_type_scope::set_getattro( const getattro_adapter_interface_ptr & _igetattro )
    {
        m_getattro = _igetattro;

        m_pytypeobject->tp_getattro = &detail::py_getattro;
        PyType_Modified( m_pytypeobject );
    }
    //////////////////////////////////////////////////////////////////////////
    void python_class_type_scope::set_mapping( const mapping_adapter_interface_ptr & _imapping )
    {
        m_mapping = _imapping;

        this->setup_mapping_slots_();
    }
    //////////////////////////////////////////////////////////////////////////
    void python_class_type_scope::set_sequence_get( const sequence_get_adapter_interface_ptr & _isequence )
    {
        m_sequence_get = _isequence;

        this->setup_sequence_slots_();
    }
    //////////////////////////////////////////////////////////////////////////
    void python_class_type_scope::set_sequence_set( const sequence_set_adapter_interface_ptr & _isequence )
    {
        m_sequence_set = _isequence;

        this->setup_sequence_slots_();
    }
    //////////////////////////////////////////////////////////////////////////
    void python_class_type_scope::set_number_neg( const number_unary_adapter_interface_ptr & _iadapter )
    {
        number_adapter_t * number_adapters = this->get_number_adapters();
        number_adapters->neg = _iadapter;

        this->setup_number_slots_();
    }
    //////////////////////////////////////////////////////////////////////////
    void python_class_type_scope::set_number_abs( const number_unary_adapter_interface_ptr & _iadapter )
    {
        number_adapter_t * number_adapters = this->get_number_adapters();
        number_adapters->abs = _iadapter;

        this->setup_number_slots_();
    }
    //////////////////////////////////////////////////////////////////////////
    python_class_type_scope::number_adapter_t * python_class_type_scope::get_number_adapters()
    {
        if( m_number_adapters == nullptr )
        {
            allocator_interface * allocator = m_kernel->get_allocator();

            m_number_adapters = allocator->newP<number_adapter_t>();
        }

        return m_number_adapters;
    }
    //////////////////////////////////////////////////////////////////////////
    python_class_type_scope::compare_adapter_t * python_class_type_scope::get_compare_adapters()
    {
        if( m_compare_adapters == nullptr )
        {
            allocator_interface * allocator = m_kernel->get_allocator();

            m_compare_adapters = allocator->newP<compare_adapter_t>();
        }

        return m_compare_adapters;
    }
    //////////////////////////////////////////////////////////////////////////
    void python_class_type_scope::set_number_add( typeid_t _typeId, const number_binary_adapter_interface_ptr & _iadapter )
    {
        number_adapter_t * number_adapters = this->get_number_adapters();
        number_adapters->adds[_typeId] = _iadapter;

        this->setup_number_slots_();
    }
    //////////////////////////////////////////////////////////////////////////
    void python_class_type_scope::set_number_sub( typeid_t _typeId, const number_binary_adapter_interface_ptr & _iadapter )
    {
        number_adapter_t * number_adapters = this->get_number_adapters();
        number_adapters->subs[_typeId] = _iadapter;

        this->setup_number_slots_();
    }
    //////////////////////////////////////////////////////////////////////////
    void python_class_type_scope::set_number_mul( typeid_t _typeId, const number_binary_adapter_interface_ptr & _iadapter )
    {
        number_adapter_t * number_adapters = this->get_number_adapters();
        number_adapters->muls[_typeId] = _iadapter;

        this->setup_number_slots_();
    }
    //////////////////////////////////////////////////////////////////////////
    void python_class_type_scope::set_number_div( typeid_t _typeId, const number_binary_adapter_interface_ptr & _iadapter )
    {
        number_adapter_t * number_adapters = this->get_number_adapters();
        number_adapters->divs[_typeId] = _iadapter;

        this->setup_number_slots_();
    }
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void python_class_type_scope::set_number_inplace_add( typeid_t _typeId, const number_inplace_adapter_interface_ptr & _iadapter )
    {
        number_adapter_t * number_adapters = this->get_number_adapters();
        number_adapters->iadds[_typeId] = _iadapter;

        this->setup_number_slots_();
    }
    //////////////////////////////////////////////////////////////////////////
    void python_class_type_scope::set_number_inplace_sub( typeid_t _typeId, const number_inplace_adapter_interface_ptr & _iadapter )
    {
        number_adapter_t * number_adapters = this->get_number_adapters();
        number_adapters->isubs[_typeId] = _iadapter;

        this->setup_number_slots_();
    }
    //////////////////////////////////////////////////////////////////////////
    void python_class_type_scope::set_number_inplace_mul( typeid_t _typeId, const number_inplace_adapter_interface_ptr & _iadapter )
    {
        number_adapter_t * number_adapters = this->get_number_adapters();
        number_adapters->imuls[_typeId] = _iadapter;

        this->setup_number_slots_();
    }
    //////////////////////////////////////////////////////////////////////////
    void python_class_type_scope::set_number_inplace_div( typeid_t _typeId, const number_inplace_adapter_interface_ptr & _iadapter )
    {
        number_adapter_t * number_adapters = this->get_number_adapters();
        number_adapters->idivs[_typeId] = _iadapter;

        this->setup_number_slots_();
    }
    //////////////////////////////////////////////////////////////////////////
    void python_class_type_scope::setup_number_slots_()
    {
        // A heap type owns its slot tables, which type_setattro rewrites when
        // Python code assigns a special method; a shared table would change
        // every bound type.
        PyHeapTypeObject * heap_type = reinterpret_cast<PyHeapTypeObject *>(m_pytypeobject);
        heap_type->as_number = detail::py_as_number;
        m_pytypeobject->tp_as_number = &heap_type->as_number;
        PyType_Modified( m_pytypeobject );
    }
    //////////////////////////////////////////////////////////////////////////
    void python_class_type_scope::setup_sequence_slots_()
    {
        PyHeapTypeObject * heap_type = reinterpret_cast<PyHeapTypeObject *>(m_pytypeobject);
        heap_type->as_sequence = detail::py_as_sequence;
        m_pytypeobject->tp_as_sequence = &heap_type->as_sequence;
        PyType_Modified( m_pytypeobject );
    }
    //////////////////////////////////////////////////////////////////////////
    void python_class_type_scope::setup_mapping_slots_()
    {
        PyHeapTypeObject * heap_type = reinterpret_cast<PyHeapTypeObject *>(m_pytypeobject);
        heap_type->as_mapping = detail::py_as_mapping;
        m_pytypeobject->tp_as_mapping = &heap_type->as_mapping;
        PyType_Modified( m_pytypeobject );
    }
    //////////////////////////////////////////////////////////////////////////
    void python_class_type_scope::set_smart_pointer( const smart_pointer_adapter_interface_ptr & _iadapter )
    {
        m_smart_pointer = _iadapter;
    }
    //////////////////////////////////////////////////////////////////////////
    void python_class_type_scope::set_bindable( const bindable_adapter_interface_ptr & _iadapter )
    {
        m_bindable = _iadapter;

        if( m_bindable != nullptr )
        {
            m_binable_base = true;
        }
        else
        {
            m_binable_base = false;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void python_class_type_scope::add_proxy_interface( const class_type_scope_interface_ptr & _scope, const proxy_adapter_interface_ptr & _iproxy )
    {
        size_t methodCount = _scope->get_methods_count();

        for( size_t index = 0; index != methodCount; ++index )
        {
            const method_adapter_interface_ptr & imethod = _scope->get_method( index );

            allocator_interface * allocator = m_kernel->get_allocator();

            const char * methodName = imethod->getName();

            method_adapter_interface_ptr iproxy_method = allocator->newT<proxy_method_adapter>( methodName, _iproxy, imethod, _scope );

            this->add_method( iproxy_method );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void * python_class_type_scope::call_new( PyObject * _obj, PyObject * _args, PyObject * _kwds )
    {
        if( m_new != nullptr )
        {
            void * impl = m_new->call( m_kernel, stdex::intrusive_from_this( this ), _obj, _args, _kwds );

            return impl;
        }

        if( m_constructor != nullptr )
        {
            void * obj = m_constructor->call( m_kernel, _obj, _args );

            return obj;
        }

        return nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    void python_class_type_scope::set_construct( const constructor_adapter_interface_ptr & _ctr )
    {
        m_constructor = _ctr;
    }
    //////////////////////////////////////////////////////////////////////////
    void python_class_type_scope::add_base( typeid_t _info, const class_type_scope_interface_ptr & _scope, pybind_metacast _cast )
    {
#if defined(PYBIND_DEBUG)
        if( m_basesCount == PYBIND_BASES_COUNT )
        {
            pybind::throw_exception( "scope '%s' maximize bases count PYBIND_BASES_COUNT is %u"
                , this->get_name()
                , PYBIND_BASES_COUNT
            );

            return;
        }
#endif

        Metacast mc;
        mc.info = _info;
        mc.scope = _scope;
        mc.cast = _cast;

        size_t newId = m_basesCount++;
        m_bases[newId] = mc;
    }
    //////////////////////////////////////////////////////////////////////////
    void * python_class_type_scope::meta_cast( typeid_t _info, void * _impl )
    {
        for( size_t i = 0; i != m_basesCount; ++i )
        {
            Metacast & mc = m_bases[i];

            if( mc.info != _info )
            {
                continue;
            }

            void * obj = mc.cast( _impl );

            return obj;
        }

        for( size_t i = 0; i != m_basesCount; ++i )
        {
            Metacast & mc = m_bases[i];

            void * down_impl = mc.cast( _impl );

            void * obj = mc.scope->meta_cast( _info, down_impl );

            if( obj == nullptr )
            {
                continue;
            }

            return obj;
        }

        return nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * python_class_type_scope::create_class( void * _impl )
    {
        if( m_pod_size != 0 )
        {
            return nullptr;
        }

        PyObject * py_self = detail::py_alloc_class( m_pytypeobject, nullptr, nullptr );

        if( py_self == nullptr )
        {
            return nullptr;
        }

        pybind::helper::wrap_pod_ptr( py_self, _impl, false );

        this->incref_smart_pointer( _impl );

        return py_self;
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * python_class_type_scope::create_holder( void * _impl )
    {
        if( m_pod_size != 0 )
        {
            return nullptr;
        }

        PyObject * py_self = detail::py_alloc_class( m_pytypeobject, nullptr, nullptr );

        if( py_self == nullptr )
        {
            return nullptr;
        }

        pybind::helper::wrap_pod_ptr( py_self, _impl, true );

        this->incref_smart_pointer( _impl );

        return py_self;
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * python_class_type_scope::create_weak( void * _impl )
    {
        if( m_pod_size != 0 )
        {
            return nullptr;
        }

        PyObject * py_self = detail::py_alloc_class( m_pytypeobject, nullptr, nullptr );

        if( py_self == nullptr )
        {
            return nullptr;
        }

        pybind::helper::wrap_pod_weak( py_self, _impl, true );

        return py_self;
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * python_class_type_scope::create_pod( void ** _impl )
    {
        if( m_pod_size == 0 )
        {
            return nullptr;
        }

        PyObject * py_self = detail::py_alloc_class( m_pytypeobject, nullptr, nullptr );

        if( py_self == nullptr )
        {
            return nullptr;
        }

        pybind::helper::wrap_pod( py_self, _impl, m_pod_size, m_pod_hash );

        return py_self;
    }
    //////////////////////////////////////////////////////////////////////////
    void python_class_type_scope::incref_smart_pointer( void * _impl )
    {
        if( m_smart_pointer != nullptr )
        {
            m_smart_pointer->incref_smart_pointer( m_kernel, _impl, stdex::intrusive_from_this( this ) );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void python_class_type_scope::decref_smart_pointer( void * _impl )
    {
        if( m_smart_pointer != nullptr )
        {
            m_smart_pointer->decref_smart_pointer( m_kernel, _impl, stdex::intrusive_from_this( this ) );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void python_class_type_scope::call_destructor( PyObject * _obj, void * _impl )
    {
        bool holder = pybind::helper::is_pod_holder( _obj );

        if( holder == true )
        {
            return;
        }

        if( m_destructor != nullptr )
        {
            m_destructor->call( m_kernel, stdex::intrusive_from_this( this ), _impl );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void python_class_type_scope::clear_bindable( void * _impl )
    {
        if( m_bindable != nullptr )
        {
            m_bindable->clear( m_kernel, _impl, stdex::intrusive_from_this( this ) );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    const new_adapter_interface_ptr & python_class_type_scope::get_new() const
    {
        return m_new;
    }
    //////////////////////////////////////////////////////////////////////////
    const destroy_adapter_interface_ptr & python_class_type_scope::get_destroy() const
    {
        return m_destructor;
    }
    //////////////////////////////////////////////////////////////////////////
    const constructor_adapter_interface_ptr & python_class_type_scope::get_constructor() const
    {
        return m_constructor;
    }
    //////////////////////////////////////////////////////////////////////////
    const convert_adapter_interface_ptr & python_class_type_scope::get_convert() const
    {
        return m_convert;
    }
    //////////////////////////////////////////////////////////////////////////
    const method_adapter_interface_ptr & python_class_type_scope::get_call() const
    {
        return m_call;
    }
    //////////////////////////////////////////////////////////////////////////
    const repr_adapter_interface_ptr & python_class_type_scope::get_repr() const
    {
        return m_repr;
    }
    //////////////////////////////////////////////////////////////////////////
    const hash_adapter_interface_ptr & python_class_type_scope::get_hash() const
    {
        return m_hash;
    }
    //////////////////////////////////////////////////////////////////////////
    const compare_adapter_interface_ptr & python_class_type_scope::get_compare( typeid_t _typeId ) const
    {
        if( m_compare_adapters == nullptr )
        {
            return compare_adapter_interface_ptr::none();
        }

        return m_compare_adapters->compares[_typeId];
    }
    //////////////////////////////////////////////////////////////////////////
    const getattro_adapter_interface_ptr & python_class_type_scope::get_getattro() const
    {
        return m_getattro;
    }
    //////////////////////////////////////////////////////////////////////////
    const mapping_adapter_interface_ptr & python_class_type_scope::get_mapping() const
    {
        return m_mapping;
    }
    //////////////////////////////////////////////////////////////////////////
    const sequence_get_adapter_interface_ptr & python_class_type_scope::get_sequence_get() const
    {
        return m_sequence_get;
    }
    //////////////////////////////////////////////////////////////////////////
    const sequence_set_adapter_interface_ptr & python_class_type_scope::get_sequence_set() const
    {
        return m_sequence_set;
    }
    //////////////////////////////////////////////////////////////////////////
    const number_unary_adapter_interface_ptr & python_class_type_scope::get_number_neg() const
    {
        if( m_number_adapters == nullptr )
        {
            return number_unary_adapter_interface_ptr::none();
        }

        return m_number_adapters->neg;
    }
    //////////////////////////////////////////////////////////////////////////
    const number_unary_adapter_interface_ptr & python_class_type_scope::get_number_abs() const
    {
        if( m_number_adapters == nullptr )
        {
            return number_unary_adapter_interface_ptr::none();
        }

        return m_number_adapters->abs;
    }
    //////////////////////////////////////////////////////////////////////////
    const number_binary_adapter_interface_ptr & python_class_type_scope::get_number_add( typeid_t _typeId ) const
    {
        if( m_number_adapters == nullptr )
        {
            return number_binary_adapter_interface_ptr::none();
        }

        return m_number_adapters->adds[_typeId];
    }
    //////////////////////////////////////////////////////////////////////////
    const number_binary_adapter_interface_ptr & python_class_type_scope::get_number_sub( typeid_t _typeId ) const
    {
        if( m_number_adapters == nullptr )
        {
            return number_binary_adapter_interface_ptr::none();
        }

        return m_number_adapters->subs[_typeId];
    }
    //////////////////////////////////////////////////////////////////////////
    const number_binary_adapter_interface_ptr & python_class_type_scope::get_number_mul( typeid_t _typeId ) const
    {
        if( m_number_adapters == nullptr )
        {
            return number_binary_adapter_interface_ptr::none();
        }

        return m_number_adapters->muls[_typeId];
    }
    //////////////////////////////////////////////////////////////////////////
    const number_binary_adapter_interface_ptr & python_class_type_scope::get_number_div( typeid_t _typeId ) const
    {
        if( m_number_adapters == nullptr )
        {
            return number_binary_adapter_interface_ptr::none();
        }

        return m_number_adapters->divs[_typeId];
    }
    //////////////////////////////////////////////////////////////////////////
    const number_inplace_adapter_interface_ptr & python_class_type_scope::get_number_inplace_add( typeid_t _typeId ) const
    {
        if( m_number_adapters == nullptr )
        {
            return number_inplace_adapter_interface_ptr::none();
        }

        return m_number_adapters->iadds[_typeId];
    }
    //////////////////////////////////////////////////////////////////////////
    const number_inplace_adapter_interface_ptr & python_class_type_scope::get_number_inplace_sub( typeid_t _typeId ) const
    {
        if( m_number_adapters == nullptr )
        {
            return number_inplace_adapter_interface_ptr::none();
        }

        return m_number_adapters->isubs[_typeId];
    }
    //////////////////////////////////////////////////////////////////////////
    const number_inplace_adapter_interface_ptr & python_class_type_scope::get_number_inplace_mul( typeid_t _typeId ) const
    {
        if( m_number_adapters == nullptr )
        {
            return number_inplace_adapter_interface_ptr::none();
        }

        return m_number_adapters->imuls[_typeId];
    }
    //////////////////////////////////////////////////////////////////////////
    const number_inplace_adapter_interface_ptr & python_class_type_scope::get_number_inplace_div( typeid_t _typeId ) const
    {
        if( m_number_adapters == nullptr )
        {
            return number_inplace_adapter_interface_ptr::none();
        }

        return m_number_adapters->idivs[_typeId];
    }
    //////////////////////////////////////////////////////////////////////////
    const smart_pointer_adapter_interface_ptr & python_class_type_scope::get_smart_pointer() const
    {
        return m_smart_pointer;
    }
    //////////////////////////////////////////////////////////////////////////
    const bindable_adapter_interface_ptr & python_class_type_scope::get_bindable() const
    {
        return m_bindable;
    }
    //////////////////////////////////////////////////////////////////////////
}
