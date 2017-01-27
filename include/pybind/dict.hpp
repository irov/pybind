#	pragma once

#	include "pybind/object.hpp"
#	include "pybind/helper/dict_helper.hpp"
#	include "pybind/helper/dict_iterator.hpp"

namespace pybind
{
	class PYBIND_API dict
		: public pybind::object
	{
	public:		
		dict();
		explicit dict( size_t _presized );

	public:
		explicit dict( PyObject * _obj, borrowed );
		explicit dict( PyObject * _obj );

    public:
        typedef dict_iterator iterator;

    public:
        iterator begin() const;
        iterator end() const;

	public:
        bool exist( const detail::import_operator_t & _name ) const;
		pybind::object get( const detail::import_operator_t & _name ) const;

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