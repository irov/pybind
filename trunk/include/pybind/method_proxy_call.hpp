#	pragma once

#	include "pybind/exports.hpp"
#	include "pybind/function_parser.hpp"
#	include "pybind/types.hpp"
#	include "pybind/system.hpp"
#	include "pybind/extract.hpp"
#	include "pybind/helper.hpp"

namespace pybind
{
	template<class P, class C, class F,class Ret, int i>
	struct method_proxy_call_impl
	{
		static Ret call( P * _proxy, C * _obj, F f, PyObject * _arg );
	};

	template<class P, class C, class F,class Ret>
	struct method_proxy_call_impl<P,C,F,Ret,0>
	{
		static Ret call( P * _proxy, C * _obj, F f, PyObject * _arg );		
	};

	template<class P, class C, class F,class Ret>
	struct method_proxy_call_impl<P,C,F,Ret,1>
	{
		static Ret call( P * _proxy, C * _obj, F f, PyObject * _arg )
		{
            (void)_arg;

			return (_proxy->*f)( _obj
				);
		}
	};

	template<class P, class C, class F,class Ret>
	struct method_proxy_call_impl<P,C,F,Ret,2>
	{
		static Ret call( P * _proxy, C * _obj, F f, PyObject * _arg )
		{
			return (_proxy->*f)( _obj
				, tuple_getitem_t( _arg, 0 )
				);
		}	
	};

	template<class P, class C, class F,class Ret>
	struct method_proxy_call_impl<P,C,F,Ret,3>
	{
		static Ret call( P * _proxy, C * _obj, F f, PyObject * _arg )
		{
			return (_proxy->*f)( _obj
				, tuple_getitem_t( _arg, 0 )
				, tuple_getitem_t( _arg, 1 )
				);
		}	
	};

	template<class P, class C, class F,class Ret>
	struct method_proxy_call_impl<P,C,F,Ret,4>
	{
		static Ret call( P * _proxy, C * _obj, F f, PyObject * _arg )
		{
			return (_proxy->*f)( _obj
				, tuple_getitem_t( _arg, 0 )
				, tuple_getitem_t( _arg, 1 )
				, tuple_getitem_t( _arg, 2 )
				);
		}
	};

	template<class P, class C, class F,class Ret>
	struct method_proxy_call_impl<P, C,F,Ret,5>
	{
		static Ret call( P * _proxy, C * _obj, F f, PyObject * _arg )
		{
			return (_proxy->*f)( _obj
				, tuple_getitem_t( _arg, 0 )
				, tuple_getitem_t( _arg, 1 )
				, tuple_getitem_t( _arg, 2 )
				, tuple_getitem_t( _arg, 3 )
				);
		}
	};

    template<class P, class C, class F,class Ret>
    struct method_proxy_call_impl<P, C,F,Ret,6>
    {
        static Ret call( P * _proxy, C * _obj, F f, PyObject * _arg )
        {
            return (_proxy->*f)( _obj
				, tuple_getitem_t( _arg, 0 )
				, tuple_getitem_t( _arg, 1 )
				, tuple_getitem_t( _arg, 2 )
				, tuple_getitem_t( _arg, 3 )
				, tuple_getitem_t( _arg, 4 )
                );
        }
    };

	template<class P, class C, class F,class Ret>
	struct method_proxy_call_ret_impl
	{
		typedef typename function_parser<F>::result f_info;

		static PyObject * call( P * _proxy, C * _obj, F f, PyObject * _arg )
		{
			PyObject * py_result = (detail::import_operator_t)method_proxy_call_impl<P, C, F, Ret, f_info::arity>::call( _proxy, _obj, f, _arg );

			return py_result;
		}
	};

	template<class P, class C, class F>
	struct method_proxy_call_ret_impl<P,C,F,void>
	{
		typedef typename function_parser<F>::result f_info;

		static PyObject * call( P * _proxy, C * _obj, F f, PyObject * _arg )
		{
			method_proxy_call_impl<P,C,F,void,f_info::arity>::call( _proxy, _obj, f, _arg );

			return ret_none();
		}
	};

	template<class P, class C, class F>
	struct method_proxy_call
	{
		typedef typename function_parser<F>::result f_info;

		static PyObject * call( P * _proxy, C * _obj, F f, PyObject * _arg )
		{
			uint32_t arg_size = pybind::tuple_size( _arg );
			uint32_t fn_arity = f_info::arity;

            if( arg_size + 1 != fn_arity )
            {
				pybind::throw_exception("invalid proxy method call args is not equal %d != %d\n"
                    , arg_size + 1
                    , fn_arity
                    );

                return nullptr;
            }

			PyObject * ret = method_proxy_call_ret_impl<P,C,F,typename f_info::ret_type>::call( _proxy, _obj, f, _arg );

			return ret;		
		}
	};
}

