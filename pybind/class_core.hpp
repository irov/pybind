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
	class class_type_scope;

	typedef PyObject * (*pybind_cfunction)(PyObject *, PyObject *);
	typedef PyObject * (*pybind_newfunc)(PyTypeObject *, PyObject *, PyObject *);
	typedef void (*pybind_destructor)(PyObject *);

	namespace detail
	{
		template<class T>
		struct rv_pointer
		{
			typedef T type;
		};

		template<class T> struct rv_pointer<T *>{	typedef typename rv_pointer<T>::type type;	};
		template<class T> struct rv_pointer<const T *>{	typedef typename rv_pointer<T>::type type;	};
	}

	template<class T>
	const type_info & class_info()
	{
		return typeid( detail::rv_pointer<T> );
	}

	class PYBIND_API class_core
	{
	public:
		static class_type_scope * create_new_type_scope( 
			const type_info & _info,
			const char * _name, 
			PyObject * _module,
			pybind_newfunc _pynew,
			pybind_destructor _pydestructor
			);

		PyObject * create_holder( const type_info & _info, void * _impl );

		void set_module( class_type_scope * m_type_scope, PyObject * _module );

		static void def_method( 
			const char * _name, 
			pybind_cfunction pyfunc, 
			int _arity, 
			const type_info & _info );

		static void add_method_from_scope( class_type_scope * _scope, class_type_scope * _basescope );

		template<class C, class B>
		static void add_method_from_base()
		{
			class_type_scope * _scope = get_class_scope( class_info<C>() );
			class_type_scope * _basescope = get_class_scope( class_info<B>() );
			
			add_method_from_scope(  _scope, _basescope );
		}

		static void end_method( const type_info & _info );		

		static PyObject * new_impl( PyTypeObject * _type, PyObject * _args, void * _impl );
		static void * dealloc_impl( PyObject * _obj );
	};
}