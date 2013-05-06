#	pragma once

#	include "pybind/extract.hpp"
#	include "pybind/exports.hpp"

namespace pybind
{
	class PYBIND_API constructor
	{
	public:
		constructor( size_t _arity );
		virtual ~constructor();

	public:
		bool valid( PyObject * _args ) const;
		size_t arity() const;

	public:
		virtual void * call( PyObject * _obj, PyObject * _args, const char * _tag ) = 0;

	protected:
		size_t m_arity;
	};
}
