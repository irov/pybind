#include "python_method.hpp"
#include "python_system.hpp"

#include "pybind/class_type_scope_interface.hpp"

#include "pybind/logger.hpp"
#include "pybind/debug.hpp"

namespace pybind
{
    //////////////////////////////////////////////////////////////////////////
    struct py_method_generator_type
    {
        PyObject_HEAD
            PyTypeObject * method_caller_type;
        method_adapter_interface * iadapter;
        PyTypeObject * classtype;
    };
    //////////////////////////////////////////////////////////////////////////
    struct py_method_caller_type
    {
        PyObject_HEAD

            method_adapter_interface * iadapter;
        PyObject * self;
    };
    //////////////////////////////////////////////////////////////////////////
    static void py_method_descr_destr( PyObject * _obj )
    {
        py_method_caller_type * mct = (py_method_caller_type *)_obj;

        factorable::intrusive_ptr_release( mct->iadapter );

        pybind::decref( mct->self );

        PyObject_Free( _obj );
    }
    //////////////////////////////////////////////////////////////////////////
    static PyObject * py_method_descr_call2( PyObject * _obj, PyObject * _args, PyObject * _kwds )
    {
        kernel_interface * kernel = pybind::get_kernel();

        py_method_caller_type * mct = (py_method_caller_type *)_obj;

        if( mct->iadapter == nullptr )
        {
            pybind::error_message( "descr_call2: invalid call unknown method [not set iadapter] for object '%s'"
                , kernel->object_repr( mct->self ).c_str()
            );

            return nullptr;
        }

        try
        {
            void * impl = kernel->get_class_impl( mct->self );

            if( impl == nullptr )
            {
                pybind::error_message( "descr_call2: method '%s' invalid call unbind object"
                    , mct->iadapter->getName()
                );

                return nullptr;
            }

            const class_type_scope_interface_ptr & scope = kernel->get_class_scope( mct->self->ob_type );

            if( scope == nullptr )
            {
                pybind::error_message( "descr_call2: method '%s' invalid call not found scope for object '%s'"
                    , mct->iadapter->getName()
                    , kernel->object_repr( mct->self ).c_str()
                );

                return nullptr;
            }

            method_adapter_interface * adapter = mct->iadapter;

            DEBUG_PYBIND_NOTIFY_BEGIN_BIND_CALL( kernel, scope->get_name(), adapter->getName(), adapter->getCallDebugSilent(), _args, _kwds );
            PyObject * py_value = adapter->call( kernel, impl, scope, _args, _kwds );
            DEBUG_PYBIND_NOTIFY_END_BIND_CALL( kernel, scope->get_name(), adapter->getName(), adapter->getCallDebugSilent(), _args, _kwds );

            return py_value;
        }
        catch( const pybind_exception & _ex )
        {
            pybind::error_message( "descr_call2: obj '%s' method '%s' invalid call pybind exception '%s'"
                , pybind::object_str( mct->self )
                , mct->iadapter->getName()
                , _ex.what()
            );
        }
        catch( const std::exception & _ex )
        {
            pybind::error_message( "descr_call2: obj '%s' method '%s' invalid call std exception '%s'"
                , pybind::object_str( mct->self )
                , mct->iadapter->getName()
                , _ex.what()
            );
        }
        catch( ... )
        {
            pybind::error_message( "descr_call2: obj '%s' method '%s' invalid call ... exception"
                , pybind::object_str( mct->self )
                , mct->iadapter->getName()
            );
        }

        return nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    static int py_method_descr_check( py_method_generator_type * _descr, PyObject * _obj, PyObject ** _pres )
    {
        if( _obj == nullptr )
        {
            pybind::incref( (PyObject *)_descr );
            *_pres = (PyObject *)_descr;

            return 1;
        }

        if( !PyObject_TypeCheck( _obj, _descr->classtype ) ) {
            //PyErr_Format(PyExc_TypeError,
            //	"descriptor '%s' for '%s' objects "
            //	"doesn't apply to '%s' object",
            //	descr_name((PyDescrObject *)descr),
            //	descr->d_type->tp_name,
            //	obj->ob_type->tp_name);
            *_pres = nullptr;

            return 1;
        }

        return 0;
    }
    //////////////////////////////////////////////////////////////////////////
    static PyObject * py_method_method_get( py_method_generator_type * _descr, PyObject * _obj, PyObject * _type )
    {
        (void)_type;

        PyObject *res;

        if( py_method_descr_check( _descr, _obj, &res ) == 1 )
        {
            return res;
        }

        py_method_caller_type * mct = (py_method_caller_type *)PyType_GenericAlloc( _descr->method_caller_type, 0 );

        factorable::intrusive_ptr_setup( mct->iadapter, _descr->iadapter );

        pybind::incref( _obj );
        mct->self = _obj;

        return (PyObject*)mct;
    }
    //////////////////////////////////////////////////////////////////////////
    static PyObject * py_method_descr_call( py_method_generator_type * _descr, PyObject * _args, PyObject * _kwds )
    {
        Py_ssize_t argc = PyTuple_GET_SIZE( _args );

        if( argc == 0 )
        {
            pybind::error_message( "descr_call: method '%s' take none args"
                , _descr->iadapter->getName()
            );

            return nullptr;
        }

        py_method_caller_type * mct = (py_method_caller_type *)PyType_GenericAlloc( _descr->method_caller_type, 0 );
        factorable::intrusive_ptr_setup( mct->iadapter, _descr->iadapter );

        mct->self = PyTuple_GetItem( _args, 0 );

        PyObject * new_args = PyTuple_GetSlice( _args, 1, argc );

        PyObject * py_method = PyEval_CallObjectWithKeywords( (PyObject*)mct, new_args, _kwds );

        return py_method;
    }
    //////////////////////////////////////////////////////////////////////////
    static void py_method_descr_destr2( PyObject * _obj )
    {
        py_method_generator_type * mgt = (py_method_generator_type *)_obj;

        pybind::decref( (PyObject *)mgt->method_caller_type );

        factorable::intrusive_ptr_release( mgt->iadapter );

        pybind::xdecref( (PyObject *)mgt->classtype );

        PyObject_Free( _obj );
    }
    //////////////////////////////////////////////////////////////////////////
    bool method_python::initialize()
    {
        PyTypeObject method_caller_type =
        {
            PyVarObject_HEAD_INIT( nullptr, 0 )
            "pybind_method_caller_type",
            sizeof( py_method_caller_type ),
            0,
            (destructor)py_method_descr_destr,					/* tp_dealloc */
            0,					/* tp_print */
            0,					/* tp_getattr */
            0,					/* tp_setattr */
            0,					/* tp_compare */
            0,					/* tp_repr */
            0,					/* tp_as_number */
            0,					/* tp_as_sequence */
            0,					/* tp_as_mapping */
            0,					/* tp_hash */
            (ternaryfunc)py_method_descr_call2, /* tp_call */
            0,					/* tp_str */
            0,					/* tp_getattro */
            0,					/* tp_setattro */
            0,					/* tp_as_buffer */
            Py_TPFLAGS_DEFAULT, /* tp_flags */
            0,					/* tp_doc */
            0,					/* tp_traverse */
            0,					/* tp_clear */
            0,					/* tp_richcompare */
            0,					/* tp_weaklistoffset */
            0,					/* tp_iter */
            0,					/* tp_iternext */
            0,					/* tp_methods */
            0,					/* tp_members */
            0,					/* tp_getset */
            0,					/* tp_base */
            0,					/* tp_dict */
            0,					/* tp_descr_get */
            0,					/* tp_descr_set */
        };

        //////////////////////////////////////////////////////////////////////////
#   if PYBIND_PYTHON_VERSION < 300 
        //////////////////////////////////////////////////////////////////////////
        PyMemberDef descr_members[] =
        {
            { const_cast<char*>("__objclass__"), T_OBJECT, offsetof( py_method_generator_type, classtype ), READONLY },
            //{const_cast<char*>("__name__"), T_OBJECT, offsetof(py_method_generator_type, methodname), READONLY},
        { 0 }
        };
#	else
        //////////////////////////////////////////////////////////////////////////
        PyMemberDef descr_members[] =
        {
            { "__objclass__", T_OBJECT, offsetof( py_method_generator_type, classtype ), READONLY, "__objclass__" },
            //{"__name__", T_OBJECT, offsetof(py_method_generator_type, methodname), READONLY, "__name__"},
        { 0 }
        };
#	endif

        m_descr_members[0] = descr_members[0];
        m_descr_members[1] = descr_members[1];

        PyTypeObject method_generator_type =
        {
            PyVarObject_HEAD_INIT( nullptr, 0 )
            "pybind_method_generator_type",
            sizeof( py_method_generator_type ),
            0,
            (destructor)py_method_descr_destr2,					/* tp_dealloc */
            0,					/* tp_print */
            0,					/* tp_getattr */
            0,					/* tp_setattr */
            0,					/* tp_compare */
            0,					/* tp_repr */
            0,					/* tp_as_number */
            0,					/* tp_as_sequence */
            0,					/* tp_as_mapping */
            0,					/* tp_hash */
            (ternaryfunc)py_method_descr_call,					/* tp_call */
            0,					/* tp_str */
            0,					/* tp_getattro */
            0,					/* tp_setattro */
            0,					/* tp_as_buffer */
            Py_TPFLAGS_DEFAULT, /* tp_flags */
            0,					/* tp_doc */
            0,					/* tp_traverse */
            0,					/* tp_clear */
            0,					/* tp_richcompare */
            0,					/* tp_weaklistoffset */
            0,					/* tp_iter */
            0,					/* tp_iternext */
            0,					/* tp_methods */
            m_descr_members,		/* tp_members */
            0,					/* tp_getset */
            0,					/* tp_base */
            0,					/* tp_dict */
            (descrgetfunc)py_method_method_get,		/* tp_descr_get */
            0,					/* tp_descr_set */
        };

        m_method_caller_type = method_caller_type;
        m_method_generator_type = method_generator_type;

        if( PyType_Ready( &m_method_generator_type ) < 0 )
        {
            pybind::log( "invalid embedding class '%s'\n"
                , m_method_generator_type.tp_name
            );

            return false;
        }

        if( PyType_Ready( &m_method_caller_type ) < 0 )
        {
            pybind::log( "invalid embedding class '%s'\n"
                , m_method_caller_type.tp_name
            );

            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void method_python::finalize()
    {

    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * method_python::create_method_adapter( const method_adapter_interface_ptr & _ifunc, PyTypeObject * _type )
    {
        py_method_generator_type * generator = (py_method_generator_type *)PyType_GenericAlloc( &m_method_generator_type, 0 );

        generator->method_caller_type = &m_method_caller_type;
        pybind::incref( (PyObject *)generator->method_caller_type );

        factorable::intrusive_ptr_setup( generator->iadapter, _ifunc.get() );

        if( _type != nullptr )
        {
            generator->classtype = _type;
            pybind::incref( (PyObject *)generator->classtype );
        }
        else
        {
            generator->classtype = nullptr;
        }

        return (PyObject*)generator;
    }
    //////////////////////////////////////////////////////////////////////////
    method_adapter_interface * method_python::get_method_adapter( PyObject * _obj )
    {
        py_method_generator_type * method = (py_method_generator_type *)_obj;

        method_adapter_interface * iadapter = method->iadapter;

        return iadapter;
    }
}
