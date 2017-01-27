#	include "pybind/dict.hpp"

namespace pybind
{
	//////////////////////////////////////////////////////////////////////////
	dict::dict()
		: pybind::object( pybind::dict_new(), borrowed() )
	{
	}
	dict::dict( size_t _presized )
		: pybind::object( pybind::dict_new_presized(_presized), borrowed() )
	{

	}
	//////////////////////////////////////////////////////////////////////////
	dict::dict( PyObject * _obj, borrowed _br )
		: pybind::object( _obj, _br )
	{ 
	}
	//////////////////////////////////////////////////////////////////////////
	dict::dict( PyObject * _obj )
		: pybind::object( _obj )
	{
	}
    //////////////////////////////////////////////////////////////////////////
    dict::iterator dict::begin() const
    {
        return dict_iterator( m_obj );
    }
    //////////////////////////////////////////////////////////////////////////
    dict::iterator dict::end() const
    {
        return dict::iterator( m_obj, t_dict_iterator_end_tag() );
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

		return pybind::object( py_attr );
	}
	//////////////////////////////////////////////////////////////////////////
	detail::set_dict_operator_t dict::operator [] ( const detail::import_operator_t & _name )
	{
		return detail::set_dict_operator_t( m_obj, _name );
	}
	//////////////////////////////////////////////////////////////////////////
	detail::extract_operator_t dict::operator [] ( const detail::import_operator_t & _name ) const
	{
		PyObject * py_object = pybind::dict_get( m_obj, _name );
		
		return detail::extract_operator_t( py_object );
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
		return pybind::dict( nullptr, pybind::borrowed() );
	}
}