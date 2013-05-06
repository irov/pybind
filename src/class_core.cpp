#	include "pybind/class_core.hpp"

#	include "config/python.hpp"

#	include "class_type.hpp"
#   include "static_var.hpp"

#	include <vector>

namespace pybind
{	
    namespace detail
    {
        //////////////////////////////////////////////////////////////////////////
        STATIC_DECLARE(TVectorTypeScope, s_typeScope);
        //////////////////////////////////////////////////////////////////////////
        void reg_class_type_scope( size_t _info, class_type_scope * _scope )
        {
            if( STATIC_VAR(s_typeScope).size() <= _info )
            {
                STATIC_VAR(s_typeScope).resize( _info + 1 );
            }

            STATIC_VAR(s_typeScope)[_info] = _scope;
        }
        //////////////////////////////////////////////////////////////////////////
        class_type_scope * get_class_type_scope( size_t _info )
        {
            class_type_scope * scope = STATIC_VAR(s_typeScope)[_info];

            return scope;		
        }
        //////////////////////////////////////////////////////////////////////////
        void get_types_scope( TVectorTypeScope & _types )
        {
            for( TVectorTypeScope::iterator
                it = STATIC_VAR(s_typeScope).begin(),
                it_end = STATIC_VAR(s_typeScope).end();
            it != it_end;
            ++it )
            {
                class_type_scope * scope = *it;
                _types.push_back( scope );
            }
        }
    }
    //////////////////////////////////////////////////////////////////////////
    typedef std::vector<class_type_scope *> TListClassType;
    //////////////////////////////////////////////////////////////////////////
    STATIC_DECLARE(TListClassType, s_listClassType);
    //////////////////////////////////////////////////////////////////////////
    void class_core::finalize()
    {
        for( TListClassType::iterator
            it = STATIC_VAR(s_listClassType).begin(),
            it_end = STATIC_VAR(s_listClassType).end();
        it != it_end;
        ++it )
        {
            class_type_scope * scope = *it;

            //scope->finalize();
            scope->decref();
        }

        STATIC_VAR(s_listClassType).clear();
    }
    //////////////////////////////////////////////////////////////////////////
    class_type_scope * class_core::create_new_type_scope( size_t _info, const char * _name, PyObject * _module, void * _user, pybind::pybind_new _pynew, pybind::pybind_destructor _pydestructor, bool _pod )
    {
        class_type_scope * scope = new class_type_scope( _name, _info, _module, _user, _pynew, _pydestructor, _pod );

        detail::reg_class_type_scope( _info, scope );

        STATIC_VAR(s_listClassType).push_back( scope );

        return scope;
    }
    //////////////////////////////////////////////////////////////////////////
    void class_core::initialize_new_type_scope( class_type_scope * _scope )
    {
        _scope->initialize();
    }
    //////////////////////////////////////////////////////////////////////////
    PyTypeObject * class_core::get_typemodule( size_t _info )
    {
        class_type_scope * t_scope = detail::get_class_type_scope( _info );

        PyTypeObject * typemodule = t_scope->get_typemodule();

        return typemodule;
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * class_core::create_holder( size_t _info, void * _impl )
    {
        class_type_scope * t_scope = detail::get_class_type_scope( _info );

        return t_scope->create_holder( _impl );
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * class_core::create_impl( size_t _info, void * _impl )
    {
        class_type_scope * t_scope = detail::get_class_type_scope( _info );

        return t_scope->create_impl( _impl );
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * class_core::create_pod( size_t _info, void ** _impl )
    {
        class_type_scope * t_scope = detail::get_class_type_scope( _info );

        return t_scope->create_pod( _impl );
    }
    //////////////////////////////////////////////////////////////////////////
    void class_core::wrap_holder( PyObject * _obj, void * _impl )
    {
        detail::wrap( _obj, _impl, true );
    }
    //////////////////////////////////////////////////////////////////////////
    void class_core::def_init( class_type_scope * _scope, constructor * _ctr )
    {
        _scope->def_init( _ctr );
    }
    //////////////////////////////////////////////////////////////////////////
    void * class_core::construct( class_type_scope * _scope, PyObject * _obj, PyObject * _args )
    {
        void * impl = _scope->construct( _obj, _args );

        return impl;
    }
    //////////////////////////////////////////////////////////////////////////
    void class_core::set_module( class_type_scope * _scope, PyObject * _module )
    {
        _scope->set_module( _module );
    }
    void class_core::def_convert( convert_adapter_interface * _iadapter, size_t _info )
    {
        class_type_scope * scope = detail::get_class_type_scope( _info );

        scope->set_convert( _iadapter );
    }
    //////////////////////////////////////////////////////////////////////////
    void class_core::def_method( const char * _name, method_adapter_interface * _iadapter, size_t _info )
    {
        class_type_scope * scope = detail::get_class_type_scope( _info );

        scope->add_method( _name, _iadapter );
    }
    //////////////////////////////////////////////////////////////////////////
    void class_core::def_member( const char * _name, member_adapter_interface * _iadapter, size_t _info )
    {
        class_type_scope * scope = detail::get_class_type_scope( _info );

        scope->add_member( _name, _iadapter );
    }
    //////////////////////////////////////////////////////////////////////////
    void class_core::def_call( method_adapter_interface * _iadapter, size_t _info )
    {
        class_type_scope * scope = detail::get_class_type_scope( _info );

        scope->set_call( _iadapter );
    }
    //////////////////////////////////////////////////////////////////////////
    void class_core::def_repr( repr_adapter_interface * _iadapter, size_t _info )
    {
        class_type_scope * scope = detail::get_class_type_scope( _info );

        scope->set_repr( _iadapter );
    }
    //////////////////////////////////////////////////////////////////////////
    void class_core::def_hash( hash_adapter_interface * _iadapter, size_t _info )
    {
        class_type_scope * scope = detail::get_class_type_scope( _info );

        scope->set_hash( _iadapter );
    }
    //////////////////////////////////////////////////////////////////////////
    void class_core::def_compare( compare_adapter_interface * _iadapter, size_t _info )
    {
        class_type_scope * scope = detail::get_class_type_scope( _info );

        scope->set_compare( _iadapter );
    }
    //////////////////////////////////////////////////////////////////////////
    void class_core::def_getattro( method_adapter_interface * _iadapter, size_t _info )
    {
        class_type_scope * scope = detail::get_class_type_scope( _info );

        scope->set_getattro( _iadapter );		
    }
    //////////////////////////////////////////////////////////////////////////
    void class_core::def_mapping( method_adapter_interface * _iadapter, size_t _info )
    {
        class_type_scope * scope = detail::get_class_type_scope( _info );

        scope->set_mapping( _iadapter );
    }
    //////////////////////////////////////////////////////////////////////////
    void class_core::def_sequence( method_adapter_interface * _iadapter, size_t _info )
    {
        class_type_scope * scope = detail::get_class_type_scope( _info );

        scope->set_sequence( _iadapter );
    }	
    //////////////////////////////////////////////////////////////////////////
    void class_core::add_base_to_scope( class_type_scope * _scope, size_t _name, size_t _base, pybind_metacast cast )
    {
        class_type_scope * basescope = detail::get_class_type_scope( _base );

        _scope->add_base( _name, basescope, cast );
    }
    //////////////////////////////////////////////////////////////////////////
    void * class_core::meta_cast( void * _impl, class_type_scope * _scope, size_t _name )
    {
        return _scope->metacast( _name, _impl );
    }
}
