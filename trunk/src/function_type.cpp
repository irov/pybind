#	include "function_type.hpp"

#   include "static_var.hpp"

#	include "config/python.hpp"

namespace pybind
{	
    //////////////////////////////////////////////////////////////////////////
    STATIC_DECLARE(PyTypeObject, s_function_type);
    //////////////////////////////////////////////////////////////////////////
	py_function_type * gen_function()
	{
		py_function_type * py_self = (py_function_type *)PyType_GenericAlloc( &STATIC_VAR(s_function_type), 0 );

		return py_self;
	}
    //////////////////////////////////////////////////////////////////////////
	static void py_dealloc( PyObject * _obj )
	{
        py_function_type * py_self = (py_function_type *)_obj;

        py_self->iadapter = nullptr;

		_obj->ob_type->tp_free( _obj );
	}
    //////////////////////////////////////////////////////////////////////////
	bool initialize_function()
	{
		STATIC_VAR(s_function_type).tp_name = "function_type_scope";
		STATIC_VAR(s_function_type).tp_basicsize = sizeof(py_function_type);
		STATIC_VAR(s_function_type).tp_dealloc = &py_dealloc;
		STATIC_VAR(s_function_type).tp_flags = Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE;

		if( PyType_Ready( &STATIC_VAR(s_function_type) ) < 0 )
		{
			printf("invalid embedding class '%s' \n"
                , STATIC_VAR(s_function_type).tp_name 
                );					

            return false;
		}

        return true;
	}
    //////////////////////////////////////////////////////////////////////////
	void finalize_function()
	{
		//Py_DecRef((PyObject*)s_def_type);
	}
}
