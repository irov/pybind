#	pragma once

#   include "pybind/adapter_interface.hpp"

#	include "pybind/types.hpp"

namespace pybind
{
	//////////////////////////////////////////////////////////////////////////
	class functor_adapter_interface
        : public adapter_interface
	{
	public:
		functor_adapter_interface( const char * _name, uint32_t _arity )
			: m_name(_name)
			, m_arity(_arity)
		{
		}

	public:
		inline const char * getName() const
		{
			return m_name;
		}

    public:
		uint32_t getArity() const
		{
			return m_arity;
		}        

	public:
		virtual PyObject * call( PyObject * _args, PyObject * _kwds ) = 0;

	protected:
		const char * m_name;
		uint32_t m_arity;
	};
    //////////////////////////////////////////////////////////////////////////
    typedef stdex::intrusive_ptr<class functor_adapter_interface> functor_adapter_interface_ptr;
}

