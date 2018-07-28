#include "python_kernel.hpp"

#include "pybind/system.hpp"
#include "pybind/exception.hpp"

#include "pybind/method_interface.hpp"

#include "python_class_type_scope.hpp"

#include "pybind/adapter/new_adapter.hpp"
#include "pybind/adapter/destroy_adapter.hpp"

namespace pybind
{
    //////////////////////////////////////////////////////////////////////////
    python_kernel::python_kernel()
        : m_current_module( nullptr )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    python_kernel::~python_kernel()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool python_kernel::initialize()
    {
        if( m_functions.initialize() == false )
        {
            return false;
        }

        if( m_functors.initialize() == false )
        {
            return false;
        }

        if( m_members.initialize() == false )
        {
            return false;
        }

        if( m_methods.initialize() == false )
        {
            return false;
        }

        if( m_pods.initialize() == false )
        {
            return false;
        }

        m_enumerator = 4;

        for( uint32_t index = 0; index != PYBIND_TYPE_COUNT; ++index )
        {
            class_info_desc_t & desc = m_class_info_desc[index];

            desc.name = nullptr;
        }

        m_class_type_dummy = new python_class_type_scope( this, "__dummy__", 0, nullptr, nullptr, nullptr, 0, false );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void python_kernel::finalize()
    {
        for( uint32_t index = 0; index != PYBIND_TYPE_COUNT; ++index )
        {
            class_type_scope_interface_ptr & scope = m_class_type_scopes[index];

            if( scope != nullptr )
            {
                scope->finalize();
                scope = nullptr;
            }

            m_class_type_scopes[index] = nullptr;

            m_type_cast[index] = nullptr;

            m_class_info_desc[index].name = nullptr;
        }

        for( uint32_t index = 0; index != PYBIND_TYPE_COUNT_HASH; ++index )
        {
            class_type_scope_interface_ptr & scope = m_class_type_hashes[index];

            scope = nullptr;
        }

        m_class_type_dummy = nullptr;

        m_functions.finalize();
        m_functors.finalize();
        m_members.finalize();
        m_methods.finalize();
        m_pods.finalize();
    }
    //////////////////////////////////////////////////////////////////////////
    void python_kernel::def_function_adapter( const function_adapter_interface_ptr & _adapter, bool _native, PyObject * _module )
    {
        PyObject * py_func = m_functions.create_function_adapter( _adapter, _native );

        if( _module == nullptr )
        {
            _module = this->get_current_module();

            if( _module == nullptr )
            {
                pybind::throw_exception( "def_function_adapter not setup module!"
                );

                return;
            }
        }

        const char * name = _adapter->getName();

        pybind::module_addobject( _module, name, py_func );
    }
    //////////////////////////////////////////////////////////////////////////
    void python_kernel::def_functor_adapter( const functor_adapter_interface_ptr & _adapter, bool _native, PyObject * _module )
    {
        PyObject * py_func = m_functors.create_functor_adapter( _adapter, _native );

        if( _module == nullptr )
        {
            _module = this->get_current_module();

            if( _module == nullptr )
            {
                pybind::throw_exception( "def_function_adapter not setup module!"
                );

                return;
            }
        }

        const char * name = _adapter->getName();

        pybind::module_addobject( _module, name, py_func );
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * python_kernel::create_function_adapter( const function_adapter_interface_ptr & _adapter, bool _native )
    {
        PyObject * py_func = m_functions.create_function_adapter( _adapter, _native );

        return py_func;
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * python_kernel::create_functor_adapter( const functor_adapter_interface_ptr & _adapter, bool _native )
    {
        PyObject * py_func = m_functors.create_functor_adapter( _adapter, _native );

        return py_func;
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * python_kernel::create_member( const member_adapter_interface_ptr & _iadapter )
    {
        PyObject * py_member = m_members.create_member_adapter( _iadapter );

        return py_member;
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * python_kernel::create_method( const method_adapter_interface_ptr & _iadapter, PyTypeObject * _type )
    {
        PyObject * py_member = m_methods.create_method_adapter( _iadapter, _type );

        return py_member;
    }
    //////////////////////////////////////////////////////////////////////////
    method_adapter_interface * python_kernel::get_method_adapter( PyObject * _obj )
    {
        method_adapter_interface * iadapter = m_methods.get_method_adapter( _obj );

        return iadapter;
    }
    //////////////////////////////////////////////////////////////////////////
    PyTypeObject * python_kernel::get_pod_type( uint32_t _pod, bool _hash )
    {
        PyTypeObject * py_pod_type = m_pods.get_pod_type( _pod, _hash );

        return py_pod_type;
    }
    //////////////////////////////////////////////////////////////////////////
    void python_kernel::set_current_module( PyObject * _module )
    {
        m_current_module = _module;
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * python_kernel::get_current_module() const
    {
        return m_current_module;
    }
    //////////////////////////////////////////////////////////////////////////
    void * python_kernel::unwrap( PyObject * _obj )
    {
        if( _obj == nullptr )
        {
            return nullptr;
        }

        void * impl = get_class_impl( _obj );

        pybind::detail::unwrap_pod( _obj );

        return impl;
    }
    //////////////////////////////////////////////////////////////////////////
    bool python_kernel::is_wrap( PyObject * _obj )
    {
        return pybind::detail::is_pod_wrap( _obj );
    }
    //////////////////////////////////////////////////////////////////////////
    void * python_kernel::meta_cast_scope( void * _self, uint32_t _typeId, uint32_t _name, const class_type_scope_interface_ptr & _scope )
    {
        uint32_t class_name = _scope->get_type_id();

        if( class_name == _typeId )
        {
            return _self;
        }

        void * impl_cast = _scope->meta_cast( _name, _self );

        return impl_cast;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t python_kernel::get_next_id()
    {
        uint32_t id = m_enumerator;

        m_enumerator++;

        return id;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t python_kernel::find_class_info_desc_name( const char * _name )
    {
        for( uint32_t index = 0; index != PYBIND_TYPE_COUNT; ++index )
        {
            const class_info_desc_t & desc = m_class_info_desc[index];

            if( desc.name == nullptr )
            {
                continue;
            }

            if( strcmp( desc.name, _name ) != 0 )
            {
                continue;
            }

            return index;
        }

        return 0;
    }
    //////////////////////////////////////////////////////////////////////////
    bool python_kernel::set_class_info_desc( uint32_t _typeId, const char * _info )
    {
        if( _typeId >= PYBIND_TYPE_COUNT )
        {
            return false;
        }

        class_info_desc_t & desc = m_class_info_desc[_typeId];

        if( desc.name != nullptr )
        {
            return false;
        }

        desc.name = _info;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t python_kernel::get_class_type_id( const std::type_info & _info )
    {
        const char * info_name = _info.name();

        uint32_t id = this->find_class_info_desc_name( info_name );

        if( id != 0 )
        {
            return id;
        }

        uint32_t new_id = this->get_next_id();

        if( this->set_class_info_desc( new_id, info_name ) == false )
        {
            pybind::throw_exception( "invalid create new type info" );

            return 0;
        }

        return new_id;
    }
    //////////////////////////////////////////////////////////////////////////
    const char * python_kernel::get_class_type_info( uint32_t _id )
    {
        if( _id >= PYBIND_TYPE_COUNT )
        {
            pybind::throw_exception( "invalid get class info %d"
                , _id
            );

            return nullptr;
        }

        const class_info_desc_t & desc = m_class_info_desc[_id];

        if( desc.name == nullptr )
        {
            pybind::throw_exception( "invalid get class info %d"
                , _id
            );

            return nullptr;
        }

        return desc.name;
    }
    //////////////////////////////////////////////////////////////////////////
    void python_kernel::register_type_info_extract( uint32_t _info, const type_cast_ptr & _cast )
    {
        m_type_cast[_info] = _cast;
    }
    //////////////////////////////////////////////////////////////////////////
    void python_kernel::unregister_type_info_extract( uint32_t _info )
    {
        m_type_cast[_info] = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    type_cast * python_kernel::find_type_info_extract( uint32_t _info )
    {
        const type_cast_ptr & cast = m_type_cast[_info];

        type_cast * t = cast.get();

        return t;
    }
    //////////////////////////////////////////////////////////////////////////
    class_type_scope_interface_ptr python_kernel::create_new_type_scope( uint32_t _info, const char * _name, void * _user, const new_adapter_interface_ptr & _pynew, const destroy_adapter_interface_ptr & _pydestructor, uint32_t _pod, bool _hash )
    {
        class_type_scope_interface_ptr scope = new python_class_type_scope( this, _name, _info, _user, _pynew, _pydestructor, _pod, _hash );

        if( scope == nullptr )
        {
            return nullptr;
        }

        m_class_type_scopes[_info] = scope;

        return scope;
    }
    //////////////////////////////////////////////////////////////////////////
    void python_kernel::remove_type_scope( uint32_t _info )
    {
        const class_type_scope_interface_ptr & scope = m_class_type_scopes[_info];

        if( scope == nullptr )
        {
            return;
        }

        this->remove_class_scope_type( scope );

        scope->finalize();

        m_class_type_scopes[_info] = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    bool python_kernel::has_class_type_scope( uint32_t _info )
    {
        const class_type_scope_interface_ptr & scope = m_class_type_scopes[_info];

        return scope != nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    const class_type_scope_interface_ptr & python_kernel::get_class_type_scope( uint32_t _info )
    {
        const class_type_scope_interface_ptr & scope = m_class_type_scopes[_info];

        return scope;
    }
    //////////////////////////////////////////////////////////////////////////
    void python_kernel::visit_types_scope( visitor_class_type_scope * _getter )
    {
        for( uint32_t index = 0; index != PYBIND_TYPE_COUNT; ++index )
        {
            const class_type_scope_interface_ptr & scope_ptr = m_class_type_scopes[index];

            if( scope_ptr == nullptr )
            {
                continue;
            }

            class_type_scope_interface * scope = scope_ptr.get();

            _getter->visit_scope( scope );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    PyTypeObject * python_kernel::get_object_type( PyObject * _obj )
    {
        PyTypeObject * py_type = pybind::object_type( _obj );

        return py_type;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t python_kernel::get_object_type_id( PyObject * _obj )
    {
        PyTypeObject * objtype = pybind::object_type( _obj );

        const class_type_scope_interface_ptr & scope = this->get_class_scope( objtype );

        if( scope == nullptr )
        {
            pybind::error_clear();

            if( pybind::is_none( _obj ) == true )
            {
                return 0;
            }
#   if PYBIND_PYTHON_VERSION < 300
            else if( pybind::int_check( _obj ) == true )
            {
                return this->class_info<int>();
            }
#	endif
            else if( pybind::long_check( _obj ) == true )
            {
                return this->class_info<int>();
            }
            else if( pybind::float_check( _obj ) == true )
            {
                return this->class_info<float>();
            }
#   if PYBIND_PYTHON_VERSION < 300
            else if( pybind::string_check( _obj ) == true )
            {
                return this->class_info<const char *>();
            }
#	endif
            else if( pybind::unicode_check( _obj ) == true )
            {
                return this->class_info<const wchar_t *>();
            }

            pybind::throw_exception( "obj %s not wrap pybind (scope)"
                , pybind::object_str( (PyObject *)objtype )
            );

            return 0;
        }

        uint32_t type_id = scope->get_type_id();

        return type_id;
    }
    //////////////////////////////////////////////////////////////////////////
    void python_kernel::cache_class_scope_type( const class_type_scope_interface_ptr & _scope )
    {
        PyTypeObject * py_type = _scope->get_typeobject();

        uint64_t py_hash = pybind::object_hash( (PyObject*)py_type );

        uint32_t slot = py_hash % PYBIND_TYPE_COUNT_HASH;

        for( uint32_t probe = slot; probe != PYBIND_TYPE_COUNT_HASH; probe = (probe + 17) % PYBIND_TYPE_COUNT_HASH )
        {
            class_type_scope_interface_ptr & scope = m_class_type_hashes[probe];

            if( scope != nullptr )
            {
                continue;
            }

            scope = _scope;

            break;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void python_kernel::remove_class_scope_type( const class_type_scope_interface_ptr & _scope )
    {
        PyTypeObject * py_type = _scope->get_typeobject();

        uint64_t py_hash = pybind::object_hash( (PyObject*)py_type );

        uint32_t slot = py_hash % PYBIND_TYPE_COUNT_HASH;

        for( uint32_t probe = slot; probe != PYBIND_TYPE_COUNT_HASH; probe = (probe + 17) % PYBIND_TYPE_COUNT_HASH )
        {
            class_type_scope_interface_ptr & scope = m_class_type_hashes[probe];

            if( scope == nullptr )
            {
                break;
            }

            PyTypeObject * py_typeobject = scope->get_typeobject();

            if( py_typeobject != py_type )
            {
                continue;
            }

            scope = m_class_type_dummy;

            break;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    const class_type_scope_interface_ptr & python_kernel::get_class_scope( PyTypeObject * _type )
    {
        uint64_t py_hash = pybind::object_hash( (PyObject*)_type );

        uint32_t slot = py_hash % PYBIND_TYPE_COUNT_HASH;

        for( uint32_t probe = slot; probe != PYBIND_TYPE_COUNT_HASH; probe = (probe + 17) % PYBIND_TYPE_COUNT_HASH )
        {
            const class_type_scope_interface_ptr & scope = m_class_type_hashes[probe];

            if( scope == nullptr )
            {
                break;
            }

            PyTypeObject * py_typeobject = scope->get_typeobject();

            if( py_typeobject != _type )
            {
                continue;
            }

            return scope;
        }

        return class_type_scope_interface_ptr::none();
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * python_kernel::scope_create_holder( const class_type_scope_interface_ptr & _scope, void * _ptr )
    {
        PyObject * py_obj = _scope->create_holder( _ptr );

        return py_obj;
    }
    //////////////////////////////////////////////////////////////////////////
    bool python_kernel::is_class( PyObject * _obj )
    {
        PyTypeObject * objtype = pybind::object_type( _obj );

        bool successful = this->is_type_class( objtype );

        return successful;
    }
    //////////////////////////////////////////////////////////////////////////
    bool python_kernel::is_type_class( PyTypeObject * _type )
    {
        uint64_t py_hash = pybind::object_hash( (PyObject *)_type );

        uint32_t slot = py_hash % PYBIND_TYPE_COUNT_HASH;

        for( uint32_t probe = slot; probe != PYBIND_TYPE_COUNT_HASH; probe = (probe + 17) % PYBIND_TYPE_COUNT_HASH )
        {
            const class_type_scope_interface_ptr & scope = m_class_type_hashes[probe];

            if( scope == nullptr )
            {
                return false;
            }

            PyTypeObject * py_typeobject = scope->get_typeobject();

            if( py_typeobject != _type )
            {
                continue;
            }

            return true;
        }

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    void * python_kernel::check_registred_class( PyObject * _obj, uint32_t _info )
    {
        PyTypeObject * py_type = pybind::object_type( _obj );

        const class_type_scope_interface_ptr & type_scope = m_class_type_scopes[_info];

        if( type_scope->is_instance( py_type ) == false )
        {
            return nullptr;
        }

        void * impl = this->get_class_impl( _obj );

        return impl;
    }
    //////////////////////////////////////////////////////////////////////////
    void python_kernel::error_invalid_extract( PyObject * _obj, uint32_t _tinfo )
    {
        pybind::check_error();

        const char * typeinfo_name = this->get_class_type_info( _tinfo );

        const char * obj_repr = pybind::object_repr( _obj );
        const char * obj_repr_type = pybind::object_repr_type( _obj );

        if( obj_repr != nullptr )
        {
            pybind::error_message( "invalid extract from %.256s type %.256s to %.256s"
                , obj_repr
                , obj_repr_type
                , typeinfo_name
            );
        }
        else
        {
            pybind::error_message( "invalid extract from unknown object type %s to %s"
                , _obj->ob_type->tp_name
                , typeinfo_name
            );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    bool python_kernel::instance_of_type( PyObject * _obj, uint32_t _tinfo )
    {
        if( this->is_class( _obj ) == false )
        {
            return false;
        }

        if( this->is_wrap( _obj ) == false )
        {
            pybind::error_message( "instance_of_type: unbind object" );

            return false;
        }

        const class_type_scope_interface_ptr & scope = this->get_class_scope( _obj->ob_type );
        const class_type_scope_interface_ptr & cur_scope = this->get_class_type_scope( _tinfo );

        if( scope != cur_scope )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void * python_kernel::get_class_impl( PyObject * _obj )
    {
        void * impl = pybind::detail::get_pod_impl( _obj );

        return impl;
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * python_kernel::call_method( void * _self, const class_type_scope_interface_ptr & _scope, const char * _name, PyObject * _args )
    {
        method_adapter_interface * iadapter = _scope->get_method( _name );

        PyObject * py_result = iadapter->call( this, _self, _scope, _args, nullptr );

        return py_result;
    }
}