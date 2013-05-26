#   pragma once

#   include "pybind/types.hpp"
#   include "pybind/exports.hpp"

#   include <typeinfo>

namespace pybind
{   
    namespace detail
    {
        PYBIND_API size_t get_class_type_id( const std::type_info & _info );
        PYBIND_API const char * get_class_type_info( size_t _id );
    }

    template<class T>
    size_t class_info()
    {
        static size_t type_id = 0;

        if( type_id == 0 )
        {
            const std::type_info & ti = typeid( T );

            size_t id = detail::get_class_type_id( ti );

            type_id = id;
        }

        return type_id;
    }

    PYBIND_API bool initialize_class_info();
}