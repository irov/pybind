#	pragma once

#	include "pybind/object.hpp"

#	include "pybind/helper/list_helper.hpp"

namespace pybind
{ 
	class PYBIND_API list
		: public object
	{
	public:		
		list( kernel_interface * _kernel );
		list( const list & _list );
				
	public:
		explicit list( pybind::invalid );
		explicit list( kernel_interface * _kernel, size_t _size );
		explicit list( kernel_interface * _kernel, PyObject * _obj, pybind::borrowed );
		explicit list( kernel_interface * _kernel, PyObject * _obj );

	public:
		detail::set_list_operator_t operator [] ( size_t _index );
		detail::extract_operator_t operator [] ( size_t _index ) const;		

	public:
		template<class T>
		list & append( const T & _t )
		{
			return this->append_i( detail::import_operator_t( m_kernel, _t ) );
		}
			
    public:
		list & append_i( const detail::import_operator_t & _t );
		
	public:
		template<class It>
		list & append( It _begin, It _end )
		{
			for( It it = _begin; it != _end; ++it )
			{
				this->append( *it );
			}

			return *this;
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
	template<>
	struct extract_specialized<pybind::list>
	{
		pybind::list operator () ( kernel_interface * _kernel, PyObject * _obj )
		{
			pybind::list value( _kernel );

			if( extract_value( _kernel, _obj, value, true ) == false )
			{
				const std::type_info & tinfo = typeid(pybind::list);

				const char * type_name = tinfo.name();

				pybind::log( "extract_value<T>: extract invalid %s:%s not cast to '%s'"
					, pybind::object_repr( _obj )
					, pybind::object_repr_type( _obj )
					, type_name
					);
			}

			return value;
		}
	};
	//////////////////////////////////////////////////////////////////////////
	PYBIND_API bool list_check_t( const pybind::object & _obj );
	//////////////////////////////////////////////////////////////////////////
	template<class C>
	inline pybind::list make_list_container_t( kernel_interface * _kernel, const C & _c )
	{
		pybind::list l( _kernel, _c.size() );

		l.fill( 0, _c.begin(), _c.end() );

		return l;
	}
	//////////////////////////////////////////////////////////////////////////
	PYBIND_API pybind::list make_invalid_list_t();
}