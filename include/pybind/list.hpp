#	pragma once

#	include "pybind/object.hpp"

namespace pybind
{ 
	class PYBIND_API list
		: public object
	{
	public:
		list();

		explicit list( PyObject * _obj, borrowed );
		explicit list( PyObject * _obj );

	public:
		detail::extract_operator_t operator [] ( size_t _index ) const;
		detail::extract_list_operator_t operator [] ( size_t _index );

	public:
		void append( const detail::borrowed_import_operator_t & _t );

	public:
		size_t size() const;
		bool empty() const;
	};
	//////////////////////////////////////////////////////////////////////////
	PYBIND_API pybind::list make_invalid_list_t();
}