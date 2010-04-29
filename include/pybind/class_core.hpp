#	pragma once

#	include "pybind/class_scope.hpp"

#	include "pybind/constructor.hpp"
#	include "pybind/exports.hpp"
#	include "pybind/types.hpp"

namespace pybind
{
	class class_type_scope;
	class method_adapter_interface;
	class member_adapter_interface;
	class repr_adapter_interface;

	namespace detail
	{
		PYBIND_API void * get_class( PyObject * _obj );
		PYBIND_API bool is_class( PyObject * _obj );
		PYBIND_API class_type_scope * get_class_scope( PyObject * _obj );
		PYBIND_API void * check_registred_class( PyObject * _obj, const std::type_info & _info );
	}

	template<class T>
	const std::type_info & class_info()
	{
		return typeid( T );
	}

	class PYBIND_API class_core
	{
	public:
		static class_type_scope * create_new_type_scope( 
			const std::type_info & _info,
			const char * _name
			);

		static void setup_new_type_scope( 
			class_type_scope * _scope,
			PyObject * _module,
			pybind_newfunc _pynew,
			pybind_destructor _pydestructor
			);


		static void finialize();

		static PyObject * create_holder( const std::type_info & _info, void * _impl );
		static PyObject * create_impl( const std::type_info & _info, void * _impl );
		static void wrap_holder( PyObject * _obj, void * _impl );

		static void def_init( class_type_scope * _scope, constructor * _ctr );
		static void * construct( class_type_scope * _scope, PyObject * _args );

		static void set_module( class_type_scope * _scope, PyObject * _module );

		static void def_method( 
			const char * _name, 
			method_adapter_interface * _iadapter, 
			int _arity, 
			const std::type_info & _info );

		static void def_member(
			const char * _name,
			member_adapter_interface * _iadapter,
			const std::type_info & _info );

		static void def_repr(
			repr_adapter_interface * _iadapter,
			const std::type_info & _info );

		static void add_base_to_scope( class_type_scope * _scope, const char * _name, class_type_scope * _base, pybind_metacast cast );

		template<class B>
		static void add_base( class_type_scope * _scope, pybind_metacast cast)
		{
			class_type_scope * basescope = class_scope::get_class_scope( class_info<B>() );

			const std::type_info & tinfo = class_info<B*>();

			const char * name = tinfo.name();

			add_base_to_scope( _scope, name, basescope, cast );			
		}

		static void * meta_cast( void * _impl, class_type_scope * _scope, const char * _name );

		static PyObject * new_impl( PyTypeObject * _type, PyObject * _args, void * _impl, const std::type_info & _tinfo );
		static void * dealloc_impl( PyObject * _obj );
	};

	template<class T>
	PyObject * class_holder( T * _value )
	{
		PyObject * result = class_core::create_holder( typeid(T), _value );

		return result;
	}
}

