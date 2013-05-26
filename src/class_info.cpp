#   include "pybind/class_info.hpp"

#   include "config/config.hpp"

#   include "pybind/system.hpp"
#	include "pybind/exception.hpp"

#   include "static_var.hpp"

#   include <string>

namespace pybind
{   
    //////////////////////////////////////////////////////////////////////////
    struct class_info_desc_t
    {
        size_t id;
        const char * name;
    };
    //////////////////////////////////////////////////////////////////////////
    class_info_desc_t g_class_info_desc[PYBIND_TYPE_COUNT];
    //////////////////////////////////////////////////////////////////////////
    bool initialize_class_info()
    {
        for( size_t index = 0; index != PYBIND_TYPE_COUNT; ++index )
        {
            class_info_desc_t & desc = g_class_info_desc[index];
            
            desc.id = 0;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    namespace detail
    {
        //////////////////////////////////////////////////////////////////////////
        static size_t s_get_next_id()
        {
            static size_t enumerator = 0;

            ++enumerator;

            return enumerator;
        }
        //////////////////////////////////////////////////////////////////////////
        static const class_info_desc_t * s_find_class_info_desc_name( const char * _name )
        {
            for( size_t index = 0; index != PYBIND_TYPE_COUNT; ++index )
            {
                const class_info_desc_t & desc = g_class_info_desc[index];

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
        static const class_info_desc_t * s_find_class_info_desc_id( size_t _id )
        {
            for( size_t index = 0; index != PYBIND_TYPE_COUNT; ++index )
            {
                const class_info_desc_t & desc = g_class_info_desc[index];

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
            for( size_t index = 0; index != PYBIND_TYPE_COUNT; ++index )
            {
                class_info_desc_t & desc = g_class_info_desc[index];

                if( desc.id != 0 )
                {
                    continue;
                }

                return &desc;
            }

            return nullptr;
        }
        //////////////////////////////////////////////////////////////////////////
        size_t get_class_type_id( const std::type_info & _info )
        {
            const char * info_name = _info.name();

            const class_info_desc_t * desc = s_find_class_info_desc_name( info_name );

            if( desc != nullptr )
            {
                size_t id = desc->id;

                return id;
            }
            
            class_info_desc_t * new_desc = s_alloc_class_info_desc();

            if( new_desc == nullptr )
            {
                pybind::throw_exception();

                return 0;
            }

            size_t new_id = s_get_next_id();

            new_desc->id = new_id;            
            new_desc->name = info_name;

            return new_id;
        }
        //////////////////////////////////////////////////////////////////////////
        const char * get_class_type_info( size_t _id )
        {
            const class_info_desc_t * desc = s_find_class_info_desc_id( _id );

            if( desc == nullptr )
            {
                pybind::throw_exception();

                return nullptr;
            }

            const char * name = desc->name;

            return name;
        }
    }
}