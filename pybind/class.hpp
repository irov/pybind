#	pragma once

#	include "pybind/bases.hpp"

#	include "pybind/class_core.hpp"
#	include "pybind/method_parser.hpp"
#	include "pybind/method_proxy.hpp"
#	include "pybind/type_cast.hpp"

#	include <typeinfo>

namespace pybind
{	
	typedef bases<void,void,void,void> no_bases;

	template<class C, class B = bases<void,void,void,void> >
	class base_
	{
	public:
		typedef B bases_type;

		virtual void setup_extract() = 0;

	public:
		base_( const char * _name, pybind_newfunc _pynew, pybind_destructor _pydestructor, PyObject * _module )
		{
			class_core::create_new_type_scope( 
				class_info<C>(),
				_name, 
				_module, 
				_pynew, 
				_pydestructor 
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

		template<class C0, class C1, class C2, class C3>
		base_ & def( const init<C0,C1,C2,C3> & _init )
		{
			constructor * ctr = 
				new constructor_params<C, init<C0,C1,C2,C3> >();

			class_core::def_init( scope(), ctr );

			return *this;
		}

		template<class F>
		base_ & def( const char * _name, F f )
		{
			typedef typename method_parser<F>::result t_info;

			struct delete_holder
			{
				~delete_holder()
				{
					for each( method_proxy_interface * ptr in m_listProxyMethod )
					{
						delete ptr;
					}
				}

				void add( method_proxy_interface * _ptr )
				{
					m_listProxyMethod.push_back( _ptr );
				}

				typedef std::list<method_proxy_interface *> TListProxyMethod;
				TListProxyMethod m_listProxyMethod;
			};

			static delete_holder s_proxyDeleter;

			method_proxy_interface * ifunc =
				new method_proxy<t_info::class_type, F>(_name, f);

			s_proxyDeleter.add( ifunc );

			class_core::def_method(
				_name,
				ifunc,
				t_info::arity,
				class_info<C>()
				);

			return *this;
		}


		static PyObject *
			new_interface( PyTypeObject * _type, PyObject * _args, PyObject * _kwds )
		{
			return class_core::new_impl( _type, _args, 0, class_info<C>() );
		}
		

		static PyObject *
			new_( PyTypeObject * _type, PyObject * _args, PyObject * _kwds )
		{
			C * obj = (C*)class_core::construct( scope(), _args );

			//list_check( _args );
			//C * obj = new C;/*A::new_<C>();*/

			return class_core::new_impl( _type, _args, obj, class_info<C>() );
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

	template<class C>
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
			//return 0;			
			return class_core::create_impl( class_info<C>(), (void *)new C(_class) );
		}
	};

	template<class C, class B = bases<void,void,void,void> >
	class class_
		: public base_<C,B>
	{
	protected:
		void setup_extract() override
		{
			static extract_class_type_ptr<C> s_registartor_ptr;
			static extract_class_type_ref<C> s_registartor_ref;
		}

	public:
		class_( const char * _name, PyObject * _module = 0 )
			: base_( _name, &base_<C,B>::new_, &base_<C,B>::dealloc_, _module )
		{
			setup_extract();

			constructor * empty_ctr = 
				new constructor_params<C, init<> >();

			class_core::def_init( scope(), empty_ctr );
		}
	};

	template<class C, class B = bases<void,void,void,void> >
	class proxy_
		: public base_<C,B>
	{
	protected:
		void setup_extract() override
		{
			static extract_class_type_ptr<C> s_registartor_ptr;
			static extract_class_type_ref<C> s_registartor_ref;
		}

	public:
		proxy_( const char * _name, PyObject * _module = 0 )
			: base_( _name, &base_<C,B>::new_, &base_<C,B>::dealloc_only_python, _module )
		{
			setup_extract();

			constructor * empty_ctr = 
				new constructor_params<C, init<> >();

			class_core::def_init( scope(), empty_ctr );
		}
	};

	template<class C, class B = bases<void,void,void,void>>
	class interface_
		: public base_<C,B>
	{
	public:
		typedef extract_class_type_ptr<C> extract_ptr_type;

	public:
		interface_( const char * _name, bool external_extract = false, PyObject * _module = 0 )
			: base_( _name, &base_<C,B>::new_interface, &base_<C,B>::dealloc_only_python, _module )
		{
			if( external_extract )
			{
				setup_extract();
			}
		}

	protected:
		void setup_extract() override
		{
			static extract_ptr_type s_registartor_ptr;
		}
	};
}