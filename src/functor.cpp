#	include "pybind/functor.hpp"
#	include "pybind/functor_type.hpp"

#	include "pybind/system.hpp"
#	include "config/python.hpp"

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

		class functor_type_scope
		{
		public:
			functor_type_scope()
				: m_type( py_empty_type )
			{
			}

			functor_type_scope( const functor_type_scope & _scope )
				: m_type( _scope.m_type )
				, m_method( _scope.m_method )
			{
			}

		public:
			void setup( const char * _name, functor_proxy_interface * _proxy, pybind_cfunction _cfunc, int _hasargs, PyObject * _module )
			{
				m_type.tp_name = _name;
				m_type.tp_basicsize = sizeof( py_functor_type );
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

				py_functor_type *self = 
					(py_functor_type *)m_type.tp_alloc( &m_type, 0 );

				self->proxy = _proxy;

				PyObject *py_func = PyCFunction_New( &m_method, (PyObject*)self );
				Py_DECREF( self );

				PyModule_AddObject( _module, _name, py_func );
			}

		protected:
			PyMethodDef m_method;
			PyTypeObject m_type;
		};

		static class GarbageTypeObjects
		{
		public:
			void push_back( const functor_type_scope & scope )
			{
				m_listTypeObject.push_back( scope );
			}

			functor_type_scope & back()
			{
				return m_listTypeObject.back();
			}

		protected:
			typedef std::list<functor_type_scope> TListTypeObject;
			TListTypeObject m_listTypeObject;
		} s_garbageTypeObjects;

		void def_functor( const char * _name, functor_proxy_interface * _proxy, pybind_cfunction _cfunc, int _hasargs, PyObject * _module )
		{
			if( _module == 0 )
			{
				_module = get_currentmodule();
			}

			s_garbageTypeObjects.push_back( functor_type_scope());
			functor_type_scope & cfunc_type = s_garbageTypeObjects.back();

			cfunc_type.setup( _name, _proxy, _cfunc, _hasargs, _module );
		}
	}

}