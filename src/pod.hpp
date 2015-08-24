#	pragma once

#	include "config/python.hpp"

#	include "pybind/types.hpp"

namespace pybind
{
	namespace detail
	{
		//////////////////////////////////////////////////////////////////////////
		static const uint32_t PY_OBJECT_UNWRAP = 0xFF000000;
		static const uint32_t PY_OBJECT_HOLDER = 0x00000001;

		static const uint32_t PY_OBJECT_PTR = 0x00000002;
		static const uint32_t PY_OBJECT_POD4 = 0x0000040;
		static const uint32_t PY_OBJECT_POD8 = 0x0000080;
		static const uint32_t PY_OBJECT_POD16 = 0x00000100;
		static const uint32_t PY_OBJECT_POD32 = 0x00000200;
		static const uint32_t PY_OBJECT_POD64 = 0x00000400;
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
		struct py_pod4_object
			: public py_base_object
		{
			uint8_t buff[4];
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
	}
	//////////////////////////////////////////////////////////////////////////
	bool initialize_pod();
	void finalize_pod();
}