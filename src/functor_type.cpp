#	include "pybind/functor.hpp"
#	include "pybind/functor_type.hpp"

#	include "pybind/system.hpp"
#	include "config/python.hpp"

#	include <list>

namespace pybind
{
	static PyTypeObject s_functor_type;
	
	py_functor_type * gen_functor()
	{
		py_functor_type * py_self = (py_functor_type *)PyType_GenericAlloc( &s_functor_type, 0 );

		return py_self;
	}

	static void py_dealloc( PyObject * _obj )
	{
		_obj->ob_type->tp_free( _obj );
	}

	void initialize_functor()
	{
		s_functor_type.tp_name = "functor_type_scope";
		s_functor_type.tp_basicsize = sizeof( py_functor_type );
		s_functor_type.tp_dealloc = &py_dealloc;
		s_functor_type.tp_flags = Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE;

		if( PyType_Ready( &s_functor_type ) < 0 )
		{
			printf("invalid embedding class '%s' \n", s_functor_type.tp_name );					
		}
	}

	void finalize_functor()
	{
		//Py_DecRef((PyObject*)s_functor_type);
	}
}
