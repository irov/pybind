#   include "pybind/class_info.hpp"

#   include <map>
#   include <string>

namespace pybind
{   
    namespace detail
    {
        typedef std::map<std::string, size_t> TMapTypeId;
        typedef std::map<size_t, std::string> TMapTypeInfo;

        static TMapTypeId & get_map_type_id()
        {
            static TMapTypeId s_mapTypeId;

            return s_mapTypeId;
        }

        static TMapTypeInfo & get_map_type_info()
        {
            static TMapTypeInfo s_mapTypeInfo;

            return s_mapTypeInfo;
        }

        static size_t get_next_id()
        {
            static size_t enumerator = 0;

            ++enumerator;

            return enumerator;
        }

        size_t get_class_type_id( const std::type_info & _info )
        {
            TMapTypeId & mapTypeId = get_map_type_id();

            const char * info_name = _info.name();

            TMapTypeId::iterator it_found = mapTypeId.find( info_name );

            if( it_found == mapTypeId.end() )
            {
                size_t id = get_next_id();

                TMapTypeInfo & mapTypeInfo = get_map_type_info();

                mapTypeInfo[id] = info_name;

                it_found = mapTypeId.insert( std::make_pair(info_name, id) ).first;
            }

            size_t id = it_found->second;

            return id;
        }

        const char * get_class_type_info( size_t _id )
        {
            TMapTypeInfo & mapTypeInfo = get_map_type_info();

            const std::string & str = mapTypeInfo[_id];

            return str.c_str();
        }
    }
}