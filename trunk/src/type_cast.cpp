#	include "pybind/type_cast.hpp"

#	include "pybind/class_type_scope.hpp"
#	include "pybind/class_info.hpp"
#	include "pybind/class_core.hpp"
#	include "pybind/convert_adapter.hpp"

#	include "pybind/system.hpp"

#	include "config/python.hpp"


#   include "static_var.hpp"

namespace pybind
{
	namespace detail
	{        
        //////////////////////////////////////////////////////////////////////////
        type_cast_ptr g_type_cast[PYBIND_TYPE_COUNT];
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

			if( const char * repr = pybind::object_repr( _obj ) )
			{          
				pybind::error_message( "invalid extract %s from %.256s"
					, typeinfo_name
					, repr
					);
			}
			else
			{
				pybind::error_message( "invalid extract %s from unknown object type %s"
					, typeinfo_name
					, _obj->ob_type->tp_name
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
				error_message( "instance_of_type: unbind object" );

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
	bool type_cast::type_info_cast( PyObject * _obj, size_t _tinfo, size_t _tptrinfo, void ** _impl )
	{
		if( detail::is_class( _obj ) == false )
		{
			return false;
		}

		void * impl = detail::get_class_impl( _obj );

		if( impl == nullptr )
		{
			error_message( "type_info_cast: unbind object" );
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
