#	pragma once

#	include "pybind/object.hpp"
#	include "pybind/list_helper.hpp"

namespace pybind
{ 
	class PYBIND_API list
		: public object
	{
	public:
		list();
		list( size_t _size );

		explicit list( PyObject * _obj, pybind::borrowed );
		explicit list( PyObject * _obj );

	public:
		detail::set_list_operator_t operator [] ( size_t _index );
		detail::extract_operator_t operator [] ( size_t _index ) const;		

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

		template<class It>
		void fill( size_t _offset, It _begin, It _end )
		{
			size_t index = 0;

			for( It it = _begin; it != _end; ++it )
			{
				this->operator [] ( _offset + index++ ) = *it;
			}
		}

	public:
		size_t size() const;
		bool empty() const;
	};
	//////////////////////////////////////////////////////////////////////////
	template<class C>
	PYBIND_API pybind::list make_list_container_t( const C & _c )
	{
		pybind::list l( _c.size() );

		l.fill( 0, _c.begin(), _c.end() );

		return l;
	}
	//////////////////////////////////////////////////////////////////////////
	PYBIND_API pybind::list make_invalid_list_t();
}