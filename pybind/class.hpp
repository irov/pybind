#	pragma once

#	include "pybind/exports.hpp"

#	include "pybind/bases.hpp"

#	include "pybind/class_core.hpp"
#	include "pybind/method_parser.hpp"
#	include "pybind/method_proxy.hpp"

namespace pybind
{	
	template<class C, class B = bases<void,void,void,void> >
	class PYBIND_API class_
	{
	public:
		class_( const char * _name, module_ * _module )
		{
			m_type_scope = 
				class_core::create_new_type_scope( 
				_name, 
				_module, 
				&new_, 
				&dealloc_ 
				);
		}

		class_( class_type_scope * _scope )
		{

		}

		virtual ~class_() {}

		template<class F>
		class_ & def( const char * _name, F f )
		{		
			typedef typename method_parser<C,F>::result f_info;

			method_proxy<C,F>::init( f );

			class_core::def_method(
				_name,
				( f_info::arity > 0 ) ? (pybind_cfunction)&method_proxy<C,F>::method1 : (pybind_cfunction)&method_proxy<C,F>::method0,
				f_info::arity,
				m_type_scope
				);

			return *this;
		}

		void end()
		{
			int arity = B::base_arity;
			
			if( arity-- > 0 )class_core::add_method_from_scope( m_type_scope, class_scope::get_class_scope( typeid( B::base0 ) );
			if( arity-- > 0 )class_core::add_method_from_scope( m_type_scope, class_scope::get_class_scope( typeid( B::base1 ) );
			if( arity-- > 0 )class_core::add_method_from_scope( m_type_scope, class_scope::get_class_scope( typeid( B::base2 ) );
			if( arity-- > 0 )class_core::add_method_from_scope( m_type_scope, class_scope::get_class_scope( typeid( B::base3 ) );
			
			

			class_core::end_method( typeid(C), m_type_scope );
		}

		static PyObject *
			new_( PyTypeObject * _type, PyObject * _args, PyObject * _kwds )
		{
			return class_core::new_impl( _type, _args, (void *) new C );
		}

		static void
			dealloc_( PyObject * self )
		{
			C * obj = (C*)class_core::dealloc_impl( self );
			delete obj;
		}

		static class_type_scope * scope()
		{
			return m_type_scope;
		}

	protected:
		static class_type_scope * m_type_scope;
	};

	template<class C, class B>
	class_type_scope * class_<C,B>::m_type_scope = 0;

}