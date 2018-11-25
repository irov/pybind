#pragma once

namespace pybind
{
    //////////////////////////////////////////////////////////////////////////
    namespace detail
    {
        template<class C>
        struct is_void
        {
            static constexpr uint32_t result = 1;
        };

        template<>
        struct is_void<void>
        {
            static constexpr uint32_t result = 0;
        };

        template<class T>
        constexpr void accumulate_base_arity( uint32_t & _arity )
        {
            typedef typename std::conditional<std::is_void<T>::value
                , typename std::integral_constant<uint32_t, 0>
                , typename std::integral_constant<uint32_t, 1>>::type increment_value_type;

            _arity += increment_value_type::value;
        }

        template<class ... T>
        constexpr uint32_t get_base_arity()
        {
            uint32_t arity = 0;

            (accumulate_base_arity<T>( arity ), ...);

            return arity;
        }
    }
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

