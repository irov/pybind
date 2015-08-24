#	pragma once

#	include "pybind/object.hpp"

namespace pybind
{ 
	class PYBIND_API list
		: public object
	{
	public:
		list();
		list( size_t _size );

		explicit list( PyObject * _obj, borrowed );
		explicit list( PyObject * _obj );

	public:
		detail::extract_operator_t operator [] ( size_t _index ) const;
		detail::extract_list_operator_t operator [] ( size_t _index );

	public:
		void append( const detail::import_operator_t & _t );

		template<class It>
		void assign( It _begin, It _end )
		{
			size_t index = 0;

			It it = _begin;
			for( ; it != _end; ++it )
			{
				this->operator [] ( index ) = *it;

				++index;
			}
		}

	public:
		size_t size() const;
		bool empty() const;
	};
	//////////////////////////////////////////////////////////////////////////
	PYBIND_API pybind::list make_list_t( size_t _size );
	PYBIND_API pybind::list make_invalid_list_t();
}