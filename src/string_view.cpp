#include "pybind/string_view.hpp"
#include "pybind/kernel_interface.hpp"

namespace pybind
{
    //////////////////////////////////////////////////////////////////////////
    string_view::string_view( kernel_interface * _kernel, PyObject * _obj )
        : m_kernel( _kernel )
        , m_obj( _obj )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    string_view::string_view( const string_view & _base )
        : m_kernel( _base.m_kernel )
        , m_obj( _base.m_obj )
    {
        if( m_kernel != nullptr )
        {
            m_kernel->incref( m_obj );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    string_view::string_view( string_view && _base )
        : m_kernel( _base.m_kernel )
        , m_obj( _base.m_obj )
    {
        _base.m_kernel = nullptr;
        _base.m_obj = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    string_view::~string_view()
    {
        if( m_kernel != nullptr )
        {
            m_kernel->decref( m_obj );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    bool string_view::is_invalid() const
    {
        return m_obj == nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    bool string_view::is_valid() const
    {
        return m_obj != nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    kernel_interface * string_view::kernel() const
    {
        return m_kernel;
    }
    //////////////////////////////////////////////////////////////////////////    
    const char * string_view::c_str() const
    {
        if( m_obj == nullptr )
        {
            return "";
        }

        const char * str = m_kernel->string_to_char( m_obj );

        return str;
    }
}