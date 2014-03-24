#	include "pybind/functor.hpp"

#	include "pybind/system.hpp"
#	include "config/python.hpp"

#	include "functor_type.hpp"
#   include "static_var.hpp"

namespace pybind
{
	namespace detail
	{
        //////////////////////////////////////////////////////////////////////////
        static const functor_adapter_interface_ptr & extract_adapter_py_functor( PyObject * _self )
        {
            py_functor_type * functor_type = reinterpret_cast<py_functor_type*>(_self);

            const functor_adapter_interface_ptr & adapter = functor_type->iadapter;

            return adapter;
        }
        //////////////////////////////////////////////////////////////////////////
        static PyObject * functor_kwds( PyObject * _self, PyObject * _args, PyObject * _kwds )
        {
            const functor_adapter_interface_ptr & adapter = detail::extract_adapter_py_functor( _self );

            PyObject * ret = adapter->call( _args, _kwds );

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
			PyObject * setup( const functor_adapter_interface_ptr & _adapter )
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
        functor_type_scope g_functor_type_scope[PYBIND_FUNCTOR_COUNT];
        size_t g_functor_type_scope_count = 0;
        //////////////////////////////////////////////////////////////////////////
        static PyObject * create_functor_adapter( const functor_adapter_interface_ptr & _adapter )
        {
            ++g_functor_type_scope_count;

            if( g_functor_type_scope_count == PYBIND_FUNCTOR_COUNT )
            {
                pybind::throw_exception();

                return nullptr;
            }

            functor_type_scope & cfunc_type = g_functor_type_scope[g_functor_type_scope_count];

            PyObject * py_func = cfunc_type.setup( _adapter );

            return py_func;
        }
        //////////////////////////////////////////////////////////////////////////
		void def_functor( const functor_adapter_interface_ptr & _adapter, PyObject * _module )
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