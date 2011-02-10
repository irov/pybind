#	include "pybind/member_type.hpp"

#	include "pybind/class_type.hpp"

namespace pybind
{
	static PyTypeObject s_member_type;
	//////////////////////////////////////////////////////////////////////////
	static void py_dealloc( PyObject * _obj )
	{
		py_member_type * mt = (py_member_type *)(_obj);

		//delete mt->ifunc;

		mt->ob_type->tp_free( mt );		
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

		class_type_scope * scope = detail::get_class_scope( py_self );

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

		class_type_scope * scope = detail::get_class_scope( py_self );

		mt->iadpter->set( impl, py_value, scope );

		Py_RETURN_NONE;
	}
	//////////////////////////////////////////////////////////////////////////
	member_type_scope::member_type_scope( const char * _name, member_adapter_interface * _ifunc )
		: m_name(_name)
		, m_interface(_ifunc)
	{
		m_getmethod.ml_name = "getmethod";
		m_getmethod.ml_meth = &py_getmethod;
		m_getmethod.ml_flags = METH_CLASS | METH_VARARGS;
		m_getmethod.ml_doc = "Embedding function cpp";

		m_setmethod.ml_name = "setmethod";
		m_setmethod.ml_meth = &py_setmethod;
		m_setmethod.ml_flags = METH_CLASS | METH_VARARGS;
		m_setmethod.ml_doc = "Embedding function cpp";	
	}
	//////////////////////////////////////////////////////////////////////////
	member_type_scope::~member_type_scope()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	PyObject * member_type_scope::instance()
	{
		py_member_type * py_member = (py_member_type *)PyType_GenericAlloc( &s_member_type, 0 );

		py_member->name = m_name;
		py_member->iadpter = m_interface;

		PyObject * py_get = PyCFunction_New( &m_getmethod, (PyObject*)py_member );
		PyObject * py_set = PyCFunction_New( &m_setmethod, (PyObject*)py_member );

		Py_DECREF( py_member );

		PyObject * py_result = PyObject_CallFunction( (PyObject*)&PyProperty_Type, "OOss", py_get, py_set, 0, "member_type_scope");		
		Py_DECREF( py_get );
		Py_DECREF( py_set );

		return py_result;
	}
	//////////////////////////////////////////////////////////////////////////
	void initialize_members()
	{
		s_member_type.tp_name = "member_type_scope";
		s_member_type.tp_basicsize = sizeof( py_member_type );
		s_member_type.tp_dealloc = &py_dealloc;
		s_member_type.tp_flags = Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE;

		if( PyType_Ready( &s_member_type ) < 0 )
		{
			printf("invalid embedding class '%s' \n", s_member_type.tp_name );
		}
	}
}
