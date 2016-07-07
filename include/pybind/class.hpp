#	pragma once

#	include "pybind/bases.hpp"

#	include "pybind/class_type_scope.hpp"

#	include "pybind/new_adapter.hpp"
#	include "pybind/destroy_adapter.hpp"
#	include "pybind/method_adapter.hpp"
#	include "pybind/member_adapter.hpp"
#	include "pybind/convert_adapter.hpp"
#   include "pybind/compare_adapter.hpp"
#	include "pybind/repr_adapter.hpp"
#	include "pybind/hash_adapter.hpp"
#	include "pybind/mapping_adapter.hpp"
#	include "pybind/getattro_adapter.hpp"
#	include "pybind/sequence_get_adapter.hpp"
#	include "pybind/sequence_set_adapter.hpp"
#	include "pybind/number_binary_adapter.hpp"
#	include "pybind/smart_pointer_adapter.hpp"

#	include "pybind/type_cast_result.hpp"
#	include "pybind/exception.hpp"

#   include "pybind/constructor_new.hpp"
#   include "pybind/constructor_placement.hpp"

namespace pybind
{
	typedef bases<void, void, void, void, void, void, void, void, void, void, void, void, void, void, void> no_bases;

	template<class C, class B = no_bases>
	class base_
	{
	protected:
		typedef B bases_type;

	public:
		base_( const char * _name, void * _user, const new_adapter_interface_ptr & _pynew, const destroy_adapter_interface_ptr & _pydestructor, size_t _size, PyObject * _module )
		{
			kernel_interface * kernel = pybind::get_kernel();

			uint32_t pod = 0;

			if( _size <= PYBIND_OBJECT_POD_SIZE )
			{
				pod = (uint32_t)_size;
			}

			uint32_t info = kernel->class_info<C>();

			m_scope = kernel->create_new_type_scope( info, _name, _user, _pynew, _pydestructor, pod );

			this->setup_bases();

			m_scope->initialize( _module );
		}

		template<class BB>
		static void * meta_cast( void * _ptr )
		{
			C * c = static_cast<C *>(_ptr);
			BB * bb = static_cast<BB *>(c);

			return bb;
		}

		void setup_bases()
		{
			int arity = B::base_arity;

			if( arity-- > 0 )m_scope->add_base_t<typename B::base0>( &meta_cast<typename B::base0> );
			if( arity-- > 0 )m_scope->add_base_t<typename B::base1>( &meta_cast<typename B::base1> );
			if( arity-- > 0 )m_scope->add_base_t<typename B::base2>( &meta_cast<typename B::base2> );
			if( arity-- > 0 )m_scope->add_base_t<typename B::base3>( &meta_cast<typename B::base3> );
			if( arity-- > 0 )m_scope->add_base_t<typename B::base4>( &meta_cast<typename B::base4> );
			if( arity-- > 0 )m_scope->add_base_t<typename B::base5>( &meta_cast<typename B::base5> );
			if( arity-- > 0 )m_scope->add_base_t<typename B::base6>( &meta_cast<typename B::base6> );
			if( arity-- > 0 )m_scope->add_base_t<typename B::base7>( &meta_cast<typename B::base7> );
			if( arity-- > 0 )m_scope->add_base_t<typename B::base8>( &meta_cast<typename B::base8> );
			if( arity-- > 0 )m_scope->add_base_t<typename B::base9>( &meta_cast<typename B::base9> );
			if( arity-- > 0 )m_scope->add_base_t<typename B::base10>( &meta_cast<typename B::base10> );
			if( arity-- > 0 )m_scope->add_base_t<typename B::base11>( &meta_cast<typename B::base11> );
			if( arity-- > 0 )m_scope->add_base_t<typename B::base12>( &meta_cast<typename B::base12> );
			if( arity-- > 0 )m_scope->add_base_t<typename B::base13>( &meta_cast<typename B::base13> );
			if( arity-- > 0 )m_scope->add_base_t<typename B::base14>( &meta_cast<typename B::base14> );
			if( arity-- > 0 )m_scope->add_base_t<typename B::base15>( &meta_cast<typename B::base15> );
		}

		template<class F>
		base_ & def( const char * _name, F f )
		{
			method_adapter_interface_ptr iadapter =
				new method_adapter<C, F>( _name, f );

			m_scope->add_method( iadapter );

			return *this;
		}

		template<class F>
		base_ & def_args( const char * _name, F f )
		{
			method_adapter_interface_ptr iadapter =
				new method_adapter_args<C, F>( _name, f );

			m_scope->add_method( iadapter );

			return *this;
		}

		template<class F>
		base_ & def_depricated( const char * _name, F f, const char * _doc )
		{
			method_adapter_interface_ptr iadapter =
				new method_adapter_depricated<C, F>( _name, f, _doc );

			m_scope->add_method( iadapter );

			return *this;
		}

		template<class F>
		base_ & def_native( const char * _name, F f )
		{
			method_adapter_interface_ptr iadapter =
				new method_adapter_native<C, F>( _name, f );

			m_scope->add_method( iadapter );

			return *this;
		}

		template<class F>
		base_ & def_convert( F f, void * _user )
		{
			convert_adapter_interface_ptr iadapter =
				new convert_adapter<C, F>( f, _user );

			m_scope->set_convert( iadapter );

			return *this;
		}

		template<class F>
		base_ & def_static( const char * _name, F f )
		{
			method_adapter_interface_ptr iadapter =
				new method_adapter_proxy_function<C, F>( _name, f );

			m_scope->add_method( iadapter );

			return *this;
		}

		template<class P, class F>
		base_ & def_proxy_static( const char * _name, P * _proxy, F f )
		{
			method_adapter_interface_ptr iadapter =
				new method_adapter_proxy<C, P, F>( _name, f, _proxy );

			m_scope->add_method( iadapter );

			return *this;
		}

		template<class P, class F>
		base_ & def_proxy_args_static( const char * _name, P * _proxy, F f )
		{
			method_adapter_interface_ptr iadapter =
				new method_adapter_proxy_args<C, P, F>( _name, f, _proxy );

			m_scope->add_method( iadapter );

			return *this;
		}

		template<class P, class F>
		base_ & def_proxy_native( const char * _name, P * _proxy, F f )
		{
			method_adapter_interface_ptr iadapter =
				new method_adapter_proxy_native<C, P, F>( _name, f, _proxy );

			m_scope->add_method( iadapter );

			return *this;
		}

		template<class A>
		base_ & def_member( const char * _name, A C:: * a )
		{
			member_adapter_interface_ptr iadapter =
				new member_adapter<C, A>( _name, a );

			m_scope->add_member( iadapter );

			return *this;
		}

		template<class FG, class FS>
		base_ & def_property( const char * _name, FG _get, FS _set )
		{
			member_adapter_interface_ptr iadapter =
				new member_adapter_property<C, FG, FS>( _name, _get, _set );

			m_scope->add_member( iadapter );

			return *this;
		}

		template<class FG, class FS>
		base_ & def_property_static( const char * _name, FG _get, FS _set )
		{
			member_adapter_interface_ptr iadapter =
				new member_adapter_property_static<C, FG, FS>( _name, _get, _set );

			m_scope->add_member( iadapter );

			return *this;
		}

		template<class F>
		base_ & def_call( F _fn )
		{
			method_adapter_interface_ptr iadapter =
				new method_adapter<C, F>( "__call__", _fn );

			m_scope->set_call( iadapter );

			return *this;
		}

		template<class F>
		base_ & def_repr( F _fn )
		{
			repr_adapter_interface_ptr iadapter =
				new repr_adapter<C, F>( _fn );

			m_scope->set_repr( iadapter );

			return *this;
		}

		template<class F>
		base_ & def_hash( F _fn )
		{
			hash_adapter_interface_ptr iadapter =
				new hash_adapter<C, F>( _fn );

			m_scope->set_hash( iadapter );

			return *this;
		}

		template<class F>
		base_ & def_compare( F _fn )
		{
			compare_adapter_interface_ptr iadapter =
				new compare_adapter<C, F>( _fn );

			m_scope->set_compare( iadapter );

			return *this;
		}

		template<class F>
		base_ & def_getattro( F _fn )
		{
			getattro_adapter_interface_ptr iadapter =
				new getattro_adapter<C, F>( "getattro", _fn );

			m_scope->set_getattro( iadapter );

			return *this;
		}

		template<class F>
		base_ & def_mapping( F _fn )
		{
			mapping_adapter_interface_ptr iadapter =
				new mapping_adapter<C, F>( "getmap", _fn );

			m_scope->set_mapping( iadapter );

			return *this;
		}

		template<class F>
		base_ & def_sequence_get( F _fn )
		{
			sequence_get_adapter_interface_ptr iadapter =
				new sequence_get_adapter<C, F>( "sequence_get", _fn );

			m_scope->set_sequence_get( iadapter );

			return *this;
		}

		template<class F>
		base_ & def_sequence_set( F _fn )
		{
			sequence_set_adapter_interface_ptr iadapter =
				new sequence_set_adapter<C, F>( "sequence_set", _fn );

			m_scope->set_sequence_set( iadapter );

			return *this;
		}

		template<class F>
		base_ & def_static_sequence_get( F _fn )
		{
			sequence_get_adapter_interface_ptr iadapter =
				new sequence_get_adapter_proxy_function<C, F>( "static_sequence_get", _fn );

			m_scope->set_sequence_get( iadapter );

			return *this;
		}

		template<class F>
		base_ & def_static_sequence_set( F _fn )
		{
			sequence_set_adapter_interface_ptr iadapter =
				new sequence_set_adapter_proxy_function<C, F>( "static_sequence_set", _fn );

			m_scope->set_sequence_set( iadapter );

			return *this;
		}
				
		base_ & def_operator_getset()
		{
			sequence_set_adapter_interface_ptr iadapter_set =
				new sequence_set_adapter_operator<C>( "operator_set" );

			m_scope->set_sequence_set( iadapter_set );

			sequence_get_adapter_interface_ptr iadapter_get =
				new sequence_get_adapter_operator<C>( "operator_get" );

			m_scope->set_sequence_get( iadapter_get );

			return *this;
		}

		template<class F>
		base_ & def_add( F _fn )
		{
			number_binary_adapter_interface_ptr iadapter =
				new number_binary_adapter<C, F>( "add", _fn );

			m_scope->set_number_add( iadapter );

			return *this;
		}

		template<class F>
		base_ & def_static_add( F _fn )
		{
			number_binary_adapter_interface_ptr iadapter =
				new number_binary_adapter_proxy_function<C, F>( "static_add", _fn );

			m_scope->set_number_add( iadapter );

			return *this;
		}

		template<class P, class F>
		base_ & def_proxy_static_add( P * _proxy, F f )
		{
			number_binary_adapter_interface_ptr iadapter =
				new number_binary_adapter_proxy<C, P, F>( "proxy_static_add", f, _proxy );

			m_scope->set_number_add( iadapter );

			return *this;
		}

		template<class V, class VC>
		base_ & def_operator_add_t()
		{
			number_binary_adapter_interface_ptr iadapter =
				new number_binary_adapter_operator_add<C, V>( "operator_add" );

			kernel_interface * kernel = pybind::get_kernel();

			uint32_t typeId = kernel->class_info<VC>();

			m_scope->add_number_add( typeId, iadapter );

			return *this;
		}

		template<class V>
		base_ & def_operator_add_t()
		{
			return def_operator_add_t<V, V>();
		}

		base_ & def_operator_add()
		{
			number_binary_adapter_interface_ptr iadapter =
				new number_binary_adapter_operator_add<C, C>( "operator_add" );

			m_scope->set_number_add( iadapter );

			return *this;
		}

		template<class F>
		base_ & def_sub( F _fn )
		{
			number_binary_adapter_interface_ptr iadapter =
				new number_binary_adapter<C, F>( "sub", _fn );

			m_scope->set_number_sub( iadapter );

			return *this;
		}

		template<class F>
		base_ & def_static_sub( F _fn )
		{
			number_binary_adapter_interface_ptr iadapter =
				new number_binary_adapter_proxy_function<C, F>( "static_sub", _fn );

			m_scope->set_number_sub( iadapter );

			return *this;
		}

		template<class V, class VC>
		base_ & def_operator_sub_t()
		{
			number_binary_adapter_interface_ptr iadapter =
				new number_binary_adapter_operator_sub<C, V>( "operator_sub" );

			kernel_interface * kernel = pybind::get_kernel();

			uint32_t typeId = kernel->class_info<VC>();

			m_scope->add_number_sub( typeId, iadapter );

			return *this;
		}

		template<class V>
		base_ & def_operator_sub_t()
		{
			return def_operator_sub_t<V, V>();
		}

		base_ & def_operator_sub()
		{
			number_binary_adapter_interface_ptr iadapter =
				new number_binary_adapter_operator_sub<C, C>( "operator_sub" );

			m_scope->set_number_sub( iadapter );

			return *this;
		}

		template<class P, class F>
		base_ & def_proxy_static_sub( P * _proxy, F f )
		{
			number_binary_adapter_interface_ptr iadapter =
				new number_binary_adapter_proxy<C, P, F>( "proxy_static_sub", f, _proxy );

			m_scope->set_number_sub( iadapter );

			return *this;
		}

		template<class F>
		base_ & def_mul( F _fn )
		{
			number_binary_adapter_interface_ptr iadapter =
				new number_binary_adapter<C, F>( "mul", _fn );

			m_scope->set_number_mul( iadapter );

			return *this;
		}

		template<class F>
		base_ & def_static_mul( F _fn )
		{
			number_binary_adapter_interface_ptr iadapter =
				new number_binary_adapter_proxy_function<C, F>( "static_mul", _fn );

			m_scope->set_number_mul( iadapter );

			return *this;
		}

		template<class P, class F>
		base_ & def_proxy_static_mul( P * _proxy, F f )
		{
			number_binary_adapter_interface_ptr iadapter =
				new number_binary_adapter_proxy<C, P, F>( "proxy_static_mul", f, _proxy );

			m_scope->set_number_mul( iadapter );

			return *this;
		}

		template<class V, class VC>
		base_ & def_operator_mul_t()
		{
			number_binary_adapter_interface_ptr iadapter =
				new number_binary_adapter_operator_mul<C, V>( "operator_mul" );
						
			kernel_interface * kernel = pybind::get_kernel();

			uint32_t typeId = kernel->class_info<VC>();

			m_scope->add_number_mul( typeId, iadapter );

			return *this;
		}

		template<class V>
		base_ & def_operator_mul_t()
		{
			return def_operator_mul_t<V, V>();
		}

		base_ & def_operator_mul()
		{
			number_binary_adapter_interface_ptr iadapter =
				new number_binary_adapter_operator_mul<C, C>( "operator_mul" );

			m_scope->set_number_mul( iadapter );

			return *this;
		}

		template<class F>
		base_ & def_div( F _fn )
		{
			number_binary_adapter_interface_ptr iadapter =
				new number_binary_adapter<C, F>( "div", _fn );

			m_scope->set_number_div( iadapter );

			return *this;
		}

		template<class F>
		base_ & def_static_div( F _fn )
		{
			number_binary_adapter_interface_ptr iadapter =
				new number_binary_adapter_proxy_function<C, F>( "static_div", _fn );

			m_scope->set_number_div( iadapter );

			return *this;
		}

		template<class P, class F>
		base_ & def_proxy_static_div( P * _proxy, F f )
		{
			number_binary_adapter_interface_ptr iadapter =
				new number_binary_adapter_proxy<C, P, F>( "proxy_static_div", f, _proxy );

			m_scope->set_number_div( iadapter );

			return *this;
		}

		template<class V, class VC>
		base_ & def_operator_div_t()
		{
			number_binary_adapter_interface_ptr iadapter =
				new number_binary_adapter_operator_div_nr<C, V>( "operator_div_nr" );

			kernel_interface * kernel = pybind::get_kernel();

			uint32_t typeId = kernel->class_info<VC>();

			m_scope->add_number_div( typeId, iadapter );

			return *this;
		}

		template<class V>
		base_ & def_operator_div_t()
		{
			return def_operator_div_t<V, V>();
		}

		base_ & def_operator_div()
		{
			number_binary_adapter_interface_ptr iadapter =
				new number_binary_adapter_operator_div<C, C>( "operator_div" );

			m_scope->set_number_div( iadapter );

			return *this;
		}

		template<class FI, class FD>
		base_ & def_static_proxy_smart_pointer( FI _incref, FD _decref )
		{
			smart_pointer_adapter_interface_ptr iadapter =
				new smart_pointer_adapter_proxy_function<C, FI, FD>( "smart_pointer", _incref, _decref );

			m_scope->set_smart_pointer( iadapter );

			return *this;
		}
		
		base_ & def_smart_pointer()
		{
			return this->def_static_proxy_smart_pointer( &C::intrusive_ptr_add_ref, &C::intrusive_ptr_dec_ref );
		}

		base_ & def_as_string()
		{
			m_scope->set_as_string();

			return *this;
		}

		const pybind::class_type_scope_ptr & get_scope() const
		{
			return m_scope;
		}

	protected:
		void setup_extract( const type_cast_ptr & _type )
		{
			pybind::registration_type_cast<C>(_type);
		}

	protected:
		class_type_scope_ptr m_scope;
	};

	class class_new_invalid
		: public new_adapter_interface
	{
	public:
		void * call( const pybind::class_type_scope_ptr & _scope, PyObject * _obj, PyObject * _args, PyObject * _kwds ) override
		{
			(void)_scope;
			(void)_obj;
			(void)_args;
			(void)_kwds;

			pybind::throw_exception( "scope %s not support 'new_interface'"
				, _scope->get_name()
				);

			return nullptr;
		}
	};

	template<class C>
	class class_destroy_delete
		: public destroy_adapter_interface
	{
	public:
		void call( const pybind::class_type_scope_ptr & _scope, void * _impl ) override
		{
			(void)_scope;

			C * obj = static_cast<C*>(_impl);

			delete obj;
		}
	};

	template<class C>
	class class_destroy_destructor
		: public destroy_adapter_interface
	{
	public:
		void call( const pybind::class_type_scope_ptr & _scope, void * _impl ) override
		{
			(void)_scope;

			C * obj = static_cast<C*>(_impl);

			(void)obj;

			obj->~C();
		}
	};

	template<class C>
	struct extract_class_type_ptr
		: public type_cast_result<C *>
	{
		bool apply( PyObject * _obj, typename type_cast_result<C *>::TCastValue _value, bool _nothrow ) override
		{
			if( pybind::is_none( _obj ) == true )
			{
				_value = nullptr;

				return true;
			}

			kernel_interface * kernel = pybind::get_kernel();

			uint32_t tinfo = kernel->class_info<C>();
			uint32_t tptrinfo = kernel->class_info<C *>();

			void * impl;
			if( this->type_info_cast( _obj, tinfo, tptrinfo, &impl ) == false )
			{
				if( _nothrow == false )
				{
					kernel->error_invalid_extract( _obj, tinfo );
				}

				_value = nullptr;

				return false;
			}

			_value = static_cast<C*>(impl);

			return true;
		}

		PyObject * wrap( typename type_cast_result<C *>::TCastRef _value ) override
		{
			if( _value == nullptr )
			{
				return pybind::ret_none();
			}

			kernel_interface * kernel = pybind::get_kernel();

			const class_type_scope_ptr & scope = kernel->class_scope<C>();

			PyObject * py_obj = scope->create_class( (void *)_value );

			return py_obj;
		}
	};

	template<class C>
	struct extract_holder_type_ptr
		: public type_cast_result<C *>
	{
		bool apply( PyObject * _obj, typename type_cast_result<C *>::TCastValue _value, bool _nothrow ) override
		{
			if( pybind::is_none( _obj ) == true )
			{
				_value = nullptr;

				return true;
			}

			pybind::kernel_interface * kernel = pybind::get_kernel();

			uint32_t tinfo = kernel->class_info<C>();
			uint32_t tptrinfo = kernel->class_info<C *>();

			void * impl;
			if( type_cast::type_info_cast( _obj, tinfo, tptrinfo, &impl ) == false )
			{
				if( _nothrow == false )
				{
					kernel->error_invalid_extract( _obj, tinfo );
				}

				_value = nullptr;

				return false;
			}

			_value = static_cast<C*>(impl);

			return true;
		}

		PyObject * wrap( typename type_cast_result<C *>::TCastRef _value ) override
		{
			if( _value == nullptr )
			{
				return pybind::ret_none();
			}

			pybind::kernel_interface * kernel = pybind::get_kernel();

			const class_type_scope_ptr & scope = kernel->class_scope<C>();

			PyObject * py_obj = scope->create_holder( (void *)_value );

			return py_obj;
		}
	};

	template<class C>
	struct extract_struct_type_ref
		: public type_cast_result<C>
	{
		bool apply( PyObject * _obj, typename type_cast_result<C>::TCastValue _value, bool _nothrow ) override
		{
			pybind::kernel_interface * kernel = pybind::get_kernel();

			uint32_t tinfo = kernel->class_info<C>();
			uint32_t tptrinfo = kernel->class_info<C *>();

			void * impl;
			if( type_cast::type_info_cast( _obj, tinfo, tptrinfo, &impl ) == false )
			{
				const class_type_scope_ptr & scope = kernel->get_class_type_scope( tinfo );

				const convert_adapter_interface_ptr & convert = scope->get_convert_adapter();

				if( convert == nullptr )
				{
					if( _nothrow == false )
					{
						kernel->error_invalid_extract( _obj, tinfo );
					}

					return false;
				}

				if( convert->convert( _obj, &_value ) == false )
				{
					if( _nothrow == false )
					{
						kernel->error_invalid_extract( _obj, tinfo );
					}

					return false;
				}

				return true;
			}

			_value = *static_cast<C*>(impl);

			return true;
		}

		PyObject * wrap( const C & _class ) override
		{
			pybind::kernel_interface * kernel = pybind::get_kernel();

			uint32_t tinfo = kernel->class_info<C>();

			void * obj_place = nullptr;

			size_t size_C = sizeof( C );

			const class_type_scope_ptr & scope = kernel->get_class_type_scope( tinfo );

			PyObject * py_obj = scope->create_pod( &obj_place, size_C );

			if( py_obj == nullptr )
			{
				return nullptr;
			}

			new (obj_place)C( _class );

			return py_obj;
		}
	};

	template<class C, class B = no_bases>
	class class_
		: public base_<C, B>
	{
	protected:
		typedef extract_class_type_ptr<C> extract_type_ptr;

	public:
		class_( const char * _name, bool external_extract = true, PyObject * _module = 0 )
			: base_<C, B>( _name, 0, nullptr, new class_destroy_delete<C>, 0, _module )
		{
			if( external_extract == true )
			{
				this->setup_extract( new extract_type_ptr );
			}
		}

	public:
		template<class C0, class C1, class C2, class C3, class C4, class C5>
		class_ & def_constructor( const init<C0, C1, C2, C3, C4, C5> & _init )
		{
			constructor_adapter_interface_ptr ctr =
				new constructor_new<C, init<C0, C1, C2, C3, C4, C5> >();

			const pybind::class_type_scope_ptr & scope = base_<C, B>::get_scope();

			scope->set_construct( ctr );

			return *this;
		}
	};

	template<class C, class B = no_bases>
	class proxy_
		: public base_<C, B>
	{
	public:
		typedef extract_holder_type_ptr<C> extract_type_ptr;

	public:
		proxy_( const char * _name, bool external_extract = true, PyObject * _module = 0 )
			: base_<C, B>( _name, 0, nullptr, nullptr, 0, _module )
		{
			if( external_extract == true )
			{
				this->setup_extract( new extract_type_ptr );
			}
		}

	public:
		template<class C0, class C1, class C2, class C3, class C4, class C5>
		proxy_ & def_constructor( const init<C0, C1, C2, C3, C4, C5> & _init )
		{
			constructor_adapter_interface_ptr ctr =
				new constructor_new<C, init<C0, C1, C2, C3, C4, C5> >();

			const class_type_scope_ptr & scope = base_<C, B>::get_scope();

			scope->set_construct( ctr );

			return *this;
		}
	};

	template<class C, class B = no_bases>
	class superclass_
		: public base_<C, B>
	{
	public:
		typedef extract_holder_type_ptr<C> extract_type_ptr;

	public:
		superclass_( const char * _name, void * _user, const new_adapter_interface_ptr & _pynew, const destroy_adapter_interface_ptr & _pydestructor, bool external_extract = true, PyObject * _module = 0 )
			: base_<C, B>( _name, _user, _pynew, _pydestructor, 0, _module )
		{
			if( external_extract == true )
			{
				this->setup_extract( new extract_type_ptr );
			}
		}

	public:
		template<class C0, class C1, class C2, class C3, class C4, class C5>
		superclass_ & def_constructor( const init<C0, C1, C2, C3, C4, C5> & _init )
		{
			(void)_init;

			constructor_adapter_interface_ptr ctr =
				new constructor_new<C, init<C0, C1, C2, C3, C4, C5> >();

			const class_type_scope_ptr & scope = base_<C, B>::get_scope();

			scope->set_construct( ctr );

			return *this;
		}
	};

	template<class C, class B = no_bases>
	class struct_
		: public base_<C, B>
	{
	public:
		typedef extract_struct_type_ref<C> extract_type_ref;

	public:
		struct_( const char * _name, bool external_extract = true, PyObject * _module = 0 )
			: base_<C, B>( _name, 0, nullptr, nullptr, sizeof( C ), _module )
		{
			if( external_extract == true )
			{
				this->setup_extract( new extract_type_ref );
			}
		}

	public:
		template<class C0, class C1, class C2, class C3, class C4, class C5>
		struct_ & def_constructor( const init<C0, C1, C2, C3, C4, C5> & _init )
		{
			(void)_init;

			constructor_adapter_interface_ptr ctr =
				new constructor_placement<C, init<C0, C1, C2, C3, C4, C5> >();

			const class_type_scope_ptr & scope = base_<C, B>::get_scope();

			scope->set_construct( ctr );

			return *this;
		}
	};

	template<class C, class B = no_bases>
	class interface_
		: public base_<C, B>
	{
	public:
		typedef extract_holder_type_ptr<C> extract_type_ptr;

	public:
		interface_( const char * _name, bool external_extract = true, PyObject * _module = 0 )
			: base_<C, B>( _name, 0, new class_new_invalid, nullptr, 0, _module )
		{
			if( external_extract == true )
			{
				this->setup_extract( new extract_type_ptr );
			}
		}
	};

	template<class T>
	PyTypeObject * get_typemodule()
	{
		kernel_interface * kernel = pybind::get_kernel();

		const class_type_scope_ptr & scope = kernel->class_scope<T>();

		PyTypeObject * typemodule = scope->get_typemodule();

		return typemodule;
	}
}

