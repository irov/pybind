#	pragma once

#	include "pybind/constructor.hpp"
#	include "pybind/exports.hpp"
#	include "pybind/types.hpp"

namespace pybind
{
	typedef stdex::intrusive_ptr<class class_type_scope> class_type_scope_ptr;

    typedef stdex::intrusive_ptr<class repr_adapter_interface> repr_adapter_interface_ptr;
    typedef stdex::intrusive_ptr<class method_adapter_interface> method_adapter_interface_ptr;
    typedef stdex::intrusive_ptr<class member_adapter_interface> member_adapter_interface_ptr;
    typedef stdex::intrusive_ptr<class convert_adapter_interface> convert_adapter_interface_ptr;
    typedef stdex::intrusive_ptr<class hash_adapter_interface> hash_adapter_interface_ptr;
    typedef stdex::intrusive_ptr<class compare_adapter_interface> compare_adapter_interface_ptr;
    
	class PYBIND_API class_core
	{
	public:
		static class_type_scope_ptr create_new_type_scope( size_t _info, const char * _name, PyObject * _module, void * _user, pybind::pybind_new _pynew, pybind::pybind_destructor _pydestructor, bool _pod );
		static void initialize_new_type_scope( const class_type_scope_ptr & _scope );
        
		static void finalize();

        static PyTypeObject * get_typemodule( size_t _info );

		static PyObject * create_holder( size_t _info, void * _impl );
		static PyObject * create_impl( size_t _info, void * _impl );
		static PyObject * create_pod( size_t _info, void ** _impl );

		static void wrap_holder( PyObject * _obj, void * _impl );

		static void def_init( const class_type_scope_ptr & _scope, const constructor_adapter_interface_ptr & _ctr );
		static void * construct( const class_type_scope_ptr & _scope, PyObject * _obj, PyObject * _args );

		static void set_module( const class_type_scope_ptr & _scope, PyObject * _module );

		static void def_convert( const convert_adapter_interface_ptr & _iadapter,	size_t _info );
		static void def_method( const char * _name, const method_adapter_interface_ptr & _iadapter, size_t _info );
		static void def_member( const char * _name, const member_adapter_interface_ptr & _iadapter, size_t _info );

        static void def_call( const method_adapter_interface_ptr & _iadapter, size_t _info );
		static void def_repr( const repr_adapter_interface_ptr & _iadapter, size_t _info );
        static void def_hash( const hash_adapter_interface_ptr &  _iadapter, size_t _info );
        static void def_compare( const compare_adapter_interface_ptr & _iadapter, size_t _info );
		static void def_getattro( const method_adapter_interface_ptr & _iadapter, size_t _info );
		static void def_mapping( const method_adapter_interface_ptr & _iadapter, size_t _info );
		static void def_sequence( const method_adapter_interface_ptr & _iadapter, size_t _info );

		static void add_base_to_scope( const class_type_scope_ptr & _scope, size_t _name, size_t _base, pybind_metacast _cast );

		template<class B>
		static void add_base( const class_type_scope_ptr & _scope, pybind_metacast _cast)
		{
			size_t tptrinfo = class_info<B*>();

			size_t tinfo = class_info<B>();

            class_core::add_base_to_scope( _scope, tptrinfo, tinfo, _cast );
		}

		static void * meta_cast( void * _impl, const class_type_scope_ptr & _scope, size_t _name );
	};

	template<class T>
	PyObject * class_holder( T * _value )
	{
        size_t id = class_info<T>();

		PyObject * result = class_core::create_holder( id, _value );

		return result;
	}
}
