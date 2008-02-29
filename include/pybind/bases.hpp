#	pragma once

namespace pybind
{
	namespace detail
	{
		template<class C>
		struct is_void
		{
			static const int result = 1; 
		};

		template<>
		struct is_void<void>
		{
			static const int result = 0;
		};
	}


	template<class C0 = void, class C1 = void, class C2 = void, class C3 = void>
	class bases
	{
	public:
		typedef C0 base0;
		typedef C1 base1;
		typedef C2 base2;
		typedef C3 base3;

		static const int base_arity =
			detail::is_void<C0>::result + 
			detail::is_void<C1>::result + 
			detail::is_void<C2>::result + 
			detail::is_void<C3>::result; 
	};


	template<class C0 = void, class C1 = void, class C2 = void, class C3 = void>
	class init
		: public bases<C0,C1,C2,C3>
	{
	public:
		typedef C0 param0;
		typedef C1 param1;
		typedef C2 param2;
		typedef C3 param3;
	};
}