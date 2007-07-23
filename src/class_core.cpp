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
		const type_info & _info,
		const char * _name, 
		PyObject * _module,
		pybind_newfunc _pynew,
		pybind_destructor _pydestructor)
	{
		s_listClassType.push_back( class_type_scope( _name, _module, _pynew, _pydestructor ) );

		class_type_scope * t_scope = &s_listClassType.back();

		class_scope::reg_class_scope( _info, t_scope );

		return t_scope;
	}
	//////////////////////////////////////////////////////////////////////////
	PyObject * class_core::create_holder( const type_info & _info, void * _impl )
	{
		class_type_scope * t_scope = class_scope::get_class_scope( _info );

		return t_scope->create_holder( _impl );
	}
	//////////////////////////////////////////////////////////////////////////
	void class_core::set_module( class_type_scope * _scope, PyObject * _module )
	{
		_scope->set_module( _module );
	}
	//////////////////////////////////////////////////////////////////////////
	void class_core::def_method( const char * _name, PyCFunction pyfunc, int _arity, const type_info & _info )
	{
		PyMethodDef md = {
			_name,
			pyfunc,
			METH_CLASS | ( _arity ) ? METH_VARARGS : METH_NOARGS,
			"Embedding method cpp"
		};

		class_type_scope * scope = class_scope::get_class_scope( _info );

		scope->add_method( md );
	}
	//////////////////////////////////////////////////////////////////////////
	void class_core::add_method_from_scope( class_type_scope * _scope, class_type_scope * _basescope )
	{
		_scope->add_method_from_scope( _basescope );
	}
	//////////////////////////////////////////////////////////////////////////
	void class_core::end_method( const type_info & _info )
	{
		class_type_scope * scope = class_scope::get_class_scope( _info );

		scope->setup_type();
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