#	include "pybind/module.hpp"

namespace pybind
{
	//////////////////////////////////////////////////////////////////////////
	module::module()
	{ 
	}
	//////////////////////////////////////////////////////////////////////////
	module::module( PyObject * _obj, pybind::borrowed _br )
		: pybind::object( _obj, _br )
	{ 
	}
	//////////////////////////////////////////////////////////////////////////
	module::module( PyObject * _obj )
		: pybind::object( _obj )
	{
	}
	//////////////////////////////////////////////////////////////////////////
	bool module::has_attr( const detail::import_operator_t & _name ) const
	{
		PyObject * module_dict = pybind::module_dict( m_obj );

		bool result = pybind::dict_exist( module_dict, _name );

		return result;
	}
	//////////////////////////////////////////////////////////////////////////
	pybind::object module::get_attr( const detail::import_operator_t & _name ) const
	{
		PyObject * module_dict = pybind::module_dict( m_obj );

		PyObject * py_attr = pybind::dict_get( module_dict, _name );

		return pybind::object( py_attr );
	}
	//////////////////////////////////////////////////////////////////////////
	pybind::dict module::get_dict() const
	{
		PyObject * module_dict = pybind::module_dict( m_obj );

		return pybind::dict( module_dict );
	}
}