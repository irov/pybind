#	include "pybind/wrap_base.hpp"

#	include "pybind/system.hpp"

namespace pybind
{
	//////////////////////////////////////////////////////////////////////////
	wrap_base::wrap_base()
		: m_impl(0)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	PyObject * wrap_base::call( const std::string & _method, const std::string & _format, ... )
	{
		va_list valist;
		va_start(valist, _format);

		PyObject * result = call_method_va( m_impl, _method.c_str(), _format.c_str(), valist );
		
		check_error();

		va_end( valist );

		return result;
	}
	//////////////////////////////////////////////////////////////////////////
	void wrap_base::wrap( PyObject * _wrapp )
	{
		m_impl = _wrapp;
	}
}