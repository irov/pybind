#	include "pybind/type_cast.hpp"

#	include "pybind/class_type_scope.hpp"
#	include "pybind/class_info.hpp"
#	include "pybind/class_core.hpp"
#	include "pybind/convert_adapter.hpp"

#	include "pybind/system.hpp"

#	include "config/config.hpp"
#	include "config/python.hpp"

#   include "static_var.hpp"

namespace pybind
{
	namespace detail
	{        
        //////////////////////////////////////////////////////////////////////////
        static type_cast_ptr g_type_cast[PYBIND_TYPE_COUNT];
        //////////////////////////////////////////////////////////////////////////
		void register_type_info_extract( size_t _info, const type_cast_ptr & _cast )
		{	
			g_type_cast[_info] = _cast;
		}
		//////////////////////////////////////////////////////////////////////////
		void unregister_type_info_extract( size_t _info )
		{	
			g_type_cast[_info] = nullptr;
		}
        //////////////////////////////////////////////////////////////////////////
		type_cast * find_type_info_extract( size_t _info )
		{
            const type_cast_ptr & cast = g_type_cast[_info];

            type_cast * t = cast.get();

			return t;
		}
        //////////////////////////////////////////////////////////////////////////
		void error_invalid_extract( PyObject * _obj, size_t _tinfo )
		{
            pybind::check_error();

            const char * typeinfo_name = detail::get_class_type_info( _tinfo );

			const char * obj_repr = pybind::object_repr( _obj );
			const char * obj_repr_type = pybind::object_repr_type( _obj );

			if( obj_repr != nullptr )
			{          
				pybind::error_message( "invalid extract from %.256s type %.256s to %.256s"					
					, obj_repr
					, obj_repr_type
					, typeinfo_name
					);
			}
			else
			{
				pybind::error_message( "invalid extract from unknown object type %s to %s"					
					, _obj->ob_type->tp_name
					, typeinfo_name
					);
			}
		}
        //////////////////////////////////////////////////////////////////////////
		bool convert_object( PyObject * _obj, size_t _tinfo, void * _place )
		{
			const class_type_scope_ptr & scope = detail::get_class_type_scope( _tinfo );

			const convert_adapter_interface_ptr & convert = scope->get_convert();

			if( convert == nullptr )
			{
				return false;
			}

			bool result = convert->convert( _obj, _place );
			
			return result;
		}
		//////////////////////////////////////////////////////////////////////////
		bool instance_of_type( PyObject * _obj, size_t _tinfo )
		{
			if( detail::is_class( _obj ) == false )
			{
				return false;
			}

			void * impl = detail::get_class_impl( _obj );

			if( impl == nullptr )
			{
				pybind::error_message( "instance_of_type: unbind object" );

				return false;
			}

			const class_type_scope_ptr & scope = detail::get_class_scope( _obj->ob_type );
			const class_type_scope_ptr & cur_scope = detail::get_class_type_scope( _tinfo );

			if( scope != cur_scope )
			{
				return false;
			}

			return true;
		}
	}
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
	void type_cast::intrusive_ptr_destroy()
	{
		delete this;
	}
	//////////////////////////////////////////////////////////////////////////
	bool type_cast::type_info_cast( PyObject * _obj, size_t _tinfo, size_t _tptrinfo, void ** _impl )
	{
		if( detail::is_class( _obj ) == false )
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
			impl = class_core::meta_cast( impl, scope, _tptrinfo );
		}

		if( impl == nullptr )
		{
			return false;
		}

		*_impl = impl;

		return true;
	}
    //////////////////////////////////////////////////////////////////////////
    bool initialize_type_cast()
    {
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void finalize_type_cast()
    {     
        for( size_t index = 0; index != PYBIND_TYPE_COUNT; ++index )
        {
            detail::g_type_cast[index] = nullptr;
        }
    }
}
