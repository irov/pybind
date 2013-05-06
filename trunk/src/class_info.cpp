#   include "pybind/class_info.hpp"

#   include "pybind/system.hpp"

#   include "static_var.hpp"

#   include <map>
#   include <string>

namespace pybind
{   
    namespace detail
    {
        //////////////////////////////////////////////////////////////////////////
        typedef std::map<std::string, size_t> TMapTypeId;
        typedef std::map<size_t, std::string> TMapTypeInfo;
        //////////////////////////////////////////////////////////////////////////
        STATIC_DECLARE(TMapTypeId, s_mapTypeId);
        STATIC_DECLARE(TMapTypeInfo, s_mapTypeInfo);
        //////////////////////////////////////////////////////////////////////////
        static size_t get_next_id()
        {
            static size_t enumerator = 0;

            ++enumerator;

            return enumerator;
        }
        //////////////////////////////////////////////////////////////////////////
        size_t get_class_type_id( const std::type_info & _info )
        {
            const char * info_name = _info.name();

            TMapTypeId & mtypeId = STATIC_VAR(s_mapTypeId);

            TMapTypeId::iterator it_found = mtypeId.find( info_name );

            if( it_found == mtypeId.end() )
            {
                size_t id = get_next_id();

                TMapTypeInfo & mtypeInfo = STATIC_VAR(s_mapTypeInfo);
                mtypeInfo[id] = info_name;

                it_found = mtypeId.insert( std::make_pair(info_name, id) ).first;
            }

            size_t id = it_found->second;

            return id;
        }
        //////////////////////////////////////////////////////////////////////////
        const char * get_class_type_info( size_t _id )
        {
            const TMapTypeInfo & mtypeInfo = STATIC_VAR(s_mapTypeInfo);
            TMapTypeInfo::const_iterator it_found = mtypeInfo.find(_id);

            if( it_found == mtypeInfo.end() )
            {
                pybind::throw_exception();

                return 0;
            }

            const std::string & str = it_found->second;

            return str.c_str();
        }
    }
}