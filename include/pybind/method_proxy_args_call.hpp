#	pragma once

#	include "pybind/exports.hpp"
#	include "pybind/types.hpp"
#	include "pybind/system.hpp"
#	include "pybind/extract.hpp"
#	include "pybind/helper.hpp"

#	include "stdex/function_traits.h"

namespace pybind
{
	template<class P, class C, class F,class Ret, int i>
	struct method_proxy_args_call_impl
	{
		static Ret call( pybind::kernel_interface * _kernel, P * _proxy, C * _obj, F f, PyObject * _arg );
	};

	template<class P, class C, class F,class Ret>
	struct method_proxy_args_call_impl<P, C, F, Ret, 2>
	{
		static Ret call( pybind::kernel_interface * _kernel, P * _proxy, C * _obj, F f, PyObject * _arg )
		{
			return (_proxy->*f)( _obj
				, make_args_t( _kernel, _arg, 0 )
				);
		}	
	};

	template<class P, class C, class F,class Ret>
	struct method_proxy_args_call_impl<P, C, F, Ret, 3>
	{
		static Ret call( pybind::kernel_interface * _kernel, P * _proxy, C * _obj, F f, PyObject * _arg )
		{
			return (_proxy->*f)( _obj
				, tuple_getitem_t( _kernel, _arg, 0 )
				, make_args_t( _kernel, _arg, 1 )
				);
		}	
	};

	template<class P, class C, class F,class Ret>
	struct method_proxy_args_call_impl<P, C, F, Ret, 4>
	{
		static Ret call( pybind::kernel_interface * _kernel, P * _proxy, C * _obj, F f, PyObject * _arg )
		{
			return (_proxy->*f)( _obj
				, tuple_getitem_t( _kernel, _arg, 0 )
				, tuple_getitem_t( _kernel, _arg, 1 )
				, make_args_t( _kernel, _arg, 2 )
				);
		}
	};

	template<class P, class C, class F,class Ret>
	struct method_proxy_args_call_impl<P, C, F, Ret, 5>
	{
		static Ret call( pybind::kernel_interface * _kernel, P * _proxy, C * _obj, F f, PyObject * _arg )
		{
			return (_proxy->*f)( _obj
				, tuple_getitem_t( _kernel, _arg, 0 )
				, tuple_getitem_t( _kernel, _arg, 1 )
				, tuple_getitem_t( _kernel, _arg, 2 )
				, make_args_t( _kernel, _arg, 3 )
				);
		}
	};

    template<class P, class C, class F,class Ret>
	struct method_proxy_args_call_impl<P, C, F, Ret, 6>
    {
		static Ret call( pybind::kernel_interface * _kernel, P * _proxy, C * _obj, F f, PyObject * _arg )
        {
            return (_proxy->*f)( _obj
				, tuple_getitem_t( _kernel, _arg, 0 )
				, tuple_getitem_t( _kernel, _arg, 1 )
				, tuple_getitem_t( _kernel, _arg, 2 )
				, tuple_getitem_t( _kernel, _arg, 3 )
				, make_args_t( _kernel, _arg, 4 )
                );
        }
    };

	template<class P, class C, class F, class Ret>
	struct method_proxy_args_call_impl<P, C, F, Ret, 7>
	{
		static Ret call( pybind::kernel_interface * _kernel, P * _proxy, C * _obj, F f, PyObject * _arg )
		{
			return (_proxy->*f)(_obj
				, tuple_getitem_t( _kernel, _arg, 0 )
				, tuple_getitem_t( _kernel, _arg, 1 )
				, tuple_getitem_t( _kernel, _arg, 2 )
				, tuple_getitem_t( _kernel, _arg, 3 )
				, tuple_getitem_t( _kernel, _arg, 4 )
				, make_args_t( _kernel, _arg, 5 )
				);
		}
	};

	template<class P, class C, class F, class Ret>
	struct method_proxy_args_call_impl<P, C, F, Ret, 8>
	{
		static Ret call( pybind::kernel_interface * _kernel, P * _proxy, C * _obj, F f, PyObject * _arg )
		{
			return (_proxy->*f)(_obj
				, tuple_getitem_t( _kernel, _arg, 0 )
				, tuple_getitem_t( _kernel, _arg, 1 )
				, tuple_getitem_t( _kernel, _arg, 2 )
				, tuple_getitem_t( _kernel, _arg, 3 )
				, tuple_getitem_t( _kernel, _arg, 4 )
				, tuple_getitem_t( _kernel, _arg, 5 )
				, make_args_t( _kernel, _arg, 6 )
				);
		}
	};

	template<class P, class C, class F, class Ret>
	struct method_proxy_args_call_impl<P, C, F, Ret, 9>
	{
		static Ret call( pybind::kernel_interface * _kernel, P * _proxy, C * _obj, F f, PyObject * _arg )
		{
			return (_proxy->*f)(_obj
				, tuple_getitem_t( _kernel, _arg, 0 )
				, tuple_getitem_t( _kernel, _arg, 1 )
				, tuple_getitem_t( _kernel, _arg, 2 )
				, tuple_getitem_t( _kernel, _arg, 3 )
				, tuple_getitem_t( _kernel, _arg, 4 )
				, tuple_getitem_t( _kernel, _arg, 5 )
				, tuple_getitem_t( _kernel, _arg, 6 )
				, make_args_t( _kernel, _arg, 7 )
				);
		}
	};

	template<class P, class C, class F, class Ret>
	struct method_proxy_args_call_impl<P, C, F, Ret, 10>
	{
		static Ret call( pybind::kernel_interface * _kernel, P * _proxy, C * _obj, F f, PyObject * _arg )
		{
			return (_proxy->*f)(_obj
				, tuple_getitem_t( _kernel, _arg, 0 )
				, tuple_getitem_t( _kernel, _arg, 1 )
				, tuple_getitem_t( _kernel, _arg, 2 )
				, tuple_getitem_t( _kernel, _arg, 3 )
				, tuple_getitem_t( _kernel, _arg, 4 )
				, tuple_getitem_t( _kernel, _arg, 5 )
				, tuple_getitem_t( _kernel, _arg, 6 )
				, tuple_getitem_t( _kernel, _arg, 7 )
				, make_args_t( _kernel, _arg, 8 )
				);
		}
	};

	template<class P, class C, class F, class Ret>
	struct method_proxy_args_call_impl<P, C, F, Ret, 11>
	{
		static Ret call( pybind::kernel_interface * _kernel, P * _proxy, C * _obj, F f, PyObject * _arg )
		{
			return (_proxy->*f)(_obj
				, tuple_getitem_t( _kernel, _arg, 0 )
				, tuple_getitem_t( _kernel, _arg, 1 )
				, tuple_getitem_t( _kernel, _arg, 2 )
				, tuple_getitem_t( _kernel, _arg, 3 )
				, tuple_getitem_t( _kernel, _arg, 4 )
				, tuple_getitem_t( _kernel, _arg, 5 )
				, tuple_getitem_t( _kernel, _arg, 6 )
				, tuple_getitem_t( _kernel, _arg, 7 )
				, tuple_getitem_t( _kernel, _arg, 8 )
				, make_args_t( _kernel, _arg, 9 )
				);
		}
	};

	template<class P, class C, class F, class Ret>
	struct method_proxy_args_call_impl<P, C, F, Ret, 12>
	{
		static Ret call( pybind::kernel_interface * _kernel, P * _proxy, C * _obj, F f, PyObject * _arg )
		{
			return (_proxy->*f)(_obj
				, tuple_getitem_t( _kernel, _arg, 0 )
				, tuple_getitem_t( _kernel, _arg, 1 )
				, tuple_getitem_t( _kernel, _arg, 2 )
				, tuple_getitem_t( _kernel, _arg, 3 )
				, tuple_getitem_t( _kernel, _arg, 4 )
				, tuple_getitem_t( _kernel, _arg, 5 )
				, tuple_getitem_t( _kernel, _arg, 6 )
				, tuple_getitem_t( _kernel, _arg, 7 )
				, tuple_getitem_t( _kernel, _arg, 8 )
				, tuple_getitem_t( _kernel, _arg, 9 )
				, make_args_t( _kernel, _arg, 10 )
				);
		}
	};

	template<class P, class C, class F, class Ret>
	struct method_proxy_args_call_impl<P, C, F, Ret, 13>
	{
		static Ret call( pybind::kernel_interface * _kernel, P * _proxy, C * _obj, F f, PyObject * _arg )
		{
			return (_proxy->*f)(_obj
				, tuple_getitem_t( _kernel, _arg, 0 )
				, tuple_getitem_t( _kernel, _arg, 1 )
				, tuple_getitem_t( _kernel, _arg, 2 )
				, tuple_getitem_t( _kernel, _arg, 3 )
				, tuple_getitem_t( _kernel, _arg, 4 )
				, tuple_getitem_t( _kernel, _arg, 5 )
				, tuple_getitem_t( _kernel, _arg, 6 )
				, tuple_getitem_t( _kernel, _arg, 7 )
				, tuple_getitem_t( _kernel, _arg, 8 )
				, tuple_getitem_t( _kernel, _arg, 9 )
				, tuple_getitem_t( _kernel, _arg, 10 )
				, make_args_t( _kernel, _arg, 11 )
				);
		}
	};

	template<class P, class C, class F, class Ret>
	struct method_proxy_args_call_impl<P, C, F, Ret, 14>
	{
		static Ret call( pybind::kernel_interface * _kernel, P * _proxy, C * _obj, F f, PyObject * _arg )
		{
			return (_proxy->*f)(_obj
				, tuple_getitem_t( _kernel, _arg, 0 )
				, tuple_getitem_t( _kernel, _arg, 1 )
				, tuple_getitem_t( _kernel, _arg, 2 )
				, tuple_getitem_t( _kernel, _arg, 3 )
				, tuple_getitem_t( _kernel, _arg, 4 )
				, tuple_getitem_t( _kernel, _arg, 5 )
				, tuple_getitem_t( _kernel, _arg, 6 )
				, tuple_getitem_t( _kernel, _arg, 7 )
				, tuple_getitem_t( _kernel, _arg, 8 )
				, tuple_getitem_t( _kernel, _arg, 9 )
				, tuple_getitem_t( _kernel, _arg, 10 )
				, tuple_getitem_t( _kernel, _arg, 11 )
				, make_args_t( _kernel, _arg, 12 )
				);
		}
	};
	
	template<class P, class C, class F, class Ret>
	struct method_proxy_args_call_impl<P, C, F, Ret, 15>
	{
		static Ret call( pybind::kernel_interface * _kernel, P * _proxy, C * _obj, F f, PyObject * _arg )
		{
			return (_proxy->*f)(_obj
				, tuple_getitem_t( _kernel, _arg, 0 )
				, tuple_getitem_t( _kernel, _arg, 1 )
				, tuple_getitem_t( _kernel, _arg, 2 )
				, tuple_getitem_t( _kernel, _arg, 3 )
				, tuple_getitem_t( _kernel, _arg, 4 )
				, tuple_getitem_t( _kernel, _arg, 5 )
				, tuple_getitem_t( _kernel, _arg, 6 )
				, tuple_getitem_t( _kernel, _arg, 7 )
				, tuple_getitem_t( _kernel, _arg, 8 )
				, tuple_getitem_t( _kernel, _arg, 9 )
				, tuple_getitem_t( _kernel, _arg, 10 )
				, tuple_getitem_t( _kernel, _arg, 11 )
				, tuple_getitem_t( _kernel, _arg, 12 )
				, make_args_t( _kernel, _arg, 13 )
				);
		}
	};

	template<class P, class C, class F,class Ret>
	struct method_proxy_args_call_ret_impl
	{
		typedef typename stdex::function_traits<F>::result f_info;

		static PyObject * call( kernel_interface * _kernel, P * _proxy, C * _obj, F f, PyObject * _arg )
		{
			PyObject * py_result = detail::return_operator_t( _kernel, method_proxy_args_call_impl<P, C, F, Ret, f_info::arity>::call( _kernel, _proxy, _obj, f, _arg ) );

			return py_result;
		}
	};

	template<class P, class C, class F>
	struct method_proxy_args_call_ret_impl<P,C,F,void>
	{
		typedef typename stdex::function_traits<F>::result f_info;

		static PyObject * call( kernel_interface * _kernel, P * _proxy, C * _obj, F f, PyObject * _arg )
		{
			method_proxy_args_call_impl<P, C, F, void, f_info::arity>::call( _kernel, _proxy, _obj, f, _arg );

			return ret_none();
		}
	};

	template<class P, class C, class F>
	struct method_proxy_args_call
	{
		typedef typename stdex::function_traits<F>::result f_info;

		static PyObject * call( pybind::kernel_interface * _kernel, P * _proxy, C * _obj, F f, PyObject * _arg )
		{
            uint32_t arg_size = pybind::tuple_size( _arg );
            uint32_t fn_arity = f_info::arity;

            if( arg_size + 2 < fn_arity )
            {
				pybind::throw_exception("invalid proxy method call args is not equal %d != %d\n"
                    , (uint32_t)arg_size + 2
                    , (uint32_t)fn_arity
                    );

                return nullptr;
            }

			PyObject * ret = method_proxy_args_call_ret_impl<P,C,F,typename f_info::ret_type>::call( _kernel, _proxy, _obj, f, _arg );

			return ret;		
		}
	};
}

