#	include "pybind/functor.hpp"

#	include "pybind/system.hpp"
#	include "config/python.hpp"

#	include "functor_type.hpp"
#   include "static_var.hpp"

#	include <list>
#   include <vector>

namespace pybind
{
	namespace detail
	{
        //////////////////////////////////////////////////////////////////////////
        static functor_adapter_interface* extract_adapter_py_functor( PyObject * _self )
        {
            py_functor_type * functor_type = reinterpret_cast<py_functor_type*>(_self);

            return functor_type->iadapter;
        }
        //////////////////////////////////////////////////////////////////////////
        static PyObject * functor_kwds( PyObject * _self, PyObject * _args, PyObject * _kwds )
        {
            functor_adapter_interface * ifunction = detail::extract_adapter_py_functor( _self );

            PyObject * ret = ifunction->call( _args, _kwds );

            return ret;
        }
        //////////////////////////////////////////////////////////////////////////
        static PyObject * functor_args( PyObject * _self, PyObject * _args )
        {
            return functor_kwds( _self, _args, 0 );
        }
        //////////////////////////////////////////////////////////////////////////
        static PyObject * functor_noargs( PyObject * _self )
        {
            return functor_args( _self, 0 );
        }
        //////////////////////////////////////////////////////////////////////////
		class functor_type_scope
		{
		public:
			functor_type_scope()
			{
			}

		public:
			PyObject * setup( functor_adapter_interface * _adapter )
			{
                int arity = _adapter->getArity();
                const char * name = _adapter->getName();

				m_method.ml_name = name;

                if( arity > 0 )
                {
                    m_method.ml_meth = (PyCFunction)&functor_args;
                    m_method.ml_flags = METH_CLASS | METH_VARARGS;
                }
                else
                {
                    m_method.ml_meth = (PyCFunction)&functor_noargs;
                    m_method.ml_flags = METH_CLASS | METH_NOARGS;
                }

				m_method.ml_doc = "Embedding functor cpp";

				py_functor_type * self = gen_functor();

				self->iadapter = _adapter;

				PyObject *py_func = PyCFunction_New( &m_method, (PyObject*)self );
				Py_DecRef( (PyObject *)self );

                return py_func;
			}

		protected:
			PyMethodDef m_method;
		};
        //////////////////////////////////////////////////////////////////////////
		class FunctorGarbage
		{
        public:
            ~FunctorGarbage()
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
			functor_type_scope & newScope()
			{
                m_listTypeObject.push_back( functor_type_scope() );

                functor_type_scope & scope = m_listTypeObject.back();

                return scope;
			}

            void storeAdapter( functor_adapter_interface * _adapter )
            {
                m_adapters.push_back( _adapter );
            }

		protected:
			typedef std::list<functor_type_scope> TListTypeObject;
			TListTypeObject m_listTypeObject;

            typedef std::vector<functor_adapter_interface *> TVectorAdapter;
            TVectorAdapter m_adapters;
		};
        //////////////////////////////////////////////////////////////////////////
        STATIC_DECLARE(FunctorGarbage, s_functorGarbage);
        //////////////////////////////////////////////////////////////////////////
        static PyObject * create_functor_adapter( functor_adapter_interface * _adapter )
        {
            functor_type_scope & cfunc_type = STATIC_VAR(s_functorGarbage).newScope();

            PyObject * py_func = cfunc_type.setup( _adapter );

            STATIC_VAR(s_functorGarbage).storeAdapter( _adapter );

            return py_func;
        }
        //////////////////////////////////////////////////////////////////////////
		void def_functor( const char * _name, functor_adapter_interface * _adapter, PyObject * _module )
		{
            PyObject * py_func = create_functor_adapter( _adapter );

			if( _module == 0 )
			{
				_module = get_currentmodule();
			}

            const char * name = _adapter->getName();

            module_addobject( _module, name, py_func );
		}
	}
}
