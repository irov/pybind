#	pragma once

#	include "pybind/adapter_interface.hpp"

namespace pybind
{
	class convert_adapter_interface
		: public adapter_interface
	{
	public:
		virtual void * convert( PyObject * _obj ) = 0;
	};
	//////////////////////////////////////////////////////////////////////////
	template<class F>
	class convert_adapter
		: public convert_adapter_interface
	{
	public:
		convert_adapter( F _convert )
			: m_convert(_convert)
		{
		}

	public:
		void * convert( PyObject * _obj ) override
		{
			void * obj = (*m_convert)( _obj );

			return obj;
		}

	protected:
		F m_convert;
	};
}

