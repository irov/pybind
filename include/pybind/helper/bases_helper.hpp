#pragma once

namespace pybind
{
	//////////////////////////////////////////////////////////////////////////
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
	//////////////////////////////////////////////////////////////////////////
	template<
		class C0 = void  
		, class C1 = void  
		, class C2 = void  
		, class C3 = void  
		, class C4 = void  
		, class C5 = void  
		, class C6 = void  
		, class C7 = void
		, class C8 = void
		, class C9 = void
		, class C10 = void
		, class C11 = void
		, class C12 = void
		, class C13 = void
		, class C14 = void
		, class C15 = void
	>
	class bases
	{
	public:
		typedef C0 base0;
		typedef C1 base1;
		typedef C2 base2;
		typedef C3 base3;
		typedef C4 base4;
		typedef C5 base5;
		typedef C6 base6;
		typedef C7 base7;
		typedef C8 base8;
		typedef C8 base9;
		typedef C10 base10;
		typedef C11 base11;
		typedef C12 base12;
		typedef C13 base13;
		typedef C14 base14;
		typedef C15 base15;

		static const int base_arity =
			detail::is_void<C0>::result
			+ detail::is_void<C1>::result
			+ detail::is_void<C2>::result 
			+ detail::is_void<C3>::result 
			+ detail::is_void<C4>::result 
			+ detail::is_void<C5>::result
			+ detail::is_void<C6>::result
			+ detail::is_void<C7>::result
			+ detail::is_void<C8>::result
			+ detail::is_void<C9>::result
			+ detail::is_void<C10>::result
			+ detail::is_void<C11>::result
			+ detail::is_void<C12>::result
			+ detail::is_void<C13>::result
			+ detail::is_void<C14>::result
			+ detail::is_void<C15>::result
			; 
	};
	//////////////////////////////////////////////////////////////////////////
	template<
		class C0 = void  
		, class C1 = void  
		, class C2 = void  
		, class C3 = void  
		, class C4 = void  
		, class C5 = void  
		, class C6 = void  
		, class C7 = void
		, class C8 = void
		, class C9 = void
		, class C10 = void
		, class C11 = void
		, class C12 = void
		, class C13 = void
		, class C14 = void
		, class C15 = void
	>
	class init
		: public bases<C0, C1, C2, C3, C4, C5, C6, C7, C8, C9, C10, C11, C12, C13, C14, C15>
	{
	public:
		typedef C0 param0;
		typedef C1 param1;
		typedef C2 param2;
		typedef C3 param3;
		typedef C4 param4;
		typedef C5 param5;
		typedef C6 param6;
		typedef C7 param7;
		typedef C8 param8;
		typedef C9 param9;
		typedef C10 param10;
		typedef C11 param11;
		typedef C12 param12;
		typedef C13 param13;
		typedef C14 param14;
		typedef C15 param15;
	};
}

