#	pragma once

#	include "pybind/bases.hpp"

#	include "pybind/class_core.hpp"
#	include "pybind/method_parser.hpp"
#	include "pybind/method_proxy.hpp"
#	include "pybind/type_cast.hpp"

namespace pybind
{	
	struct base_alloc
	{
		template<class C>
		static void * new_()
		{
			return new C;
		}
	};

	struct interface
	{
		template<class C>
		static void * new_()
		{
			return 0;
		}
	};
	class Sprite;

	template<class C, class B = bases<void,void,void,void>, class A = base_alloc >
	class class_
	{
		struct extract_class_type
			: public type_cast_result<C *>
		{
			void apply( PyObject * _obj ) override
			{
				m_valid = true;
				m_result = static_cast<C*>(detail::get_class( _obj ));
			}

			PyObject * wrapp( C * _class )
			{
				return class_core::create_holder( class_info<C>(), (void *)_class );
			}
		};

	public:
		class_( const char * _name, PyObject * _module = 0 )
		{
			static extract_class_type s_registartor;

			class_core::create_new_type_scope( 
				class_info<C>(),
				_name, 
				_module, 
				&new_, 
				&dealloc_ 
				);

			int arity = B::base_arity;

			if( arity-- > 0 )class_core::add_method_from_base<C, B::base0>();
			if( arity-- > 0 )class_core::add_method_from_base<C, B::base1>();
			if( arity-- > 0 )class_core::add_method_from_base<C, B::base2>();
			if( arity-- > 0 )class_core::add_method_from_base<C, B::base3>();
		}

		template<class F>
		class_ & def( const char * _name, F f )
		{
			typedef typename method_parser<F>::result t_info;

			method_proxy_interface * ifunc =
				new method_proxy<C,F>(_name, f);

			class_core::def_method(
				_name,
				ifunc,
				t_info::arity,
				class_info<C>()
				);

			return *this;
		}

		static PyObject *
			new_( PyTypeObject * _type, PyObject * _args, PyObject * _kwds )
		{
			return class_core::new_impl( _type, _args, A::new_<C>(), class_info<C>() );
		}

		static void 
			dealloc_only_python( PyObject * self )
		{
			class_core::dealloc_impl( self );
		}

		static void
			dealloc_( PyObject * self )
		{
			C * obj = (C*)class_core::dealloc_impl( self );
			delete obj;
		}

		static class_type_scope * scope()
		{
			return class_scope::get_class_scope( class_info<C>() );
		}
	};
}