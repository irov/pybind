#	pragma once

#	include "pybind/constructor_adapter.hpp"

namespace pybind
{
	template<class C, class P, int i>
	struct call_constructor_placement_impl
	{
		static C * call( void * _impl, PyObject * _args );
	};

	template<class C, class P>
	struct call_constructor_placement_impl<C, P, 0>
	{
		static C * call( void * _impl, PyObject * _args )
		{
			return new (_impl) C();	
		}
	};

	template<class C, class P>
	struct call_constructor_placement_impl<C, P, 1>
	{
		static C * call( void * _impl, PyObject * _args )
		{
			return new (_impl)C( 
				tuple_getitem_t( _args, 0 )
				);
		}
	};

	template<class C, class P>
	struct call_constructor_placement_impl<C, P, 2>
	{
		static C * call( void * _impl, PyObject * _args )
		{
			return new (_impl)C( 
				tuple_getitem_t( _args, 0 ),
				tuple_getitem_t( _args, 1 )
				);
		}
	};

	template<class C, class P>
	struct call_constructor_placement_impl<C, P, 3>
	{
		static C * call( void * _impl, PyObject * _args )
		{
			return new (_impl)C( 
				tuple_getitem_t( _args, 0 ),
				tuple_getitem_t( _args, 1 ),
				tuple_getitem_t( _args, 2 )
				);
		}
	};

	template<class C, class P>
	struct call_constructor_placement_impl<C, P, 4>
	{
		static C * call( void * _impl, PyObject * _args )
		{
			return new (_impl)C( 
				tuple_getitem_t( _args, 0 ),
				tuple_getitem_t( _args, 1 ),
				tuple_getitem_t( _args, 2 ),
				tuple_getitem_t( _args, 3 )
				);
		}
	};

	template<class C, class P>
	class constructor_placement
		: public constructor_adapter_interface
	{
	public:
		constructor_placement()
			: constructor_adapter_interface( P::base_arity )
		{
		}

	public:
		void * call( PyObject * _obj, PyObject * _args ) override
		{
			if( this->valid( _args ) == false )
			{
				return nullptr;
			}

            void * impl = pybind::detail::get_class_impl( _obj );

			if( impl == nullptr )
			{
				return nullptr;
			}

            C * self = static_cast<C *>(impl);

			C * obj = call_constructor_placement_impl<C, P, P::base_arity>::call( self,  _args );

			return obj;
		}
	};
}

