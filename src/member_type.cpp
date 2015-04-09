#	include "member_type.hpp"

#	include "pybind/system.hpp"
#	include "pybind/class_type_scope.hpp"

#   include "static_var.hpp"

namespace pybind
{
	//////////////////////////////////////////////////////////////////////////
	static void py_dealloc( PyObject * _obj )
	{
		py_member_type * mt = (py_member_type *)(_obj);

		stdex::intrusive_ptr_release( mt->iadapter );

		PyObject_Free(_obj);
	}
	//////////////////////////////////////////////////////////////////////////
	STATIC_DECLARE_VALUE_BEGIN(PyTypeObject, s_member_type)
	{
		PyVarObject_HEAD_INIT(&PyType_Type, 0)
			"member_type_scope",
			sizeof(py_member_type),
			0,
			&py_dealloc,                             /* tp_dealloc */
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
	}
	STATIC_DECLARE_VALUE_END();
	//////////////////////////////////////////////////////////////////////////
	static PyObject * py_getmethod( PyObject * _member, PyObject * _args )
	{
		PyObject * py_self = PyTuple_GetItem( _args, 0 );

		py_member_type * mt = (py_member_type *)_member;

		void * impl = detail::get_class_impl( py_self );

		if( impl == nullptr )
		{
			error_message( "py_getmethod: unbind object" );

			return nullptr;
		}

		const class_type_scope_ptr & scope = detail::get_class_scope( py_self->ob_type );

		PyObject * py_method = mt->iadapter->get( impl, scope );

		return py_method;
	}
	//////////////////////////////////////////////////////////////////////////
	static PyObject * py_setmethod( PyObject * _member, PyObject * _args )
	{
		PyObject * py_self = PyTuple_GetItem( _args, 0 );
		PyObject * py_value = PyTuple_GetItem( _args, 1 );

		py_member_type * mt = (py_member_type *)(_member);

		void * impl = detail::get_class_impl( py_self );

		if( impl == nullptr )
		{
			error_message( "py_setmethod: unbind object" );

			return nullptr;
		}

		const class_type_scope_ptr & scope = detail::get_class_scope( py_self->ob_type );

		mt->iadapter->set( impl, py_value, scope );

		Py_RETURN_NONE;
	}
	//////////////////////////////////////////////////////////////////////////
	STATIC_DECLARE_VALUE_BEGIN(PyMethodDef, s_getmethod)
	{
		"getmethod",
			&py_getmethod,
			METH_CLASS | METH_VARARGS,
			"Embedding function cpp"
	}
	STATIC_DECLARE_VALUE_END();
	//////////////////////////////////////////////////////////////////////////
	STATIC_DECLARE_VALUE_BEGIN(PyMethodDef, s_setmethod)
	{
		"setmethod",
			&py_setmethod,
			METH_CLASS | METH_VARARGS,
			"Embedding function cpp"
	}
	STATIC_DECLARE_VALUE_END();
	//////////////////////////////////////////////////////////////////////////
	PyObject * member_type_scope::instance( const member_adapter_interface_ptr & _iadapter )
	{
		py_member_type * py_member = (py_member_type *)PyType_GenericAlloc( &STATIC_VAR(s_member_type), 0 );
		
		stdex::intrusive_ptr_setup( py_member->iadapter, _iadapter );

		PyObject * py_get = PyCFunction_New( &STATIC_VAR(s_getmethod), (PyObject*)py_member );
		PyObject * py_set = PyCFunction_New( &STATIC_VAR(s_setmethod), (PyObject*)py_member );

		Py_DecRef( (PyObject *)py_member );

		PyObject * py_result = PyObject_CallFunction( (PyObject*)&PyProperty_Type, const_cast<char *>("OOss"), py_get, py_set, 0, "member_type_scope");		
		Py_DecRef( py_get );
		Py_DecRef( py_set );

		return py_result;
	}
	//////////////////////////////////////////////////////////////////////////
	bool initialize_members()
	{
		PyTypeObject * member_type = &STATIC_VAR(s_member_type);

		if( PyType_Ready( member_type ) < 0 )
		{
			printf("invalid embedding class '%s' \n"
                , member_type->tp_name
                );

            return false;
		}

        return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void finalize_members()
	{
		Py_DecRef( (PyObject *)&STATIC_VAR(s_member_type) );
	}
}
