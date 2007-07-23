#	include "pybind/class_type.hpp"
#	include "pybind/class_scope.hpp"

#	include "pybind/module.hpp"

#	include "pybind/config.hpp"

#	include <list>

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
	//////////////////////////////////////////////////////////////////////////
	class_type_scope::class_type_scope(
		const char * _name, 
		module_ * _module,
		newfunc _pynew,
		destructor _pydestructor)
		: m_module( _module )
		, m_type( py_empty_type )
	{
		m_type.tp_name = _name;
		m_type.tp_basicsize = sizeof( py_class_type );
		m_type.tp_doc = "Embedding class from cpp";
		m_type.tp_new = _pynew;
		m_type.tp_dealloc = _pydestructor;
		m_type.tp_flags = Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE;

		m_type_holder;
	}
	//////////////////////////////////////////////////////////////////////////
	void class_type_scope::add_method( const PyMethodDef & md )
	{
		m_vectorMethodDef.push_back( md );
	}
	//////////////////////////////////////////////////////////////////////////
	void class_type_scope::set_module( module_ * _module )
	{
		m_module = _module;
	}
	//////////////////////////////////////////////////////////////////////////
	void class_type_scope::add_method_from_scope( class_type_scope * _basescope )
	{
		m_vectorMethodDef.insert( 
			m_vectorMethodDef.end(), 
			_basescope->m_vectorMethodDef.begin(), 
			_basescope->m_vectorMethodDef.end() );
	}
	//////////////////////////////////////////////////////////////////////////
	void class_type_scope::setup_type( const type_info & _info )
	{
		PyMethodDef md = {0};

		m_vectorMethodDef.push_back( md );

		m_type.tp_methods = &m_vectorMethodDef.front();

		if( PyType_Ready(&m_type) < 0 )
		{
			printf("invalid embedding class '%s' \n", m_type.tp_name );					
		}

		class_scope::reg_class_type( &m_type );

		class_scope::reg_class_scope( _info, this );

		m_module->addObject( m_type.tp_name, (PyObject*)&m_type );			
	}

}