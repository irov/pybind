#	include "method_type.hpp"

#	include "pybind/class_type_scope.hpp"

#   include "static_var.hpp"

#	include "config/python.hpp"

namespace pybind
{
	//////////////////////////////////////////////////////////////////////////
	struct py_method_generator_type{
		PyObject_HEAD
		method_adapter_interface * iadapter;
		PyTypeObject * classtype;
		PyObject * methodname;
	};
	//////////////////////////////////////////////////////////////////////////
	struct py_method_caller_type{
		PyObject_HEAD
		method_adapter_interface * iadapter;
		PyObject * self;
	};
	//////////////////////////////////////////////////////////////////////////
	static void descr_destr( PyObject * _obj )
	{
		py_method_caller_type * mct = (py_method_caller_type *)_obj;

		Py_DecRef( mct->self );

		PyObject_Free( _obj );
	}
	//////////////////////////////////////////////////////////////////////////
	static PyObject * descr_call2( PyObject * _obj, PyObject * _args , PyObject * _kwds )
	{
		py_method_caller_type * mct = (py_method_caller_type *)_obj;

		void * impl = detail::get_class_impl( mct->self );

		if( impl == 0 )
		{
			error_message( "method_call: unbind object" );
			return 0;
		}

		class_type_scope * scope = detail::get_class_scope( mct->self->ob_type );

		return mct->iadapter->call( impl, scope, _args, _kwds );
	}
	//////////////////////////////////////////////////////////////////////////
	STATIC_DECLARE_VALUE_BEGIN(PyTypeObject, s_method_caller_type)
    {
#   if PYBIND_PYTHON_VERSION < 300
        PyObject_HEAD_INIT(&PyType_Type)
            0,
#   else
        PyVarObject_HEAD_INIT(&PyType_Type, 0)
#   endif
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
            Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /* tp_flags */
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
		{const_cast<char*>("__name__"), T_OBJECT, offsetof(py_method_generator_type, methodname), READONLY},
		{0}
	}
    STATIC_DECLARE_VALUE_END();
#	else
    //////////////////////////////////////////////////////////////////////////
    STATIC_DECLARE_ARRAY_BEGIN(PyMemberDef, s_descr_members)
	{
		{"__objclass__", T_OBJECT, offsetof(py_method_generator_type, classtype), READONLY, "__objclass__"},
		{"__name__", T_OBJECT, offsetof(py_method_generator_type, methodname), READONLY, "__name__"},
		{0}
	}
    STATIC_DECLARE_VALUE_END();
#	endif
	//////////////////////////////////////////////////////////////////////////
	static int descr_check( py_method_generator_type *descr, PyObject *obj, PyObject **pres )
	{
		if (obj == NULL) {
			Py_IncRef( (PyObject *)descr );
			*pres = (PyObject *)descr;
			return 1;
		}
		if (!PyObject_TypeCheck(obj, descr->classtype)) {
			//PyErr_Format(PyExc_TypeError,
			//	"descriptor '%s' for '%s' objects "
			//	"doesn't apply to '%s' object",
			//	descr_name((PyDescrObject *)descr),
			//	descr->d_type->tp_name,
			//	obj->ob_type->tp_name);
			*pres = NULL;
			return 1;
		}
		return 0;
	}
	//////////////////////////////////////////////////////////////////////////
	static PyObject * method_get(py_method_generator_type * _descr, PyObject * _obj, PyObject * _type)
	{
        (void)_type;

		PyObject *res;

		if( descr_check( _descr, _obj, &res ) == 1 )
		{
			return res;
		}

		py_method_caller_type * mct = (py_method_caller_type *)PyType_GenericAlloc( &STATIC_VAR(s_method_caller_type), 0 );
		mct->iadapter = _descr->iadapter;

		Py_IncRef( _obj );
		mct->self = _obj;

		return (PyObject*)mct;
	}
	//////////////////////////////////////////////////////////////////////////
	static PyObject * descr_call( py_method_generator_type * _descr, PyObject * _args , PyObject * _kwds )
	{
		Py_ssize_t argc = PyTuple_GET_SIZE(_args);

		if( argc == 0 )
		{
			const char * methodname = string_to_char( _descr->methodname );

			error_message( "method_call: %s take none args"
				, methodname
				);

			return 0;
		}

		py_method_caller_type * mct = (py_method_caller_type *)PyType_GenericAlloc( &STATIC_VAR(s_method_caller_type), 0 );
		mct->iadapter = _descr->iadapter;
		mct->self = PyTuple_GetItem(_args, 0);
		
		PyObject * new_args = PyTuple_GetSlice(_args, 1, argc);

		return PyEval_CallObjectWithKeywords((PyObject*)mct, new_args, _kwds);
	}
	////////////////////////////////////////////////////////////////////////////
	//static int descr_traverse( py_method_generator_type *self, visitproc visit, void *arg)
	//{
	//	Py_VISIT(self->classtype);
	//	return 0;
	//}
	//////////////////////////////////////////////////////////////////////////
	static void descr_destr2( PyObject * _obj )
	{
		py_method_generator_type * mgt = (py_method_generator_type *)_obj;

		delete mgt->iadapter;

		Py_DecRef( (PyObject *)mgt->classtype );
		Py_DecRef( mgt->methodname );

		PyObject_Free( _obj );
	}
	//////////////////////////////////////////////////////////////////////////
    STATIC_DECLARE_VALUE_BEGIN(PyTypeObject, s_method_generator_type)
    {
#   if PYBIND_PYTHON_VERSION < 300
		PyObject_HEAD_INIT(&PyType_Type)
		0,
#   else
		PyVarObject_HEAD_INIT(&PyType_Type, 0)
#   endif
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
		Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /* tp_flags */
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
	////////////////////////////////////////////////////////////////////////////
	//method_type_scope::method_type_scope( const char * _name, method_adapter_interface * _ifunc )
	//	: m_name(_name)
	//	, m_interface(_ifunc)
	//{
	//}
	////////////////////////////////////////////////////////////////////////////
	//method_type_scope::~method_type_scope()
	//{
	//}
	//////////////////////////////////////////////////////////////////////////
	PyObject * method_type_scope::instance( const char * _name, method_adapter_interface * _ifunc, PyTypeObject * _type )
	{
		py_method_generator_type * self = (py_method_generator_type *)PyType_GenericAlloc( &STATIC_VAR(s_method_generator_type), 0 );

		self->iadapter = _ifunc;

        if( _type != NULL )
        {
		    Py_IncRef( (PyObject *)_type );
		    self->classtype = _type;
        }

#   if PYBIND_PYTHON_VERSION < 300
		self->methodname = PyString_InternFromString( _name );
#	else
		self->methodname = PyUnicode_InternFromString( _name );
#	endif
		
		return (PyObject*)self;
	}
	//////////////////////////////////////////////////////////////////////////
	bool initialize_methods()
	{
		if( PyType_Ready( &STATIC_VAR(s_method_generator_type) ) < 0 )
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
		Py_DecRef( (PyObject *)&STATIC_VAR(s_method_generator_type) );
		Py_DecRef( (PyObject *)&STATIC_VAR(s_method_caller_type) );
	}
}
