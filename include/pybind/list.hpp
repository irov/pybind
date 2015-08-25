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
		void append( It _begin, It _end )
		{
			for( It it = _begin; it != _end; ++it )
			{
				this->append( *it );
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