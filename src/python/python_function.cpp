#include "python_function.hpp"

#include "python_system.hpp"

#include "pybind/debug.hpp"
#include "pybind/exception.hpp"
#include "pybind/logger.hpp"

#include "config/stdex.hpp"

namespace pybind
{
    //////////////////////////////////////////////////////////////////////////
    struct py_function_type
    {
        PyObject_HEAD

            function_adapter_interface * iadapter;
    };
    //////////////////////////////////////////////////////////////////////////
    static void py_function_dealloc( PyObject * _obj )
    {
        py_function_type * py_self = (py_function_type *)_obj;

        factorable::intrusive_ptr_release( py_self->iadapter );

        PyObject_Free( _obj );
    }
    //////////////////////////////////////////////////////////////////////////
    static function_adapter_interface * py_function_extract_adapter( PyObject * _self )
    {
        py_function_type * function_type = reinterpret_cast<py_function_type *>(_self);

        function_adapter_interface * iadapter = function_type->iadapter;

        return iadapter;
    }
    //////////////////////////////////////////////////////////////////////////
    static PyObject * py_function_kwds( PyObject * _obj, PyObject * _args, PyObject * _kwds )
    {
        kernel_interface * kernel = pybind::get_kernel();

        function_adapter_interface * adapter = py_function_extract_adapter( _obj );

        try
        {
            DEBUG_PYBIND_NOTIFY_BEGIN_BIND_CALL( kernel, "", adapter->getName(), _args, _kwds );
            PyObject * ret = adapter->call( kernel, _args, _kwds );
            DEBUG_PYBIND_NOTIFY_END_BIND_CALL( kernel, "", adapter->getName(), _args, _kwds );

            return ret;
        }
        catch( const pybind_exception & _ex )
        {
            pybind::error_message( "obj %s invalid function call '%s' error '%s'\n"
                , pybind::object_str( _obj )
                , adapter->getName()
                , _ex.what()
            );
        }

        return nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    static PyObject * py_function_args( PyObject * _self, PyObject * _args )
    {
        PyObject * arg = py_function_kwds( _self, _args, nullptr );

        return arg;
    }
    //////////////////////////////////////////////////////////////////////////
    static PyObject * py_function_noargs( PyObject * _self )
    {
        PyObject * arg = py_function_args( _self, nullptr );

        return arg;
    }
    //////////////////////////////////////////////////////////////////////////
    bool function_python::initialize()
    {
        PyTypeObject function_type =
        {
            PyVarObject_HEAD_INIT( nullptr, 0 )
            "function_type_scope",
            sizeof( py_function_type ),
            0,
            &py_function_dealloc,                             /* tp_dealloc */
            0,                    /* tp_print */
            0,                                          /* tp_getattr */
            0,                                          /* tp_setattr */
            0,                                          /* tp_compare */
            0,                                /* tp_repr */
            0,                          /* tp_as_number */
            0,                        /* tp_as_sequence */
            0,                         /* tp_as_mapping */
            0,                      /* tp_hash */
            0,                                          /* tp_call */
            0,                                 /* tp_str */
            0,                    /* tp_getattro */
            0,                                          /* tp_setattro */
            0,                          /* tp_as_buffer */
            0,              /* tp_flags */
            0,                                 /* tp_doc */
            0,                                          /* tp_traverse */
            0,                                          /* tp_clear */
            0,            /* tp_richcompare */
            0,                                          /* tp_weaklistoffset */
            0,                                          /* tp_iter */
            0,                                          /* tp_iternext */
            0,                             /* tp_methods */
            0,                                          /* tp_members */
            0,                                          /* tp_getset */
            0,                         /* tp_base */
            0,                                          /* tp_dict */
            0,                                          /* tp_descr_get */
            0,                                          /* tp_descr_set */
            0,                                          /* tp_dictoffset */
            0,                                          /* tp_init */
            0,                                          /* tp_alloc */
            0,                                 /* tp_new */
            0,                               /* tp_free */
        };

        m_function_type = function_type;

        if( PyType_Ready( &m_function_type ) < 0 )
        {
            pybind::log( "invalid embedding class '%s'\n"
                , m_function_type.tp_name
            );

            return false;
        }

        m_method_native.ml_name = "";
        m_method_native.ml_meth = (PyCFunction)&py_function_kwds;
        m_method_native.ml_flags = METH_CLASS | METH_VARARGS | METH_KEYWORDS;
        m_method_native.ml_doc = "pybind embedding native function";

        m_method_args.ml_name = "";
        m_method_args.ml_meth = (PyCFunction)&py_function_args;
        m_method_args.ml_flags = METH_CLASS | METH_VARARGS;
        m_method_args.ml_doc = "pybind embedding args function";

        m_method_noargs.ml_name = "";
        m_method_noargs.ml_meth = (PyCFunction)&py_function_noargs;
        m_method_noargs.ml_flags = METH_CLASS | METH_NOARGS;
        m_method_noargs.ml_doc = "pybind embedding no args function";

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void function_python::finalize()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * function_python::create_function_adapter( const function_adapter_interface_ptr & _adapter, bool _native )
    {
        uint32_t arity = _adapter->getArity();

        PyMethodDef * method;

        if( _native == true )
        {
            method = &m_method_native;
        }
        else
        {
            if( arity > 0 )
            {
                method = &m_method_args;
            }
            else
            {
                method = &m_method_noargs;
            }
        }

        py_function_type * py_self = (py_function_type *)PyType_GenericAlloc( &m_function_type, 0 );

        factorable::intrusive_ptr_setup( py_self->iadapter, _adapter.get() );

        PyObject * py_func = PyCFunction_New( method, (PyObject*)py_self );
        Py_DECREF( (PyObject *)py_self );

        return py_func;
    }
}