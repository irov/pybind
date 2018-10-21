#include "python_member.hpp"
#include "python_system.hpp"

#include "pybind/class_type_scope_interface.hpp"

namespace pybind
{
    //////////////////////////////////////////////////////////////////////////
    struct py_member_type
    {
        PyObject_HEAD

            member_adapter_interface * iadapter;
    };
    //////////////////////////////////////////////////////////////////////////
    static void py_member_dealloc( PyObject * _obj )
    {
        py_member_type * mt = (py_member_type *)(_obj);

        stdex::intrusive_ptr_release( mt->iadapter );

        PyObject_Free( _obj );
    }
    //////////////////////////////////////////////////////////////////////////
    static PyObject * py_getmethod( PyObject * _member, PyObject * _args )
    {
        kernel_interface * kernel = pybind::get_kernel();

        py_member_type * mt = (py_member_type *)_member;

        PyObject * py_self = PyTuple_GetItem( _args, 0 );

        try
        {
            void * impl = kernel->get_class_impl( py_self );

            if( impl == nullptr )
            {
                error_message( "py_getmethod: unbind object" );

                return nullptr;
            }

            const class_type_scope_interface_ptr & scope = kernel->get_class_scope( py_self->ob_type );

            PyObject * py_method = mt->iadapter->get( kernel, impl, scope );

            return py_method;
        }
        catch( const pybind_exception & _ex )
        {
            pybind::error_message( "py_getmethod: obj %s method %s invalid call exception '%s'"
                , pybind::object_str( py_self )
                , mt->iadapter->getName()
                , _ex.what()
            );
        }

        return nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    static PyObject * py_setmethod( PyObject * _member, PyObject * _args )
    {
        kernel_interface * kernel = pybind::get_kernel();

        py_member_type * mt = (py_member_type *)(_member);

        PyObject * py_self = PyTuple_GetItem( _args, 0 );
        PyObject * py_value = PyTuple_GetItem( _args, 1 );

        try
        {
            void * impl = kernel->get_class_impl( py_self );

            if( impl == nullptr )
            {
                error_message( "py_setmethod: unbind object" );

                return nullptr;
            }

            const class_type_scope_interface_ptr & scope = kernel->get_class_scope( py_self->ob_type );

            mt->iadapter->set( kernel, impl, py_value, scope );

            Py_RETURN_NONE;
        }
        catch( const pybind_exception & _ex )
        {
            pybind::error_message( "py_setmethod: obj %s method %s invalid call exception '%s'"
                , pybind::object_str( py_self )
                , mt->iadapter->getName()
                , _ex.what()
            );
        }

        return nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    bool member_python::initialize()
    {
        m_setmethod.ml_name = "setmethod";
        m_setmethod.ml_meth = &py_setmethod;
        m_setmethod.ml_flags = METH_CLASS | METH_VARARGS;
        m_setmethod.ml_doc = "pybind embedding member set";

        m_getmethod.ml_name = "getmethod";
        m_getmethod.ml_meth = &py_getmethod;
        m_getmethod.ml_flags = METH_CLASS | METH_VARARGS;
        m_getmethod.ml_doc = "pybind embedding member get";

        PyTypeObject member_type =
        {
            PyVarObject_HEAD_INIT( &PyType_Type, 0 )
            "member_type_scope",
            sizeof( py_member_type ),
            0,
            &py_member_dealloc,                             /* tp_dealloc */
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

        m_member_type = member_type;

        if( PyType_Ready( &m_member_type ) < 0 )
        {
            pybind::log( "invalid embedding member type '%s' \n"
                , m_member_type.tp_name
            );

            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void member_python::finalize()
    {

    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * member_python::create_member_adapter( const member_adapter_interface_ptr & _iadapter )
    {
        py_member_type * py_member = (py_member_type *)PyType_GenericAlloc( &m_member_type, 0 );

        stdex::intrusive_ptr_setup( py_member->iadapter, _iadapter );

        PyObject * py_get = PyCFunction_New( &m_getmethod, (PyObject*)py_member );
        PyObject * py_set = PyCFunction_New( &m_setmethod, (PyObject*)py_member );

        Py_DecRef( (PyObject *)py_member );

        PyObject * py_result = PyObject_CallFunction( (PyObject*)&PyProperty_Type, const_cast<char *>("OOss"), py_get, py_set, 0, "member_type_scope" );
        Py_DecRef( py_get );
        Py_DecRef( py_set );

        return py_result;
    }
}
