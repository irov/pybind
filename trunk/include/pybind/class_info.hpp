#   pragma once

#   include "pybind/types.hpp"
#   include "pybind/exports.hpp"

#   include <typeinfo>

namespace pybind
{   
    namespace detail
    {
        PYBIND_API uint32_t get_class_type_id( const std::type_info & _info );
        PYBIND_API const char * get_class_type_info( uint32_t _id );
    }

    template<class T>
    uint32_t class_info()
    {
        static uint32_t type_id = 0;

        if( type_id == 0 )
        {
            const std::type_info & ti = typeid( T );

            uint32_t id = detail::get_class_type_id( ti );

            type_id = id;
        }

        return type_id;
    }

    PYBIND_API bool initialize_class_info();
}