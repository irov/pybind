#	pragma once

#	include "exports.hpp"

namespace pybind
{
	class pybind_exception
	{
	public:
		pybind_exception()
		{
		}

	public:
		char * buff()
		{
			return m_message;
		}

	public:
		const char * what() const
		{
			return m_message;
		}

	protected:
		char m_message[2048];
	};

	PYBIND_API void throw_exception( const char * _format, ... );
}

