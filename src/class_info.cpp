#   include "pybind/class_info.hpp"

#   include "config/config.hpp"

#   include "pybind/system.hpp"
#	include "pybind/exception.hpp"

#   include "static_var.hpp"

#   include <string.h>

namespace pybind
{   
    //////////////////////////////////////////////////////////////////////////
    namespace detail
    {
		//////////////////////////////////////////////////////////////////////////
		struct class_info_desc_t
		{
			uint32_t id;
			const char * name;
		};
		//////////////////////////////////////////////////////////////////////////
		static class_info_desc_t & modify_class_info_desc( uint32_t _index )
		{
			static class_info_desc_t g_class_info_desc[PYBIND_TYPE_COUNT];

			class_info_desc_t & desc = g_class_info_desc[_index];

			return desc;
		}
        //////////////////////////////////////////////////////////////////////////
        static uint32_t s_get_next_id()
        {
            static uint32_t enumerator = 0;

            ++enumerator;

            return enumerator;
        }
        //////////////////////////////////////////////////////////////////////////
        static const class_info_desc_t * s_find_class_info_desc_name( const char * _name )
        {
            for( uint32_t index = 0; index != PYBIND_TYPE_COUNT; ++index )
            {
                class_info_desc_t & desc = modify_class_info_desc( index );

                if( desc.id == 0 )
                {
                    continue;
                }

                if( strcmp( desc.name, _name ) != 0 )
                {
                    continue;
                }

                return &desc;
            }

            return nullptr;
        }
        //////////////////////////////////////////////////////////////////////////
        static const class_info_desc_t * s_find_class_info_desc_id( uint32_t _id )
        {
            for( uint32_t index = 0; index != PYBIND_TYPE_COUNT; ++index )
            {
                class_info_desc_t & desc = modify_class_info_desc( index );

                if( desc.id != _id )
                {
                    continue;
                }

                return &desc;
            }

            return nullptr;
        }
        //////////////////////////////////////////////////////////////////////////
        static class_info_desc_t * s_alloc_class_info_desc()
        {
            for( uint32_t index = 0; index != PYBIND_TYPE_COUNT; ++index )
            {
                class_info_desc_t & desc = modify_class_info_desc( index );

                if( desc.id != 0 )
                {
                    continue;
                }

                return &desc;
            }

            return nullptr;
        }
        //////////////////////////////////////////////////////////////////////////
        uint32_t get_class_type_id( const std::type_info & _info )
        {
            const char * info_name = _info.name();

            const class_info_desc_t * desc = s_find_class_info_desc_name( info_name );

            if( desc != nullptr )
            {
                uint32_t id = desc->id;

                return id;
            }
            
            class_info_desc_t * new_desc = s_alloc_class_info_desc();

            if( new_desc == nullptr )
            {
                pybind::throw_exception("invalid create new type info");

                return 0;
            }

            uint32_t new_id = s_get_next_id();

            new_desc->id = new_id;            
            new_desc->name = info_name;

            return new_id;
        }
        //////////////////////////////////////////////////////////////////////////
        const char * get_class_type_info( uint32_t _id )
        {
            const class_info_desc_t * desc = s_find_class_info_desc_id( _id );

            if( desc == nullptr )
            {
                pybind::throw_exception("invalid get class info %d"
					, _id
					);

                return nullptr;
            }

            const char * name = desc->name;

            return name;
        }
    }
	//////////////////////////////////////////////////////////////////////////
	bool initialize_class_info()
	{
		for( uint32_t index = 0; index != PYBIND_TYPE_COUNT; ++index )
		{
			detail::class_info_desc_t & desc = detail::modify_class_info_desc( index );

			desc.id = 0;
			desc.name = nullptr;
		}

		return true;
	}
}