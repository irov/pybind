#	include "pybind/functor.hpp"
#	include "pybind/functor_type.hpp"

#	include "pybind/system.hpp"
#	include "config/python.hpp"

#	include <list>

namespace pybind
{
	namespace detail
	{
		class functor_type_scope
		{
		public:
			functor_type_scope()
			{
			}

		public:
			void setup( const char * _name, functor_proxy_interface * _proxy, pybind_cfunction _cfunc, int _hasargs, PyObject * _module )
			{
				m_method.ml_name = _name;
				m_method.ml_meth = _cfunc;
				m_method.ml_flags = METH_CLASS | ( _hasargs ) ? METH_VARARGS : METH_NOARGS;
				m_method.ml_doc = "Embedding function cpp";

				py_functor_type * self = gen_functor();

				self->proxy = _proxy;

				PyObject *py_func = PyCFunction_New( &m_method, (PyObject*)self );
				Py_DECREF( self );

				PyModule_AddObject( _module, _name, py_func );
			}

		protected:
			PyMethodDef m_method;
		};

		static class Garbage
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
		} s_garbage;

		void def_functor( const char * _name, functor_proxy_interface * _proxy, pybind_cfunction _cfunc, int _hasargs, PyObject * _module )
		{
			if( _module == 0 )
			{
				_module = get_currentmodule();
			}

			s_garbage.push_back( functor_type_scope());
			functor_type_scope & cfunc_type = s_garbage.back();

			cfunc_type.setup( _name, _proxy, _cfunc, _hasargs, _module );
		}
	}
}
