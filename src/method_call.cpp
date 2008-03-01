#	include "pybind/method_call.hpp"

#	include "config/python.hpp"

namespace pybind
{
	namespace detail
	{
		PyObject * return_none()
		{
			Py_RETURN_NONE;
		}

		PyObject * getarg( PyObject * _args, int _item)
		{
			return PyTuple_GetItem( _args, _item );
		}
	}
}