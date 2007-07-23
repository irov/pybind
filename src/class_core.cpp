#	include "pybind/class_core.hpp"

#	include "pybind/class_scope.hpp"
#	include "pybind/class_type.hpp"

#	include "pybind/config.hpp"

#	include <list>

namespace pybind
{
	//////////////////////////////////////////////////////////////////////////
	typedef std::list<class_type_scope> TListClassType;
	static TListClassType s_listClassType;
	//////////////////////////////////////////////////////////////////////////
	class_type_scope * class_core::create_new_type_scope( 
		const char * _name, 
		module_ * _module,
		pybind_newfunc _pynew,
		pybind_destructor _pydestructor)
	{
		s_listClassType.push_back( class_type_scope( _name, _module, _pynew, _pydestructor ) );

		return &s_listClassType.back();
	}
	//////////////////////////////////////////////////////////////////////////
	void class_core::set_module( class_type_scope * m_type_scope, module_ * _module )
	{
		m_type_scope->set_module( _module );
	}
	//////////////////////////////////////////////////////////////////////////
	void class_core::def_method( const char * _name, PyCFunction pyfunc, int _arity, class_type_scope * _scope )
	{
		PyMethodDef md = {
			_name,
			pyfunc,
			METH_CLASS | ( _arity ) ? METH_VARARGS : METH_NOARGS,
			"Embedding method cpp"
		};

		_scope->add_method( md );
	}
	//////////////////////////////////////////////////////////////////////////
	void class_core::add_method_from_scope( class_type_scope * _scope, class_type_scope * _basescope )
	{
		_scope->add_method_from_scope( _basescope );
	}
	//////////////////////////////////////////////////////////////////////////
	void class_core::end_method( const type_info & _info, class_type_scope * _scope )
	{
		_scope->setup_type( _info );
	}
	//////////////////////////////////////////////////////////////////////////
	PyObject * class_core::new_impl( PyTypeObject * _type, PyObject * _args, void * _impl )
	{
		py_class_type *self = 
			(py_class_type *)_type->tp_alloc( _type, 0 );

		if( self != NULL )
		{
			self->impl = _impl;
		}

		return (PyObject *)self;
	}
	//////////////////////////////////////////////////////////////////////////
	void * class_core::dealloc_impl( PyObject * _obj )
	{
		py_class_type * self = (py_class_type *)(_obj);
		void * impl = self->impl;

		self->ob_type->tp_free( (PyObject*)self );

		return impl;		
	}
}