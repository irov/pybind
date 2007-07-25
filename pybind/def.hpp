#	pragma once

namespace pybind
{

	template<class T, class F>
	void def( const char * _name, F f )
	{
		typedef typename def_parser<F>::result f_info;

		def_proxy<F>::init( _name, f );

		class_core::def_method(
			_name,
			( f_info::arity > 0 ) ? (pybind_cfunction)&method_proxy<C,F>::method1 : (pybind_cfunction)&method_proxy<C,F>::method0,
			f_info::arity,
			class_info<C>()
			);

		return *this;
	}

}