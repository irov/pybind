#	include "function_type.hpp"

#	include "config/python.hpp"

namespace pybind
{	
	//////////////////////////////////////////////////////////////////////////
	struct function_scope
	{
		PyTypeObject function_type;
	};
	//////////////////////////////////////////////////////////////////////////
	static function_scope & get_scope()
	{
		static function_scope k;

		return k;
	}
    //////////////////////////////////////////////////////////////////////////
	static void py_dealloc( PyObject * _obj )
	{
        py_function_type * py_self = (py_function_type *)_obj;

		stdex::intrusive_ptr_release( py_self->iadapter );

		PyObject_Free( _obj );
	}
	//////////////////////////////////////////////////////////////////////////
	py_function_type * gen_function()
	{
		function_scope & k = get_scope();

		py_function_type * py_self = (py_function_type *)PyType_GenericAlloc( &k.function_type, 0 );

		return py_self;
	}
    //////////////////////////////////////////////////////////////////////////
	bool initialize_function()
	{
		function_scope & k = get_scope();
		
		PyTypeObject function_type =
		{
			PyVarObject_HEAD_INIT( &PyType_Type, 0 )
				"function_type_scope",
				sizeof( py_function_type ),
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
		};

		k.function_type = function_type;

		if( PyType_Ready( &k.function_type ) < 0 )
		{
			printf("invalid embedding class '%s'\n"
				, k.function_type.tp_name
                );					

            return false;
		}

        return true;
	}
    //////////////////////////////////////////////////////////////////////////
	void finalize_function()
	{		
	}
}
