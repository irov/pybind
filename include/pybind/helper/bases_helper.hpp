#pragma once

namespace pybind
{
    //////////////////////////////////////////////////////////////////////////
    template<class ... Bases>
    class bases
    {
    public:
        static constexpr uint32_t base_arity = sizeof ... (Bases);

        typedef std::tuple<Bases...> bases_tuple;
    };
    //////////////////////////////////////////////////////////////////////////
    template<class ... Args>
    class init
    {
    public:
        static constexpr uint32_t args_arity = sizeof ... (Args);

        typedef std::tuple<Args...> args_tuple;
    };
}

