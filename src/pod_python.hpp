#	pragma once

#	include "config/python.hpp"

#	include "pybind/types.hpp"

namespace pybind
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
	namespace detail
	{
		bool is_pod_holder( PyObject * _obj );
		void wrap_pod_ptr( PyObject * _obj, void * _impl, bool _holder );
		void wrap_pod( PyObject * _obj, void ** _impl, size_t _size );
		void * get_pod_impl( PyObject * _obj );
		void unwrap_pod( PyObject * _obj );
		bool is_pod_wrap( PyObject * _obj );
	}
	//////////////////////////////////////////////////////////////////////////
	class pod_python
	{
	public:
		bool initialize();
		void finalize();

	public:
		PyTypeObject * get_pod_type( uint32_t _pod );

	protected:
		PyTypeObject m_pod4_type;
		PyTypeObject m_pod8_type;
		PyTypeObject m_pod16_type;
		PyTypeObject m_pod32_type;
		PyTypeObject m_pod64_type;

		PyTypeObject m_ptr_type;
	};
}