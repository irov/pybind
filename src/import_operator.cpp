#include "pybind/import_operator.hpp"

#include "pybind/object.hpp"

namespace pybind
{
    //////////////////////////////////////////////////////////////////////////
    namespace detail
    {
        //////////////////////////////////////////////////////////////////////////
        import_operator_t::import_operator_t()
            : m_kernel( nullptr )
            , m_obj( nullptr )
        {
        }
        //////////////////////////////////////////////////////////////////////////
        import_operator_t::import_operator_t( const import_operator_t & _op )
            : m_kernel( _op.m_kernel )
            , m_obj( _op.m_obj )
        {
            if( m_obj != nullptr )
            {
                m_kernel->incref( m_obj );
            }
        }
        //////////////////////////////////////////////////////////////////////////
        import_operator_t::import_operator_t( import_operator_t && _op )
            : m_kernel( _op.m_kernel )
            , m_obj( _op.m_obj )
        {
            _op.m_kernel = nullptr;
            _op.m_obj = nullptr;
        }
        //////////////////////////////////////////////////////////////////////////
        import_operator_t::import_operator_t( kernel_interface * _kernel, std::nullptr_t )
            : m_kernel( _kernel )
            , m_obj( _kernel->ret_none() )
        {
        }
        //////////////////////////////////////////////////////////////////////////
        import_operator_t::import_operator_t( kernel_interface * _kernel, PyObject * _value )
            : m_kernel( _kernel )
            , m_obj( _value )
        {
            if( m_obj != nullptr )
            {
                m_kernel->incref( m_obj );
            }
        }
        //////////////////////////////////////////////////////////////////////////
        import_operator_t::import_operator_t( const object & _obj )
            : m_kernel( _obj.kernel() )
            , m_obj( _obj.ret() )
        {
        }
        //////////////////////////////////////////////////////////////////////////
        import_operator_t::~import_operator_t()
        {
            if( m_obj != nullptr )
            {
                m_kernel->decref( m_obj );
            }
        }
        //////////////////////////////////////////////////////////////////////////
        import_operator_t::operator PyObject * () const
        {
            return m_obj;
        }
        //////////////////////////////////////////////////////////////////////////
        PyObject * import_operator_t::ptr() const
        {
            return m_obj;
        }
        //////////////////////////////////////////////////////////////////////////
    }
}