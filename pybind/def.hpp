#	pragma once

namespace pybind
{

	template<class T, class F>
	void def( const char * _name, F f );

}