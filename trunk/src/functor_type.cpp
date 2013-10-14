#	include "functor_type.hpp"

#	include "pybind/system.hpp"
#	include "config/python.hpp"


#   include "static_var.hpp"

namespace pybind
{
	//////////////////////////////////////////////////////////////////////////
	static void py_dealloc( PyObject * _obj )
	{
		py_functor_type * py_self = (py_functor_type *)(_obj);

		py_self->iadapter = nullptr;

		_obj->ob_type->tp_free( _obj );
	}
	//////////////////////////////////////////////////////////////////////////
	STATIC_DECLARE_VALUE_BEGIN(PyTypeObject, s_functor_type)
	{
		PyVarObject_HEAD_INIT(&PyType_Type, 0)
			"functor_type_scope",
			sizeof(py_functor_type),
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
			Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,              /* tp_flags */
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
	py_functor_type * gen_functor()
	{
		py_functor_type * py_self = (py_functor_type *)PyType_GenericAlloc( &STATIC_VAR(s_functor_type), 0 );

		return py_self;
	}
    //////////////////////////////////////////////////////////////////////////
	bool initialize_functor()
	{
		PyTypeObject * functor_type = &STATIC_VAR(s_functor_type);

		if( PyType_Ready( functor_type ) < 0 )
		{
			printf("invalid embedding class '%s' \n"
                , functor_type->tp_name 
                );					

            return false;
		}

        return true;
	}
    //////////////////////////////////////////////////////////////////////////
	void finalize_functor()
	{
		Py_DecRef( (PyObject*)&STATIC_VAR(s_functor_type) );
	}
}
