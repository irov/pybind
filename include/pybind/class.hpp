#	pragma once

#	include "pybind/bases.hpp"

#	include "pybind/class_info.hpp"
#	include "pybind/class_core.hpp"
//#	include "pybind/class_type.hpp"
#	include "pybind/function_parser.hpp"

#	include "pybind/method_adapter.hpp"
#	include "pybind/member_adapter.hpp"
#	include "pybind/convert_adapter.hpp"
#   include "pybind/compare_adapter.hpp"
#	include "pybind/repr_adapter.hpp"
#	include "pybind/hash_adapter.hpp"
#	include "pybind/type_cast.hpp"

#   include "pybind/constructor_new.hpp"
#   include "pybind/constructor_placement.hpp"

#	include <list>

namespace pybind
{	
	typedef bases<void,void,void,void,void,void,void> no_bases;

	template<class C, class B = no_bases>
	class base_
	{
	protected:
		typedef B bases_type;

	public:
		base_( const char * _name, void * _user, pybind_new _pynew, pybind_destructor _pydestructor, bool _pod, PyObject * _module )
		{
			m_info = class_info<C>();

			class_type_scope * scope = class_core::create_new_type_scope( m_info, _name, _module, _user, _pynew, _pydestructor, _pod );

			setup_bases( scope );

			class_core::initialize_new_type_scope( scope );
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
			if( arity-- > 0 )class_core::add_base<typename B::base6>(_scope, &meta_cast<typename B::base6>);
			if( arity-- > 0 )class_core::add_base<typename B::base7>(_scope, &meta_cast<typename B::base7>);
			if( arity-- > 0 )class_core::add_base<typename B::base8>(_scope, &meta_cast<typename B::base8>);
		}

		template<class F>
		base_ & def( const char * _name, F f )
		{			
			method_adapter_interface * iadapter =
				new method_adapter<C, F>(f, _name);

            class_core::def_method( _name, iadapter, m_info );

			return *this;
		}

		template<class F>
		base_ & def_native( const char * _name, F f )
		{			
			method_adapter_interface * iadapter =
				new method_adapter_native<C, F>(f, _name);

			class_core::def_method( _name, iadapter, m_info );

			return *this;
		}

		template<class F>
		base_ & def_convert( F f )
		{
			convert_adapter_interface * iadapter =
				new convert_adapter<F>(f);

			class_core::def_convert( iadapter, m_info );

			return *this;
		}

		template<class F>
		base_ & def_static( const char * _name, F f )
		{			
			method_adapter_interface * iadapter =
				new method_adapter_proxy_function<C, F>(f, _name);

			class_core::def_method( _name, iadapter, m_info );

			return *this;
		}

		template<class P, class F>
		base_ & def_proxy_static( const char * _name, P * _proxy, F f )
		{
			method_adapter_interface * iadapter =
				new method_adapter_proxy_member<C, P, F>(_proxy, f, _name);

			class_core::def_method( _name, iadapter, m_info );

			return *this;
		}

		template<class A>
		base_ & def_member( const char * _name, A C:: * a )
		{
			member_adapter_interface * iadapter =
				new member_adapter<C, A>(_name, a);

			class_core::def_member( _name, iadapter, m_info );

			return *this;
		}

		template<class FG, class FS>
		base_ & def_property( const char * _name, FG _get, FS _set )
		{
			member_adapter_interface * iadapter =
				new member_adapter_property<C, FG, FS>(_name, _get, _set);

			class_core::def_member( _name, iadapter, m_info );

			return *this;
		}

		template<class FG, class FS>
		base_ & def_property_static( const char * _name, FG _get, FS _set )
		{
			member_adapter_interface * iadapter =
				new member_adapter_property_static<C, FG, FS>(_name, _get, _set);

			class_core::def_member( _name, iadapter, m_info );

			return *this;
		}

        template<class F>
        base_ & def_call( F _fn )
        {
            method_adapter_interface * iadapter =
                new method_adapter<C, F>( _fn, "__call__" );

            class_core::def_call( iadapter, m_info );

            return *this;
        }

		template<class F>
		base_ & def_repr( F _fn )
		{
			repr_adapter_interface * iadapter =
				new repr_adapter<C, F>( _fn );

			class_core::def_repr( iadapter, m_info );

			return *this;
		}

        template<class F>
        base_ & def_hash( F _fn )
        {
            hash_adapter_interface * iadapter =
                new hash_adapter<C, F>( _fn );

            class_core::def_hash( iadapter, m_info );

            return *this;
        }

        template<class F>
        base_ & def_compare( F _fn )
        {
            compare_adapter_interface * iadapter =
                new compare_adapter<C, F>( _fn );

            class_core::def_compare( iadapter, m_info );

            return *this;
        }

		template<class F>
		base_ & def_getattro( F _fn )
		{
			method_adapter_interface * iadapter =
				new method_adapter<C, F>(_fn, "getattro");

			class_core::def_getattro( iadapter, m_info );

			return *this;
		}

		template<class F>
		base_ & def_mapping( F _fn )
		{
			method_adapter_interface * iadapter =
				new method_adapter<C, F>(_fn, "getmap");

			class_core::def_mapping( iadapter, m_info );

			return *this;
		}

		//template<class F>
		//base_ & def_sequence( F _fn )
		//{
		//	method_adapter_interface * iadapter =
		//		new method_adapter<C, F>(_fn, "sequence");

		//	class_core::def_sequence( 
		//		iadapter, 
		//		class_info<C>() 
		//		);

		//	return *this;
		//}

		template<class F>
		base_ & def_static_sequence( F _fn )
		{
			method_adapter_interface * iadapter =
				new method_adapter_proxy_function<C, F>(_fn, "sequence");

			class_core::def_sequence( iadapter, m_info );

			return *this;
		}
        
		static void *
			new_( pybind::class_type_scope * _scope, PyObject * _obj, PyObject * _args, PyObject * _kwds )
		{
            (void)_kwds;

			void * impl = class_core::construct( _scope, _obj, _args );

			return impl;
		}

        static void *
            new_interface( pybind::class_type_scope * _scope, PyObject * _obj, PyObject * _args, PyObject * _kwds )
        {
            (void)_scope;
            (void)_obj;
            (void)_args;
            (void)_kwds;

            throw_exception();

            return 0;
        }

		static void 
			dealloc_only_python_( pybind::class_type_scope * _scope, void * _impl )
		{
            (void)_scope;
            (void)_impl;
		}

		static void			
            dealloc_( pybind::class_type_scope * _scope, void * _impl )
		{
            (void)_scope;

			C * obj = static_cast<C*>(_impl);

			delete obj;
		}

        static void
            dealloc_only_destructor_( pybind::class_type_scope * _scope, void * _impl )
        {
            (void)_scope;

            C * obj = static_cast<C*>(_impl);

            (void)obj;

            obj->~C();
        }

		static class_type_scope * get_scope()
		{
			size_t cinfo = class_info<C>();

            class_type_scope * scope = detail::get_class_type_scope( cinfo );

			return scope;
		}

    protected:
        size_t m_info;
	};

	template<class C> 
	struct extract_class_type_ptr
		: public type_cast_result<C *>
	{
		bool apply( PyObject * _obj, C *& _value ) override
		{
			size_t tinfo = class_info<C>();
			size_t tptrinfo = class_info<C *>();

			void * impl;
			if( type_cast::type_info_cast( _obj, tinfo, tptrinfo, &impl ) == false )
			{
				detail::error_invalid_extract( _obj, tinfo );

				_value = NULL;

				return false;
			}

			_value = static_cast<C*>(impl);

			return true;
		}

        PyObject * wrap( typename type_cast_result<C *>::TCastRef _class ) override
		{			
            if( _class == 0 )
            {
                return pybind::ret_none();
            }

            size_t tinfo = class_info<C>();
    
            PyObject * py_obj = class_core::create_holder( tinfo, (void *)_class );
    
            return py_obj;
		}
	};

	template<class C>
	struct extract_struct_type_ref
		: public type_cast_result<C>
	{
		bool apply( PyObject * _obj, C & _value ) override
		{
			size_t tinfo = class_info<C>();
			size_t tptrinfo = class_info<C *>();

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
			//return 0;
			size_t tinfo = class_info<C>();
			//C * obj = new C(_class);

			void * obj_place;

			PyObject * py_obj = class_core::create_pod( tinfo, &obj_place );

			new (obj_place) C(_class);

			return py_obj;
		}
	};

	template<class C, class B = no_bases>
	class class_
		: public base_<C,B>
	{
	public:
		class_( const char * _name, bool external_extract = true, PyObject * _module = 0 )
			: base_<C,B>( _name, 0, &base_<C,B>::new_, &base_<C,B>::dealloc_, false, _module )
		{
			if( external_extract )
			{
				this->setup_extract();
			}
		}

    public:
        template<class C0, class C1, class C2, class C3, class C4, class C5>
        class_ & def_constructor( const init<C0,C1,C2,C3,C4,C5> & _init )
        {
            constructor * ctr = 
                new constructor_new<C, init<C0,C1,C2,C3,C4,C5> >();

            class_type_scope * scope = base_<C,B>::get_scope();
            class_core::def_init( scope, ctr );

            return *this;
        }

	protected:
		typedef extract_class_type_ptr<C> extract_type_ptr;

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
		typedef extract_class_type_ptr<C> extract_type_ptr;

	public:
		proxy_( const char * _name, bool external_extract = true, PyObject * _module = 0 )
			: base_<C,B>( _name, 0, &base_<C,B>::new_, &base_<C,B>::dealloc_only_python_, false, _module )
		{
			if( external_extract )
			{
				this->setup_extract();
			}
		}

    public:
        template<class C0, class C1, class C2, class C3, class C4, class C5>
        proxy_ & def_constructor( const init<C0,C1,C2,C3,C4,C5> & _init )
        {
            constructor * ctr = 
                new constructor_new<C, init<C0,C1,C2,C3,C4,C5> >();

            class_type_scope * scope = base_<C,B>::get_scope();
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
        typedef extract_class_type_ptr<C> extract_type_ptr;

    public:
        superclass_( const char * _name, void * _user, pybind_new _pynew, pybind_destructor _pydestructor, bool external_extract = true, PyObject * _module = 0 )
            : base_<C,B>(_name, _user, _pynew, _pydestructor, false, _module)
        {
            if( external_extract )
            {
                this->setup_extract();
            }
        }

    public:
        template<class C0, class C1, class C2, class C3, class C4, class C5>
        superclass_ & def_constructor( const init<C0,C1,C2,C3,C4,C5> & _init )
        {
            (void)_init;

            constructor * ctr = 
                new constructor_new<C, init<C0,C1,C2,C3,C4,C5> >();

            class_type_scope * scope = base_<C,B>::get_scope();
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
			: base_<C,B>( _name, 0, &base_<C,B>::new_, &base_<C,B>::dealloc_only_destructor_, true, _module )
		{
			if( external_extract )
			{
				this->setup_extract();
			}
		}

    public:
        template<class C0, class C1, class C2, class C3, class C4, class C5>
        struct_ & def_constructor( const init<C0,C1,C2,C3,C4,C5> & _init )
        {
            (void)_init;

            constructor * ctr = 
                new constructor_placement<C, init<C0,C1,C2,C3,C4,C5> >();

            class_type_scope * scope = base_<C,B>::get_scope();
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
		typedef extract_class_type_ptr<C> extract_type_ptr;

	public:
		interface_( const char * _name, bool external_extract = true, PyObject * _module = 0 )
			: base_<C,B>( _name, 0, &base_<C,B>::new_interface, &base_<C,B>::dealloc_only_python_, false, _module )
		{
			if( external_extract )
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
        size_t tinfo = class_info<T>();

        PyTypeObject * typemodule = class_core::get_typemodule( tinfo );

        return typemodule;
    }
}

