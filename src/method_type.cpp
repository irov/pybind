#	include "pybind/method_type.hpp"

#	include "pybind/class_type.hpp"

namespace pybind
{
	static PyTypeObject * s_method_type = 0;
	//////////////////////////////////////////////////////////////////////////
	static void py_dealloc( PyObject * _obj )
	{
		py_method_type * mt = (py_method_type *)(_obj);

		//delete mt->ifunc;

		mt->ob_type->tp_free( mt );		
	}
	//////////////////////////////////////////////////////////////////////////
	method_type_scope::method_type_scope( PyTypeObject * _class, 
		const char * _name, 
		method_adapter_interface * _ifunc, 
		pybind_cfunction _cfunc, 
		int _hasargs )
		: m_name(_name)
		, m_interface(_ifunc)
	{
		m_method.ml_name = _name;
		m_method.ml_meth = _cfunc;
		m_method.ml_flags = METH_CLASS | ( _hasargs ) ? METH_VARARGS : METH_NOARGS;
		m_method.ml_doc = "Embedding function cpp";
	}
	//////////////////////////////////////////////////////////////////////////
	method_type_scope::~method_type_scope()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	PyObject * method_type_scope::instance( py_class_type * _obj )
	{
		py_method_type * self = (py_method_type *)PyType_GenericAlloc( s_method_type, 0 );

		self->ifunc = m_interface;
		self->impl = _obj->impl;
		self->scope = _obj->scope;

		PyObject * py_function = PyCFunction_New( &m_method, (PyObject*)self );
		Py_DECREF( self );

		return py_function;
	}
	//////////////////////////////////////////////////////////////////////////
	void initialize_method()
	{
		s_method_type = new PyTypeObject();

		s_method_type->tp_name = "method_type_scope";
		s_method_type->tp_basicsize = sizeof( py_method_type );
		s_method_type->tp_dealloc = &py_dealloc;
		s_method_type->tp_flags = Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE;

		if( PyType_Ready( s_method_type ) < 0 )
		{
			printf("invalid embedding class '%s' \n", s_method_type->tp_name );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void finialize_method()
	{
		delete s_method_type;
	}
}
