#	include "pybind/member_type.hpp"

#	include "pybind/class_type.hpp"

namespace pybind
{
	//////////////////////////////////////////////////////////////////////////
	member_type_scope::member_type_scope( PyTypeObject * _class, 
		const char * _name, 
		method_adapter_interface * _ifunc, 
		pybind_cfunction _cfunc, 
		int _hasargs )
		: m_name(_name)
		, m_interface(_ifunc)
	{
		m_member.ml_name = _name;
		m_method.ml_meth = _cfunc;
		m_method.ml_flags = METH_CLASS | ( _hasargs ) ? METH_VARARGS : METH_NOARGS;
		m_method.ml_doc = "Embedding function cpp";
	}
	//////////////////////////////////////////////////////////////////////////
	member_type_scope::~member_type_scope()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	PyObject * member_type_scope::instance( py_class_type * _obj )
	{
		PyGetSetDef m_member;
		PyObject * props = PyDescr_NewGetSet( (PyTypeObject *)_obj, &m_member );

		return props;
	}
}
