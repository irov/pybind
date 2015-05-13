#	pragma once

#	include "pybind/bases.hpp"

#	include "pybind/class_info.hpp"
#	include "pybind/class_core.hpp"
#	include "pybind/class_type_scope.hpp"
#	include "pybind/function_parser.hpp"

#	include "pybind/method_adapter.hpp"
#	include "pybind/member_adapter.hpp"
#	include "pybind/convert_adapter.hpp"
#   include "pybind/compare_adapter.hpp"
#	include "pybind/repr_adapter.hpp"
#	include "pybind/hash_adapter.hpp"
#	include "pybind/type_cast.hpp"
#	include "pybind/exception.hpp"

#   include "pybind/constructor_new.hpp"
#   include "pybind/constructor_placement.hpp"

namespace pybind
{	
	namespace detail
	{
		template<class T>
		struct declare_base_type_
		{
			static bool value;
		};

		template<class T>
		bool declare_base_type_<T>::value = false;

		template<class T>
		static void declare_base_type()
		{
			declare_base_type_<T>::value = true;
		};

		template<class T>
		static bool is_declare_base_type()
		{
			return declare_base_type_<T>::value;
		}
	}
	
	typedef bases<void, void, void, void, void, void, void, void, void, void, void, void, void, void, void> no_bases;

	template<class C, class B = no_bases>
	class base_
	{
	protected:
		typedef B bases_type;

	public:
		base_( const char * _name, void * _user, pybind_new _pynew, pybind_destructor _pydestructor, size_t _pod, PyObject * _module )
		{
			detail::declare_base_type<C>();

			m_info = class_info<C>();

			class_type_scope_ptr scope = class_core::create_new_type_scope( m_info, _name, _user, _pynew, _pydestructor, _pod );

			this->setup_bases( scope );

			class_core::initialize_new_type_scope( scope, _module );
		}

		template<class BB>
		static void * meta_cast( void * _ptr )
		{
			C * c = static_cast<C *>(_ptr);
			BB * bb = static_cast<BB *>(c);

			return bb;
		}

		void setup_bases( const class_type_scope_ptr & _scope )
		{
			int arity = B::base_arity;

			if( arity-- > 0 )class_core::add_base<typename B::base0>( _scope, &meta_cast<typename B::base0> );
			if( arity-- > 0 )class_core::add_base<typename B::base1>( _scope, &meta_cast<typename B::base1> );
			if( arity-- > 0 )class_core::add_base<typename B::base2>( _scope, &meta_cast<typename B::base2> );
			if( arity-- > 0 )class_core::add_base<typename B::base3>( _scope, &meta_cast<typename B::base3> );
			if( arity-- > 0 )class_core::add_base<typename B::base4>( _scope, &meta_cast<typename B::base4> );
			if( arity-- > 0 )class_core::add_base<typename B::base5>( _scope, &meta_cast<typename B::base5> );
			if( arity-- > 0 )class_core::add_base<typename B::base6>( _scope, &meta_cast<typename B::base6> );
			if( arity-- > 0 )class_core::add_base<typename B::base7>( _scope, &meta_cast<typename B::base7> );
			if( arity-- > 0 )class_core::add_base<typename B::base8>( _scope, &meta_cast<typename B::base8> );
			if( arity-- > 0 )class_core::add_base<typename B::base9>( _scope, &meta_cast<typename B::base9> );
			if( arity-- > 0 )class_core::add_base<typename B::base10>( _scope, &meta_cast<typename B::base10> );
			if( arity-- > 0 )class_core::add_base<typename B::base11>( _scope, &meta_cast<typename B::base11> );
			if( arity-- > 0 )class_core::add_base<typename B::base12>( _scope, &meta_cast<typename B::base12> );
			if( arity-- > 0 )class_core::add_base<typename B::base13>( _scope, &meta_cast<typename B::base13> );
			if( arity-- > 0 )class_core::add_base<typename B::base14>( _scope, &meta_cast<typename B::base14> );
			if( arity-- > 0 )class_core::add_base<typename B::base15>( _scope, &meta_cast<typename B::base15> );
		}

		template<class F>
		base_ & def( const char * _name, F f )
		{			
			method_adapter_interface_ptr iadapter =
				new method_adapter<C, F>(_name, f);

            class_core::def_method( iadapter, m_info );

			return *this;
		}

        template<class F>
        base_ & def_depricated( const char * _name, F f, const char * _doc )
        {			
            method_adapter_interface_ptr iadapter =
                new method_adapter_depricated<C, F>(_name, f, _doc);

            class_core::def_method( iadapter, m_info );

            return *this;
        }

		template<class F>
		base_ & def_native( const char * _name, F f )
		{			
			method_adapter_interface_ptr iadapter =
				new method_adapter_native<C, F>(_name, f);

			class_core::def_method( iadapter, m_info );

			return *this;
		}

		template<class F>
		base_ & def_convert( F f, void * _user )
		{
			convert_adapter_interface_ptr iadapter =
				new convert_adapter<C, F>(f, _user);

			class_core::def_convert( iadapter, m_info );

			return *this;
		}

		template<class F>
		base_ & def_static( const char * _name, F f )
		{			
			method_adapter_interface_ptr iadapter =
				new method_adapter_proxy_function<C, F>(_name, f);

			class_core::def_method( iadapter, m_info );

			return *this;
		}

		template<class P, class F>
		base_ & def_proxy_static( const char * _name, P * _proxy, F f )
		{
			method_adapter_interface_ptr iadapter =
				new method_adapter_proxy_member<C, P, F>(_name, f, _proxy);

			class_core::def_method( iadapter, m_info );

			return *this;
		}

		template<class P, class F>
		base_ & def_proxy_native( const char * _name, P * _proxy, F f )
		{
			method_adapter_interface_ptr iadapter =
				new method_adapter_proxy_native<C, P, F>(_name, f, _proxy );

			class_core::def_method( iadapter, m_info );

			return *this;
		}

		template<class A>
		base_ & def_member( const char * _name, A C:: * a )
		{
			member_adapter_interface_ptr iadapter =
				new member_adapter<C, A>(_name, a);

			class_core::def_member( iadapter, m_info );

			return *this;
		}

		template<class FG, class FS>
		base_ & def_property( const char * _name, FG _get, FS _set )
		{
			member_adapter_interface_ptr iadapter =
				new member_adapter_property<C, FG, FS>(_name, _get, _set);

			class_core::def_member( iadapter, m_info );

			return *this;
		}

		template<class FG, class FS>
		base_ & def_property_static( const char * _name, FG _get, FS _set )
		{
			member_adapter_interface_ptr iadapter =
				new member_adapter_property_static<C, FG, FS>(_name, _get, _set);

			class_core::def_member( iadapter, m_info );

			return *this;
		}

        template<class F>
        base_ & def_call( F _fn )
        {
            method_adapter_interface_ptr iadapter =
                new method_adapter<C, F>( "__call__", _fn );

            class_core::def_call( iadapter, m_info );

            return *this;
        }

		template<class F>
		base_ & def_repr( F _fn )
		{
			repr_adapter_interface_ptr iadapter =
				new repr_adapter<C, F>( _fn );

			class_core::def_repr( iadapter, m_info );

			return *this;
		}

        template<class F>
        base_ & def_hash( F _fn )
        {
            hash_adapter_interface_ptr iadapter =
                new hash_adapter<C, F>( _fn );

            class_core::def_hash( iadapter, m_info );

            return *this;
        }

        template<class F>
        base_ & def_compare( F _fn )
        {
            compare_adapter_interface_ptr iadapter =
                new compare_adapter<C, F>( _fn );

            class_core::def_compare( iadapter, m_info );

            return *this;
        }

		template<class F>
		base_ & def_getattro( F _fn )
		{
			method_adapter_interface_ptr iadapter =
				new method_adapter<C, F>("getattro", _fn);

			class_core::def_getattro( iadapter, m_info );

			return *this;
		}

		template<class F>
		base_ & def_mapping( F _fn )
		{
			method_adapter_interface_ptr iadapter =
				new method_adapter<C, F>("getmap", _fn);

			class_core::def_mapping( iadapter, m_info );

			return *this;
		}

		template<class F>
		base_ & def_sequence_get( F _fn )
		{
			method_adapter_interface_ptr iadapter =
				new method_adapter<C, F>("sequence_get", _fn);

			class_core::def_sequence_get( iadapter, m_info );

			return *this;
		}

		template<class F>
		base_ & def_sequence_set( F _fn )
		{
			method_adapter_interface_ptr iadapter =
				new method_adapter<C, F>("sequence_set", _fn);

			class_core::def_sequence_set( iadapter, m_info );

			return *this;
		}


		template<class F>
		base_ & def_static_sequence_get( F _fn )
		{
			method_adapter_interface_ptr iadapter =
				new method_adapter_proxy_function<C, F>("static_sequence_get", _fn);

			class_core::def_sequence_get( iadapter, m_info );

			return *this;
		}

		template<class F>
		base_ & def_static_sequence_set( F _fn )
		{
			method_adapter_interface_ptr iadapter =
				new method_adapter_proxy_function<C, F>("static_sequence_set", _fn);

			class_core::def_sequence_set( iadapter, m_info );

			return *this;
		}
        
		static void * new_( const pybind::class_type_scope_ptr & _scope, PyObject * _obj, PyObject * _args, PyObject * _kwds )
		{
            (void)_kwds;

			void * impl = class_core::construct( _scope, _obj, _args );

			return impl;
		}

        static void * new_interface( const pybind::class_type_scope_ptr & _scope, PyObject * _obj, PyObject * _args, PyObject * _kwds )
        {
            (void)_scope;
            (void)_obj;
            (void)_args;
            (void)_kwds;

			pybind::throw_exception("scope %s not support 'new_interface'"
				, _scope->get_name()
				);

            return nullptr;
        }

		static void dealloc_only_python_( const pybind::class_type_scope_ptr & _scope, void * _impl )
		{
            (void)_scope;
            (void)_impl;
		}

		static void	dealloc_( const pybind::class_type_scope_ptr & _scope, void * _impl )
		{
            (void)_scope;

			C * obj = static_cast<C*>(_impl);

			delete obj;
		}

        static void dealloc_only_destructor_( const pybind::class_type_scope_ptr & _scope, void * _impl )
        {
            (void)_scope;

            C * obj = static_cast<C*>(_impl);

            (void)obj;

            obj->~C();
        }

		static const pybind::class_type_scope_ptr & get_scope()
		{
			uint32_t cinfo = class_info<C>();

            const pybind::class_type_scope_ptr & scope = detail::get_class_type_scope( cinfo );

			return scope;
		}

    protected:
        uint32_t m_info;
	};

	template<class C> 
	struct extract_class_type_ptr
		: public type_cast_result<C *>
	{
		bool apply( PyObject * _obj, typename type_cast_result<C *>::TCastValue _value ) override
		{
            if( pybind::is_none( _obj ) == true )
            {
                _value = nullptr;

                return true;
            }

			uint32_t tinfo = class_info<C>();
			uint32_t tptrinfo = class_info<C *>();

			void * impl;
			if( type_cast::type_info_cast( _obj, tinfo, tptrinfo, &impl ) == false )
			{
				detail::error_invalid_extract( _obj, tinfo );

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

            uint32_t tinfo = class_info<C>();
    
            PyObject * py_obj = class_core::create_class( tinfo, (void *)_value );
    
            return py_obj;
		}
	};

	template<class C> 
	struct extract_holder_type_ptr
		: public type_cast_result<C *>
	{
		bool apply( PyObject * _obj, typename type_cast_result<C *>::TCastValue _value ) override
		{
			if( pybind::is_none( _obj ) == true )
			{
				_value = nullptr;

				return true;
			}

			uint32_t tinfo = class_info<C>();
			uint32_t tptrinfo = class_info<C *>();

			void * impl;
			if( type_cast::type_info_cast( _obj, tinfo, tptrinfo, &impl ) == false )
			{
				detail::error_invalid_extract( _obj, tinfo );

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

			uint32_t tinfo = class_info<C>();

			PyObject * py_obj = class_core::create_holder( tinfo, (void *)_value );

			return py_obj;
		}
	};

	template<class C>
	struct extract_struct_type_ref
		: public type_cast_result<C>
	{
		bool apply( PyObject * _obj, typename type_cast_result<C>::TCastValue _value ) override
		{
			uint32_t tinfo = class_info<C>();
			uint32_t tptrinfo = class_info<C *>();

			void * impl;
			if( type_cast::type_info_cast( _obj, tinfo, tptrinfo, &impl ) == false )
			{
				if( detail::convert_object( _obj, tinfo, &_value ) == false )
				{
					detail::error_invalid_extract( _obj, tinfo );

					return false;
				}

				return true;
			}

			_value = *static_cast<C*>(impl);

			return true;
		}

		PyObject * wrap( const C & _class )
		{
			uint32_t tinfo = class_info<C>();

			void * obj_place = nullptr;

			size_t size_C = sizeof(C);

			PyObject * py_obj = class_core::create_pod( tinfo, &obj_place, size_C );

			if( py_obj == nullptr )
			{
				return nullptr;
			}

			new (obj_place) C(_class);

			return py_obj;
		}
	};

	template<class C, class B = no_bases>
	class class_
		: public base_<C,B>
	{
	protected:
		typedef extract_class_type_ptr<C> extract_type_ptr;

	public:
		class_( const char * _name, bool external_extract = true, PyObject * _module = 0 )
			: base_<C,B>( _name, 0, &base_<C,B>::new_, &base_<C,B>::dealloc_, 0, _module )
		{
			if( external_extract == true )
			{
				this->setup_extract();
			}
		}

    public:
        template<class C0, class C1, class C2, class C3, class C4, class C5>
        class_ & def_constructor( const init<C0,C1,C2,C3,C4,C5> & _init )
        {
            constructor_adapter_interface_ptr ctr = 
                new constructor_new<C, init<C0,C1,C2,C3,C4,C5> >();

            const pybind::class_type_scope_ptr & scope = base_<C,B>::get_scope();
            class_core::def_init( scope, ctr );

            return *this;
        }

	protected:
		void setup_extract()
		{
			pybind::registration_type_cast<C>( new extract_type_ptr );
		}
	};

	template<class C, class B = no_bases>
	class proxy_
		: public base_<C,B>
	{
	public:
		typedef extract_holder_type_ptr<C> extract_type_ptr;

	public:
		proxy_( const char * _name, bool external_extract = true, PyObject * _module = 0 )
			: base_<C,B>( _name, 0, &base_<C,B>::new_, &base_<C,B>::dealloc_only_python_, 0, _module )
		{
			if( external_extract == true )
			{
				this->setup_extract();
			}
		}

    public:
        template<class C0, class C1, class C2, class C3, class C4, class C5>
        proxy_ & def_constructor( const init<C0,C1,C2,C3,C4,C5> & _init )
        {
            constructor_adapter_interface_ptr ctr = 
                new constructor_new<C, init<C0,C1,C2,C3,C4,C5> >();

            const pybind::class_type_scope_ptr & scope = base_<C,B>::get_scope();
            class_core::def_init( scope, ctr );

            return *this;
        }

    protected:
        void setup_extract()
        {
            pybind::registration_type_cast<C>( new extract_type_ptr );
        }
	};

    template<class C, class B = no_bases>
    class superclass_
        : public base_<C,B>
    {
    public:
        typedef extract_holder_type_ptr<C> extract_type_ptr;

    public:
        superclass_( const char * _name, void * _user, pybind_new _pynew, pybind_destructor _pydestructor, bool external_extract = true, PyObject * _module = 0 )
            : base_<C,B>(_name, _user, _pynew, _pydestructor, 0, _module)
        {
            if( external_extract == true )
            {
                this->setup_extract();
            }
        }

    public:
        template<class C0, class C1, class C2, class C3, class C4, class C5>
        superclass_ & def_constructor( const init<C0,C1,C2,C3,C4,C5> & _init )
        {
            (void)_init;

            constructor_adapter_interface_ptr ctr = 
                new constructor_new<C, init<C0,C1,C2,C3,C4,C5> >();

            const class_type_scope_ptr & scope = base_<C,B>::get_scope();

            class_core::def_init( scope, ctr );

            return *this;
        }

    protected:
        void setup_extract()
        {
            pybind::registration_type_cast<C>( new extract_type_ptr );
        }
    };

	template<class C, class B = no_bases>
	class struct_
		: public base_<C,B>
	{
	public:
		typedef extract_struct_type_ref<C> extract_type_ref;

	public:
		struct_( const char * _name, bool external_extract = true, PyObject * _module = 0 )
			: base_<C,B>( _name, 0, &base_<C,B>::new_, &base_<C,B>::dealloc_only_destructor_, sizeof(C), _module )
		{
			if( external_extract == true )
			{
				this->setup_extract();
			}
		}

    public:
        template<class C0, class C1, class C2, class C3, class C4, class C5>
        struct_ & def_constructor( const init<C0,C1,C2,C3,C4,C5> & _init )
        {
            (void)_init;

            constructor_adapter_interface_ptr ctr = 
                new constructor_placement<C, init<C0,C1,C2,C3,C4,C5> >();

            const class_type_scope_ptr & scope = base_<C,B>::get_scope();
            class_core::def_init( scope, ctr );

            return *this;
        }

    protected:
        void setup_extract()
        {
            pybind::registration_type_cast<C>( new extract_type_ref );
        }
	};

	template<class C, class B = no_bases>
	class interface_
		: public base_<C,B>
	{
	public:
		typedef extract_holder_type_ptr<C> extract_type_ptr;

	public:
		interface_( const char * _name, bool external_extract = true, PyObject * _module = 0 )
			: base_<C,B>( _name, 0, &base_<C,B>::new_interface, &base_<C,B>::dealloc_only_python_, 0, _module )
		{
			if( external_extract == true )
			{
				this->setup_extract();
			}
		}

	protected:
		void setup_extract()
		{
			pybind::registration_type_cast<C>( new extract_type_ptr );
		}
	};

    template<class T>
    PyTypeObject * get_typemodule()
    {
        uint32_t tinfo = class_info<T>();

        PyTypeObject * typemodule = class_core::get_typemodule( tinfo );

        return typemodule;
    }
}

