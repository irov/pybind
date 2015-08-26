#	pragma once

#	include "pybind/object.hpp"
#	include "pybind/dict_helper.hpp"

namespace pybind
{
	class PYBIND_API dict
		: public pybind::object
	{
	public:		
		dict();

		explicit dict( PyObject * _obj, borrowed );
		explicit dict( PyObject * _obj );

	public:
		size_t exist( const detail::import_operator_t & _name ) const;
		size_t contains( const detail::import_operator_t & _name ) const;

	public:
		detail::set_dict_operator_t operator [] ( const detail::import_operator_t & _name );
		detail::extract_operator_t operator [] ( const detail::import_operator_t & _name ) const;

	public:
		void remove( const detail::import_operator_t & _name ) const;

	public:
		size_t size() const;
		bool empty() const;
	};
	//////////////////////////////////////////////////////////////////////////
	PYBIND_API pybind::dict make_invalid_dict_t();
}