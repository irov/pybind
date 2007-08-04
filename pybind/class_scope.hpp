#	pragma once

#	include "pybind/exports.hpp"

extern "C" 
{ 
	struct _typeobject; 
	typedef _typeobject PyTypeObject;
}

namespace pybind
{
	class class_type_scope;

	class PYBIND_API class_scope
	{
	public:
		static void reg_class_type( PyTypeObject * _type );
		static PyTypeObject * find_sub_type( PyTypeObject * _subtype );

		static void reg_class_scope( const type_info & _info, class_type_scope * _scope );
		static class_type_scope * get_class_scope( const type_info & _info );
		static bool has_class_scope( const type_info & _info );
	};
}