#	pragma once

#	include "pybind/class_info.hpp"

#	include "pybind/types.hpp"
#	include "pybind/exports.hpp"

namespace pybind
{
	typedef stdex::intrusive_ptr<class class_type_scope> class_type_scope_ptr;

    typedef stdex::intrusive_ptr<class repr_adapter_interface> repr_adapter_interface_ptr;
    typedef stdex::intrusive_ptr<class method_adapter_interface> method_adapter_interface_ptr;
    typedef stdex::intrusive_ptr<class member_adapter_interface> member_adapter_interface_ptr;
    typedef stdex::intrusive_ptr<class convert_adapter_interface> convert_adapter_interface_ptr;
    typedef stdex::intrusive_ptr<class hash_adapter_interface> hash_adapter_interface_ptr;
    typedef stdex::intrusive_ptr<class compare_adapter_interface> compare_adapter_interface_ptr;
	typedef stdex::intrusive_ptr<class constructor_adapter_interface> constructor_adapter_interface_ptr;
    
	class PYBIND_API class_core
	{
	public:
		static class_type_scope_ptr create_new_type_scope( uint32_t _info, const char * _name, void * _user, pybind::pybind_new _pynew, pybind::pybind_destructor _pydestructor, size_t _pod );
		static void initialize_new_type_scope( const class_type_scope_ptr & _scope, PyObject * _module );

        static PyTypeObject * get_typemodule( uint32_t _info );

		static PyObject * create_class( uint32_t _info, void * _impl );
		static PyObject * create_holder( uint32_t _info, void * _impl );
		static PyObject * create_pod( uint32_t _info, void ** _impl, size_t _size );

		static void def_init( const class_type_scope_ptr & _scope, const constructor_adapter_interface_ptr & _ctr );
		static void * construct( const class_type_scope_ptr & _scope, PyObject * _obj, PyObject * _args );

		static void def_convert( const convert_adapter_interface_ptr & _iadapter, uint32_t _info );
		static void def_method( const method_adapter_interface_ptr & _iadapter, uint32_t _info );
		static void def_member( const member_adapter_interface_ptr & _iadapter, uint32_t _info );

        static void def_call( const method_adapter_interface_ptr & _iadapter, uint32_t _info );
		static void def_repr( const repr_adapter_interface_ptr & _iadapter, uint32_t _info );
        static void def_hash( const hash_adapter_interface_ptr &  _iadapter, uint32_t _info );
        static void def_compare( const compare_adapter_interface_ptr & _iadapter, uint32_t _info );
		static void def_getattro( const method_adapter_interface_ptr & _iadapter, uint32_t _info );
		static void def_mapping( const method_adapter_interface_ptr & _iadapter, uint32_t _info );
		static void def_sequence_get( const method_adapter_interface_ptr & _iadapter, uint32_t _info );
		static void def_sequence_set( const method_adapter_interface_ptr & _iadapter, uint32_t _info );

		static void add_base_to_scope( const class_type_scope_ptr & _scope, uint32_t _name, uint32_t _base, pybind_metacast _cast );

		template<class B>
		static void add_base( const class_type_scope_ptr & _scope, pybind_metacast _cast)
		{
			uint32_t tptrinfo = class_info<B*>();

			uint32_t tinfo = class_info<B>();

			if( detail::has_class_type_scope( tinfo ) == false )
			{
				pybind::throw_exception( "class_core::add_base %s not bind base type %s"
					, _scope->get_name()
					, pybind::detail::get_class_type_info( tinfo )
					);

				return;
			}

            class_core::add_base_to_scope( _scope, tptrinfo, tinfo, _cast );
		}

		static void * meta_cast( void * _impl, const class_type_scope_ptr & _scope, uint32_t _name );
	};

	template<class T>
	PyObject * class_holder( T * _value )
	{
        uint32_t id = class_info<T>();

		PyObject * result = class_core::create_holder( id, _value );

		return result;
	}
}

