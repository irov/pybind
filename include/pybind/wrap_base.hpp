#	pragma once

#	include "pybind/types.hpp"
#	include <string>

namespace pybind
{
	class wrap_base
	{
	public:
		wrap_base();

	public:
		PyObject * call( const std::string & _method, const std::string & format, ... );

	public:
		void wrap( PyObject * _wrapp );

	protected:
		PyObject * m_impl;
	};
}