#	pragma once

#	include "pybind/types.hpp"

namespace pybind
{
	class object
	{
	public:
		object();
		~object();

	public:
		object( const object & _obj );
		object( PyObject * _obj );

	public:
		void operator = ( const object & _obj );
				
	protected:
		PyObject * m_object;
	};
}