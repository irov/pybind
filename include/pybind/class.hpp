#	pragma once

#	include "pybind/helper.hpp"
#	include "pybind/bases.hpp"

#	include "pybind/class_core.hpp"
#	include "pybind/class_type.hpp"
#	include "pybind/function_parser.hpp"
#	include "pybind/method_adapter.hpp"
#	include "pybind/member_adapter.hpp"
#	include "pybind/repr_adapter.hpp"
#	include "pybind/type_cast.hpp"

#	include <list>

#	include <typeinfo>

namespace pybind
{	
	typedef bases<void,void,void,void,void,void> no_bases;

	template<class C, class B = no_bases>
	class base_
	{
	protected:
		typedef B bases_type;

	public:
		virtual void setup_extract() = 0;

	public:
		base_( const char * _name, pybind_newfunc _pynew, pybind_destructor _pydestructor, PyObject * _module )
		{
			class_type_scope * scope = class_core::create_new_type_scope( class_info<C>(), _name);
			
			setup_bases( scope );

			class_core::setup_new_type_scope( 
				scope, 
				_module, 
				_pynew, 
				_pydestructor 
				);
		}

		template<class BB>
		static void * meta_cast( void * _ptr )
		{
			return static_cast<BB*>(static_cast<C*>(_ptr));
		}

		void setup_bases( class_type_scope * _scope )
		{
			int arity = B::base_arity;

			if( arity-- > 0 )class_core::add_base<typename B::base0>(_scope, &meta_cast<typename B::base0>);
			if( arity-- > 0 )class_core::add_base<typename B::base1>(_scope, &meta_cast<typename B::base1>);
			if( arity-- > 0 )class_core::add_base<typename B::base2>(_scope, &meta_cast<typename B::base2>);
			if( arity-- > 0 )class_core::add_base<typename B::base3>(_scope, &meta_cast<typename B::base3>);
			if( arity-- > 0 )class_core::add_base<typename B::base4>(_scope, &meta_cast<typename B::base4>);
			if( arity-- > 0 )class_core::add_base<typename B::base5>(_scope, &meta_cast<typename B::base5>);
		}

		template<class C0, class C1, class C2, class C3, class C4, class C5>
		base_ & def( const init<C0,C1,C2,C3,C4,C5> & _init )
		{
			constructor * ctr = 
				new constructor_params<C, init<C0,C1,C2,C3,C4,C5> >();

			class_core::def_init( scope(), ctr );

			return *this;
		}

		template<class F>
		base_ & def( const char * _name, F f )
		{			
			method_adapter_interface * ifunc =
				new method_adapter<C, F>(f);

			s_adapterDeleter.add( ifunc );

			typedef typename function_parser<F>::result t_info;

			class_core::def_method(
				_name,
				ifunc,
				t_info::arity,
				class_info<C>()
				);

			return *this;
		}

		template<class F>
		base_ & def_static( const char * _name, F f )
		{			
			method_adapter_interface * ifunc =
				new method_adapter_proxy_function<C, F>(f);

			s_adapterDeleter.add( ifunc );

			typedef typename function_parser<F>::result t_info;

			class_core::def_method(
				_name,
				ifunc,
				t_info::arity,
				class_info<C>()
				);

			return *this;
		}

		template<class P, class F>
		base_ & def_static( const char * _name, P * _proxy, F f )
		{
			method_adapter_interface * iadpter =
				new method_adapter_proxy_member<C, P, F>(_name, _proxy, f);

			s_adapterDeleter.add( iadpter );

			typedef typename function_parser<F>::result t_info;

			class_core::def_method(
				_name,
				iadpter,
				t_info::arity,
				class_info<C>()
				);

			return *this;
		}

		template<class A>
		base_ & def_member( const char * _name, A C:: * a )
		{
			member_adapter_interface * iadpter =
				new member_adapter<C, A>(_name, a);

			s_adapterDeleter.add( iadpter );

			class_core::def_member(
				_name,
				iadpter,
				class_info<C>()
				);

			return *this;
		}

		template<class FG, class FS>
		base_ & def_property( const char * _name, FG _get, FS _set )
		{
			member_adapter_interface * iadpter =
				new member_adapter_property<C, FG, FS>(_get, _set);

			s_adapterDeleter.add( iadpter );

			class_core::def_member(
				_name,
				iadpter,
				class_info<C>()
				);

			return *this;
		}

		template<class FG, class FS>
		base_ & def_property_static( const char * _name, FG _get, FS _set )
		{
			member_adapter_interface * iadpter =
				new member_adapter_property_static<C, FG, FS>( _get, _set);

			s_adapterDeleter.add( iadpter );

			class_core::def_member(
				_name,
				iadpter,
				class_info<C>()
				);

			return *this;
		}

		template<class F>
		base_ & def_repr( F _repr )
		{
			repr_adapter_interface * iadpter =
				new repr_adapter<C, F>( _repr );

			s_adapterDeleter.add( iadpter );

			class_core::def_repr( 
				iadpter, 
				class_info<C>() 
				);

			return *this;
		}

		template<class F>
		base_ & def_getattro( F _attro )
		{
			method_adapter_interface * iadpter =
				new method_adapter<C, F>( _attro );

			s_adapterDeleter.add( iadpter );

			class_core::def_getattro( 
				iadpter, 
				class_info<C>() 
				);

			return *this;
		}

		template<class F>
		base_ & def_getmap( F _map )
		{
			method_adapter_interface * iadpter =
				new method_adapter<C, F>( _map );

			s_adapterDeleter.add( iadpter );

			class_core::def_getmap( 
				iadpter, 
				class_info<C>() 
				);

			return *this;
		}

		static PyObject *
			new_interface( PyTypeObject * _type, PyObject * _args, PyObject * _kwds )
		{
			return detail::alloc_class( _type, _args, 0, class_info<C>() );
		}
		

		static PyObject *
			new_( PyTypeObject * _type, PyObject * _args, PyObject * _kwds )
		{
			C * obj = (C*)class_core::construct( scope(), _args );

			return detail::alloc_class( _type, _args, obj, class_info<C>() );
		}

		static void 
			dealloc_only_python( PyObject * self )
		{
			C * obj = (C*)detail::dealloc_class( self );
		}

		static void
			dealloc_( PyObject * self )
		{
			C * obj = (C*)detail::dealloc_class( self );
			delete obj;
		}

		static class_type_scope * scope()
		{
			return detail::get_class_type_scope( class_info<C>() );
		}

	protected:
		static helper::delete_holder<adapter_interface> s_adapterDeleter;
	};

	template<class C, class B>
	helper::delete_holder<adapter_interface> base_<C,B>::s_adapterDeleter;

	template<class C> 
	struct extract_class_type_ptr
		: public type_cast_result<C *>
	{
		C * apply( PyObject * _obj ) override
		{
			const std::type_info & tinfo = class_info<C>();
			const std::type_info & tptrinfo = class_info<C *>();

			void * impl = type_cast::type_info_cast( _obj, tinfo, tptrinfo );

			return static_cast<C*>(impl);
		}

		PyObject * wrap( C * _class ) override
		{
			return class_core::create_holder( class_info<C>(), (void *)_class );
		}
	};

	template<class C>
	struct extract_class_type_ref
		: public type_cast_result<C>
	{
		C apply( PyObject * _obj ) override
		{
			if( detail::is_class( _obj ) == false )
			{
				if( const char * repr = pybind::object_to_string( _obj ) )
				{
					pybind::error_message( "extract from %.256s"
						, repr
						);
				}

				throw_exception();
			}

			type_cast_result<C>::m_valid = true;

			const std::type_info & tinfo = class_info<C>();

			void * impl = detail::check_registred_class( _obj, tinfo );

			if( impl == 0 )
			{
				if( const char * repr = pybind::object_to_string( _obj ) )
				{
					const char * type_name = tinfo.name();
					pybind::error_message( "extract from %.128s to %.128s"
						, repr
						, type_name
						);
				}

				throw_exception();
			}

			return *static_cast<C*>(impl);
		}

		PyObject * wrap( C _class )
		{
			//return 0;			
			return class_core::create_impl( class_info<C>(), (void *)new C(_class) );
		}
	};

	template<class C, class B = no_bases>
	class class_
		: public base_<C,B>
	{
	public:
		class_( const char * _name, PyObject * _module, bool external_extract = true )
			: base_<C,B>( _name, &base_<C,B>::new_, &base_<C,B>::dealloc_, _module )
		{
			if( external_extract )
			{
				setup_extract();
			}

			constructor * empty_ctr = 
				new constructor_params<C, init<> >();

			class_core::def_init( base_<C,B>::scope(), empty_ctr );
		}

	protected:
		void setup_extract() override
		{
			static extract_class_type_ptr<C> s_registartor_ptr;
			static extract_class_type_ref<C> s_registartor_ref;
		}
	};

	template<class C, class B = no_bases>
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
		proxy_( const char * _name, PyObject * _module, bool external_extract = true )
			: base_<C,B>( _name, &base_<C,B>::new_, &base_<C,B>::dealloc_only_python, _module )
		{
			if( external_extract )
			{
				setup_extract();
			}

			constructor * empty_ctr = 
				new constructor_params<C, init<> >();

			class_core::def_init( base_<C,B>::scope(), empty_ctr );
		}
	};

	template<class C, class B = no_bases>
	class interface_
		: public base_<C,B>
	{
	public:
		typedef extract_class_type_ptr<C> extract_ptr_type;

	public:
		interface_( const char * _name, PyObject * _module, bool external_extract = true )
			: base_<C,B>( _name, &base_<C,B>::new_interface, &base_<C,B>::dealloc_only_python, _module )
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

