#	pragma once

#	include "pybind/extract.hpp"
#	include "pybind/exports.hpp"

namespace pybind
{
	class PYBIND_API constructor
	{
	public:
		constructor( size_t _arity );
		virtual ~constructor();

	public:
		bool valid( PyObject * _args ) const;
		size_t arity() const;

	public:
		virtual void * call( PyObject * _args ) = 0;

	protected:
		size_t m_arity;
	};

	namespace detail
	{
		PyObject * getarg( PyObject * _args, size_t _it );
	}

	template<class C, class P>
	class constructor_params
		: public constructor
	{
	public:
		constructor_params()
			: constructor( P::base_arity )
		{
		}

	public:
		void * call( PyObject * _args ) override
		{
			if( this->valid( _args ) == false )
			{
				return 0;
			}

			return call_ctr<P::base_arity>( _args );
		}

		template<int i>
		static C * call_ctr( PyObject * _args );

		template<>
		static C * call_ctr<0>( PyObject * _args )
		{
			return new C();	
		}

		template<>
		static C * call_ctr<1>( PyObject * _args )
		{
			PyObject * arg0 = detail::getarg( _args, 0 );

			typename P::param0 param0 = extract<typename P::param0>( arg0 );
			
			return new C( param0 );
		}

		template<>
		static C * call_ctr<2>( PyObject * _args )
		{
			PyObject * arg0 = detail::getarg( _args, 0 );
			PyObject * arg1 = detail::getarg( _args, 1 );

			typename P::param0 param0 = extract<typename P::param0>( arg0 );
			typename P::param1 param1 = extract<typename P::param1>( arg1 );

			return new C( param0, param1 );
		}

		template<>
		static C * call_ctr<3>( PyObject * _args )
		{
			PyObject * arg0 = detail::getarg( _args, 0 );
			PyObject * arg1 = detail::getarg( _args, 1 );
			PyObject * arg2 = detail::getarg( _args, 2 );

			typename P::param0 param0 = extract<typename P::param0>( arg0 );
			typename P::param1 param1 = extract<typename P::param1>( arg1 );
			typename P::param2 param2 = extract<typename P::param2>( arg2 );

			return new C( param0, param1, param2 );
		}

		template<>
		static C * call_ctr<4>( PyObject * _args )
		{
			PyObject * arg0 = detail::getarg( _args, 0 );
			PyObject * arg1 = detail::getarg( _args, 1 );
			PyObject * arg2 = detail::getarg( _args, 2 );
			PyObject * arg3 = detail::getarg( _args, 3 );

			typename P::param0 param0 = extract<typename P::param0>( arg0 );
			typename P::param1 param1 = extract<typename P::param1>( arg1 );
			typename P::param2 param2 = extract<typename P::param2>( arg2 );
			typename P::param3 param3 = extract<typename P::param3>( arg3 );

			return new C( param0, param1, param2, param3 );
		}

	};
}