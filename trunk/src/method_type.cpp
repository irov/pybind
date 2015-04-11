#	include "method_type.hpp"

#	include "pybind/class_type_scope.hpp"

#   include "static_var.hpp"

#	include "config/python.hpp"

#	include "pybind/debug.hpp"

namespace pybind
{
	//////////////////////////////////////////////////////////////////////////
	struct py_method_generator_type
	{
		PyObject_HEAD
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
	static void descr_destr( PyObject * _obj )
	{
		py_method_caller_type * mct = (py_method_caller_type *)_obj;

		stdex::intrusive_ptr_release( mct->iadapter );

		Py_DECREF( mct->self );

		PyObject_Free( _obj );
	}
	//////////////////////////////////////////////////////////////////////////
	static PyObject * descr_call2( PyObject * _obj, PyObject * _args , PyObject * _kwds )
	{
		py_method_caller_type * mct = (py_method_caller_type *)_obj;

		try
		{
			void * impl = detail::get_class_impl( mct->self );

			if( impl == nullptr )
			{
				pybind::error_message( "descr_call2: method %s invalid call unbind object"
					, mct->iadapter->getName()
					);

				return nullptr;
			}

			const class_type_scope_ptr & scope = detail::get_class_scope( mct->self->ob_type );

			method_adapter_interface * adapter = mct->iadapter;

			DEBUG_PYBIND_NOTIFY_BEGIN_BIND_CALL( scope->get_name(), adapter->getName(), _args, _kwds );
			PyObject * py_value = adapter->call( impl, scope, _args, _kwds );
			DEBUG_PYBIND_NOTIFY_END_BIND_CALL( scope->get_name(), adapter->getName(), _args, _kwds );

			return py_value;
		}
		catch( const pybind_exception & _ex )
		{
			pybind::error_message( "descr_call2: obj %s method %s invalid call exception '%s'"
				, pybind::object_str(mct->self)
				, mct->iadapter->getName()
				, _ex.what()
				);			
		}

		return nullptr;
	}
	//////////////////////////////////////////////////////////////////////////
	STATIC_DECLARE_VALUE_BEGIN(PyTypeObject, s_method_caller_type)
    {
        PyVarObject_HEAD_INIT(&PyType_Type, 0)
            "pybind_method_caller_type",
            sizeof(py_method_caller_type),
            0,
            (destructor)descr_destr,					/* tp_dealloc */
            0,					/* tp_print */
            0,					/* tp_getattr */
            0,					/* tp_setattr */
            0,					/* tp_compare */
            0,					/* tp_repr */
            0,					/* tp_as_number */
            0,					/* tp_as_sequence */
            0,					/* tp_as_mapping */
            0,					/* tp_hash */
            (ternaryfunc)descr_call2, /* tp_call */
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
    }
    STATIC_DECLARE_VALUE_END();	
    //////////////////////////////////////////////////////////////////////////
#   if PYBIND_PYTHON_VERSION < 300 
    //////////////////////////////////////////////////////////////////////////
	STATIC_DECLARE_ARRAY_BEGIN(PyMemberDef, s_descr_members)
    {
		{const_cast<char*>("__objclass__"), T_OBJECT, offsetof(py_method_generator_type, classtype), READONLY},
		//{const_cast<char*>("__name__"), T_OBJECT, offsetof(py_method_generator_type, methodname), READONLY},
		{0}
	}
    STATIC_DECLARE_VALUE_END();
#	else
    //////////////////////////////////////////////////////////////////////////
    STATIC_DECLARE_ARRAY_BEGIN(PyMemberDef, s_descr_members)
	{
		{"__objclass__", T_OBJECT, offsetof(py_method_generator_type, classtype), READONLY, "__objclass__"},
		//{"__name__", T_OBJECT, offsetof(py_method_generator_type, methodname), READONLY, "__name__"},
		{0}
	}
    STATIC_DECLARE_VALUE_END();
#	endif
	//////////////////////////////////////////////////////////////////////////
	static int descr_check( py_method_generator_type * _descr, PyObject * _obj, PyObject ** _pres )
	{
		if( _obj == nullptr )
		{
			Py_INCREF( (PyObject *)_descr );
			*_pres = (PyObject *)_descr;

			return 1;
		}

		if (!PyObject_TypeCheck(_obj, _descr->classtype)) {
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
	static PyObject * method_get( py_method_generator_type * _descr, PyObject * _obj, PyObject * _type )
	{
        (void)_type;

		PyObject *res;

		if( descr_check( _descr, _obj, &res ) == 1 )
		{
			return res;
		}

		py_method_caller_type * mct = (py_method_caller_type *)PyType_GenericAlloc( &STATIC_VAR(s_method_caller_type), 0 );
		stdex::intrusive_ptr_setup( mct->iadapter, _descr->iadapter );

		Py_INCREF( _obj );
		mct->self = _obj;

		return (PyObject*)mct;
	}
	//////////////////////////////////////////////////////////////////////////
	static PyObject * descr_call( py_method_generator_type * _descr, PyObject * _args , PyObject * _kwds )
	{
		Py_ssize_t argc = PyTuple_GET_SIZE(_args);

		if( argc == 0 )
		{
			pybind::error_message( "descr_call: method %s take none args"
				, _descr->iadapter->getName()
				);

			return nullptr;
		}

		py_method_caller_type * mct = (py_method_caller_type *)PyType_GenericAlloc( &STATIC_VAR(s_method_caller_type), 0 );
		stdex::intrusive_ptr_setup( mct->iadapter, _descr->iadapter );

		mct->self = PyTuple_GetItem(_args, 0);
		
		PyObject * new_args = PyTuple_GetSlice(_args, 1, argc);

		PyObject * py_method = PyEval_CallObjectWithKeywords((PyObject*)mct, new_args, _kwds);

		return py_method;
	}
	//////////////////////////////////////////////////////////////////////////
	static void descr_destr2( PyObject * _obj )
	{
		py_method_generator_type * mgt = (py_method_generator_type *)_obj;

		stdex::intrusive_ptr_release( mgt->iadapter );

		Py_DECREF( (PyObject *)mgt->classtype );

		PyObject_Free( _obj );
	}
	//////////////////////////////////////////////////////////////////////////
	STATIC_DECLARE_VALUE_BEGIN(PyTypeObject, s_method_generator_type)
	{
		PyVarObject_HEAD_INIT(&PyType_Type, 0)
			"pybind_method_generator_type",
			sizeof(py_method_generator_type),
			0,
			(destructor)descr_destr2,					/* tp_dealloc */
			0,					/* tp_print */
			0,					/* tp_getattr */
			0,					/* tp_setattr */
			0,					/* tp_compare */
			0,					/* tp_repr */
			0,					/* tp_as_number */
			0,					/* tp_as_sequence */
			0,					/* tp_as_mapping */
			0,					/* tp_hash */
			(ternaryfunc)descr_call,					/* tp_call */
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
			STATIC_VAR(s_descr_members),		/* tp_members */
			0,					/* tp_getset */
			0,					/* tp_base */
			0,					/* tp_dict */
			(descrgetfunc)method_get,		/* tp_descr_get */
			0,					/* tp_descr_set */
	}
    STATIC_DECLARE_VALUE_END();
	//////////////////////////////////////////////////////////////////////////
	PyObject * method_type_scope::instance( const method_adapter_interface_ptr & _ifunc, PyTypeObject * _type )
	{
		py_method_generator_type * self = (py_method_generator_type *)PyType_GenericAlloc( &STATIC_VAR(s_method_generator_type), 0 );

		stdex::intrusive_ptr_setup( self->iadapter, _ifunc );

        if( _type != nullptr )
        {		    
		    self->classtype = _type;
			Py_INCREF( (PyObject *)self->classtype );
        }
		
		return (PyObject*)self;
	}
	//////////////////////////////////////////////////////////////////////////
	bool initialize_methods()
	{
		PyTypeObject * pt = &STATIC_VAR(s_method_generator_type);
		if( PyType_Ready( pt ) < 0 )
		{
			printf("invalid embedding class '%s'\n"
                , STATIC_VAR(s_method_generator_type).tp_name 
                );

            return false;
		}

		if( PyType_Ready( &STATIC_VAR(s_method_caller_type) ) < 0 )
		{
			printf("invalid embedding class '%s'\n"
                , STATIC_VAR(s_method_caller_type).tp_name 
                );

            return false;
		}

        return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void finalize_methods()
	{
		Py_DECREF( (PyObject *)&STATIC_VAR(s_method_generator_type) );
		Py_DECREF( (PyObject *)&STATIC_VAR(s_method_caller_type) );
	}
}
