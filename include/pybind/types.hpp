#	pragma once

#	include <stdint.h>
#	include <memory.h>

#   include "stdex/intrusive_ptr.h"

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
    typedef stdex::intrusive_ptr<class class_type_scope> class_type_scope_ptr;

	typedef void * (*pybind_metacast)( void * );

	typedef void (*pybind_callback)();
	typedef PyObject * (*pybind_cfunction)(PyObject *, PyObject *);
	typedef PyObject * (*pybind_newfunc)(PyTypeObject *, PyObject *, PyObject *);

	typedef PyObject *(*pybind_getter)(PyObject *, void *);
	typedef int (*pybind_setter)(PyObject *, PyObject *, void *);

	typedef void * (*pybind_new)( const class_type_scope_ptr & _scope, PyObject * _obj, PyObject * _args, PyObject * _kwds );
	typedef void (*pybind_destructor)( const class_type_scope_ptr & _scope, void * impl );

#	ifdef PYBIND_VISIT_OBJECTS
	class pybind_visit_objects
	{
	public:
		virtual void visit_object( PyObject * _obj ) = 0;
	};
#   endif

    class visitor_class_type_scope
    {
    public:
        virtual void visit_scope( class_type_scope * _scope ) = 0;
    };
}

