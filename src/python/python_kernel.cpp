#include "python_kernel.hpp"
#include "python_system.hpp"

#include "pybind/exception.hpp"
#include "pybind/debug.hpp"

#include "pybind/function.hpp"

#include "pybind/method_interface.hpp"

#include "python_class_type_scope.hpp"

#include "pybind/adapter/new_adapter.hpp"
#include "pybind/adapter/destroy_adapter.hpp"

#ifdef PYBIND_STL_SUPPORT
#   include "pybind/stl/stl_type_cast.hpp"
#endif

namespace pybind
{
    //////////////////////////////////////////////////////////////////////////
    python_kernel::python_kernel()
        : m_allocator( nullptr )
        , m_current_module( nullptr )
        , m_enumerator( 0 )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    python_kernel::~python_kernel()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool python_kernel::initialize( allocator_interface * _allocator )
    {
        m_allocator = _allocator;

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

        for( size_t index = 0; index != PYBIND_TYPE_COUNT; ++index )
        {
            class_info_desc_t & desc = m_class_info_desc[index];

            desc.name = nullptr;
        }

        m_class_type_dummy = m_allocator->newT<python_class_type_scope>( this, "__dummy__", 0, nullptr, nullptr, nullptr, 0, false );

#ifdef PYBIND_STL_SUPPORT
        pybind::initialize_stl_type_cast( this );
#endif

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void python_kernel::finalize()
    {
        PyType_ClearCache();
#if PYBIND_PYTHON_VERSION < 300
        PyImport_Cleanup();
#endif

#ifdef PYBIND_STL_SUPPORT
        pybind::finalize_stl_type_cast( this );
#endif

        for( size_t index = 0; index != PYBIND_TYPE_COUNT; ++index )
        {
            const char * name = m_class_info_desc[index].name;

            class_type_scope_interface_ptr & scope = m_class_type_scopes[index];

            if( scope != nullptr )
            {
                scope->finalize();
                scope = nullptr;
            }

            m_class_info_desc[index].name = nullptr;

            m_class_type_scopes[index] = nullptr;

            m_type_cast[index] = nullptr;

            m_allocator->free( (void *)name );
        }

        for( size_t index = 0; index != PYBIND_TYPE_COUNT_HASH; ++index )
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

        Py_Finalize();
    }
    //////////////////////////////////////////////////////////////////////////
    allocator_interface * python_kernel::get_allocator() const
    {
        return m_allocator;
    }
    //////////////////////////////////////////////////////////////////////////
    void python_kernel::collect()
    {
        PyType_ClearCache();
        PyGC_Collect();
    }
    //////////////////////////////////////////////////////////////////////////
    void python_kernel::destroy()
    {
        this->finalize();

        m_allocator->deleteT( this );
    }
    //////////////////////////////////////////////////////////////////////////
    void python_kernel::remove_from_module( const char * _name, PyObject * _module )
    {
        if( _module == nullptr )
        {
            _module = this->get_current_module();

            if( _module == nullptr )
            {
                pybind::throw_exception( "remove_from_module not setup module!"
                );

                return;
            }
        }

        pybind::module_removeobject( _module, _name );
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

#if defined(PYBIND_DEBUG)
        if( pybind::module_hasobject( _module, name ) == true )
        {
            pybind::throw_exception( "def_function_adapter module '%s' overriding object '%s'"
                , this->object_repr( _module ).c_str()
                , name
            );

            return;
        }
#endif

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

#if defined(PYBIND_DEBUG)
        if( pybind::module_hasobject( _module, name ) == true )
        {
            pybind::throw_exception( "def_functor_adapter module '%s' overriding object '%s'"
                , this->object_repr( _module ).c_str()
                , name
            );

            return;
        }
#endif

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
    PyObject * python_kernel::create_member_adapter( const member_adapter_interface_ptr & _iadapter )
    {
        PyObject * py_member = m_members.create_member_adapter( _iadapter );

        return py_member;
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * python_kernel::create_method_adapter( const method_adapter_interface_ptr & _iadapter, PyTypeObject * _type )
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
    PyTypeObject * python_kernel::get_pod_type( size_t _pod, bool _hash )
    {
        PyTypeObject * py_pod_type = m_pods.get_pod_type( _pod, _hash );

        return py_pod_type;
    }
    //////////////////////////////////////////////////////////////////////////
    bool python_kernel::is_object_bindable( PyObject * _obj )
    {
        if( _obj == nullptr )
        {
            return false;
        }

        PyTypeObject * objtype = pybind::object_type( _obj );

        const class_type_scope_interface_ptr & scope = this->get_class_scope( objtype );

        if( scope == nullptr )
        {
            return false;
        }

        if( scope->get_bindable() == nullptr )
        {
            return false;
        }

        return true;
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
    observer_bind_call * python_kernel::set_observer_bind_call( observer_bind_call * _observer )
    {
        return DEBUG_PYBIND_SET_OBSERVER_BIND_CALL( _observer );
    }
    //////////////////////////////////////////////////////////////////////////
    observer_bind_call * python_kernel::get_observer_bind_call()
    {
        return DEBUG_PYBIND_GET_OBSERVER_BIND_CALL();
    }
    //////////////////////////////////////////////////////////////////////////
    void * python_kernel::unwrap( PyObject * _obj )
    {
        if( _obj == nullptr )
        {
            return nullptr;
        }

        void * impl = this->get_class_impl( _obj );

        pybind::helper::unwrap_pod( _obj );

        return impl;
    }
    //////////////////////////////////////////////////////////////////////////
    bool python_kernel::is_wrap( PyObject * _obj )
    {
        return pybind::helper::is_pod_wrap( _obj );
    }
    //////////////////////////////////////////////////////////////////////////
    void * python_kernel::meta_cast_scope( void * _self, typeid_t _typeId, typeid_t _name, const class_type_scope_interface_ptr & _scope )
    {
        typeid_t class_name = _scope->get_type_id();

        if( class_name == _typeId )
        {
            return _self;
        }

        void * impl_cast = _scope->meta_cast( _name, _self );

        return impl_cast;
    }
    //////////////////////////////////////////////////////////////////////////
    typeid_t python_kernel::get_next_id()
    {
        typeid_t id = m_enumerator;

        m_enumerator++;

        return id;
    }
    //////////////////////////////////////////////////////////////////////////
    typeid_t python_kernel::find_class_info_desc_name( const char * _name )
    {
        for( typeid_t index = 0; index != PYBIND_TYPE_COUNT; ++index )
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
    bool python_kernel::set_class_info_desc( typeid_t _typeId, const char * _info )
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

        size_t len = strlen( _info );

        void * name_memory = m_allocator->malloc( len + 1 );

        strcpy( (char *)name_memory, _info );

        desc.name = (const char *)name_memory;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    typeid_t python_kernel::get_class_type_id( const std::type_info & _info )
    {
        const char * info_name = _info.name();

        typeid_t id = this->find_class_info_desc_name( info_name );

        if( id != 0 )
        {
            return id;
        }

        typeid_t new_id = this->get_next_id();

        if( this->set_class_info_desc( new_id, info_name ) == false )
        {
            pybind::throw_exception( "invalid create new type info" );

            return 0;
        }

        return new_id;
    }
    //////////////////////////////////////////////////////////////////////////
    const char * python_kernel::get_class_type_info( typeid_t _id )
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
    void python_kernel::register_type_info_extract( typeid_t _info, const type_cast_ptr & _cast )
    {
        m_type_cast[_info] = _cast;
    }
    //////////////////////////////////////////////////////////////////////////
    void python_kernel::unregister_type_info_extract( typeid_t _info )
    {
        m_type_cast[_info] = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    type_cast * python_kernel::find_type_info_extract( typeid_t _info )
    {
        const type_cast_ptr & cast = m_type_cast[_info];

#if defined(PYBIND_DEBUG)
        if( cast == nullptr )
        {
            return nullptr;
        }
#endif

        type_cast * t = cast.get();

        return t;
    }
    //////////////////////////////////////////////////////////////////////////
    class_type_scope_interface_ptr python_kernel::create_new_type_scope( typeid_t _info, const char * _name, void * _user, const new_adapter_interface_ptr & _pynew, const destroy_adapter_interface_ptr & _pydestructor, size_t _pod, bool _hash )
    {
        class_type_scope_interface_ptr scope = m_allocator->newT<python_class_type_scope>( this, _name, _info, _user, _pynew, _pydestructor, _pod, _hash );

#if defined(PYBIND_DEBUG)
        if( scope == nullptr )
        {
            return nullptr;
        }
#endif

        m_class_type_scopes[_info] = scope;

        return scope;
    }
    //////////////////////////////////////////////////////////////////////////
    void python_kernel::remove_type_scope( typeid_t _info )
    {
        const class_type_scope_interface_ptr & scope = m_class_type_scopes[_info];

#if defined(PYBIND_DEBUG)
        if( scope == nullptr )
        {
            return;
        }
#endif

        scope->finalize();

        m_class_type_scopes[_info] = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    bool python_kernel::has_class_type_scope( typeid_t _info )
    {
        const class_type_scope_interface_ptr & scope = m_class_type_scopes[_info];

        return scope != nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    const class_type_scope_interface_ptr & python_kernel::get_class_type_scope( typeid_t _info )
    {
        const class_type_scope_interface_ptr & scope = m_class_type_scopes[_info];

        return scope;
    }
    //////////////////////////////////////////////////////////////////////////
    void python_kernel::visit_types_scope( visitor_class_type_scope * _getter )
    {
        for( size_t index = 0; index != PYBIND_TYPE_COUNT; ++index )
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
    typeid_t python_kernel::get_object_type_id( PyObject * _obj )
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
            else if( pybind::tuple_check( _obj ) == true )
            {
                return 0;
            }
            else if( pybind::list_check( _obj ) == true )
            {
                return 0;
            }
            else if( pybind::dict_check( _obj ) == true )
            {
                return 0;
            }
            else if( pybind::set_check( _obj ) == true )
            {
                return 0;
            }
#if PYBIND_PYTHON_VERSION < 300
            else if( pybind::int_check( _obj ) == true )
            {
                return this->class_info<int>();
            }
#endif
            else if( pybind::long_check( _obj ) == true )
            {
                return this->class_info<int>();
            }
            else if( pybind::float_check( _obj ) == true )
            {
                return this->class_info<float>();
            }
#if PYBIND_PYTHON_VERSION < 300
            else if( pybind::string_check( _obj ) == true )
            {
                return this->class_info<const char *>();
            }
#endif
            else if( pybind::unicode_check( _obj ) == true )
            {
                return this->class_info<const wchar_t *>();
            }

            pybind::throw_exception( "obj '%s' not wrap pybind (scope)"
                , this->object_str( (PyObject *)objtype ).c_str()
            );

            return 0;
        }

        typeid_t type_id = scope->get_type_id();

        return type_id;
    }
    //////////////////////////////////////////////////////////////////////////
    const char * python_kernel::object_type_name( PyObject * _type )
    {
        const char * type_name = pybind::object_type_name( _type );

        return type_name;
    }
    //////////////////////////////////////////////////////////////////////////
    const char * python_kernel::type_name( PyTypeObject * _type )
    {
        const char * type_name = pybind::type_name( _type );

        return type_name;
    }
    //////////////////////////////////////////////////////////////////////////
    void python_kernel::cache_class_scope_type( const class_type_scope_interface_ptr & _scope )
    {
        PyTypeObject * py_type = _scope->get_typeobject();

        uint64_t py_hash = pybind::type_hash( py_type );

        uint32_t slot = py_hash % PYBIND_TYPE_COUNT_HASH;

        for( uint32_t probe = slot; probe != PYBIND_TYPE_COUNT_HASH; probe = (probe + 17) % PYBIND_TYPE_COUNT_HASH )
        {
            class_type_scope_interface_ptr & scope = m_class_type_hashes[probe];

            if( scope != nullptr && scope != m_class_type_dummy )
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

        uint64_t py_hash = pybind::type_hash( py_type );

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
        uint64_t py_hash = pybind::type_hash( _type );

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
    PyObject * python_kernel::scope_create_weak( const class_type_scope_interface_ptr & _scope, void * _ptr )
    {
        PyObject * py_obj = _scope->create_weak( _ptr );

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
        uint64_t py_hash = pybind::type_hash( _type );

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
    void * python_kernel::check_registred_class( PyObject * _obj, typeid_t _info )
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
    void python_kernel::error_invalid_extract( PyObject * _obj, typeid_t _tinfo )
    {
        pybind::check_error();

        const char * typeinfo_name = this->get_class_type_info( _tinfo );

        PyObject * obj_repr = pybind::object_repr( _obj );

        if( obj_repr == nullptr )
        {
            pybind::error_traceback( "invalid extract from unknown object type '%s' to '%s'"
                , pybind::object_type_name( _obj )
                , typeinfo_name
            );

            return;
        }

        PyObject * obj_repr_type = pybind::object_repr_type( _obj );

        pybind::error_traceback( "invalid extract from %.256s type %.256s to %.256s"
            , pybind::string_to_char( obj_repr )
            , pybind::string_to_char( obj_repr_type )
            , typeinfo_name
        );

        pybind::decref( obj_repr );
        pybind::decref( obj_repr_type );
    }
    //////////////////////////////////////////////////////////////////////////
    bool python_kernel::instance_of_type( PyObject * _obj, typeid_t _tinfo )
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
        void * impl = pybind::helper::get_pod_impl( _obj );

        return impl;
    }
    //////////////////////////////////////////////////////////////////////////
    bool python_kernel::is_class_weak( PyObject * _obj )
    {
        bool result = pybind::helper::is_pod_weak( _obj );

        return result;
    }
    //////////////////////////////////////////////////////////////////////////
    bool python_kernel::type_initialize( PyObject * _obj )
    {
        PyTypeObject * type = (PyTypeObject *)_obj;

        const class_type_scope_interface_ptr & scope = this->get_class_scope( type );

        if( scope == nullptr )
        {
            return false;
        }

        scope->type_initialize( type );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t python_kernel::get_python_version()
    {
        return pybind::get_python_version();
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * python_kernel::get_builtins()
    {
        return pybind::get_builtins();
    }
    //////////////////////////////////////////////////////////////////////////
    void python_kernel::set_syspath( PyObject * _path )
    {
        pybind::set_syspath( _path );
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * python_kernel::code_compile_file( const void * _buf, const char * _module )
    {
        PyObject * py_code = pybind::code_compile_file( _buf, _module );

        pybind::check_error();

        return py_code;
    }
    //////////////////////////////////////////////////////////////////////////
    bool python_kernel::code_check( PyObject * _code )
    {
        return pybind::code_check( _code );
    }
    //////////////////////////////////////////////////////////////////////////
    long python_kernel::marshal_magic_number()
    {
        return pybind::marshal_magic_number();
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * python_kernel::marshal_get_object( const void * _buf, size_t _len )
    {
        PyObject * py_code = pybind::marshal_get_object( _buf, _len );

        pybind::check_error();

        return py_code;
    }
    //////////////////////////////////////////////////////////////////////////
    void python_kernel::set_module_finder( PyObject * _finder )
    {
        pybind::set_module_finder( _finder );
    }
    //////////////////////////////////////////////////////////////////////////
    void python_kernel::remove_module_finder()
    {
        pybind::remove_module_finder();
    }
    //////////////////////////////////////////////////////////////////////////
    void python_kernel::set_sys_excepthook( pybind_excepthook_handler_f _excepthook, void * _ud )
    {
        function_adapter_interface_ptr adapter = pybind::make_function_proxy( this, "excepthook", _excepthook, _ud );

        PyObject * py_excepthook = this->create_function_adapter( adapter, false );

        PySys_SetObject( (char *)"excepthook", py_excepthook );

        Py_DECREF( py_excepthook );
    }
    //////////////////////////////////////////////////////////////////////////
    void python_kernel::call_native( PyObject * _obj, PyObject * _args )
    {
        pybind::call_native( _obj, _args );
    }
    //////////////////////////////////////////////////////////////////////////
    void python_kernel::call_method( PyObject * _obj, const char * _method, const char * _format, ... )
    {
        va_list valist;
        va_start( valist, _format );
        pybind::call_method_va( _obj, _method, _format, valist );
        va_end( valist );
    }
    //////////////////////////////////////////////////////////////////////////
    void python_kernel::call_method_native( PyObject * _obj, const char * _method, PyObject * _args )
    {
        pybind::call_method_native( _obj, _method, _args );
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * python_kernel::ask_native( PyObject * _obj, PyObject * _args )
    {
        PyObject * py_result = pybind::ask_native( _obj, _args );

        return py_result;
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * python_kernel::ask_method( PyObject * _obj, const char * _method, const char * _format, ... )
    {
        va_list valist;
        va_start( valist, _format );
        PyObject * py_result = pybind::ask_method_va( _obj, _method, _format, valist );
        va_end( valist );

        return py_result;
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * python_kernel::ask_method_native( PyObject * _obj, const char * _method, PyObject * _args )
    {
        PyObject * py_result = pybind::ask_method_native( _obj, _method, _args );

        return py_result;
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * python_kernel::ask_adapter( void * _self, const class_type_scope_interface_ptr & _scope, const char * _name, PyObject * _args )
    {
        const method_adapter_interface_ptr & iadapter = _scope->find_method( _name );

        PyObject * py_result = iadapter->call( this, _self, _scope, _args, nullptr );

        return py_result;
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * python_kernel::compile_string( const char * _string, const char * _file )
    {
        return pybind::compile_string( _string, _file );
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * python_kernel::eval_string( const char * _string, PyObject * _globals, PyObject * _locals )
    {
        return pybind::eval_string( _string, _globals, _locals );
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * python_kernel::exec_file( const char * _code, PyObject * _globals, PyObject * _locals )
    {
        return pybind::exec_file( _code, _globals, _locals );
    }
    //////////////////////////////////////////////////////////////////////////
    void python_kernel::setStdOutHandle( PyObject * _obj )
    {
        pybind::setStdOutHandle( _obj );
    }
    //////////////////////////////////////////////////////////////////////////
    void python_kernel::setStdErrorHandle( PyObject * _obj )
    {
        pybind::setStdErrorHandle( _obj );
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * python_kernel::getStdOutHandle()
    {
        return pybind::getStdOutHandle();
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * python_kernel::getStdErrorHandle()
    {
        return pybind::getStdErrorHandle();
    }
    //////////////////////////////////////////////////////////////////////////
    void python_kernel::incref( PyObject * _obj )
    {
        pybind::incref( _obj );
    }
    //////////////////////////////////////////////////////////////////////////
    void python_kernel::decref( PyObject * _obj )
    {
        pybind::decref( _obj );
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t python_kernel::refcount( PyObject * _obj )
    {
        return pybind::refcount( _obj );
    }
    //////////////////////////////////////////////////////////////////////////
    bool python_kernel::is_none( PyObject * _obj ) const
    {
        return pybind::is_none( _obj );
    }
    //////////////////////////////////////////////////////////////////////////
    bool python_kernel::bool_check( PyObject * _obj ) const
    {
        return pybind::bool_check( _obj );
    }
    //////////////////////////////////////////////////////////////////////////
    bool python_kernel::is_callable( PyObject * _obj ) const
    {
        return pybind::is_callable( _obj );
    }
    //////////////////////////////////////////////////////////////////////////
    bool python_kernel::is_instanceof( PyObject * _obj, PyTypeObject * _type ) const
    {
        return pybind::is_instanceof( _obj, _type );
    }
    //////////////////////////////////////////////////////////////////////////
    bool python_kernel::test_equal( PyObject * _lhs, PyObject * _rhs )
    {
        return pybind::test_equal( _lhs, _rhs );
    }
    //////////////////////////////////////////////////////////////////////////
    bool python_kernel::has_attr( PyObject * _obj, PyObject * _attr )
    {
        return pybind::has_attr( _obj, _attr );
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * python_kernel::get_attr( PyObject * _obj, PyObject * _attr )
    {
        return pybind::get_attr( _obj, _attr );
    }
    //////////////////////////////////////////////////////////////////////////
    bool python_kernel::set_attr( PyObject * _obj, PyObject * _attr, PyObject * _value )
    {
        return pybind::set_attr( _obj, _attr, _value );
    }
    //////////////////////////////////////////////////////////////////////////
    bool python_kernel::set_attrstring( PyObject * _obj, const char * _attr, PyObject * _value )
    {
        return pybind::set_attrstring( _obj, _attr, _value );
    }
    //////////////////////////////////////////////////////////////////////////
    bool python_kernel::has_attrstring( PyObject * _obj, const char * _attr )
    {
        return pybind::has_attrstring( _obj, _attr );
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * python_kernel::get_attrstring( PyObject * _obj, const char * _attr )
    {
        return pybind::get_attrstring( _obj, _attr );
    }
    //////////////////////////////////////////////////////////////////////////
    string_view python_kernel::object_str( PyObject * _obj )
    {
        PyObject * py_str = pybind::object_str( _obj );

        return string_view( this, py_str, pybind::borrowed );
    }
    //////////////////////////////////////////////////////////////////////////
    string_view python_kernel::object_repr( PyObject * _obj )
    {
        PyObject * py_repr = pybind::object_repr( _obj );

        if( py_repr == nullptr )
        {
            return string_view( this, nullptr, pybind::borrowed );
        }

        if( pybind::string_check( py_repr ) == false )
        {
            pybind::xdecref( py_repr );

            return string_view( this, nullptr, pybind::borrowed );
        }

        return string_view( this, py_repr, pybind::borrowed );
    }
    //////////////////////////////////////////////////////////////////////////
    string_view python_kernel::object_repr_type( PyObject * _obj )
    {
        PyObject * py_repr = pybind::object_repr_type( _obj );

        if( py_repr == nullptr )
        {
            return string_view( this, nullptr, pybind::borrowed );
        }

        if( pybind::string_check( py_repr ) == false )
        {
            pybind::xdecref( py_repr );

            return string_view( this, nullptr, pybind::borrowed );
        }

        return string_view( this, py_repr, pybind::borrowed );
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * python_kernel::get_none()
    {
        return pybind::get_none();
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * python_kernel::ret_none()
    {
        return pybind::ret_none();
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * python_kernel::ret_not_implemented()
    {
        return pybind::ret_not_implemented();
    }
    //////////////////////////////////////////////////////////////////////////
    bool python_kernel::is_true( PyObject * _obj ) const
    {
        return pybind::is_true( _obj );
    }
    //////////////////////////////////////////////////////////////////////////
    bool python_kernel::is_false( PyObject * _obj ) const
    {
        return pybind::is_false( _obj );
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * python_kernel::ret_bool( bool _value )
    {
        return pybind::ret_bool( _value );
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * python_kernel::ret_true()
    {
        return pybind::ret_true();
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * python_kernel::ret_false()
    {
        return pybind::ret_false();
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * python_kernel::get_true()
    {
        return pybind::get_true();
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * python_kernel::get_false()
    {
        return pybind::get_false();
    }
    //////////////////////////////////////////////////////////////////////////
    bool python_kernel::extract_bool( PyObject * _obj, bool & _value )
    {
        return pybind::extract_bool( _obj, _value );
    }
    //////////////////////////////////////////////////////////////////////////
    bool python_kernel::extract_int8( PyObject * _obj, int8_t & _value )
    {
        return pybind::extract_int8( _obj, _value );
    }
    //////////////////////////////////////////////////////////////////////////
    bool python_kernel::extract_int16( PyObject * _obj, int16_t & _value )
    {
        return pybind::extract_int16( _obj, _value );
    }
    //////////////////////////////////////////////////////////////////////////
    bool python_kernel::extract_int32( PyObject * _obj, int32_t & _value )
    {
        return pybind::extract_int32( _obj, _value );
    }
    //////////////////////////////////////////////////////////////////////////
    bool python_kernel::extract_int64( PyObject * _obj, int64_t & _value )
    {
        return pybind::extract_int64( _obj, _value );
    }
    //////////////////////////////////////////////////////////////////////////
    bool python_kernel::extract_uint8( PyObject * _obj, uint8_t & _value )
    {
        return pybind::extract_uint8( _obj, _value );
    }
    //////////////////////////////////////////////////////////////////////////
    bool python_kernel::extract_uint16( PyObject * _obj, uint16_t & _value )
    {
        return pybind::extract_uint16( _obj, _value );
    }
    //////////////////////////////////////////////////////////////////////////
    bool python_kernel::extract_uint32( PyObject * _obj, uint32_t & _value )
    {
        return pybind::extract_uint32( _obj, _value );
    }
    //////////////////////////////////////////////////////////////////////////
    bool python_kernel::extract_uint64( PyObject * _obj, uint64_t & _value )
    {
        return pybind::extract_uint64( _obj, _value );
    }
    //////////////////////////////////////////////////////////////////////////
    bool python_kernel::extract_float( PyObject * _obj, float & _value )
    {
        return pybind::extract_float( _obj, _value );
    }
    //////////////////////////////////////////////////////////////////////////
    bool python_kernel::extract_double( PyObject * _obj, double & _value )
    {
        return pybind::extract_double( _obj, _value );
    }
    //////////////////////////////////////////////////////////////////////////
    bool python_kernel::extract_wchar( PyObject * _obj, wchar_t & _value )
    {
        return pybind::extract_wchar( _obj, _value );
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * python_kernel::ptr_bool( bool _value )
    {
        return pybind::ptr_bool( _value );
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * python_kernel::ptr_int8( int8_t _value )
    {
        return pybind::ptr_int8( _value );
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * python_kernel::ptr_int16( int16_t _value )
    {
        return pybind::ptr_int16( _value );
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * python_kernel::ptr_int32( int32_t _value )
    {
        return pybind::ptr_int32( _value );
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * python_kernel::ptr_int64( int64_t _value )
    {
        return pybind::ptr_int64( _value );
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * python_kernel::ptr_uint8( uint8_t _value )
    {
        return pybind::ptr_uint8( _value );
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * python_kernel::ptr_uint16( uint16_t _value )
    {
        return pybind::ptr_uint16( _value );
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * python_kernel::ptr_uint32( uint32_t _value )
    {
        return pybind::ptr_uint32( _value );
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * python_kernel::ptr_uint64( uint64_t _value )
    {
        return pybind::ptr_uint64( _value );
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * python_kernel::ptr_float( float _value )
    {
        return pybind::ptr_float( _value );
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * python_kernel::ptr_double( double _value )
    {
        return pybind::ptr_double( _value );
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * python_kernel::ptr_long( long _value )
    {
        return pybind::ptr_long( _value );
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * python_kernel::ptr_ulong( unsigned long _value )
    {
        return pybind::ptr_ulong( _value );
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * python_kernel::ptr_string( const char * _str )
    {
        return pybind::string_from_char( _str );
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * python_kernel::ptr_unicode( const wchar_t * _value )
    {
        return pybind::unicode_from_wchar( _value );
    }
    //////////////////////////////////////////////////////////////////////////
    bool python_kernel::int_check( PyObject * _obj ) const
    {
        return pybind::int_check( _obj );
    }
    //////////////////////////////////////////////////////////////////////////
    bool python_kernel::long_check( PyObject * _obj ) const
    {
        return pybind::long_check( _obj );
    }
    //////////////////////////////////////////////////////////////////////////
    bool python_kernel::float_check( PyObject * _obj ) const
    {
        return pybind::float_check( _obj );
    }
    //////////////////////////////////////////////////////////////////////////
    bool python_kernel::string_check( PyObject * _obj ) const
    {
        return pybind::string_check( _obj );
    }
    //////////////////////////////////////////////////////////////////////////
    size_t python_kernel::string_size( PyObject * _obj ) const
    {
        return pybind::string_size( _obj );
    }
    //////////////////////////////////////////////////////////////////////////
    int64_t python_kernel::string_hash( PyObject * _obj ) const
    {
        return pybind::string_hash( _obj );
    }
    //////////////////////////////////////////////////////////////////////////
    const char * python_kernel::string_to_char( PyObject * _obj )
    {
        return pybind::string_to_char( _obj );
    }
    //////////////////////////////////////////////////////////////////////////
    const char * python_kernel::string_to_char_and_size( PyObject * _obj, size_t * _size )
    {
        return pybind::string_to_char_and_size( _obj, _size );
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * python_kernel::string_from_char( const char * _str )
    {
        return pybind::string_from_char( _str );
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * python_kernel::string_from_char_size( const char * _str, size_t _size )
    {
        return pybind::string_from_char_size( _str, _size );
    }
    //////////////////////////////////////////////////////////////////////////
    bool python_kernel::bytearray_check( PyObject * _bytearray ) const
    {
        return pybind::bytearray_check( _bytearray );
    }
    //////////////////////////////////////////////////////////////////////////
    size_t python_kernel::bytearray_size( PyObject * _bytearray ) const
    {
        return pybind::bytearray_size( _bytearray );
    }
    //////////////////////////////////////////////////////////////////////////
    const uint8_t * python_kernel::bytearray_to_data( PyObject * _bytearray ) const
    {
        return pybind::bytearray_to_data( _bytearray );
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * python_kernel::bytearray_from_data( const uint8_t * _data, size_t _size )
    {
        return pybind::bytearray_from_data( _data, _size );
    }
    //////////////////////////////////////////////////////////////////////////
    bool python_kernel::unicode_check( PyObject * _obj ) const
    {
        return pybind::unicode_check( _obj );
    }
    //////////////////////////////////////////////////////////////////////////
    const wchar_t * python_kernel::unicode_to_wchar( PyObject * _obj )
    {
        return pybind::unicode_to_wchar( _obj );
    }
    //////////////////////////////////////////////////////////////////////////
    const wchar_t * python_kernel::unicode_to_wchar_and_size( PyObject * _obj, size_t * _size )
    {
        return pybind::unicode_to_wchar_and_size( _obj, _size );
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * python_kernel::unicode_from_wchar( const wchar_t * _value )
    {
        return pybind::unicode_from_wchar( _value );
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * python_kernel::unicode_from_wchar_size( const wchar_t * _value, size_t _size )
    {
        return pybind::unicode_from_wchar_size( _value, _size );
    }
    //////////////////////////////////////////////////////////////////////////
    const char * python_kernel::unicode_to_utf8( PyObject * _obj )
    {
        return pybind::unicode_to_utf8( _obj );
    }
    //////////////////////////////////////////////////////////////////////////
    const char * python_kernel::unicode_to_utf8_and_size( PyObject * _obj, size_t * _size )
    {
        return pybind::unicode_to_utf8_and_size( _obj, _size );
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * python_kernel::unicode_from_utf8( const char * _utf8 )
    {
        return pybind::unicode_from_utf8( _utf8 );
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * python_kernel::unicode_from_utf8_size( const char * _utf8, size_t _size )
    {
        return pybind::unicode_from_utf8_size( _utf8, _size );
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * python_kernel::list_new( size_t _size )
    {
        return pybind::list_new( _size );
    }
    //////////////////////////////////////////////////////////////////////////
    bool python_kernel::list_check( PyObject * _list ) const
    {
        return pybind::list_check( _list );
    }
    //////////////////////////////////////////////////////////////////////////
    size_t python_kernel::list_size( PyObject * _list ) const
    {
        return pybind::list_size( _list );
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * python_kernel::list_getitem( PyObject * _list, size_t _it )
    {
        return pybind::list_getitem( _list, _it );
    }
    //////////////////////////////////////////////////////////////////////////
    bool python_kernel::list_insert( PyObject * _list, size_t _it, PyObject * _item )
    {
        return pybind::list_insert( _list, _it, _item );
    }
    //////////////////////////////////////////////////////////////////////////
    bool python_kernel::list_remove( PyObject * _list, size_t _it )
    {
        return pybind::list_remove( _list, _it );
    }
    //////////////////////////////////////////////////////////////////////////
    bool python_kernel::list_setitem( PyObject * _list, size_t _it, PyObject * _item )
    {
        return pybind::list_setitem( _list, _it, _item );
    }
    //////////////////////////////////////////////////////////////////////////
    bool python_kernel::list_appenditem( PyObject * _list, PyObject * _item )
    {
        return pybind::list_appenditem( _list, _item );
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * python_kernel::dict_new()
    {
        return pybind::dict_new();
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * python_kernel::dict_new_presized( size_t _count )
    {
        return pybind::dict_new_presized( _count );
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * python_kernel::dict_from( PyObject * _obj )
    {
        return pybind::dict_from( _obj );
    }
    //////////////////////////////////////////////////////////////////////////
    bool python_kernel::dict_check( PyObject * _dict ) const
    {
        return pybind::dict_check( _dict );
    }
    //////////////////////////////////////////////////////////////////////////
    size_t python_kernel::dict_size( PyObject * _obj ) const
    {
        return pybind::dict_size( _obj );
    }
    //////////////////////////////////////////////////////////////////////////
    bool python_kernel::dict_set( PyObject * _obj, PyObject * _name, PyObject * _value )
    {
        return pybind::dict_set( _obj, _name, _value );
    }
    //////////////////////////////////////////////////////////////////////////
    bool python_kernel::dict_setstring( PyObject * _obj, const char * _name, PyObject * _value )
    {
        return pybind::dict_setstring( _obj, _name, _value );
    }
    //////////////////////////////////////////////////////////////////////////
    bool python_kernel::dict_remove( PyObject * _obj, PyObject * _name )
    {
        return pybind::dict_remove( _obj, _name );
    }
    //////////////////////////////////////////////////////////////////////////
    bool python_kernel::dict_removestring( PyObject * _obj, const char * _name )
    {
        return pybind::dict_removestring( _obj, _name );
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * python_kernel::dict_getstring( PyObject * _obj, const char * _key )
    {
        return pybind::dict_getstring( _obj, _key );
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * python_kernel::dict_get( PyObject * _obj, PyObject * _name )
    {
        return pybind::dict_get( _obj, _name );
    }
    //////////////////////////////////////////////////////////////////////////
    bool python_kernel::dict_exist( PyObject * _obj, PyObject * _name )
    {
        return pybind::dict_exist( _obj, _name );
    }
    //////////////////////////////////////////////////////////////////////////
    bool python_kernel::dict_existstring( PyObject * _obj, const char * _name )
    {
        return pybind::dict_existstring( _obj, _name );
    }
    //////////////////////////////////////////////////////////////////////////
    bool python_kernel::dict_next( PyObject * _obj, size_t * const _pos, PyObject ** _key, PyObject ** _value )
    {
        return pybind::dict_next( _obj, _pos, _key, _value );
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * python_kernel::set_new()
    {
        return pybind::set_new();
    }
    //////////////////////////////////////////////////////////////////////////
    bool python_kernel::set_check( PyObject * _dict ) const
    {
        return pybind::set_check( _dict );
    }
    //////////////////////////////////////////////////////////////////////////
    size_t python_kernel::set_size( PyObject * _obj ) const
    {
        return pybind::set_size( _obj );
    }
    //////////////////////////////////////////////////////////////////////////
    bool python_kernel::set_set( PyObject * _obj, PyObject * _value )
    {
        return pybind::set_set( _obj, _value );
    }
    //////////////////////////////////////////////////////////////////////////
    bool python_kernel::set_remove( PyObject * _obj, PyObject * _value )
    {
        return pybind::set_remove( _obj, _value );
    }
    //////////////////////////////////////////////////////////////////////////
    bool python_kernel::set_exist( PyObject * _obj, PyObject * _value )
    {
        return pybind::set_exist( _obj, _value );
    }
    //////////////////////////////////////////////////////////////////////////
    bool python_kernel::iterator_get( PyObject * _collections, PyObject ** _iterator )
    {
        return pybind::iterator_get( _collections, _iterator );
    }
    //////////////////////////////////////////////////////////////////////////
    bool python_kernel::iterator_next( PyObject * _iterator, PyObject ** _value )
    {
        return pybind::iterator_next( _iterator, _value );
    }
    //////////////////////////////////////////////////////////////////////////
    void python_kernel::iterator_end( PyObject * _iterator )
    {
        pybind::iterator_end( _iterator );
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * python_kernel::tuple_new( size_t _it )
    {
        return pybind::tuple_new( _it );
    }
    //////////////////////////////////////////////////////////////////////////
    bool python_kernel::tuple_check( PyObject * _obj ) const
    {
        return pybind::tuple_check( _obj );
    }
    //////////////////////////////////////////////////////////////////////////
    size_t python_kernel::tuple_size( PyObject * _obj ) const
    {
        return pybind::tuple_size( _obj );
    }
    //////////////////////////////////////////////////////////////////////////
    bool python_kernel::tuple_setitem( PyObject * _obj, size_t _it, PyObject * _value )
    {
        return pybind::tuple_setitem( _obj, _it, _value );
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * python_kernel::exception_new( const char * _msg )
    {
        return pybind::exception_new( _msg );
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * python_kernel::tuple_getitem( PyObject * _obj, size_t _it )
    {
        return pybind::tuple_getitem( _obj, _it );
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * python_kernel::tuple_slice( PyObject * _obj, size_t _low, size_t _high )
    {
        return pybind::tuple_slice( _obj, _low, _high );
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * python_kernel::tuple_slice_tail( PyObject * _obj, size_t _size )
    {
        return pybind::tuple_slice_tail( _obj, _size );
    }
    //////////////////////////////////////////////////////////////////////////
    bool python_kernel::get_statetrace_top( char * const _filename, size_t _maxlenfilename, char * const _function, size_t _maxlenfunction, uint32_t * _lineno )
    {
        return pybind::get_statetrace_top( _filename, _maxlenfilename, _function, _maxlenfunction, _lineno );
    }
    //////////////////////////////////////////////////////////////////////////
    bool python_kernel::get_statetrace( char * _buffer, size_t _maxlen, bool _append )
    {
        bool successful = pybind::get_statetrace( _buffer, _maxlen, _append );

        return successful;
    }
    //////////////////////////////////////////////////////////////////////////
    void python_kernel::error_traceback( const char * _format, ... )
    {
        va_list valist;
        va_start( valist, _format );
        this->error_traceback_va( _format, valist );
        va_end( valist );
    }
    //////////////////////////////////////////////////////////////////////////
    void python_kernel::error_traceback_va( const char * _format, va_list _va )
    {
        pybind::error_traceback_va( _format, _va );
    }
    //////////////////////////////////////////////////////////////////////////
    void python_kernel::error_message( const char * _format, ... )
    {
        va_list valist;
        va_start( valist, _format );
        this->error_message_va( _format, valist );
        va_end( valist );
    }
    //////////////////////////////////////////////////////////////////////////
    void python_kernel::error_message_va( const char * _format, va_list _va )
    {
        pybind::error_message_va( _format, _va );
    }
    //////////////////////////////////////////////////////////////////////////
    void python_kernel::throw_message( const char * _format, ... )
    {
        va_list valist;
        va_start( valist, _format );
        this->throw_message_va( _format, valist );
        va_end( valist );
    }
    //////////////////////////////////////////////////////////////////////////
    void python_kernel::throw_message_va( const char * _format, va_list _va )
    {
        pybind::throw_message_va( _format, _va );
    }
    //////////////////////////////////////////////////////////////////////////
    void python_kernel::warning_traceback( const char * _format, ... )
    {
        va_list valist;
        va_start( valist, _format );
        this->warning_traceback_va( _format, valist );
        va_end( valist );
    }
    //////////////////////////////////////////////////////////////////////////
    void python_kernel::warning_traceback_va( const char * _format, va_list _va )
    {
        pybind::warning_traceback_va( _format, _va );
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * python_kernel::build_value( const char * _format, ... )
    {
        va_list valist;
        va_start( valist, _format );

        PyObject * value = this->build_value_va( _format, valist );

        va_end( valist );

        return value;
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * python_kernel::build_value_va( const char * _format, va_list _va )
    {
        return pybind::build_value_va( _format, _va );
    }
    //////////////////////////////////////////////////////////////////////////
    bool python_kernel::traceback_check( PyObject * _traceback )
    {
        return pybind::traceback_check( _traceback );
    }
    //////////////////////////////////////////////////////////////////////////
    string_view python_kernel::traceback_filename( PyObject * _traceback )
    {
        PyObject * filename = pybind::traceback_filename( _traceback );

        return string_view( this, filename );
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t python_kernel::traceback_lineno( PyObject * _traceback )
    {
        uint32_t lineno = pybind::traceback_lineno( _traceback );

        return lineno;
    }
    //////////////////////////////////////////////////////////////////////////
    string_view python_kernel::traceback_function( PyObject * _traceback )
    {
        PyObject * function = pybind::traceback_function( _traceback );

        return string_view( this, function );
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * python_kernel::traceback_next( PyObject * _traceback )
    {
        PyObject * next = pybind::traceback_next( _traceback );

        return next;
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * python_kernel::module_import( const char * _name, bool & _exsist )
    {
        return pybind::module_import( _name, _exsist );
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * python_kernel::module_init( const char * _name )
    {
        return pybind::module_init( _name );
    }
    //////////////////////////////////////////////////////////////////////////
    void python_kernel::module_fini( PyObject * _module )
    {
        return pybind::module_fini( _module );
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * python_kernel::module_dict( PyObject * _module )
    {
        return pybind::module_dict( _module );
    }
    //////////////////////////////////////////////////////////////////////////
    void python_kernel::module_addobject( PyObject * _module, const char * _name, PyObject * _obj )
    {
        return pybind::module_addobject( _module, _name, _obj );
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * python_kernel::module_execcode( const char * _name, PyObject * _code )
    {
        return pybind::module_execcode( _name, _code );
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * python_kernel::module_reload( PyObject * _module )
    {
        return pybind::module_reload( _module );
    }
    //////////////////////////////////////////////////////////////////////////
}