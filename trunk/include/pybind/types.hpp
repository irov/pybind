#	pragma once

#	include <stdint.h>
#	include <memory.h>

#	include "config/stdex.hpp"

extern "C" 
{ 
	struct _object;
	typedef _object PyObject;
}

extern "C" 
{ 
	struct _typeobject; 
	typedef _typeobject PyTypeObject;
}

namespace pybind
{
	class kernel_interface;

	typedef stdex::intrusive_ptr<class class_type_scope> class_type_scope_ptr;

	typedef void * (*pybind_metacast)( void * );

	typedef void (*pybind_callback)();
	typedef PyObject * (*pybind_cfunction)(PyObject *, PyObject *);
	typedef PyObject * (*pybind_newfunc)(PyTypeObject *, PyObject *, PyObject *);

	typedef PyObject *(*pybind_getter)(PyObject *, void *);
	typedef int (*pybind_setter)(PyObject *, PyObject *, void *);

    class visitor_class_type_scope
    {
    public:
		virtual void visit_scope( class_type_scope * _scope ) = 0;
    };

	class observer_bind_call
	{
	public:
		virtual void begin_bind_call( const char * _className, const char * _functionName, PyObject * _args, PyObject * _kwds ) = 0;
		virtual void end_bind_call( const char * _className, const char * _functionName, PyObject * _args, PyObject * _kwds ) = 0;
	};
}

