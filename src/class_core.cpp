#	include "pybind/class_core.hpp"
#	include "pybind/class_type_scope.hpp"

#	include "pybind/exception.hpp"
#	include "pybind/system.hpp"

#	include "config/python.hpp"

#   include "static_var.hpp"

namespace pybind
{	
    //////////////////////////////////////////////////////////////////////////
    class_type_scope_ptr class_core::create_new_type_scope( size_t _info, const char * _name, PyObject * _module, void * _user, pybind::pybind_new _pynew, pybind::pybind_destructor _pydestructor, bool _pod )
    {
        class_type_scope_ptr scope = new class_type_scope( _name, _info, _module, _user, _pynew, _pydestructor, _pod );

        if( detail::reg_class_type_scope( _info, scope ) == false )
		{
			return nullptr;
		}

        return scope;
    }
    //////////////////////////////////////////////////////////////////////////
    void class_core::initialize_new_type_scope( const class_type_scope_ptr & _scope )
    {
        _scope->initialize();
    }
    //////////////////////////////////////////////////////////////////////////
    PyTypeObject * class_core::get_typemodule( size_t _info )
    {
        const class_type_scope_ptr & t_scope = detail::get_class_type_scope( _info );

        PyTypeObject * typemodule = t_scope->get_typemodule();

        return typemodule;
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * class_core::create_holder( size_t _info, void * _impl )
    {
        const class_type_scope_ptr & t_scope = detail::get_class_type_scope( _info );

        PyObject * obj = t_scope->create_holder( _impl );

		return obj;
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * class_core::create_pod( size_t _info, void ** _impl, size_t & _size )
    {
        const class_type_scope_ptr & t_scope = detail::get_class_type_scope( _info );

        PyObject * obj = t_scope->create_pod( _impl, _size );

		return obj;
    }
    //////////////////////////////////////////////////////////////////////////
    void class_core::def_init( const class_type_scope_ptr & _scope, const constructor_adapter_interface_ptr & _ctr )
    {
        _scope->def_init( _ctr );
    }
    //////////////////////////////////////////////////////////////////////////
    void * class_core::construct( const class_type_scope_ptr & _scope, PyObject * _obj, PyObject * _args )
    {
		try
		{
			void * impl = _scope->construct( _obj, _args );

			return impl;
		}
		catch( const pybind::pybind_exception & _ex )
		{
			pybind::error_message("obj %s invalid construct '%s' error '%s'\n"
				, pybind::object_str( _obj )
				, _scope->get_name()
				, _ex.what()
				);
		}

		return nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    void class_core::set_module( const class_type_scope_ptr & _scope, PyObject * _module )
    {
        _scope->set_module( _module );
    }
    //////////////////////////////////////////////////////////////////////////
    void class_core::def_convert( const convert_adapter_interface_ptr & _iadapter, size_t _info )
    {
        const class_type_scope_ptr & scope = detail::get_class_type_scope( _info );

        scope->set_convert( _iadapter );
    }
    //////////////////////////////////////////////////////////////////////////
    void class_core::def_method( const method_adapter_interface_ptr & _iadapter, size_t _info )
    {
        const class_type_scope_ptr & scope = detail::get_class_type_scope( _info );

        scope->add_method( _iadapter );
    }
    //////////////////////////////////////////////////////////////////////////
    void class_core::def_member( const member_adapter_interface_ptr & _iadapter, size_t _info )
    {
        const class_type_scope_ptr & scope = detail::get_class_type_scope( _info );

        scope->add_member( _iadapter );
    }
    //////////////////////////////////////////////////////////////////////////
    void class_core::def_call( const method_adapter_interface_ptr & _iadapter, size_t _info )
    {
        const class_type_scope_ptr & scope = detail::get_class_type_scope( _info );

        scope->set_call( _iadapter );
    }
    //////////////////////////////////////////////////////////////////////////
    void class_core::def_repr( const repr_adapter_interface_ptr & _iadapter, size_t _info )
    {
        const class_type_scope_ptr & scope = detail::get_class_type_scope( _info );

        scope->set_repr( _iadapter );
    }
    //////////////////////////////////////////////////////////////////////////
    void class_core::def_hash( const hash_adapter_interface_ptr & _iadapter, size_t _info )
    {
        const class_type_scope_ptr & scope = detail::get_class_type_scope( _info );

        scope->set_hash( _iadapter );
    }
    //////////////////////////////////////////////////////////////////////////
    void class_core::def_compare( const compare_adapter_interface_ptr & _iadapter, size_t _info )
    {
        const class_type_scope_ptr & scope = detail::get_class_type_scope( _info );

        scope->set_compare( _iadapter );
    }
    //////////////////////////////////////////////////////////////////////////
    void class_core::def_getattro( const method_adapter_interface_ptr & _iadapter, size_t _info )
    {
        const class_type_scope_ptr & scope = detail::get_class_type_scope( _info );

        scope->set_getattro( _iadapter );		
    }
    //////////////////////////////////////////////////////////////////////////
    void class_core::def_mapping( const method_adapter_interface_ptr & _iadapter, size_t _info )
    {
        const class_type_scope_ptr & scope = detail::get_class_type_scope( _info );

        scope->set_mapping( _iadapter );
    }
    //////////////////////////////////////////////////////////////////////////
    void class_core::def_sequence( const method_adapter_interface_ptr & _iadapter, size_t _info )
    {
        const class_type_scope_ptr & scope = detail::get_class_type_scope( _info );

        scope->set_sequence( _iadapter );
    }	
    //////////////////////////////////////////////////////////////////////////
    void class_core::add_base_to_scope( const class_type_scope_ptr & _scope, size_t _name, size_t _base, pybind_metacast cast )
    {
        const class_type_scope_ptr & basescope = detail::get_class_type_scope( _base );

        _scope->add_base( _name, basescope, cast );
    }
    //////////////////////////////////////////////////////////////////////////
    void * class_core::meta_cast( void * _impl, const class_type_scope_ptr & _scope, size_t _name )
    {
        void * impl_cast = _scope->metacast( _name, _impl );

        return impl_cast;
    }
}
