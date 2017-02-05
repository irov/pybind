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
		dict( const dict & _dict );

	public:
		explicit dict( pybind::invalid );
		explicit dict( kernel_interface * _kernel );
		explicit dict( kernel_interface * _kernel, size_t _presized );
		explicit dict( kernel_interface * _kernel, PyObject * _obj, pybind::borrowed );
		explicit dict( kernel_interface * _kernel, PyObject * _obj );

    public:
        typedef dict_iterator iterator;

    public:
        iterator begin() const;
        iterator end() const;

	public:
		template<class T>
		bool exist( const T & _name ) const
		{
			return this->exist( detail::import_operator_t( m_kernel, _name ) );
		}

		template<class T>
		pybind::object get( const T & _name ) const
		{
			return this->get( detail::import_operator_t( m_kernel, _name ) );
		}

	public:
		template<class T>
		detail::set_dict_operator_t operator [] ( const T & _name )
		{
			return this->operator [] ( detail::import_operator_t( m_kernel, _name ) );
		}

		template<class T>
		detail::extract_operator_t operator [] ( const T & _name ) const
		{
			return this->operator [] ( detail::import_operator_t( m_kernel, _name ) );
		}

	public:
		template<class T>
		void remove( const T & _name ) const
		{
			this->remove( detail::import_operator_t( m_kernel, _name ) );
		}

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