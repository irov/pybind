#	pragma once

#	include "pybind/object.hpp"

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
		bool contains( const char * _name ) const;
		detail::extract_operator_t operator [] ( const char * _name ) const;

		void remove( const char * _name ) const;

	public:
		size_t size() const;
		bool empty() const;
	};
	//////////////////////////////////////////////////////////////////////////
	PYBIND_API pybind::dict make_invalid_dict_t();
}