#	include "pybind/function.hpp"
#	include "pybind/function_type.hpp"

#	include "pybind/system.hpp"
#	include "config/python.hpp"

#	include <list>

namespace pybind
{
	static PyTypeObject s_function_type;

	namespace detail
	{		
		class function_type_scope
		{
		public:
			function_type_scope()
			{
			}

		public:
			PyObject * setup( function_adapter_interface * _adapter )
			{
				int arity = _adapter->getArity();

				const char * name = _adapter->getName();

				m_method.ml_name = name;

				if( arity > 0 )
				{
					m_method.ml_meth = (PyCFunction)&function_args;
					m_method.ml_flags = METH_CLASS | METH_VARARGS;
				}
				else
				{
					m_method.ml_meth = (PyCFunction)&function_noargs;
					m_method.ml_flags = METH_CLASS | METH_NOARGS;
				}

				m_method.ml_doc = "Embedding function cpp";

				py_function_type * py_self = (py_function_type *)PyType_GenericAlloc( &s_function_type, 0 );

				py_self->ifunction = _adapter;

				PyObject * py_func = PyCFunction_New( &m_method, (PyObject*)py_self );
				Py_DECREF( py_self );

				return py_func;
			}

		protected:
			PyMethodDef m_method;
		};

		static class Garbage
		{
		public:
			~Garbage()
			{
				for( TListAdapter::iterator
					it = m_adapters.begin(),
					it_end = m_adapters.end();
				it != it_end;
				++it )
				{
					delete *it;
				}
			}

		public:
			function_type_scope & newScope()
			{
				m_scopes.push_back( function_type_scope() );

				return m_scopes.back();
			}

			void storeAdapter( function_adapter_interface * _adapter )
			{
				m_adapters.push_back( _adapter );
			}

		protected:
			typedef std::list<function_type_scope> TListScope;
			TListScope m_scopes;

			typedef std::list<function_adapter_interface *> TListAdapter;
			TListAdapter m_adapters;
		} s_garbage;

		function_adapter_interface * extract_adapter_py_function( PyObject * _self )
		{
			py_function_type * function_type = reinterpret_cast<py_function_type*>(_self);

			return function_type->ifunction;
		}

		PyObject * function_noargs( PyObject * _self )
		{
			return function_args( _self, 0 );
		}

		PyObject * function_args( PyObject * _self, PyObject * _args )
		{
			function_adapter_interface * ifunction = detail::extract_adapter_py_function( _self );

			PyObject * ret = ifunction->call( _args );

			return ret;
		}

		PyObject * function_native( PyObject * _self, PyObject * _args, PyObject * _kwds )
		{
			function_adapter_interface * ifunction = detail::extract_adapter_py_function( _self );

			PyObject * ret = ifunction->call_kwds( _args, _kwds );

			return ret;
		}

		void store_function_adapter( function_adapter_interface * _adapter )
		{
			s_garbage.storeAdapter(_adapter);
		}

		PyObject * create_function_adapter( function_adapter_interface * _adapter )
		{
			function_type_scope & cfunc_type = s_garbage.newScope();
			
			PyObject * py_func = cfunc_type.setup( _adapter );

			return py_func;
		}
	}

	static void py_dealloc( PyObject * _obj )
	{
		_obj->ob_type->tp_free( _obj );
	}

	void initialize_function()
	{
		s_function_type.tp_name = "function_type_scope";
		s_function_type.tp_basicsize = sizeof( py_function_type );
		s_function_type.tp_dealloc = &py_dealloc;
		s_function_type.tp_flags = Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE;

		if( PyType_Ready( &s_function_type ) < 0 )
		{
			printf("invalid embedding class '%s' \n", s_function_type.tp_name );					
		}
	}

	void finalize_function()
	{
		//Py_DecRef((PyObject*)s_def_type);
	}
}
