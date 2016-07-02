#	include "pybind/type_cast.hpp"

#	include "pybind/class_type_scope.hpp"

#	include "pybind/system.hpp"

#	include "config/config.hpp"

namespace pybind
{
	//////////////////////////////////////////////////////////////////////////
	void * type_cast::operator new ( size_t _size )
	{
		return PYBIND_MALLOC(_size);
	}
	//////////////////////////////////////////////////////////////////////////
	void type_cast::operator delete ( void * _ptr, size_t _size )
	{		
		(void)_size;

		PYBIND_FREE(_ptr, _size);
	}
	//////////////////////////////////////////////////////////////////////////
	void type_cast::intrusive_ptr_destroy( type_cast * _ptr )
	{
		delete _ptr;
	}
	//////////////////////////////////////////////////////////////////////////
	bool type_cast::type_info_cast( PyObject * _obj, uint32_t _tinfo, uint32_t _tptrinfo, void ** _impl )
	{
		kernel_interface * kernel = pybind::get_kernel();

		if( kernel->is_class( _obj ) == false )
		{
			return false;
		}

		if( kernel->is_wrap( _obj ) == false )
		{
			pybind::error_message( "type_info_cast: unwrap object" );

			return false;
		}

		void * impl = kernel->get_class_impl( _obj );

		if( impl == nullptr )
		{
			pybind::error_message( "type_info_cast: unbind object" );

			return false;
		}

		PyTypeObject * py_type = kernel->get_object_type( _obj );
		const class_type_scope_ptr & scope = kernel->get_class_scope( py_type );
		const class_type_scope_ptr & cur_scope = kernel->get_class_type_scope( _tinfo );

		if( cur_scope != scope )
		{
			impl = scope->metacast( _tptrinfo, impl );
		}

		if( impl == nullptr )
		{
			return false;
		}

		*_impl = impl;

		return true;
	}
}
