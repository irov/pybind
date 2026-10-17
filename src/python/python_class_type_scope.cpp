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
        static void * get_bound_impl( kernel_interface * _kernel, PyObject * _obj, const char * _slot )
        {
            // As the methods: a slot of an unwrapped object fails in every build.
            // The object is named by its type, as its str may call the slot again.
            void * impl = _kernel->get_class_impl( _obj );

            if( impl == nullptr )
            {
                PyTypeObject * objtype = pybind::object_type( _obj );

                pybind::error_message( "%s unbind object of type '%s'"
                    , _slot
                    , _kernel->type_name( objtype )
                );
            }

            return impl;
        }
        //////////////////////////////////////////////////////////////////////////
        static PyObject * py_callfunc( PyObject * _obj, PyObject * _args, PyObject * _kwds )
        {
            kernel_interface * kernel = pybind::get_kernel();

            void * impl = detail::get_bound_impl( kernel, _obj, "callfunc" );

            if( impl == nullptr )
            {
                return nullptr;
            }

            PyTypeObject * objtype = pybind::object_type( _obj );

            // Held, as the scope and the adapter are in every slot: the adapter
            // may remove the scope, whose finalize releases both.
            class_type_scope_interface_ptr scope = kernel->get_class_scope( objtype );

            if( scope == nullptr )
            {
                pybind::error_message( "obj '%s' type '%s' py_callfunc scope is null"
                    , kernel->object_str( _obj ).c_str()
                    , kernel->type_name( objtype )
                );

                return nullptr;
            }

            method_adapter_interface_ptr adapter = scope->get_call();

            // A bound type derived in C++ inherits the slot through the
            // descriptor of its base, not the adapter, as in the tinypy backend.
            if( adapter == nullptr )
            {
                PyErr_Format( PyExc_TypeError, "'%s' object is not callable"
                    , objtype->tp_name
                );

                return nullptr;
            }

            try
            {
                DEBUG_PYBIND_NOTIFY_BIND_CALL_SCOPE( kernel, scope->get_name(), adapter->getName(), adapter->getCallDebugSilent(), _args, _kwds );

                PyObject * ret = adapter->call( kernel, impl, scope, _args, _kwds );

                return ret;
            }
            catch( const pybind::pybind_exception & _ex )
            {
                pybind::set_error( error_type_e::Runtime, _ex.what() );
            }

            return nullptr;
        }
        //////////////////////////////////////////////////////////////////////////
        static PyObject * py_reprfunc( PyObject * _obj )
        {
            kernel_interface * kernel = pybind::get_kernel();

            void * impl = detail::get_bound_impl( kernel, _obj, "reprfunc" );

            if( impl == nullptr )
            {
                return nullptr;
            }

            PyTypeObject * objtype = pybind::object_type( _obj );

            class_type_scope_interface_ptr scope = kernel->get_class_scope( objtype );

            if( scope == nullptr )
            {
                PyObject * ret = pybind::string_from_char( "[pybind scope is nullptr]" );

                return ret;
            }

            repr_adapter_interface_ptr adapter = scope->get_repr();

            // A bound type derived in C++ inherits the slot through the
            // descriptor of its base, not the adapter, as in the tinypy backend.
            if( adapter == nullptr )
            {
                PyObject * ret = PyBaseObject_Type.tp_repr( _obj );

                return ret;
            }

            try
            {
                PyObject * ret = adapter->repr( kernel, impl, scope );

                return ret;
            }
            catch( const pybind::pybind_exception & _ex )
            {
                pybind::set_error( error_type_e::Runtime, _ex.what() );
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

            void * impl = detail::get_bound_impl( kernel, _obj, "hash" );

            if( impl == nullptr )
            {
                return -1;
            }

            PyTypeObject * objtype = pybind::object_type( _obj );

            class_type_scope_interface_ptr scope = kernel->get_class_scope( objtype );

            if( scope == nullptr )
            {
                pybind::error_message( "obj '%s' type '%s' py_hash scope is null"
                    , kernel->object_str( _obj ).c_str()
                    , kernel->type_name( objtype )
                );

                return -1;
            }

            hash_adapter_interface_ptr adapter = scope->get_hash();

            // A bound type derived in C++ inherits the slot through the
            // descriptor of its base, not the adapter, as in the tinypy backend.
            if( adapter == nullptr )
            {
                long py_object_hash = PyBaseObject_Type.tp_hash( _obj );

                return py_object_hash;
            }

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
                pybind::set_error( error_type_e::Runtime, _ex.what() );
            }

            return -1;
        }
        //////////////////////////////////////////////////////////////////////////
        static PyObject * py_richcompare( PyObject * _obj, PyObject * _compare, int _op )
        {
            kernel_interface * kernel = pybind::get_kernel();

            void * impl = detail::get_bound_impl( kernel, _obj, "richcompare" );

            if( impl == nullptr )
            {
                return nullptr;
            }

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

            class_type_scope_interface_ptr scope = kernel->get_class_scope( objtype );

            if( scope == nullptr )
            {
                pybind::error_message( "obj '%s' type '%s' py_richcompare scope is null"
                    , kernel->object_str( _obj ).c_str()
                    , kernel->type_name( objtype )
                );

                return nullptr;
            }

            typeid_t compare_type_id = kernel->get_object_type_id( _compare );

            compare_adapter_interface_ptr adapter = scope->get_compare( compare_type_id );

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
                    pybind::set_error( error_type_e::Runtime, _ex.what() );

                    return nullptr;
                }
            }

            typeid_t base_type_id = kernel->get_object_type_id( _obj );

            compare_adapter_interface_ptr base_adapter = scope->get_compare( base_type_id );

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
                    pybind::set_error( error_type_e::Runtime, _ex.what() );

                    return nullptr;
                }
            }

            return kernel->ret_not_implemented();
        }
        //////////////////////////////////////////////////////////////////////////
        static PyObject * py_getattro( PyObject * _obj, PyObject * _key )
        {
            kernel_interface * kernel = pybind::get_kernel();

            void * impl = detail::get_bound_impl( kernel, _obj, "getattro" );

            if( impl == nullptr )
            {
                return nullptr;
            }

            PyTypeObject * objtype = pybind::object_type( _obj );

            class_type_scope_interface_ptr scope = kernel->get_class_scope( objtype );

            if( scope == nullptr )
            {
                pybind::error_message( "obj '%s' type '%s' py_getattro scope is null"
                    , kernel->object_str( _obj ).c_str()
                    , kernel->type_name( objtype )
                );

                return nullptr;
            }

            getattro_adapter_interface_ptr adapter = scope->get_getattro();

            // A bound type derived in C++ inherits the slot through the
            // descriptor of its base, not the adapter, as in the tinypy backend.
            if( adapter == nullptr )
            {
                PyObject * res = PyObject_GenericGetAttr( _obj, _key );

                return res;
            }

            try
            {
                DEBUG_PYBIND_NOTIFY_BIND_CALL_SCOPE( kernel, scope->get_name(), adapter->getName(), adapter->getCallDebugSilent(), _key, nullptr );

                PyObject * res = adapter->call( kernel, impl, scope, _key );

                return res;
            }
            catch( const pybind::pybind_exception & _ex )
            {
                pybind::set_error( error_type_e::Runtime, _ex.what() );
            }

            return nullptr;
        }
        //////////////////////////////////////////////////////////////////////////
        static PyObject * py_subscript( PyObject * _obj, PyObject * _key )
        {
            kernel_interface * kernel = pybind::get_kernel();

            void * impl = detail::get_bound_impl( kernel, _obj, "subscript" );

            if( impl == nullptr )
            {
                return nullptr;
            }

            PyTypeObject * objtype = pybind::object_type( _obj );

            class_type_scope_interface_ptr scope = kernel->get_class_scope( objtype );

            if( scope == nullptr )
            {
                pybind::error_message( "obj '%s' type '%s' py_subscript scope is null"
                    , kernel->object_str( _obj ).c_str()
                    , kernel->type_name( objtype )
                );

                return nullptr;
            }

            mapping_adapter_interface_ptr adapter = scope->get_mapping();

            // A bound type derived in C++ inherits the slot through the
            // descriptor of its base, not the adapter, as in the tinypy backend;
            // as PyObject_GetItem without mp_subscript, its own sequence serves.
            if( adapter == nullptr )
            {
                if( scope->get_sequence_get() != nullptr && PyIndex_Check( _key ) != 0 )
                {
                    Py_ssize_t index = PyNumber_AsSsize_t( _key, PyExc_IndexError );

                    if( index == -1 && PyErr_Occurred() != nullptr )
                    {
                        return nullptr;
                    }

                    PyObject * res = PySequence_GetItem( _obj, index );

                    return res;
                }

                PyErr_Format( PyExc_TypeError, "'%s' object has no attribute '__getitem__'"
                    , objtype->tp_name
                );

                return nullptr;
            }

            try
            {
                DEBUG_PYBIND_NOTIFY_BIND_CALL_SCOPE( kernel, scope->get_name(), adapter->getName(), adapter->getCallDebugSilent(), _key, nullptr );

                PyObject * res = adapter->call( kernel, impl, scope, _key );

                return res;
            }
            catch( const pybind::pybind_exception & _ex )
            {
                pybind::set_error( error_type_e::Runtime, _ex.what() );
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

            void * impl = detail::get_bound_impl( kernel, _obj, "item get" );

            if( impl == nullptr )
            {
                return nullptr;
            }

            PyTypeObject * objtype = pybind::object_type( _obj );

            class_type_scope_interface_ptr scope = kernel->get_class_scope( objtype );

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

            sequence_get_adapter_interface_ptr adapter = scope->get_sequence_get();

            // A bound type derived in C++ inherits the slot through the
            // descriptor of its base, not the adapter, as in the tinypy backend.
            if( adapter == nullptr )
            {
                PyErr_Format( PyExc_TypeError, "'%s' object does not support indexing"
                    , objtype->tp_name
                );

                return nullptr;
            }

            try
            {
                DEBUG_PYBIND_NOTIFY_BIND_CALL_SCOPE( kernel, scope->get_name(), adapter->getName(), adapter->getCallDebugSilent(), nullptr, nullptr );

                PyObject * res = adapter->call( kernel, impl, scope, (size_t)_index );

                return res;
            }
            catch( const pybind::pybind_exception & _ex )
            {
                pybind::set_error( error_type_e::Runtime, _ex.what() );
            }

            return nullptr;
        }
        //////////////////////////////////////////////////////////////////////////
        static int py_item_set( PyObject * _obj, Py_ssize_t _index, PyObject * _value )
        {
            kernel_interface * kernel = pybind::get_kernel();

            void * impl = detail::get_bound_impl( kernel, _obj, "item set" );

            if( impl == nullptr )
            {
                return -1;
            }

            PyTypeObject * objtype = pybind::object_type( _obj );

            class_type_scope_interface_ptr scope = kernel->get_class_scope( objtype );

            if( scope == nullptr )
            {
                pybind::error_message( "obj '%s' type '%s' py_item_set scope is null"
                    , kernel->object_str( _obj ).c_str()
                    , kernel->type_name( objtype )
                );

                return -1;
            }

            // As PyObject_DelItem for a sequence that can only be assigned, and
            // the tinypy backend: whatever its index.
            if( _value == nullptr )
            {
                PyErr_Format( PyExc_TypeError, "'%s' object doesn't support item deletion"
                    , objtype->tp_name
                );

                return -1;
            }

            if( _index < 0 )
            {
                PyErr_SetString( PyExc_IndexError, "index out of range" );

                return -1;
            }

            sequence_set_adapter_interface_ptr adapter = scope->get_sequence_set();

            // A bound type derived in C++ inherits the slot through the
            // descriptor of its base, not the adapter, as in the tinypy backend.
            if( adapter == nullptr )
            {
                PyErr_Format( PyExc_TypeError, "'%s' object does not support item assignment"
                    , objtype->tp_name
                );

                return -1;
            }

            try
            {
                DEBUG_PYBIND_NOTIFY_BIND_CALL_SCOPE( kernel, scope->get_name(), adapter->getName(), adapter->getCallDebugSilent(), nullptr, nullptr );

                adapter->call( kernel, impl, scope, (size_t)_index, _value );

                return 0;
            }
            catch( const pybind::pybind_exception & _ex )
            {
                pybind::set_error( error_type_e::Runtime, _ex.what() );
            }

            return -1;
        }
        //////////////////////////////////////////////////////////////////////////
        static PySequenceMethods py_as_sequence = {
            (lenfunc)0,                       /* sq_length */
            (binaryfunc)0,                    /* sq_concat */
            (ssizeargfunc)0,                  /* sq_repeat */
            (ssizeargfunc)&detail::py_item_get,                    /* sq_item */
            nullptr,              /* sq_slice, was_sq_slice of Python 3 */
            (ssizeobjargproc)&detail::py_item_set,             /* sq_ass_item */
            nullptr,       /* sq_ass_slice, was_sq_ass_slice of Python 3 */
            (objobjproc)0,                  /* sq_contains */
            (binaryfunc)0,            /* sq_inplace_concat */
            (ssizeargfunc)0,          /* sq_inplace_repeat */
        };
        //////////////////////////////////////////////////////////////////////////
        static PyObject * py_nb_unary_method( kernel_interface * _kernel, PyObject * _obj, const class_type_scope_interface_ptr & _scope, const number_unary_adapter_interface_ptr & _adapter )
        {
            void * impl = detail::get_bound_impl( _kernel, _obj, "nb unary" );

            if( impl == nullptr )
            {
                return nullptr;
            }

            DEBUG_PYBIND_NOTIFY_BIND_CALL_SCOPE( _kernel, _scope->get_name(), _adapter->getName(), _adapter->getCallDebugSilent(), nullptr, nullptr );

            PyObject * res = _adapter->call( _kernel, impl, _scope );

            return res;
        }
        //////////////////////////////////////////////////////////////////////////
        static PyObject * py_nb_binary_method( kernel_interface * _kernel, PyObject * _obj, PyObject * _value, const class_type_scope_interface_ptr & _scope, const number_binary_adapter_interface_ptr & _adapter, bool _rotate )
        {
            void * impl = detail::get_bound_impl( _kernel, _obj, "nb binary" );

            if( impl == nullptr )
            {
                return nullptr;
            }

            DEBUG_PYBIND_NOTIFY_BIND_CALL_SCOPE( _kernel, _scope->get_name(), _adapter->getName(), _adapter->getCallDebugSilent(), nullptr, nullptr );

            PyObject * res = _adapter->call( _kernel, impl, _scope, _value, _rotate );

            // An adapter that cannot take the operand returns null; one that
            // raised keeps its error rather than letting the other operand run.
            if( res == nullptr && PyErr_Occurred() == nullptr )
            {
                return _kernel->ret_not_implemented();
            }

            return res;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool py_nb_inplace_method( kernel_interface * _kernel, PyObject * _obj, PyObject * _value, const class_type_scope_interface_ptr & _scope, const number_inplace_adapter_interface_ptr & _adapter )
        {
            void * impl = detail::get_bound_impl( _kernel, _obj, "nb inplace" );

            if( impl == nullptr )
            {
                return false;
            }

            DEBUG_PYBIND_NOTIFY_BIND_CALL_SCOPE( _kernel, _scope->get_name(), _adapter->getName(), _adapter->getCallDebugSilent(), nullptr, nullptr );

            _adapter->call( _kernel, impl, _scope, _value );

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        static PyObject * py_nb_neg( PyObject * _obj )
        {
            kernel_interface * kernel = pybind::get_kernel();

            PyTypeObject * objtype = pybind::object_type( _obj );

            class_type_scope_interface_ptr scope = kernel->get_class_scope( objtype );

            if( scope != nullptr )
            {
                number_unary_adapter_interface_ptr adapter = scope->get_number_neg();

                if( adapter != nullptr )
                {
                    try
                    {
                        PyObject * result = detail::py_nb_unary_method( kernel, _obj, scope, adapter );

                        return result;
                    }
                    catch( const pybind_exception & _ex )
                    {
                        pybind::set_error( error_type_e::Runtime, _ex.what() );

                        return nullptr;
                    }
                }
            }

            // A removed scope or one without the operator; a unary operator has
            // no reflected operand to fall back to.
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

            class_type_scope_interface_ptr scope = kernel->get_class_scope( objtype );

            if( scope != nullptr )
            {
                number_unary_adapter_interface_ptr adapter = scope->get_number_abs();

                if( adapter != nullptr )
                {
                    try
                    {
                        PyObject * result = detail::py_nb_unary_method( kernel, _obj, scope, adapter );

                        return result;
                    }
                    catch( const pybind_exception & _ex )
                    {
                        pybind::set_error( error_type_e::Runtime, _ex.what() );

                        return nullptr;
                    }
                }
            }

            // A removed scope or one without the operator; a unary operator has
            // no reflected operand to fall back to.
            PyErr_Format( PyExc_TypeError, "bad operand type for abs(): '%s'"
                , objtype->tp_name
            );

            return nullptr;
        }
        //////////////////////////////////////////////////////////////////////////
        static PyObject * py_nb_add2( kernel_interface * _kernel, PyObject * _obj, PyObject * _value, bool _rotate )
        {
            PyTypeObject * objtype = pybind::object_type( _obj );

            class_type_scope_interface_ptr scope = _kernel->get_class_scope( objtype );

            if( scope == nullptr )
            {
                return _kernel->ret_not_implemented();
            }

            typeid_t value_type_id = _kernel->get_object_type_id( _value );

            number_binary_adapter_interface_ptr adapter = scope->get_number_add( value_type_id );

            if( adapter != nullptr )
            {
                try
                {
                    PyObject * result = detail::py_nb_binary_method( _kernel, _obj, _value, scope, adapter, _rotate );

                    return result;
                }
                catch( const pybind_exception & _ex )
                {
                    pybind::set_error( error_type_e::Runtime, _ex.what() );

                    return nullptr;
                }
            }

            typeid_t base_type_id = _kernel->get_object_type_id( _obj );

            number_binary_adapter_interface_ptr base_adapter = scope->get_number_add( base_type_id );

            if( base_adapter != nullptr )
            {
                try
                {
                    PyObject * result = detail::py_nb_binary_method( _kernel, _obj, _value, scope, base_adapter, _rotate );

                    return result;
                }
                catch( const pybind_exception & _ex )
                {
                    pybind::set_error( error_type_e::Runtime, _ex.what() );

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

            class_type_scope_interface_ptr scope = _kernel->get_class_scope( objtype );

            if( scope == nullptr )
            {
                return _kernel->ret_not_implemented();
            }

            typeid_t value_type_id = _kernel->get_object_type_id( _value );

            number_binary_adapter_interface_ptr adapter = scope->get_number_sub( value_type_id );

            if( adapter != nullptr )
            {
                try
                {
                    PyObject * result = detail::py_nb_binary_method( _kernel, _obj, _value, scope, adapter, _rotate );

                    return result;
                }
                catch( const pybind_exception & _ex )
                {
                    pybind::set_error( error_type_e::Runtime, _ex.what() );

                    return nullptr;
                }
            }

            typeid_t base_type_id = _kernel->get_object_type_id( _obj );

            number_binary_adapter_interface_ptr base_adapter = scope->get_number_sub( base_type_id );

            if( base_adapter != nullptr )
            {
                try
                {
                    PyObject * result = detail::py_nb_binary_method( _kernel, _obj, _value, scope, base_adapter, _rotate );

                    return result;
                }
                catch( const pybind_exception & _ex )
                {
                    pybind::set_error( error_type_e::Runtime, _ex.what() );

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

            class_type_scope_interface_ptr scope = _kernel->get_class_scope( objtype );

            if( scope == nullptr )
            {
                return _kernel->ret_not_implemented();
            }

            typeid_t value_type_id = _kernel->get_object_type_id( _value );

            number_binary_adapter_interface_ptr adapter = scope->get_number_mul( value_type_id );

            if( adapter != nullptr )
            {
                try
                {
                    PyObject * result = detail::py_nb_binary_method( _kernel, _obj, _value, scope, adapter, _rotate );

                    return result;
                }
                catch( const pybind_exception & _ex )
                {
                    pybind::set_error( error_type_e::Runtime, _ex.what() );

                    return nullptr;
                }
            }

            typeid_t base_type_id = _kernel->get_object_type_id( _obj );

            number_binary_adapter_interface_ptr base_adapter = scope->get_number_mul( base_type_id );

            if( base_adapter != nullptr )
            {
                try
                {
                    PyObject * result = detail::py_nb_binary_method( _kernel, _obj, _value, scope, base_adapter, _rotate );

                    return result;
                }
                catch( const pybind_exception & _ex )
                {
                    pybind::set_error( error_type_e::Runtime, _ex.what() );

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

            class_type_scope_interface_ptr scope = _kernel->get_class_scope( objtype );

            if( scope == nullptr )
            {
                return _kernel->ret_not_implemented();
            }

            typeid_t value_type_id = _kernel->get_object_type_id( _value );

            number_binary_adapter_interface_ptr adapter = scope->get_number_div( value_type_id );

            if( adapter != nullptr )
            {
                try
                {
                    PyObject * result = detail::py_nb_binary_method( _kernel, _obj, _value, scope, adapter, _rotate );

                    return result;
                }
                catch( const pybind_exception & _ex )
                {
                    pybind::set_error( error_type_e::Runtime, _ex.what() );

                    return nullptr;
                }
            }

            typeid_t base_type_id = _kernel->get_object_type_id( _obj );

            number_binary_adapter_interface_ptr base_adapter = scope->get_number_div( base_type_id );

            if( base_adapter != nullptr )
            {
                try
                {
                    PyObject * result = detail::py_nb_binary_method( _kernel, _obj, _value, scope, base_adapter, _rotate );

                    return result;
                }
                catch( const pybind_exception & _ex )
                {
                    pybind::set_error( error_type_e::Runtime, _ex.what() );

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

            class_type_scope_interface_ptr scope = kernel->get_class_scope( objtype );

            if( scope == nullptr )
            {
                return kernel->ret_not_implemented();
            }

            typeid_t value_type_id = kernel->get_object_type_id( _value );

            number_inplace_adapter_interface_ptr adapter = scope->get_number_inplace_add( value_type_id );

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
                catch( const pybind_exception & _ex )
                {
                    pybind::set_error( error_type_e::Runtime, _ex.what() );

                    return nullptr;
                }
            }

            typeid_t base_type_id = kernel->get_object_type_id( _obj );

            number_inplace_adapter_interface_ptr base_adapter = scope->get_number_inplace_add( base_type_id );

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
                catch( const pybind_exception & _ex )
                {
                    pybind::set_error( error_type_e::Runtime, _ex.what() );

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

            class_type_scope_interface_ptr scope = kernel->get_class_scope( objtype );

            if( scope == nullptr )
            {
                return kernel->ret_not_implemented();
            }

            typeid_t value_type_id = kernel->get_object_type_id( _value );

            number_inplace_adapter_interface_ptr adapter = scope->get_number_inplace_sub( value_type_id );

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
                catch( const pybind_exception & _ex )
                {
                    pybind::set_error( error_type_e::Runtime, _ex.what() );

                    return nullptr;
                }
            }

            typeid_t base_type_id = kernel->get_object_type_id( _obj );

            number_inplace_adapter_interface_ptr base_adapter = scope->get_number_inplace_sub( base_type_id );

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
                catch( const pybind_exception & _ex )
                {
                    pybind::set_error( error_type_e::Runtime, _ex.what() );

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

            class_type_scope_interface_ptr scope = kernel->get_class_scope( objtype );

            if( scope == nullptr )
            {
                return kernel->ret_not_implemented();
            }

            typeid_t value_type_id = kernel->get_object_type_id( _value );

            number_inplace_adapter_interface_ptr adapter = scope->get_number_inplace_mul( value_type_id );

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
                catch( const pybind_exception & _ex )
                {
                    pybind::set_error( error_type_e::Runtime, _ex.what() );

                    return nullptr;
                }
            }

            typeid_t base_type_id = kernel->get_object_type_id( _obj );

            number_inplace_adapter_interface_ptr base_adapter = scope->get_number_inplace_mul( base_type_id );

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
                catch( const pybind_exception & _ex )
                {
                    pybind::set_error( error_type_e::Runtime, _ex.what() );

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

            class_type_scope_interface_ptr scope = kernel->get_class_scope( objtype );

            if( scope == nullptr )
            {
                return kernel->ret_not_implemented();
            }

            typeid_t value_type_id = kernel->get_object_type_id( _value );

            number_inplace_adapter_interface_ptr adapter = scope->get_number_inplace_div( value_type_id );

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
                catch( const pybind_exception & _ex )
                {
                    pybind::set_error( error_type_e::Runtime, _ex.what() );

                    return nullptr;
                }
            }

            typeid_t base_type_id = kernel->get_object_type_id( _obj );

            number_inplace_adapter_interface_ptr base_adapter = scope->get_number_inplace_div( base_type_id );

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
                catch( const pybind_exception & _ex )
                {
                    pybind::set_error( error_type_e::Runtime, _ex.what() );

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
            (binaryfunc)0, /*nb_remainder*/
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
            (binaryfunc)0, /*nb_inplace_remainder*/
            (ternaryfunc)0, /*nb_inplace_power*/
            (binaryfunc)0, /*nb_inplace_lshift*/
            (binaryfunc)0, /*nb_inplace_rshift*/
            (binaryfunc)0, /*nb_inplace_and*/
            (binaryfunc)0, /*nb_inplace_xor*/
            (binaryfunc)0, /*nb_inplace_or*/

            (binaryfunc)0, /*nb_floor_divide*/
            (binaryfunc)&detail::py_nb_div, /*nb_true_divide*/
            (binaryfunc)0, /*nb_inplace_floor_divide*/
            (binaryfunc)&detail::py_nb_inplace_div, /*nb_inplace_true_divide*/

            (unaryfunc)0, /*nb_index*/

            (binaryfunc)0, /*nb_matrix_multiply*/
            (binaryfunc)0, /*nb_inplace_matrix_multiply*/
        };
        //////////////////////////////////////////////////////////////////////////
#	endif
        //////////////////////////////////////////////////////////////////////////
        static void ** slot_pointer( PyTypeObject * _type, int _offset )
        {
            // As slotptr of typeobject.c: a slot offset is into PyHeapTypeObject.
            size_t offset = (size_t)_offset;
            char * slots;

            if( offset >= offsetof( PyHeapTypeObject, as_sequence ) )
            {
                slots = reinterpret_cast<char *>(_type->tp_as_sequence);
                offset -= offsetof( PyHeapTypeObject, as_sequence );
            }
            else if( offset >= offsetof( PyHeapTypeObject, as_mapping ) )
            {
                slots = reinterpret_cast<char *>(_type->tp_as_mapping);
                offset -= offsetof( PyHeapTypeObject, as_mapping );
            }
            else if( offset >= offsetof( PyHeapTypeObject, as_number ) )
            {
                slots = reinterpret_cast<char *>(_type->tp_as_number);
                offset -= offsetof( PyHeapTypeObject, as_number );
            }
            else
            {
                slots = reinterpret_cast<char *>(_type);
            }

            if( slots == nullptr )
            {
                return nullptr;
            }

            return reinterpret_cast<void **>(slots + offset);
        }
        //////////////////////////////////////////////////////////////////////////
        static bool ready_slots_type( PyTypeObject * _type, const char * _name )
        {
            reinterpret_cast<PyObject *>(_type)->ob_refcnt = 1;
            _type->tp_name = _name;
            _type->tp_flags = Py_TPFLAGS_DEFAULT;

            if( PyType_Ready( _type ) < 0 )
            {
                pybind::check_error();

                return false;
            }

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        static void add_slot_wrappers( PyTypeObject * _type )
        {
            // As add_operators names the slots of a static type, a bound type
            // names its slots with wrapper descriptors, so type_new keeps them in
            // a Python subclass, which composes its special methods with them.
            // PyType_Ready gives the descriptors to these templates of the bound
            // slots; the sequence one names sq_item, which mp_subscript shadows.
            static PyTypeObject s_slots_type;
            static PyTypeObject s_sequence_slots_type;

            if( s_slots_type.tp_dict == nullptr )
            {
                s_slots_type.tp_call = &detail::py_callfunc;
                s_slots_type.tp_repr = &detail::py_reprfunc;
                s_slots_type.tp_hash = (hashfunc)&detail::py_hash;
                s_slots_type.tp_richcompare = &detail::py_richcompare;
                s_slots_type.tp_getattro = &detail::py_getattro;
                s_slots_type.tp_as_number = &detail::py_as_number;
                s_slots_type.tp_as_mapping = &detail::py_as_mapping;
                s_slots_type.tp_as_sequence = &detail::py_as_sequence;

                s_sequence_slots_type.tp_as_sequence = &detail::py_as_sequence;

                if( detail::ready_slots_type( &s_slots_type, "pybind_slots" ) == false )
                {
                    return;
                }

                if( detail::ready_slots_type( &s_sequence_slots_type, "pybind_sequence_slots" ) == false )
                {
                    return;
                }
            }

            for( PyTypeObject * slots_type : {&s_slots_type, &s_sequence_slots_type} )
            {
                PyObject * py_key;
                PyObject * py_value;
                Py_ssize_t position = 0;

                while( PyDict_Next( slots_type->tp_dict, &position, &py_key, &py_value ) != 0 )
                {
                    if( Py_TYPE( py_value ) != &PyWrapperDescr_Type )
                    {
                        continue;
                    }

                    PyWrapperDescrObject * py_descr = reinterpret_cast<PyWrapperDescrObject *>(py_value);

                    void ** slot = detail::slot_pointer( _type, py_descr->d_base->offset );

                    if( slot == nullptr || *slot != py_descr->d_wrapped )
                    {
                        continue;
                    }

                    PyObject * py_existing = PyDict_GetItem( _type->tp_dict, py_key );

                    if( py_existing != nullptr )
                    {
                        // As mp_subscript shadows sq_item: a wrapper of the slots
                        // template replaces one this code added for another slot
                        // of its name, as for sq_item when the sequence is set
                        // before the mapping.
                        if( slots_type != &s_slots_type || Py_TYPE( py_existing ) != &PyWrapperDescr_Type )
                        {
                            continue;
                        }

                        PyWrapperDescrObject * py_existing_descr = reinterpret_cast<PyWrapperDescrObject *>(py_existing);
                        PyTypeObject * py_existing_type = reinterpret_cast<PyDescrObject *>(py_existing)->d_type;

                        if( py_existing_type != _type || py_existing_descr->d_base == py_descr->d_base )
                        {
                            continue;
                        }
                    }

                    PyObject * py_wrapper = PyDescr_NewWrapper( _type, py_descr->d_base, py_descr->d_wrapped );

                    if( py_wrapper == nullptr )
                    {
                        pybind::check_error();

                        continue;
                    }

                    if( PyDict_SetItem( _type->tp_dict, py_key, py_wrapper ) < 0 )
                    {
                        pybind::check_error();
                    }

                    Py_DECREF( py_wrapper );
                }
            }

            PyType_Modified( _type );
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

#if defined(PYBIND_DEBUG)
                // A bound object keeps its impl or pod in the layout its bases have.
                PyTypeObject * py_layout_type = m_kernel->get_pod_type( m_pod_size, m_pod_hash );

                if( py_layout_type == nullptr || PyType_IsSubtype( py_base, py_layout_type ) == 0 )
                {
                    pybind::xdecref( (PyObject *)py_layout_type );
                    pybind::decref( py_bases );
                    pybind::decref( py_name );

                    pybind::throw_exception( "scope '%s' layout of pod %zu does not fit base '%s'"
                        , m_name
                        , m_pod_size
                        , mc.scope->get_name()
                    );

                    return false;
                }

                pybind::decref( (PyObject *)py_layout_type );
#endif

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

        PyObject * new_pytypeobject = PyObject_Call( (PyObject *)&PyType_Type, py_args, nullptr );

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

        pybind::throw_exception( "scope '%s' find_method '%s' not found"
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

        pybind::throw_exception( "scope '%s' find_member '%s' not found"
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
        detail::add_slot_wrappers( m_pytypeobject );
    }
    //////////////////////////////////////////////////////////////////////////
    void python_class_type_scope::set_repr( const repr_adapter_interface_ptr & _irepr )
    {
        m_repr = _irepr;

        m_pytypeobject->tp_repr = &detail::py_reprfunc;
        detail::add_slot_wrappers( m_pytypeobject );
    }
    //////////////////////////////////////////////////////////////////////////
    void python_class_type_scope::set_hash( const hash_adapter_interface_ptr & _ihash )
    {
        m_hash = _ihash;

        m_pytypeobject->tp_hash = (hashfunc)&detail::py_hash;
        detail::add_slot_wrappers( m_pytypeobject );
    }
    //////////////////////////////////////////////////////////////////////////
    void python_class_type_scope::set_compare( typeid_t _typeId, const compare_adapter_interface_ptr & _icompare )
    {
        compare_adapter_t * compare_adapters = this->get_compare_adapters();
        compare_adapters->compares[_typeId] = _icompare;

        m_pytypeobject->tp_richcompare = &detail::py_richcompare;
        detail::add_slot_wrappers( m_pytypeobject );
    }
    //////////////////////////////////////////////////////////////////////////
    void python_class_type_scope::set_getattro( const getattro_adapter_interface_ptr & _igetattro )
    {
        m_getattro = _igetattro;

        m_pytypeobject->tp_getattro = &detail::py_getattro;
        detail::add_slot_wrappers( m_pytypeobject );
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
        detail::add_slot_wrappers( m_pytypeobject );
    }
    //////////////////////////////////////////////////////////////////////////
    void python_class_type_scope::setup_sequence_slots_()
    {
        PyHeapTypeObject * heap_type = reinterpret_cast<PyHeapTypeObject *>(m_pytypeobject);
        heap_type->as_sequence = detail::py_as_sequence;
        m_pytypeobject->tp_as_sequence = &heap_type->as_sequence;
        detail::add_slot_wrappers( m_pytypeobject );
    }
    //////////////////////////////////////////////////////////////////////////
    void python_class_type_scope::setup_mapping_slots_()
    {
        PyHeapTypeObject * heap_type = reinterpret_cast<PyHeapTypeObject *>(m_pytypeobject);
        heap_type->as_mapping = detail::py_as_mapping;
        m_pytypeobject->tp_as_mapping = &heap_type->as_mapping;
        detail::add_slot_wrappers( m_pytypeobject );
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
        // As the tinypy backend: the class of the scope is its impl, which the
        // search through the bases also stops at.
        if( _info == m_typeId )
        {
            return _impl;
        }

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

        pybind::helper::wrap_pod_ptr( py_self, _impl, false, m_typeId );

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

        pybind::helper::wrap_pod_ptr( py_self, _impl, true, m_typeId );

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

        pybind::helper::wrap_pod_weak( py_self, _impl, true, m_typeId );

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

        pybind::helper::wrap_pod( py_self, _impl, m_pod_size, m_pod_hash, m_typeId );

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

        // A constructed counted object belongs to its smart pointer, while the
        // object of a new adapter is released by its destroy adapter.
        if( m_smart_pointer != nullptr && m_new == nullptr )
        {
            return;
        }

        // Held, as the destructor may remove the scope, whose finalize releases
        // the adapter.
        destroy_adapter_interface_ptr destructor = m_destructor;

        if( destructor != nullptr )
        {
            destructor->call( m_kernel, stdex::intrusive_from_this( this ), _impl );
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
