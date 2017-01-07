#	pragma once

#	include "pybind/adapter_interface.hpp"

#	include "pybind/extract.hpp"
#	include "pybind/exports.hpp"

namespace pybind
{
	class constructor_adapter_interface
        : public adapter_interface
	{
	public:
		constructor_adapter_interface( size_t _arity )
			: m_arity(_arity)
		{
		}
		
	public:
		bool valid( PyObject * _args ) const
		{
			if( pybind::tuple_check( _args ) == false )
			{
				return false;
			}

			size_t args_arity = pybind::tuple_size( _args );

			if( m_arity != args_arity )
			{
				pybind::throw_exception("construct have %d args, but you call %d params"
					, m_arity
					, args_arity
					);

				return false;
			}

			return true;
		}

		size_t arity() const
		{
			return m_arity;
		}

	public:
		virtual void * call( kernel_interface * _kernel, PyObject * _obj, PyObject * _args ) = 0;

	protected:
		size_t m_arity;
	};
    //////////////////////////////////////////////////////////////////////////
    typedef stdex::intrusive_ptr<constructor_adapter_interface> constructor_adapter_interface_ptr;
}
