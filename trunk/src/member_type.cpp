#	include "member_type.hpp"

#	include "class_type.hpp"

#   include "static_var.hpp"

namespace pybind
{
    //////////////////////////////////////////////////////////////////////////
    STATIC_DECLARE(PyTypeObject, s_member_type)
	//////////////////////////////////////////////////////////////////////////
	static void py_dealloc( PyObject * _obj )
	{
		py_member_type * mt = (py_member_type *)(_obj);

		delete mt->iadpter;

#   if PYBIND_PYTHON_VERSION < 300
		mt->ob_type->tp_free( mt );		
#	else
		mt->ob_base.ob_type->tp_free( mt );		
#	endif
	}
	//////////////////////////////////////////////////////////////////////////
	static PyObject * py_getmethod( PyObject * _member, PyObject * _args )
	{
		PyObject * py_self = PyTuple_GetItem( _args, 0 );

		py_member_type * mt = (py_member_type *)_member;

		void * impl = detail::get_class_impl( py_self );

		if( impl == 0 )
		{
			error_message( "py_getmethod: unbind object" );
			return 0;
		}

		class_type_scope * scope = detail::get_class_scope( py_self->ob_type );

		return mt->iadpter->get( impl, scope );
	}
	//////////////////////////////////////////////////////////////////////////
	static PyObject * py_setmethod( PyObject * _member, PyObject * _args )
	{
		PyObject * py_self = PyTuple_GetItem( _args, 0 );
		PyObject * py_value = PyTuple_GetItem( _args, 1 );

		py_member_type * mt = (py_member_type *)(_member);

		void * impl = detail::get_class_impl( py_self );

		if( impl == 0 )
		{
			error_message( "py_setmethod: unbind object" );
			return 0;
		}

		class_type_scope * scope = detail::get_class_scope( py_self->ob_type );

		mt->iadpter->set( impl, py_value, scope );

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
	PyObject * member_type_scope::instance( const char * _name, member_adapter_interface * _iadpter )
	{
		py_member_type * py_member = (py_member_type *)PyType_GenericAlloc( &STATIC_VAR(s_member_type), 0 );

		py_member->name = _name;
		py_member->iadpter = _iadpter;

		PyObject * py_get = PyCFunction_New( &STATIC_VAR(s_getmethod), (PyObject*)py_member );
		PyObject * py_set = PyCFunction_New( &STATIC_VAR(s_setmethod), (PyObject*)py_member );

		Py_DecRef( (PyObject *)py_member );

		PyObject * py_result = PyObject_CallFunction( (PyObject*)&PyProperty_Type, "OOss", py_get, py_set, 0, "member_type_scope");		
		Py_DecRef( py_get );
		Py_DecRef( py_set );

		return py_result;
	}
	//////////////////////////////////////////////////////////////////////////
	bool initialize_members()
	{
		STATIC_VAR(s_member_type).tp_name = "member_type_scope";
		STATIC_VAR(s_member_type).tp_basicsize = sizeof( py_member_type );
		STATIC_VAR(s_member_type).tp_dealloc = &py_dealloc;
		STATIC_VAR(s_member_type).tp_flags = Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE;

		if( PyType_Ready( &STATIC_VAR(s_member_type) ) < 0 )
		{
			printf("invalid embedding class '%s' \n"
                , STATIC_VAR(s_member_type).tp_name 
                );

            return false;
		}

        return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void finalize_members()
	{

	}
}