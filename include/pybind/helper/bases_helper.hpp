#pragma once

#include <tuple>
#include <cstdint>

namespace pybind
{
    //////////////////////////////////////////////////////////////////////////
    template<class ... Bases>
    class bases
    {
    public:
        static constexpr size_t base_arity = sizeof ... (Bases);

        typedef std::tuple<Bases...> bases_tuple;
    };
    //////////////////////////////////////////////////////////////////////////
    template<class ... Args>
    class init
    {
    public:
        static constexpr size_t args_arity = sizeof ... (Args);

        typedef std::tuple<Args...> args_tuple;
    };
}

