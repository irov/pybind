#	include "pybind/detail.hpp"

#	include "pybind/class_type_scope_interface.hpp"

#	include "pybind/system.hpp"
#	include "pybind/exception.hpp"

#	include "pybind/pod.hpp"

#	include "config/python.hpp"

namespace pybind
{
	namespace detail
	{
		//////////////////////////////////////////////////////////////////////////
		void * get_class_impl( PyObject * _obj )
		{
			py_base_object * py_base = (py_base_object *)_obj;

			uint32_t flag = py_base->flag;

			if( flag & PY_OBJECT_PTR )
			{
				py_ptr_object * py_ptr = (py_ptr_object *)_obj;

				void * impl = py_ptr->impl;

				return impl;
			}
			else if( flag & PY_OBJECT_POD8 )
			{
				py_pod8_object * py_pod = (py_pod8_object *)_obj;

				void * impl = (void *)py_pod->buff;

				return impl;
			}
			else if( flag & PY_OBJECT_POD16 )
			{
				py_pod16_object * py_pod = (py_pod16_object *)_obj;

				void * impl = (void *)py_pod->buff;

				return impl;
			}
			else if( flag & PY_OBJECT_POD32 )
			{
				py_pod32_object * py_pod = (py_pod32_object *)_obj;

				void * impl = (void *)py_pod->buff;

				return impl;
			}
			else if( flag & PY_OBJECT_POD64 )
			{
				py_pod64_object * py_pod = (py_pod64_object *)_obj;

				void * impl = (void *)py_pod->buff;

				return impl;
			}
			else if( flag & PY_OBJECT_UNWRAP )
			{
				return nullptr;
			}

			pybind::throw_exception( "obj %s not wrap pybind (impl)"
				, pybind::object_str( _obj )
				);

			return nullptr;
		}
		//////////////////////////////////////////////////////////////////////////
		void * unwrap( PyObject * _obj )
		{
			if( _obj == nullptr )
			{
				return nullptr;
			}

			void * impl = get_class_impl( _obj );

			py_base_object * py_base = (py_base_object *)_obj;

			py_base->flag |= PY_OBJECT_UNWRAP;

			return impl;
		}
		//////////////////////////////////////////////////////////////////////////
		bool is_wrap( PyObject * _obj )
		{
			py_base_object * py_base = (py_base_object *)_obj;

			if( py_base->flag & PY_OBJECT_UNWRAP )
			{
				return false;
			}

			return true;
		}
		//////////////////////////////////////////////////////////////////////////
		void * meta_cast_scope( void * _self, uint32_t _typeId, uint32_t _name, const class_type_scope_interface_ptr & _scope )
		{
			uint32_t class_name = _scope->get_type_id();

			if( class_name == _typeId )
			{
				return _self;
			}

			void * impl_cast = _scope->metacast( _name, _self );

			return impl_cast;
		}
	}
}