#	pragma once

#	include "pybind/constructor.hpp"
#	include "pybind/exports.hpp"
#	include "pybind/types.hpp"

#   include <vector>

namespace pybind
{
	class class_type_scope;

	class convert_adapter_interface;
	class method_adapter_interface;
	class member_adapter_interface;
	class repr_adapter_interface;
    class hash_adapter_interface;
    class compare_adapter_interface;

	class PYBIND_API class_core
	{
	public:
		static class_type_scope * create_new_type_scope( size_t _info, const char * _name, PyObject * _module, void * _user, pybind::pybind_new _pynew, pybind::pybind_destructor _pydestructor, bool _pod );
		static void initialize_new_type_scope( class_type_scope * _scope );
        
		static void finalize();

        static PyTypeObject * get_typemodule( size_t _info );

		static PyObject * create_holder( size_t _info, void * _impl );
		static PyObject * create_impl( size_t _info, void * _impl );
		static PyObject * create_pod( size_t _info, void ** _impl );

		static void wrap_holder( PyObject * _obj, void * _impl );

		static void def_init( class_type_scope * _scope, constructor * _ctr );
		static void * construct( class_type_scope * _scope, PyObject * _obj, PyObject * _args );

		static void set_module( class_type_scope * _scope, PyObject * _module );

		static void def_convert( convert_adapter_interface * _iadapter,	size_t _info );
		static void def_method( const char * _name, method_adapter_interface * _iadapter, size_t _info );
		static void def_member( const char * _name, member_adapter_interface * _iadapter, size_t _info );

        static void def_call( method_adapter_interface * _iadapter, size_t _info );
		static void def_repr( repr_adapter_interface * _iadapter, size_t _info );
        static void def_hash( hash_adapter_interface * _iadapter, size_t _info );
        static void def_compare( compare_adapter_interface * _iadapter, size_t _info );
		static void def_getattro( method_adapter_interface * _iadapter, size_t _info );
		static void def_mapping( method_adapter_interface * _iadapter, size_t _info );
		static void def_sequence( method_adapter_interface * _iadapter, size_t _info );

		static void add_base_to_scope( class_type_scope * _scope, size_t _name, size_t _base, pybind_metacast _cast );

		template<class B>
		static void add_base( class_type_scope * _scope, pybind_metacast _cast)
		{
			size_t tptrinfo = class_info<B*>();

			size_t tinfo = class_info<B>();

			add_base_to_scope( _scope, tptrinfo, tinfo, _cast );			
		}

		static void * meta_cast( void * _impl, class_type_scope * _scope, size_t _name );
	};

	template<class T>
	PyObject * class_holder( T * _value )
	{
        size_t id = class_info<T>();

		PyObject * result = class_core::create_holder( id, _value );

		return result;
	}
}

