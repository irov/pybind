#	pragma once

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
	typedef void * (*pybind_metacast)( void * );

	typedef void (*pybind_callback)();
	typedef PyObject * (*pybind_cfunction)(PyObject *, PyObject *);
	typedef PyObject * (*pybind_newfunc)(PyTypeObject *, PyObject *, PyObject *);
	typedef void (*pybind_destructor)(PyObject *);
}