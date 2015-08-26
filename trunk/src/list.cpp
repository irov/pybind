#	include "pybind/list.hpp"

namespace pybind
{
	//////////////////////////////////////////////////////////////////////////
	list::list()
		: pybind::object( pybind::tuple_new( 0 ), pybind::borrowed() )
	{
	}
	//////////////////////////////////////////////////////////////////////////
	list::list( size_t _size )
		: pybind::object( pybind::tuple_new( _size ), pybind::borrowed() )
	{
	}
	//////////////////////////////////////////////////////////////////////////
	list::list( PyObject * _obj, pybind::borrowed _br )
		: pybind::object( _obj, _br )
	{ 
	}
	//////////////////////////////////////////////////////////////////////////
	list::list( PyObject * _obj )
		: pybind::object( _obj )
	{
	}
	//////////////////////////////////////////////////////////////////////////
	detail::extract_operator_t list::operator [] ( size_t _index ) const
	{
		PyObject * py_item = pybind::list_getitem( m_obj, _index );

		return detail::extract_operator_t( py_item );
	}
	//////////////////////////////////////////////////////////////////////////
	detail::set_list_operator_t list::operator [] ( size_t _index )
	{
		return detail::set_list_operator_t( m_obj, _index );
	}
	//////////////////////////////////////////////////////////////////////////
	void list::append( const detail::import_operator_t & _t )
	{
		pybind::list_appenditem_t( m_obj, _t );
	}
	//////////////////////////////////////////////////////////////////////////
	size_t list::size() const
	{
		return pybind::list_size( m_obj );
	}
	//////////////////////////////////////////////////////////////////////////
	bool list::empty() const
	{
		return this->size() == 0;
	}
	//////////////////////////////////////////////////////////////////////////
	pybind::list make_invalid_list_t()
	{
		return pybind::list( nullptr, pybind::borrowed() );
	}
}