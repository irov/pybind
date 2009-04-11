#	pragma once

#	include <cstddef>

namespace pybind
{
	template<class T>
	struct external_type_cast
	{
		typedef T Type;
	};

	template<class F>
	struct method_parser
	{
		template<class T>
		struct types
		{
			typedef void ret_type;
		};

		template<class R, class C>
		struct types< R (C::*)() >
		{
			typedef R ret_type;
			typedef C class_type;

			static const std::size_t arity = 0;
		};

		template<class R, class C>
		struct types< R (C::*)() const>
		{
			typedef R ret_type;
			typedef C class_type;

			static const std::size_t arity = 0;
		};

		template<class R, class C, class P1>
		struct types< R (C::*)(P1) >
		{
			typedef R ret_type;
			typedef C class_type;

			typedef typename external_type_cast<P1>::Type param1;

			static const std::size_t arity = 1;
		};

		template<class R, class C, class P1>
		struct types< R (C::*)(P1) const >
		{
			typedef R ret_type;
			typedef C class_type;

			typedef typename external_type_cast<P1>::Type param1;

			static const std::size_t arity = 1;
		};

		template<class R, class C, class P1, class P2>
		struct types< R (C::*)(P1, P2) >
		{
			typedef R ret_type;
			typedef C class_type;

			typedef typename external_type_cast<P1>::Type param1;
			typedef typename external_type_cast<P2>::Type param2;

			static const std::size_t arity = 2;
		};

		template<class R, class C, class P1, class P2>
		struct types< R (C::*)(P1, P2) const>
		{
			typedef R ret_type;
			typedef C class_type;

			typedef typename external_type_cast<P1>::Type param1;
			typedef typename external_type_cast<P2>::Type param2;

			static const std::size_t arity = 2;
		};

		template<class R, class C, class P1, class P2, class P3>
		struct types< R (C::*)(P1, P2, P3) >
		{
			typedef R ret_type;
			typedef C class_type;

			typedef typename external_type_cast<P1>::Type param1;
			typedef typename external_type_cast<P2>::Type param2;
			typedef typename external_type_cast<P3>::Type param3;

			static const std::size_t arity = 3;
		};

		template<class R, class C, class P1, class P2, class P3>
		struct types< R (C::*)(P1, P2, P3) const>
		{
			typedef R ret_type;
			typedef C class_type;

			typedef typename external_type_cast<P1>::Type param1;
			typedef typename external_type_cast<P2>::Type param2;
			typedef typename external_type_cast<P3>::Type param3;

			static const std::size_t arity = 3;
		};

		template<class R, class C, class P1, class P2, class P3, class P4>
		struct types< R (C::*)(P1, P2, P3, P4) >
		{
			typedef R ret_type;
			typedef C class_type;

			typedef typename external_type_cast<P1>::Type param1;
			typedef typename external_type_cast<P2>::Type param2;
			typedef typename external_type_cast<P3>::Type param3;
			typedef typename external_type_cast<P4>::Type param4;

			static const std::size_t arity = 4;
		};

		template<class R, class C, class P1, class P2, class P3, class P4>
		struct types< R (C::*)(P1, P2, P3, P4) const>
		{
			typedef R ret_type;
			typedef C class_type;

			typedef typename external_type_cast<P1>::Type param1;
			typedef typename external_type_cast<P2>::Type param2;
			typedef typename external_type_cast<P3>::Type param3;
			typedef typename external_type_cast<P4>::Type param4;

			static const std::size_t arity = 4;
		};

		template<class R, class C, class P1, class P2, class P3, class P4, class P5>
		struct types< R (C::*)(P1, P2, P3, P4, P5) >
		{
			typedef R ret_type;
			typedef C class_type;

			typedef typename external_type_cast<P1>::Type param1;
			typedef typename external_type_cast<P2>::Type param2;
			typedef typename external_type_cast<P3>::Type param3;
			typedef typename external_type_cast<P4>::Type param4;
			typedef typename external_type_cast<P5>::Type param5;

			static const std::size_t arity = 5;
		};

		template<class R, class C, class P1, class P2, class P3, class P4, class P5>
		struct types< R (C::*)(P1, P2, P3, P4, P5) const>
		{
			typedef R ret_type;
			typedef C class_type;

			typedef typename external_type_cast<P1>::Type param1;
			typedef typename external_type_cast<P2>::Type param2;
			typedef typename external_type_cast<P3>::Type param3;
			typedef typename external_type_cast<P4>::Type param4;
			typedef typename external_type_cast<P5>::Type param5;

			static const std::size_t arity = 5;
		};

		typedef types<F> result;
	};
}

