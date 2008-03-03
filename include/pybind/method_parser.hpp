#	pragma once

namespace pybind
{
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

			static const size_t arity = 0;
		};

		template<class R, class C>
		struct types< R (C::*)() const>
		{
			typedef R ret_type;
			typedef C class_type;

			static const size_t arity = 0;
		};

		template<class R, class C, class P1>
		struct types< R (C::*)(P1) >
		{
			typedef R ret_type;
			typedef C class_type;

			typedef P1 param1;

			static const size_t arity = 1;
		};

		template<class R, class C, class P1>
		struct types< R (C::*)(P1) const >
		{
			typedef R ret_type;
			typedef C class_type;

			typedef P1 param1;

			static const size_t arity = 1;
		};

		template<class R, class C, class P1, class P2>
		struct types< R (C::*)(P1, P2) >
		{
			typedef R ret_type;
			typedef C class_type;

			typedef P1 param1;
			typedef P2 param2;

			static const size_t arity = 2;
		};

		template<class R, class C, class P1, class P2>
		struct types< R (C::*)(P1, P2) const>
		{
			typedef R ret_type;
			typedef C class_type;

			typedef P1 param1;
			typedef P2 param2;

			static const size_t arity = 2;
		};

		template<class R, class C, class P1, class P2, class P3>
		struct types< R (C::*)(P1, P2, P3) >
		{
			typedef R ret_type;
			typedef C class_type;

			typedef P1 param1;
			typedef P2 param2;
			typedef P3 param3;

			static const size_t arity = 3;
		};

		template<class R, class C, class P1, class P2, class P3>
		struct types< R (C::*)(P1, P2, P3) const>
		{
			typedef R ret_type;
			typedef C class_type;

			typedef P1 param1;
			typedef P2 param2;
			typedef P3 param3;

			static const size_t arity = 3;
		};

		template<class R, class C, class P1, class P2, class P3, class P4>
		struct types< R (C::*)(P1, P2, P3, P4) >
		{
			typedef R ret_type;
			typedef C class_type;

			typedef P1 param1;
			typedef P2 param2;
			typedef P3 param3;
			typedef P4 param4;

			static const size_t arity = 4;
		};

		template<class R, class C, class P1, class P2, class P3, class P4>
		struct types< R (C::*)(P1, P2, P3, P4) const>
		{
			typedef R ret_type;
			typedef C class_type;

			typedef P1 param1;
			typedef P2 param2;
			typedef P3 param3;
			typedef P4 param4;

			static const size_t arity = 4;
		};

		template<class R, class C, class P1, class P2, class P3, class P4, class P5>
		struct types< R (C::*)(P1, P2, P3, P4, P5) >
		{
			typedef R ret_type;
			typedef C class_type;

			typedef P1 param1;
			typedef P2 param2;
			typedef P3 param3;
			typedef P4 param4;
			typedef P5 param5;

			static const size_t arity = 5;
		};

		template<class R, class C, class P1, class P2, class P3, class P4, class P5>
		struct types< R (C::*)(P1, P2, P3, P4, P5) const>
		{
			typedef R ret_type;
			typedef C class_type;

			typedef P1 param1;
			typedef P2 param2;
			typedef P3 param3;
			typedef P4 param4;
			typedef P5 param5;

			static const size_t arity = 5;
		};

		typedef types<F> result;
	};
}