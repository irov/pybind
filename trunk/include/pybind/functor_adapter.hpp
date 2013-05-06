#	pragma once

#   include "pybind/adapter_interface.hpp"

#	include "pybind/method_call.hpp"

namespace pybind
{
	class functor_adapter_interface
        : public adapter_interface
	{
    public:
        virtual int getArity() const = 0;
        virtual const char * getName() const = 0;

	public:
		virtual PyObject * call( PyObject * _args, PyObject * _kwds ) = 0;
	};

	namespace detail
	{
		PYBIND_API functor_adapter_interface * extract_from_py_functor( PyObject * _self );
	}

	template<class C, class F>
	class functor_proxy_adapter
		: public functor_adapter_interface
	{
		typedef typename function_parser<F>::result f_info;

	public:
		functor_proxy_adapter( C * _self, F f, const char * _tag )
			: m_self(_self)
			, m_fn(f)
            , m_tag(_tag)
		{
		}

    public:
        size_t getArity() const override
        {
            return f_info::arity;
        }

        const char * getName() const override
        {
            return m_tag;
        }

	public:
		PyObject * call( PyObject * _args, PyObject * _kwds ) override
		{
            (void)_kwds;

			PyObject *ret = method_call<C,F>::call( m_self, m_fn, _args, m_tag );
			
			return ret;	
		}

	protected:
		C * m_self;
		F m_fn;
        const char * m_tag;
	};
}

