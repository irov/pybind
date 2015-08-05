#	include "pybind/type_cast.hpp"

#	include "pybind/class_type_scope.hpp"

#	include "pybind/system.hpp"
#	include "pybind/detail.hpp"

#	include "pod.hpp"

#	include "config/config.hpp"
#	include "config/python.hpp"

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
		if( detail::is_class( _obj ) == false )
		{
			return false;
		}

		if( detail::is_wrap( _obj ) == false )
		{
			return false;
		}

		void * impl = detail::get_class_impl( _obj );

		if( impl == nullptr )
		{
			pybind::error_message( "type_info_cast: unbind object" );

			return false;
		}

		const class_type_scope_ptr & scope = detail::get_class_scope( _obj->ob_type );
		const class_type_scope_ptr & cur_scope = detail::get_class_type_scope( _tinfo );

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
