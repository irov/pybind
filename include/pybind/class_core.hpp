#	pragma once

#	include "pybind/class_scope.hpp"

#	include "pybind/constructor.hpp"
#	include "pybind/exports.hpp"
#	include "pybind/types.hpp"

namespace pybind
{
	class class_type_scope;
	class method_proxy_interface;

	namespace detail
	{
		PYBIND_API void * get_class( PyObject * _obj );
		PYBIND_API class_type_scope * get_class_scope( PyObject * _obj );
		PYBIND_API void * check_registred_class( PyObject * _obj, const type_info & _info );
	}

	template<class T>
	const type_info & class_info()
	{
		return typeid( T );
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

		static PyObject * create_holder( const type_info & _info, void * _impl );
		static PyObject * create_impl( const type_info & _info, void * _impl );
		static void wrap_holder( PyObject * _obj, void * _impl );

		static void def_init( class_type_scope * _scope, constructor * _ctr );
		static void * construct( class_type_scope * _scope, PyObject * _args );

		static void set_module( class_type_scope * _scope, PyObject * _module );

		static void def_method( 
			const char * _name, 
			method_proxy_interface * _ifunc, 
			int _arity, 
			const type_info & _info );

		static void add_method_from_scope( class_type_scope * _scope, class_type_scope * _basescope );

		template<class C, class B>
		static void add_method_from_base()
		{
			class_type_scope * _scope = class_scope::get_class_scope( class_info<C>() );
			class_type_scope * _basescope = class_scope::get_class_scope( class_info<B>() );
			
			add_method_from_scope(  _scope, _basescope );
		}

		static void add_base_to_scope( class_type_scope * _scope, const char * _name, class_type_scope * _base, pybind_metacast cast );

		template<class C, class B>
		static void add_base( pybind_metacast cast)
		{
			class_type_scope * _scope = class_scope::get_class_scope( class_info<C>() );
			class_type_scope * _basescope = class_scope::get_class_scope( class_info<B>() );

			const type_info & tinfo = class_info<B*>();

			const char * name = tinfo.name();

			add_base_to_scope( _scope, name, _basescope, cast );			
		}

		static void * meta_cast( void * _impl, class_type_scope * _scope, const char * _name );

		static PyObject * new_impl( PyTypeObject * _type, PyObject * _args, void * _impl, const type_info & _tinfo );
		static void * dealloc_impl( PyObject * _obj );
	};

	template<class T>
	PyObject * class_holder( T * _value )
	{
		PyObject * result = class_core::create_holder( typeid(T), _value );

		return result;
	}
}

