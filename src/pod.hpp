#	pragma once

#	include "config/python.hpp"

namespace pybind
{
	namespace detail
	{
		//////////////////////////////////////////////////////////////////////////
		static const uint32_t PY_OBJECT_UNWRAP = 0xFF000000;
		static const uint32_t PY_OBJECT_HOLDER = 0x00000001;

		static const uint32_t PY_OBJECT_PTR = 0x00000002;
		static const uint32_t PY_OBJECT_POD8 = 0x00000010;
		static const uint32_t PY_OBJECT_POD16 = 0x00000020;
		static const uint32_t PY_OBJECT_POD32 = 0x00000040;
		static const uint32_t PY_OBJECT_POD64 = 0x00000080;
		//////////////////////////////////////////////////////////////////////////
		struct py_base_object
		{
			PyObject_HEAD
				uint32_t flag;
		};
		//////////////////////////////////////////////////////////////////////////
		struct py_ptr_object
			: public py_base_object
		{
			void * impl;
		};
		//////////////////////////////////////////////////////////////////////////
		struct py_pod8_object
			: public py_base_object
		{
			uint8_t buff[8];
		};
		//////////////////////////////////////////////////////////////////////////
		struct py_pod16_object
			: public py_base_object
		{
			uint8_t buff[16];
		};
		//////////////////////////////////////////////////////////////////////////
		struct py_pod32_object
			: public py_base_object
		{
			uint8_t buff[32];
		};
		//////////////////////////////////////////////////////////////////////////
		struct py_pod64_object
			: public py_base_object
		{
			uint8_t buff[64];
		};
		//////////////////////////////////////////////////////////////////////////
		PyTypeObject * get_pod_type( uint32_t _pod );
		void * get_class_impl( PyObject * _obj );
	}
	//////////////////////////////////////////////////////////////////////////
	bool pod_initialize();
	void pod_finalize();
}