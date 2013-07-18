#	include "functor_type.hpp"

#	include "pybind/system.hpp"
#	include "config/python.hpp"


#   include "static_var.hpp"

namespace pybind
{
    //////////////////////////////////////////////////////////////////////////	
    STATIC_DECLARE(PyTypeObject, s_functor_type);
	//////////////////////////////////////////////////////////////////////////
	py_functor_type * gen_functor()
	{
		py_functor_type * py_self = (py_functor_type *)PyType_GenericAlloc( &STATIC_VAR(s_functor_type), 0 );

		return py_self;
	}
    //////////////////////////////////////////////////////////////////////////
	static void py_dealloc( PyObject * _obj )
	{
        py_functor_type * py_self = (py_functor_type *)(_obj);

        py_self->iadapter = nullptr;

		_obj->ob_type->tp_free( _obj );
	}
    //////////////////////////////////////////////////////////////////////////
	bool initialize_functor()
	{
		STATIC_VAR(s_functor_type).tp_name = "functor_type_scope";
		STATIC_VAR(s_functor_type).tp_basicsize = sizeof(py_functor_type);
		STATIC_VAR(s_functor_type).tp_dealloc = &py_dealloc;
		STATIC_VAR(s_functor_type).tp_flags = Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE;

		if( PyType_Ready( &STATIC_VAR(s_functor_type) ) < 0 )
		{
			printf("invalid embedding class '%s' \n"
                , STATIC_VAR(s_functor_type).tp_name 
                );					

            return false;
		}

        return true;
	}
    //////////////////////////////////////////////////////////////////////////
	void finalize_functor()
	{
		//Py_DecRef((PyObject*)s_functor_type);
	}
}
