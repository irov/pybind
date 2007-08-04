#	include "pybind/def.hpp"
#	include "pybind/def_type.hpp"

#	include "pybind/system.hpp"
#	include "pybind/config.hpp"

#	include <list>

namespace pybind
{
	namespace detail
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
			_obj->ob_type->tp_free( _obj );
		}

		class def_type_scope
		{
		public:
			def_type_scope()
				: m_type( py_empty_type )
			{

			}

			def_type_scope( const def_type_scope & _scope )
				: m_type( _scope.m_type )
				, m_method( _scope.m_method )
			{

			}

		public:
			void setup( const char * _name, pybind_callback f, pybind_cfunction _cfunc, int _hasargs, PyObject * _module )
			{
				m_type.tp_name = _name;
				m_type.tp_basicsize = sizeof( py_function_type );
				m_type.tp_doc = "Embedding function from cpp";
				m_type.tp_dealloc = &py_dealloc;
				m_type.tp_flags = Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE;

				if( PyType_Ready(&m_type) < 0 )
				{
					printf("invalid embedding class '%s' \n", m_type.tp_name );					
				}

				m_method.ml_name = _name;
				m_method.ml_meth = _cfunc;
				m_method.ml_flags = METH_CLASS | ( _hasargs ) ? METH_VARARGS : METH_NOARGS;
				m_method.ml_doc = "Embedding function cpp";

				py_function_type *self = 
					(py_function_type *)m_type.tp_alloc( &m_type, 0 );

				self->f = f;

				PyObject *py_func = PyCFunction_New( &m_method, (PyObject*)self );
				//		Py_DECREF( self );
				//		Py_DECREF( _module );

				PyModule_AddObject( _module, _name, py_func );
			}

		protected:
			PyMethodDef m_method;
			PyTypeObject m_type;
		};

		typedef std::list<def_type_scope> TListTypeObject;
		TListTypeObject s_listTypeObject;

		void def_function( const char * _name, pybind_callback f, pybind_cfunction _cfunc, int _hasargs, PyObject * _module )
		{
			if( _module == 0 )
			{
				_module = get_currentmodule();
			}

			s_listTypeObject.push_back(def_type_scope());
			def_type_scope & cfunc_type = s_listTypeObject.back();
				
			cfunc_type.setup( _name, f, _cfunc, _hasargs, _module );
		}
	}

}