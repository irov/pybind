#	pragma once

#	include "pybind/bases.hpp"

#	include "pybind/class_core.hpp"
#	include "pybind/method_parser.hpp"
#	include "pybind/method_proxy.hpp"
#	include "pybind/type_cast.hpp"

#	include <typeinfo>

namespace pybind
{	
	namespace allocator
	{
		struct base_alloc
		{
			template<class C>
			static void * new_()
			{
				return new C;
			}

			template<class C>
			static void * new_( const C & _impl )
			{
				return new C(_impl);
			}
		};

		struct interface
		{
			template<class C>
			static void * new_()
			{
				return 0;
			}

			template<class C>
			static void * new_( const C & _impl )
			{
				return 0;
			}
		};
	}


	template<class C, class B = bases<void,void,void,void>, class A = allocator::base_alloc >
	class base_
	{
	public:
		typedef B bases_type;

		virtual void setup_extract() = 0;

	public:
		base_( const char * _name, PyObject * _module = 0 )
		{
			class_core::create_new_type_scope( 
				class_info<C>(),
				_name, 
				_module, 
				&new_, 
				&dealloc_ 
				);

			setup_bases();
			setup_method_from_base();
		}

		template<class B>
		static void * meta_cast( void * _ptr )
		{
			return static_cast<B*>(static_cast<C*>(_ptr));
		}

		void setup_bases()
		{
			int arity = B::base_arity;

			if( arity-- > 0 )class_core::add_base<C, B::base0>(&meta_cast<B::base0>);
			if( arity-- > 0 )class_core::add_base<C, B::base1>(&meta_cast<B::base1>);
			if( arity-- > 0 )class_core::add_base<C, B::base2>(&meta_cast<B::base2>);
			if( arity-- > 0 )class_core::add_base<C, B::base3>(&meta_cast<B::base3>);
		}

		void setup_method_from_base()
		{
			int arity = B::base_arity;

			if( arity-- > 0 )class_core::add_method_from_base<C, B::base0>();
			if( arity-- > 0 )class_core::add_method_from_base<C, B::base1>();
			if( arity-- > 0 )class_core::add_method_from_base<C, B::base2>();
			if( arity-- > 0 )class_core::add_method_from_base<C, B::base3>();
		}



		template<class F>
		base_ & def( const char * _name, F f )
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

	template<class C> 
	struct extract_class_type_ptr
		: public type_cast_result<C *>
	{
		void apply( PyObject * _obj ) override
		{
			m_valid = true;
			void * impl = detail::get_class( _obj );
			class_type_scope * scope = detail::get_class_scope( _obj );

			const type_info & cur_tinfo = class_info<C>();
			class_type_scope * cur_scope = class_scope::get_class_scope( cur_tinfo );

			void * result = 0;

			if( cur_scope != scope )
			{
				const type_info & tinfo = class_info<C *>();
				const char * name = tinfo.name();
				impl = class_core::meta_cast( impl, scope, name );
			}

			m_result = static_cast<C*>(impl);
		}

		PyObject * wrapp( C * _class )
		{
			return class_core::create_holder( class_info<C>(), (void *)_class );
		}
	};

	template<class C, class A>
	struct extract_class_type_ref
		: public type_cast_result<C>
	{
		void apply( PyObject * _obj ) override
		{
			m_valid = true;
			void * impl = detail::get_class( _obj );
			m_result = *static_cast<C*>(impl);
		}

		PyObject * wrapp( C _class )
		{
			return class_core::create_holder( class_info<C>(), (void *) A::new_<C>( _class ) );
		}
	};

	template<class C, class B = bases<void,void,void,void>, class A = allocator::base_alloc >
	class class_
		: public base_<C,B, A>
	{
	protected:
		void setup_extract() override
		{
			static extract_class_type_ptr<C> s_registartor_ptr;
			static extract_class_type_ref<C,A> s_registartor_ref;
		}

	public:
		class_( const char * _name, PyObject * _module = 0 )
			: base_( _name, _module )
		{
			setup_extract();
		}
	};

	template<class C, class B = bases<void,void,void,void>>
	class interface_
		: public base_<C,B, allocator::interface>
	{
	protected:
		void setup_extract() override
		{
			static extract_class_type_ptr<C> s_registartor_ptr;
		}

	public:
		interface_( const char * _name, PyObject * _module = 0 )
			: base_( _name, _module )
		{
			setup_extract();
		}
	};

}