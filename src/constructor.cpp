#	include "pybind/constructor.hpp"
#	include "pybind/system.hpp"

namespace pybind
{
	namespace detail
	{
		//////////////////////////////////////////////////////////////////////////
		PyObject * getarg( PyObject * _args, size_t _it )
		{
			return tuple_getitem( _args, _it );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	constructor::constructor( size_t _arity )
		: m_arity(_arity)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	constructor::~constructor()
	{
		
	}
	//////////////////////////////////////////////////////////////////////////
	bool constructor::valid( PyObject * _args ) const
	{
		if( pybind::tuple_check( _args ) == false )
		{
			return false;
		}

		size_t args_arity = pybind::tuple_size( _args );

		if( m_arity != args_arity )
		{
			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	size_t constructor::arity() const
	{
		return m_arity;
	}
}