#	include "pybind/function_type.hpp"

#	include "config/python.hpp"

namespace pybind
{	
	static PyTypeObject s_function_type;

	py_function_type * gen_function()
	{
		py_function_type * py_self = (py_function_type *)PyType_GenericAlloc( &s_function_type, 0 );

		return py_self;
	}

	static void py_dealloc( PyObject * _obj )
	{
		_obj->ob_type->tp_free( _obj );
	}

	void initialize_function()
	{
		s_function_type.tp_name = "function_type_scope";
		s_function_type.tp_basicsize = sizeof( py_function_type );
		s_function_type.tp_dealloc = &py_dealloc;
		s_function_type.tp_flags = Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE;

		if( PyType_Ready( &s_function_type ) < 0 )
		{
			printf("invalid embedding class '%s' \n", s_function_type.tp_name );					
		}
	}

	void finalize_function()
	{
		//Py_DecRef((PyObject*)s_def_type);
	}
}
