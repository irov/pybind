#	pragma once

#	include "pybind/exports.hpp"

extern "C" 
{ 
	struct _object; 
	typedef _object PyObject;
}

namespace pybind
{
	class PYBIND_API module_
	{
	public:
		module_( const char * _name );
		bool addObject( const char * _name, PyObject * _object );

	private:
		PyObject * m_module;
	};
}