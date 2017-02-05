#	include "pybind/dict.hpp"

namespace pybind
{
	//////////////////////////////////////////////////////////////////////////
	dict::dict( kernel_interface * _kernel )
		: pybind::object( _kernel, pybind::dict_new(), borrowed() )
	{
	}
	//////////////////////////////////////////////////////////////////////////
	dict::dict( const dict & _dict )
		: pybind::object( _dict.kernel(), _dict.ptr() )
	{
	}
	//////////////////////////////////////////////////////////////////////////
	dict::dict( pybind::invalid _iv )
		: object( _iv )
	{
	}
	//////////////////////////////////////////////////////////////////////////
	dict::dict( kernel_interface * _kernel, size_t _presized )
		: pybind::object( _kernel, pybind::dict_new_presized( _presized ), pybind::borrowed() )
	{
	}
	//////////////////////////////////////////////////////////////////////////
	dict::dict( kernel_interface * _kernel, PyObject * _obj, pybind::borrowed _br )
		: pybind::object( _kernel, _obj, _br )
	{ 
	}
	//////////////////////////////////////////////////////////////////////////
	dict::dict( kernel_interface * _kernel, PyObject * _obj )
		: pybind::object( _kernel, _obj )
	{
	}
    //////////////////////////////////////////////////////////////////////////
    dict::iterator dict::begin() const
    {
        return dict_iterator( m_kernel, m_obj );
    }
    //////////////////////////////////////////////////////////////////////////
    dict::iterator dict::end() const
    {
		return dict::iterator( m_kernel, m_obj, t_dict_iterator_end_tag() );
    }
	//////////////////////////////////////////////////////////////////////////
	bool dict::exist( const detail::import_operator_t & _name ) const
	{
		return pybind::dict_exist( m_obj, _name );
	}
	//////////////////////////////////////////////////////////////////////////
	pybind::object dict::get( const detail::import_operator_t & _name ) const
	{
		PyObject * py_attr = pybind::dict_get( m_obj, _name );

		return pybind::object( m_kernel, py_attr );
	}
	//////////////////////////////////////////////////////////////////////////
	detail::set_dict_operator_t dict::operator [] ( const detail::import_operator_t & _name )
	{
		return detail::set_dict_operator_t( m_kernel, m_obj, _name );
	}
	//////////////////////////////////////////////////////////////////////////
	detail::extract_operator_t dict::operator [] ( const detail::import_operator_t & _name ) const
	{
		PyObject * py_object = pybind::dict_get( m_obj, _name );
		
		return detail::extract_operator_t( m_kernel, py_object );
	}
	//////////////////////////////////////////////////////////////////////////
	void dict::remove( const detail::import_operator_t & _name ) const
	{
		pybind::dict_remove( m_obj, _name );
	}
	//////////////////////////////////////////////////////////////////////////
	size_t dict::size() const
	{
		return pybind::dict_size( m_obj );
	}
	//////////////////////////////////////////////////////////////////////////
	bool dict::empty() const
	{
		return this->size() == 0;
	}
	//////////////////////////////////////////////////////////////////////////
	pybind::dict make_invalid_dict_t()
	{
		return pybind::dict( pybind::invalid() );
	}
}