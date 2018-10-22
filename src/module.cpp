#include "pybind/module.hpp"

namespace pybind
{
    //////////////////////////////////////////////////////////////////////////
    module::module()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    module::module( const module & _module )
        : object( _module.kernel(), _module.ptr() )
    {
    }
	//////////////////////////////////////////////////////////////////////////
	module::module( module && _module )
		: object( std::move( _module ) )
	{
	}
    //////////////////////////////////////////////////////////////////////////
    module::module( kernel_interface * _kernel, PyObject * _obj, pybind::borrowed _br )
        : object( _kernel, _obj, _br )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    module::module( kernel_interface * _kernel, PyObject * _obj )
        : object( _kernel, _obj )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    module::~module()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    module & module::operator = ( std::nullptr_t )
    {
        this->object::operator = ( nullptr );

        return *this;
    }
    //////////////////////////////////////////////////////////////////////////
    module & module::operator = ( const module & _obj )
    {
        this->object::operator = ( _obj );

        return *this;
    }
    //////////////////////////////////////////////////////////////////////////
    module & module::operator = ( module && _obj )
    {
        this->object::operator = ( std::move( _obj ) );

        return *this;
    }
    //////////////////////////////////////////////////////////////////////////
    bool module::has_attr_i( const detail::import_operator_t & _name ) const
    {
        PyObject * module_dict = m_kernel->module_dict( m_obj );

        bool result = m_kernel->dict_exist( module_dict, _name );

        return result;
    }
    //////////////////////////////////////////////////////////////////////////
    detail::extract_operator_t module::get_attr_i( const detail::import_operator_t & _name ) const
    {
        PyObject * module_dict = m_kernel->module_dict( m_obj );

        PyObject * py_attr = m_kernel->dict_get( module_dict, _name );

        return detail::extract_operator_t( m_kernel, py_attr );
    }
    //////////////////////////////////////////////////////////////////////////
    pybind::dict module::get_dict() const
    {
        PyObject * module_dict = m_kernel->module_dict( m_obj );

        return pybind::dict( m_kernel, module_dict );
    }
}