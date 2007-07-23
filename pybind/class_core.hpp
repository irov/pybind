#	pragma once

#	include "pybind/exports.hpp"

extern "C" 
{ 
	struct _object;
	typedef _object PyObject;
}

extern "C" 
{ 
	struct _typeobject; 
	typedef _typeobject PyTypeObject;
}

namespace pybind
{
	class module_;
	class class_type_scope;

	typedef PyObject * (*pybind_cfunction)(PyObject *, PyObject *);
	typedef PyObject * (*pybind_newfunc)(PyTypeObject *, PyObject *, PyObject *);
	typedef void (*pybind_destructor)(PyObject *);


	class PYBIND_API class_core
	{
	public:
		static class_type_scope * create_new_type_scope( 
			const char * _name, 
			module_ * _module,
			pybind_newfunc _pynew,
			pybind_destructor _pydestructor
			);

		void set_module( class_type_scope * m_type_scope, module_ * _module );

		static void def_method( 
			const char * _name, 
			pybind_cfunction pyfunc, 
			int _arity, 
			class_type_scope * _scope );

		static void add_method_from_scope( class_type_scope * _scope, class_type_scope * _basescope );

		static void end_method( const type_info & _info, class_type_scope * _scope );		

		static PyObject * new_impl( PyTypeObject * _type, PyObject * _args, void * _impl );
		static void * dealloc_impl( PyObject * _obj );
	};
}