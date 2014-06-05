#	pragma once

#	include "pybind/constructor.hpp"

namespace pybind
{
	template<class C, class P, int i>
	struct call_constructor_new_impl
	{
		static C * call( PyObject * _obj, PyObject * _args );
	};

	template<class C, class P>
	struct call_constructor_new_impl<C, P, 0>
	{
		static C * call( PyObject * _args )
		{
            (void)_args;

			return new C();	
		}
	};

	template<class C, class P>
	struct call_constructor_new_impl<C, P, 1>
	{
		static C * call( PyObject * _args )
		{
			PyObject * arg0 = tuple_getitem( _args, 0 );

			typename P::param0 param0 = extract<typename P::param0>( arg0 );

			return new C( param0 );
		}
	};

	template<class C, class P>
	struct call_constructor_new_impl<C, P, 2>
	{
		static C * call( PyObject * _args )
		{
			PyObject * arg0 = tuple_getitem( _args, 0 );
			PyObject * arg1 = tuple_getitem( _args, 1 );

			typename P::param0 param0 = extract<typename P::param0>( arg0 );
			typename P::param1 param1 = extract<typename P::param1>( arg1 );

			return new C( param0, param1 );
		}
	};

	template<class C, class P>
	struct call_constructor_new_impl<C, P, 3>
	{
		static C * call( PyObject * _args )
		{
			PyObject * arg0 = tuple_getitem( _args, 0 );
			PyObject * arg1 = tuple_getitem( _args, 1 );
			PyObject * arg2 = tuple_getitem( _args, 2 );

			typename P::param0 param0 = extract<typename P::param0>( arg0 );
			typename P::param1 param1 = extract<typename P::param1>( arg1 );
			typename P::param2 param2 = extract<typename P::param2>( arg2 );

			return new C( param0, param1, param2 );
		}
	};

	template<class C, class P>
	struct call_constructor_new_impl<C, P, 4>
	{
		static C * call( PyObject * _args )
		{
			PyObject * arg0 = tuple_getitem( _args, 0 );
			PyObject * arg1 = tuple_getitem( _args, 1 );
			PyObject * arg2 = tuple_getitem( _args, 2 );
			PyObject * arg3 = tuple_getitem( _args, 3 );

			typename P::param0 param0 = extract<typename P::param0>( arg0 );
			typename P::param1 param1 = extract<typename P::param1>( arg1 );
			typename P::param2 param2 = extract<typename P::param2>( arg2 );
			typename P::param3 param3 = extract<typename P::param3>( arg3 );

			return new C( param0, param1, param2, param3 );
		}
	};

	template<class C, class P>
	class constructor_new
		: public constructor_adapter_interface
	{
	public:
		constructor_new()
			: constructor_adapter_interface( P::base_arity )
		{
		}

	public:
		void * call( PyObject * _obj, PyObject * _args ) override
		{
            (void)_obj;

			if( this->valid( _args ) == false )
			{
				return nullptr;
			}

			void * impl = call_constructor_new_impl<C, P, P::base_arity>::call( _args );

			return impl;
		}
	};
}

