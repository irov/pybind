#	include "pybind/function.hpp"

#	include "pybind/system.hpp"

#	include "function_type.hpp"
#   include "static_var.hpp"

#	include "config/python.hpp"

#	include <list>
#   include <vector>

namespace pybind
{
	namespace detail
	{		
        //////////////////////////////////////////////////////////////////////////
        static function_adapter_interface * extract_adapter_py_function( PyObject * _self )
        {
            py_function_type * function_type = reinterpret_cast<py_function_type*>(_self);

            function_adapter_interface * iadapter = function_type->iadapter;

            return iadapter;
        }
        //////////////////////////////////////////////////////////////////////////
        static PyObject * function_kwds( PyObject * _self, PyObject * _args, PyObject * _kwds )
        {
            function_adapter_interface * ifunction = detail::extract_adapter_py_function( _self );

            PyObject * ret = ifunction->call( _args, _kwds );

            return ret;
        }
        //////////////////////////////////////////////////////////////////////////
        static PyObject * function_args( PyObject * _self, PyObject * _args )
        {
            PyObject * arg = function_kwds( _self, _args, 0 );

            return arg;
        }
        //////////////////////////////////////////////////////////////////////////
        static PyObject * function_noargs( PyObject * _self )
        {
            PyObject * arg = function_args( _self, 0 );

            return arg;
        }
        //////////////////////////////////////////////////////////////////////////
		class function_type_scope
		{
		public:
			function_type_scope()
			{
			}

		public:
			PyObject * setup( function_adapter_interface * _adapter, bool _native )
			{
				size_t arity = _adapter->getArity();

				const char * name = _adapter->getName();

				m_method.ml_name = name;

				if( _native == true )
				{
					m_method.ml_meth = (PyCFunction)&function_kwds;
					m_method.ml_flags = METH_CLASS | METH_VARARGS | METH_KEYWORDS;
				}
				else
				{
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
				}


				m_method.ml_doc = "Embedding function cpp";

				py_function_type * py_self = gen_function();

				py_self->iadapter = _adapter;

				PyObject * py_func = PyCFunction_New( &m_method, (PyObject*)py_self );
				Py_DecRef( (PyObject *)py_self );

				return py_func;
			}

		protected:
			PyMethodDef m_method;
		};
        //////////////////////////////////////////////////////////////////////////
		class FunctionGarbage
		{
		public:
			~FunctionGarbage()
			{
				for( TVectorAdapter::iterator
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

                function_type_scope & scope = m_scopes.back();

				return scope;
			}

			void storeAdapter( function_adapter_interface * _adapter )
			{
				m_adapters.push_back( _adapter );
			}

		protected:
			typedef std::list<function_type_scope> TListScope;
			TListScope m_scopes;

			typedef std::vector<function_adapter_interface *> TVectorAdapter;
			TVectorAdapter m_adapters;
		};
        //////////////////////////////////////////////////////////////////////////
        STATIC_DECLARE(FunctionGarbage, s_functionGarbage);
        //////////////////////////////////////////////////////////////////////////
		PyObject * create_function_adapter( function_adapter_interface * _adapter, bool _native )
		{
			function_type_scope & cfunc_type = STATIC_VAR(s_functionGarbage).newScope();
			
			PyObject * py_func = cfunc_type.setup( _adapter, _native );

            STATIC_VAR(s_functionGarbage).storeAdapter( _adapter );

			return py_func;
		}
        //////////////////////////////////////////////////////////////////////////
		void def_function_adapter( function_adapter_interface * _adapter, bool _native, PyObject * _module )
		{
			PyObject * py_func = create_function_adapter( _adapter, _native );

			if( _module == 0 )
			{
				_module = get_currentmodule();
			}

			const char * name = _adapter->getName();

			module_addobject( _module, name, py_func );
		}
	}
}
