#	pragma once

#	include "pybind/method_call.hpp"

namespace pybind
{
	struct py_class_type;

	namespace detail
	{
		PYBIND_API void * class_type_impl( py_class_type * _class_type );
	}

	template<class C, class F>
	class method_proxy
	{
	public:
		static void init( F f )
		{
			m_f = f;
		}

		template<class F, F f>
		struct method0
		{
			static PyObject * 
				call( py_class_type * _self )
			{
				C* impl = (C*)detail::class_type_impl( _self );
	
				PyObject *ret = method_call<C,F>::call( impl, f, 0 );

				return ret;
			}
		}

		template<class F>
		method0<F> def_method0()


		template<class F, F f>
		static PyObject * 
			method1( py_class_type * _self, PyObject * _args )
		{
			C* impl = (C*)detail::class_type_impl( _self );

			PyObject *ret = method_call<C,F>::call( impl, f, _args );

			return ret;
		}

	private:
		static F m_f;
	};

	template<class C, class F>
	F method_proxy<C,F>::m_f;
}