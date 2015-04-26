#	pragma once

namespace pybind
{
	namespace detail
	{
		template<bool C, typename T = void>
		struct enable_if
		{
			typedef T type;
		};

		template<typename T>
		struct enable_if < false, T >
		{
		};

		template<typename, typename>
		struct is_same
		{
			static bool const value = false;
		};

		template<typename A>
		struct is_same < A, A >
		{
			static bool const value = true;
		};

		template<typename B, typename D>
		struct is_base_of
		{
			static D * method();

			static char( &check( B * ) )[1];
			static char( &check( ... ) )[2];
						
			static const bool value = sizeof check( method() ) == 1 && !is_same < volatile const B, volatile const void > ::value;
		};

		template<class T>
		struct remove_ptr
		{
			typedef T type;
		};

		template<class T>
		struct remove_ptr < T * >
		{
			typedef T type;
		};
	}
}
