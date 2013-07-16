#	include "pybind/constructor.hpp"
#	include "pybind/system.hpp"

#	include <stdio.h>

namespace pybind
{
	//////////////////////////////////////////////////////////////////////////
	constructor_adapter_interface::constructor_adapter_interface( size_t _arity )
		: m_arity(_arity)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	constructor_adapter_interface::~constructor_adapter_interface()
	{
		
	}
	//////////////////////////////////////////////////////////////////////////
	bool constructor_adapter_interface::valid( PyObject * _args ) const
	{
		if( pybind::tuple_check( _args ) == false )
		{
			return false;
		}

		size_t args_arity = pybind::tuple_size( _args );

		if( m_arity != args_arity )
		{
			printf("construct have %d args, but you call %d params"
				, m_arity
				, args_arity
				);

			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	size_t constructor_adapter_interface::arity() const
	{
		return m_arity;
	}
}
