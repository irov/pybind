#	include "pybind/method_type.hpp"

#	include "pybind/class_type.hpp"

namespace pybind
{
	static PyTypeObject py_empty_type = {
		PyObject_HEAD_INIT(NULL)
		0,                         /*ob_size*/
		0,				           /*tp_name*/
		0,				           /*tp_basicsize*/
		0,                         /*tp_itemsize*/
		0, /*tp_dealloc*/
		0,                         /*tp_print*/
		0,                         /*tp_getattr*/
		0,                         /*tp_setattr*/
		0,                         /*tp_compare*/
		0,                         /*tp_repr*/
		0,                         /*tp_as_number*/
		0,                         /*tp_as_sequence*/
		0,                         /*tp_as_mapping*/
		0,                         /*tp_hash */
		0,                         /*tp_call*/
		0,                         /*tp_str*/
		0,                         /*tp_getattro*/
		0,                         /*tp_setattro*/
		0,                         /*tp_as_buffer*/
		0, /*tp_flags*/
		0,           /* tp_doc */
		0,		               /* tp_traverse */
		0,		               /* tp_clear */
		0,		               /* tp_richcompare */
		0,		               /* tp_weaklistoffset */
		0,		               /* tp_iter */
		0,		               /* tp_iternext */
		0,             /* tp_methods */
		0,             /* tp_members */
		0,           /* tp_getset */
		0,                         /* tp_base */
		0,                         /* tp_dict */
		0,                         /* tp_descr_get */
		0,                         /* tp_descr_set */
		0,                         /* tp_dictoffset */
		0,      /* tp_init */
		0,                         /* tp_alloc */
		0,                 /* tp_new */
	};

	static void py_dealloc( PyObject * _obj )
	{
		py_method_type * mt = (py_method_type *)(_obj);

		delete mt->ifunc;

		mt->ob_type->tp_free( mt );		
	}

	method_type_scope::method_type_scope()
		: m_type( py_empty_type )
		, m_name( 0 )
		, m_interface( 0 )
	{

	}

	method_type_scope::method_type_scope( const method_type_scope & _scope )
		: m_name( _scope.m_name )
		, m_interface( _scope.m_interface )
		, m_method( _scope.m_method )
		, m_type( _scope.m_type )
	{

	}
	
	void method_type_scope::setup( PyTypeObject * _class, 
		const char * _name, 
		method_proxy_interface * _ifunc, 
		pybind_cfunction _cfunc, 
		int _hasargs )
	{
		m_name =  _name;
		m_interface = _ifunc;

		m_type.tp_name = _name;
		m_type.tp_basicsize = sizeof( py_method_type );
		m_type.tp_doc = "Embedding function from cpp";
		m_type.tp_dealloc = &py_dealloc;
		m_type.tp_flags = Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE;

		if( PyType_Ready(&m_type) < 0 )
		{
			printf("invalid embedding class '%s' \n", m_type.tp_name );					
		}

		Py_DECREF( &m_type );

		m_method.ml_name = _name;
		m_method.ml_meth = _cfunc;
		m_method.ml_flags = METH_CLASS | ( _hasargs ) ? METH_VARARGS : METH_NOARGS;
		m_method.ml_doc = "Embedding function cpp";
	}

	PyObject * method_type_scope::instance( py_class_type * _obj )
	{
		py_method_type * self = 
			(py_method_type *)m_type.tp_alloc( &m_type, 0 );

		self->ifunc = m_interface;
		self->impl = _obj->impl;
		self->scope = _obj->scope;

		PyObject * py_function = PyCFunction_New( &m_method, (PyObject*)self );
		Py_DECREF( self );

		return py_function;
	}
}