#	pragma once

#	include "pybind/exports.hpp"
#	include "pybind/function_parser.hpp"
#	include "pybind/types.hpp"
#	include "pybind/system.hpp"
#	include "pybind/extract.hpp"

namespace pybind
{
	template<class P, class C, class F,class Ret, int i>
	struct method_proxy_call_impl
	{
		//static Ret call( P * _proxy, C * _obj, F f, PyObject * _arg );
	};

	template<class P, class C, class F,class Ret>
	struct method_proxy_call_impl<P,C,F,Ret,0>
	{
	};

	template<class P, class C, class F,class Ret>
	struct method_proxy_call_impl<P,C,F,Ret,1>
	{
		typedef typename function_parser<F>::result f_info;

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
		typedef typename function_parser<F>::result f_info;

		static Ret call( P * _proxy, C * _obj, F f, PyObject * _arg )
		{
			PyObject * _arg0 = tuple_getitem( _arg, 0 );

			return (_proxy->*f)( _obj
				, extract_throw<typename f_info::param1>( _arg0 )
				);
		}	
	};

	template<class P, class C, class F,class Ret>
	struct method_proxy_call_impl<P,C,F,Ret,3>
	{
		typedef typename function_parser<F>::result f_info;

		static Ret call( P * _proxy, C * _obj, F f, PyObject * _arg )
		{
			PyObject * _arg0 = tuple_getitem( _arg, 0 );
			PyObject * _arg1 = tuple_getitem( _arg, 1 );

			return (_proxy->*f)( _obj
				, extract_throw<typename f_info::param1>( _arg0 )
				, extract_throw<typename f_info::param2>( _arg1 )
				);
		}	
	};

	template<class P, class C, class F,class Ret>
	struct method_proxy_call_impl<P,C,F,Ret,4>
	{
		typedef typename function_parser<F>::result f_info;

		static Ret call( P * _proxy, C * _obj, F f, PyObject * _arg )
		{
			PyObject * _arg0 = tuple_getitem( _arg, 0 );
			PyObject * _arg1 = tuple_getitem( _arg, 1 );
			PyObject * _arg2 = tuple_getitem( _arg, 2 );

			return (_proxy->*f)( _obj
				, extract_throw<typename f_info::param1>( _arg0 )
				, extract_throw<typename f_info::param2>( _arg1 )
				, extract_throw<typename f_info::param3>( _arg2 )
				);
		}
	};

	template<class P, class C, class F,class Ret>
	struct method_proxy_call_impl<P, C,F,Ret,5>
	{
		typedef typename function_parser<F>::result f_info;

		static Ret call( P * _proxy, C * _obj, F f, PyObject * _arg )
		{
			PyObject * _arg0 = tuple_getitem( _arg, 0 );
			PyObject * _arg1 = tuple_getitem( _arg, 1 );
			PyObject * _arg2 = tuple_getitem( _arg, 2 );
			PyObject * _arg3 = tuple_getitem( _arg, 3 );

			return (_proxy->*f)( _obj
				, extract_throw<typename f_info::param1>( _arg0 )
				, extract_throw<typename f_info::param2>( _arg1 )
				, extract_throw<typename f_info::param3>( _arg2 )
				, extract_throw<typename f_info::param4>( _arg3 )
				);
		}
	};

    template<class P, class C, class F,class Ret>
    struct method_proxy_call_impl<P, C,F,Ret,6>
    {
        typedef typename function_parser<F>::result f_info;

        static Ret call( P * _proxy, C * _obj, F f, PyObject * _arg )
        {
            PyObject * _arg0 = tuple_getitem( _arg, 0 );
            PyObject * _arg1 = tuple_getitem( _arg, 1 );
            PyObject * _arg2 = tuple_getitem( _arg, 2 );
            PyObject * _arg3 = tuple_getitem( _arg, 3 );
            PyObject * _arg4 = tuple_getitem( _arg, 4 );

            return (_proxy->*f)( _obj
				, extract_throw<typename f_info::param1>( _arg0 )
				, extract_throw<typename f_info::param2>( _arg1 )
				, extract_throw<typename f_info::param3>( _arg2 )
				, extract_throw<typename f_info::param4>( _arg3 )
				, extract_throw<typename f_info::param5>( _arg4 )
                );
        }
    };

	template<class P, class C, class F,class Ret>
	struct method_proxy_call_ret_impl
	{
		typedef typename function_parser<F>::result f_info;

		static PyObject * call( P * _proxy, C * _obj, F f, PyObject * _arg )
		{
			Ret result = method_proxy_call_impl<P,C,F,Ret,f_info::arity>::call( _proxy, _obj, f, _arg );

			return ptr_throw( result );
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

