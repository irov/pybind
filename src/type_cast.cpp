#include "pybind/type_cast.hpp"

#include "pybind/kernel.hpp"
#include "pybind/class_type_scope_interface.hpp"

#include "pybind/system.hpp"

#include "config/config.hpp"

namespace pybind
{
    //////////////////////////////////////////////////////////////////////////
    void * type_cast::operator new (size_t _size)
    {
        return PYBIND_MALLOC( _size );
    }
    //////////////////////////////////////////////////////////////////////////
    void type_cast::operator delete (void * _ptr, size_t _size)
    {
        (void)_size;

        PYBIND_FREE( _ptr, _size );
    }
    //////////////////////////////////////////////////////////////////////////
    void type_cast::destroy()
    {
        delete this;
    }
    //////////////////////////////////////////////////////////////////////////
    bool type_cast::type_info_cast( kernel_interface * _kernel, PyObject * _obj, uint32_t _tinfo, uint32_t _tptrinfo, void ** _impl )
    {
        if( _kernel->is_class( _obj ) == false )
        {
            return false;
        }

        if( _kernel->is_wrap( _obj ) == false )
        {
            pybind::error_message( "type_info_cast: unwrap object" );

            return false;
        }

        void * impl = _kernel->get_class_impl( _obj );

        if( impl == nullptr )
        {
            pybind::error_message( "type_info_cast: unbind object" );

            return false;
        }

        PyTypeObject * py_type = _kernel->get_object_type( _obj );
        const class_type_scope_interface_ptr & scope = _kernel->get_class_scope( py_type );
        const class_type_scope_interface_ptr & cur_scope = _kernel->get_class_type_scope( _tinfo );

        if( cur_scope != scope )
        {
            impl = scope->meta_cast( _tptrinfo, impl );
        }

        if( impl == nullptr )
        {
            return false;
        }

        *_impl = impl;

        return true;
    }
}
