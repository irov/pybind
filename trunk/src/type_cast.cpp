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
        struct Extract
        {
            Extract()
                : cast(NULL)
                , setup(false)
            {
            }

            type_cast * cast;
            bool setup;
        };
        //////////////////////////////////////////////////////////////////////////
		typedef std::vector<Extract> TExtractTypes;        
        //////////////////////////////////////////////////////////////////////////
        STATIC_DECLARE(TExtractTypes, s_extractTypes);
        //////////////////////////////////////////////////////////////////////////
		void register_type_info_extract( size_t _info, type_cast * _type )
		{	
            TExtractTypes & extractTypes = STATIC_VAR(s_extractTypes);

            size_t last_size = extractTypes.size();

            if( last_size <= _info )
            {
                extractTypes.resize( _info + 1 );
            }

            Extract ex;

            ex.cast = _type;
            ex.setup = true;

			extractTypes[_info] = ex;
		}
        //////////////////////////////////////////////////////////////////////////
		type_cast * find_type_info_extract( size_t _info )
		{
            const TExtractTypes & extractTypes = STATIC_VAR(s_extractTypes);

            size_t size = extractTypes.size();

            if( _info >= size )
            {
                return 0;
            }

            const Extract & ex = extractTypes[_info];

			if( ex.setup == false )
			{
				return 0;
			}

			return ex.cast;
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
			class_type_scope * scope = detail::get_class_type_scope( _tinfo );

			convert_adapter_interface * convert = scope->get_convert();

			if( convert == 0 )
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

			if( impl == 0 )
			{
				error_message( "instance_of_type: unbind object" );
				return false;
			}

			class_type_scope * scope = detail::get_class_scope( _obj->ob_type );
			class_type_scope * cur_scope = detail::get_class_type_scope( _tinfo );

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

		if( impl == 0 )
		{
			error_message( "type_info_cast: unbind object" );
			return false;
		}

		class_type_scope * scope = detail::get_class_scope( _obj->ob_type );
		class_type_scope * cur_scope = detail::get_class_type_scope( _tinfo );

		if( cur_scope != scope )
		{
			impl = class_core::meta_cast( impl, scope, _tptrinfo );
		}

		if( impl == 0 )
		{
			return false;
		}

		*_impl = impl;

		return true;
	}
    //////////////////////////////////////////////////////////////////////////
    void finialize_type_cast()
    {
        detail::TExtractTypes & extractTypes = detail::STATIC_VAR(s_extractTypes);

        for( detail::TExtractTypes::iterator
            it = extractTypes.begin(),
            it_end = extractTypes.end();
        it != it_end;
        ++it )
        {
            detail::Extract & tc = *it;

            delete tc.cast;
        }
    }
}
