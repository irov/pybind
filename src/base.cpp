#include "pybind/base.hpp"

#include "pybind/kernel_interface.hpp"

namespace pybind
{
    //////////////////////////////////////////////////////////////////////////
    base::base()
        : m_kernel( nullptr )
        , m_obj( nullptr )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    base::base( const base & _base )
        : m_kernel( _base.m_kernel )
        , m_obj( _base.m_obj )
    {
        if( m_obj != nullptr )
        {
            m_kernel->incref( m_obj );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    base::base( base && _base )
        : m_kernel( _base.m_kernel )
        , m_obj( _base.m_obj )
    {
        _base.m_kernel = nullptr;
        _base.m_obj = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    base::base( std::nullptr_t )
        : m_kernel( nullptr )
        , m_obj( nullptr )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    base::base( kernel_interface * _kernel, PyObject * _obj )
        : m_kernel( _kernel )
        , m_obj( _obj )
    {
        if( m_obj != nullptr )
        {
            m_kernel->incref( m_obj );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    base::base( kernel_interface * _kernel, PyObject * _obj, pybind::borrowed_t )
        : m_kernel( _kernel )
        , m_obj( _obj )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    base::base( pybind::invalid_t )
        : m_kernel( nullptr )
        , m_obj( nullptr )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    base::~base()
    {
        if( m_obj != nullptr )
        {
            m_kernel->decref( m_obj );
#ifndef NDEBUG
            m_obj = nullptr;
#endif
        }
    }
    //////////////////////////////////////////////////////////////////////////
    base & base::operator = ( std::nullptr_t )
    {
        if( m_obj != nullptr )
        {
            m_kernel->decref( m_obj );
        }

        m_kernel = nullptr;
        m_obj = nullptr;

        return *this;
    }
    //////////////////////////////////////////////////////////////////////////
    base & base::operator = ( const base & _obj )
    {
        if( m_obj != nullptr )
        {
            m_kernel->decref( m_obj );
        }

        m_kernel = _obj.m_kernel;
        m_obj = _obj.ptr();

        if( m_obj != nullptr )
        {
            m_kernel->incref( m_obj );
        }

        return *this;
    }
    //////////////////////////////////////////////////////////////////////////
    base & base::operator = ( base && _obj )
    {
        if( m_obj != nullptr )
        {
            m_kernel->decref( m_obj );
        }

        m_kernel = _obj.m_kernel;
        m_obj = _obj.ptr();

        _obj.m_kernel = nullptr;
        _obj.m_obj = nullptr;

        return *this;
    }
    //////////////////////////////////////////////////////////////////////////
    kernel_interface * base::kernel() const
    {
        return m_kernel;
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * base::ptr() const
    {
        return m_obj;
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * base::ret() const
    {
        if( m_obj != nullptr )
        {
            m_kernel->incref( m_obj );
        }

        return m_obj;
    }
    //////////////////////////////////////////////////////////////////////////
    void base::reset()
    {
        if( m_obj != nullptr )
        {
            m_kernel->decref( m_obj );
            m_obj = nullptr;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t base::get_ref() const
    {
        uint32_t ref = m_kernel->refcount( m_obj );

        return ref;
    }
    //////////////////////////////////////////////////////////////////////////
    bool base::is_invalid() const
    {
        return m_obj == nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    bool base::is_valid() const
    {
        return m_obj != nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    bool base::is_wrap() const
    {
        bool successful = m_kernel->is_wrap( m_obj );

        return successful;
    }
    //////////////////////////////////////////////////////////////////////////
    bool base::is_class() const
    {
        if( m_kernel == nullptr )
        {
            return false;
        }

        return m_kernel->is_class( m_obj );
    }
    //////////////////////////////////////////////////////////////////////////
    bool base::is_type_class() const
    {
        if( m_kernel == nullptr )
        {
            return false;
        }

        return m_kernel->is_type_class( (PyTypeObject *)m_obj );
    }
    //////////////////////////////////////////////////////////////////////////
    bool base::is_none() const
    {
        if( m_kernel == nullptr )
        {
            return false;
        }

        return m_kernel->is_none( m_obj );
    }
    //////////////////////////////////////////////////////////////////////////
    bool base::is_bool() const
    {
        if( m_kernel == nullptr )
        {
            return false;
        }

        return m_kernel->bool_check( m_obj );
    }
    //////////////////////////////////////////////////////////////////////////
    bool base::is_integer() const
    {
        if( m_kernel == nullptr )
        {
            return false;
        }

        return m_kernel->int_check( m_obj );
    }
    //////////////////////////////////////////////////////////////////////////
    bool base::is_long() const
    {
        if( m_kernel == nullptr )
        {
            return false;
        }

        return m_kernel->long_check( m_obj );
    }
    //////////////////////////////////////////////////////////////////////////
    bool base::is_float() const
    {
        if( m_kernel == nullptr )
        {
            return false;
        }

        return m_kernel->float_check( m_obj );
    }
    //////////////////////////////////////////////////////////////////////////
    bool base::is_string() const
    {
        if( m_kernel == nullptr )
        {
            return false;
        }

        return m_kernel->string_check( m_obj );
    }
    //////////////////////////////////////////////////////////////////////////
    bool base::is_unicode() const
    {
        if( m_kernel == nullptr )
        {
            return false;
        }

        return m_kernel->unicode_check( m_obj );
    }
    //////////////////////////////////////////////////////////////////////////
    bool base::is_list() const
    {
        if( m_kernel == nullptr )
        {
            return false;
        }

        return m_kernel->list_check( m_obj );
    }
    //////////////////////////////////////////////////////////////////////////
    bool base::is_callable() const
    {
        if( m_kernel == nullptr )
        {
            return false;
        }

        return m_kernel->is_callable( m_obj );
    }
    //////////////////////////////////////////////////////////////////////////
    void base::unwrap() const
    {
        if( m_kernel == nullptr )
        {
            return;
        }

        m_kernel->unwrap( m_obj );
    }
    //////////////////////////////////////////////////////////////////////////
    string_view base::str() const
    {
        if( m_kernel == nullptr )
        {
            return string_view( nullptr, nullptr );
        }

        string_view py_str = m_kernel->object_str( m_obj );

        return py_str;
    }
    //////////////////////////////////////////////////////////////////////////
    string_view base::repr() const
    {
        if( m_kernel == nullptr )
        {
            return string_view( nullptr, nullptr );
        }

        string_view py_repr = m_kernel->object_repr( m_obj );

        return py_repr;
    }
    //////////////////////////////////////////////////////////////////////////
    string_view base::repr_type() const
    {
        if( m_kernel == nullptr )
        {
            return string_view( nullptr, nullptr );
        }

        string_view py_repr = m_kernel->object_repr_type( m_obj );

        return py_repr;
    }
    //////////////////////////////////////////////////////////////////////////
}
