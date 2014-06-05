#	pragma once

#	include "exports.hpp"

namespace pybind
{
	class pybind_exception
	{
	public:
		pybind_exception( const char * _message )
			: m_message(_message)
		{
		}

	public:
		const char * what() const
		{
			return m_message;
		}

	protected:
		const char * m_message;
	};

	PYBIND_API void throw_exception( const char * _format, ... );
}

