#include "pybind/object.hpp"

namespace pybind
{
    //////////////////////////////////////////////////////////////////////////
    const pybind::object & object::get_invalid()
    {
        static pybind::object s_obj;

        return s_obj;
    }
    //////////////////////////////////////////////////////////////////////////
    object::object()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    object::object( const object & _obj )
        : base( _obj.kernel(), _obj.ptr() )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    object::object( object && _obj )
        : base( std::move( _obj ) )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    object::object( const detail::extract_operator_t & _extract )
        : base( _extract.kernel(), _extract.ptr() )
    {

    }
    //////////////////////////////////////////////////////////////////////////
    object::object( pybind::invalid_t _iv )
        : base( _iv )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    object::object( kernel_interface * _kernel, PyObject * _obj )
        : base( _kernel, _obj )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    object::object( kernel_interface * _kernel, PyObject * _obj, pybind::borrowed_t _br )
        : base( _kernel, _obj, _br )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    object::~object()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    object & object::operator = ( std::nullptr_t )
    {
        this->base::operator = ( nullptr );

        return *this;
    }
    //////////////////////////////////////////////////////////////////////////
    object & object::operator = ( const object & _obj )
    {
        this->base::operator = ( _obj );

        return *this;
    }
    //////////////////////////////////////////////////////////////////////////
    object & object::operator = ( object && _obj )
    {
        this->base::operator = ( std::move( _obj ) );

        return *this;
    }
    //////////////////////////////////////////////////////////////////////////
    bool object::has_attr_i( const detail::import_operator_t & _name ) const
    {
        return m_kernel->has_attr( m_obj, _name );
    }
    //////////////////////////////////////////////////////////////////////////
    pybind::object object::get_attr_i( const detail::import_operator_t & _name ) const
    {
        PyObject * py_attr = m_kernel->get_attr( m_obj, _name );

        return pybind::object( m_kernel, py_attr, pybind::borrowed );
    }
    //////////////////////////////////////////////////////////////////////////
    detail::extract_operator_t object::extract() const
    {
        return detail::extract_operator_t( m_kernel, m_obj );
    }
    //////////////////////////////////////////////////////////////////////////
    detail::extract_operator_t object::call() const
    {
        return pybind::call_t( m_kernel, m_obj );
    }
    //////////////////////////////////////////////////////////////////////////
    detail::extract_operator_t object::call_i( const detail::import_operator_t & _t0 ) const
    {
        return pybind::call_i( m_kernel, m_obj, _t0 );
    }
    //////////////////////////////////////////////////////////////////////////
    detail::extract_operator_t object::call_i( const detail::import_operator_t & _t0, const detail::import_operator_t & _t1 ) const
    {
        return pybind::call_i( m_kernel, m_obj, _t0, _t1 );
    }
    //////////////////////////////////////////////////////////////////////////
    detail::extract_operator_t object::call_i( const detail::import_operator_t & _t0, const detail::import_operator_t & _t1, const detail::import_operator_t & _t2 ) const
    {
        return pybind::call_i( m_kernel, m_obj, _t0, _t1, _t2 );
    }
    //////////////////////////////////////////////////////////////////////////
    detail::extract_operator_t object::call_i( const detail::import_operator_t & _t0, const detail::import_operator_t & _t1, const detail::import_operator_t & _t2, const detail::import_operator_t & _t3 ) const
    {
        return pybind::call_i( m_kernel, m_obj, _t0, _t1, _t2, _t3 );
    }
    //////////////////////////////////////////////////////////////////////////
    detail::extract_operator_t object::call_i( const detail::import_operator_t & _t0, const detail::import_operator_t & _t1, const detail::import_operator_t & _t2, const detail::import_operator_t & _t3, const detail::import_operator_t & _t4 ) const
    {
        return pybind::call_i( m_kernel, m_obj, _t0, _t1, _t2, _t3, _t4 );
    }
    //////////////////////////////////////////////////////////////////////////
    detail::extract_operator_t object::call_i( const detail::import_operator_t & _t0, const detail::import_operator_t & _t1, const detail::import_operator_t & _t2, const detail::import_operator_t & _t3, const detail::import_operator_t & _t4, const detail::import_operator_t & _t5 ) const
    {
        return pybind::call_i( m_kernel, m_obj, _t0, _t1, _t2, _t3, _t4, _t5 );
    }
    //////////////////////////////////////////////////////////////////////////
    detail::extract_operator_t object::call_i( const detail::import_operator_t & _t0, const detail::import_operator_t & _t1, const detail::import_operator_t & _t2, const detail::import_operator_t & _t3, const detail::import_operator_t & _t4, const detail::import_operator_t & _t5, const detail::import_operator_t & _t6 ) const
    {
        return pybind::call_i( m_kernel, m_obj, _t0, _t1, _t2, _t3, _t4, _t5, _t6 );
    }
    //////////////////////////////////////////////////////////////////////////
    detail::extract_operator_t object::call_i( const detail::import_operator_t & _t0, const detail::import_operator_t & _t1, const detail::import_operator_t & _t2, const detail::import_operator_t & _t3, const detail::import_operator_t & _t4, const detail::import_operator_t & _t5, const detail::import_operator_t & _t6, const detail::import_operator_t & _t7 ) const
    {
        return pybind::call_i( m_kernel, m_obj, _t0, _t1, _t2, _t3, _t4, _t5, _t6, _t7 );
    }
    //////////////////////////////////////////////////////////////////////////
    detail::extract_operator_t object::call_args( const args & _args ) const
    {
        return pybind::call_args_t( m_kernel, m_obj, _args );
    }
    //////////////////////////////////////////////////////////////////////////
    detail::extract_operator_t object::call_args_i( const detail::import_operator_t & _t0, const args & _args ) const
    {
        return pybind::call_args_i( m_kernel, m_obj, _t0, _args );
    }
    //////////////////////////////////////////////////////////////////////////
    detail::extract_operator_t object::call_args_i( const detail::import_operator_t & _t0, const detail::import_operator_t & _t1, const args & _args ) const
    {
        return pybind::call_args_i( m_kernel, m_obj, _t0, _t1, _args );
    }
    //////////////////////////////////////////////////////////////////////////
    detail::extract_operator_t object::call_args_i( const detail::import_operator_t & _t0, const detail::import_operator_t & _t1, const detail::import_operator_t & _t2, const args & _args ) const
    {
        return pybind::call_args_i( m_kernel, m_obj, _t0, _t1, _t2, _args );
    }
    //////////////////////////////////////////////////////////////////////////
    detail::extract_operator_t object::call_args_i( const detail::import_operator_t & _t0, const detail::import_operator_t & _t1, const detail::import_operator_t & _t2, const detail::import_operator_t & _t3, const args & _args ) const
    {
        return pybind::call_args_i( m_kernel, m_obj, _t0, _t1, _t2, _t3, _args );
    }
    //////////////////////////////////////////////////////////////////////////
    detail::extract_operator_t object::call_args_i( const detail::import_operator_t & _t0, const detail::import_operator_t & _t1, const detail::import_operator_t & _t2, const detail::import_operator_t & _t3, const detail::import_operator_t & _t4, const args & _args ) const
    {
        return pybind::call_args_i( m_kernel, m_obj, _t0, _t1, _t2, _t3, _t4, _args );
    }
    //////////////////////////////////////////////////////////////////////////
    detail::extract_operator_t object::call_args_i( const detail::import_operator_t & _t0, const detail::import_operator_t & _t1, const detail::import_operator_t & _t2, const detail::import_operator_t & _t3, const detail::import_operator_t & _t4, const detail::import_operator_t & _t5, const args & _args ) const
    {
        return pybind::call_args_i( m_kernel, m_obj, _t0, _t1, _t2, _t3, _t4, _t5, _args );
    }
    //////////////////////////////////////////////////////////////////////////
    detail::extract_operator_t object::call_args_i( const detail::import_operator_t & _t0, const detail::import_operator_t & _t1, const detail::import_operator_t & _t2, const detail::import_operator_t & _t3, const detail::import_operator_t & _t4, const detail::import_operator_t & _t5, const detail::import_operator_t & _t6, const args & _args ) const
    {
        return pybind::call_args_i( m_kernel, m_obj, _t0, _t1, _t2, _t3, _t4, _t5, _t6, _args );
    }
    //////////////////////////////////////////////////////////////////////////
    detail::extract_operator_t object::call_args_i( const detail::import_operator_t & _t0, const detail::import_operator_t & _t1, const detail::import_operator_t & _t2, const detail::import_operator_t & _t3, const detail::import_operator_t & _t4, const detail::import_operator_t & _t5, const detail::import_operator_t & _t6, const detail::import_operator_t & _t7, const args & _args ) const
    {
        return pybind::call_args_i( m_kernel, m_obj, _t0, _t1, _t2, _t3, _t4, _t5, _t6, _t7, _args );
    }
    //////////////////////////////////////////////////////////////////////////
    pybind::object make_none_t( kernel_interface * _kernel )
    {
        PyObject * py_none = _kernel->ret_none();

        return pybind::object( _kernel, py_none, pybind::borrowed );
    }
    //////////////////////////////////////////////////////////////////////////
    pybind::object make_true_t( kernel_interface * _kernel )
    {
        PyObject * py_true = _kernel->ret_true();

        return pybind::object( _kernel, py_true, pybind::borrowed );
    }
    //////////////////////////////////////////////////////////////////////////
    pybind::object make_false_t( kernel_interface * _kernel )
    {
        PyObject * py_false = _kernel->ret_false();

        return pybind::object( _kernel, py_false, pybind::borrowed );
    }
    //////////////////////////////////////////////////////////////////////////
    pybind::object make_object_i( kernel_interface * _kernel, const detail::import_operator_t & _t0 )
    {
        return pybind::object( _kernel, _t0 );
    }
    //////////////////////////////////////////////////////////////////////////
    pybind::object make_invalid_object_t()
    {
        return pybind::object( pybind::invalid );
    }
}