#pragma once

#include "pybind/method_interface.hpp"

#include "pybind/call/method_call.hpp"
#include "pybind/call/method_kernel_call.hpp"
#include "pybind/call/method_args_call.hpp"
#include "pybind/call/method_proxy_call.hpp"
#include "pybind/call/method_proxy_kernel_call.hpp"
#include "pybind/call/method_proxy_kernel_args_call.hpp"
#include "pybind/call/method_proxy_args_call.hpp"

#include "pybind/call/function_proxy_call.hpp"
#include "pybind/call/function_proxy_kernel_call.hpp"
#include "pybind/call/function_proxy_args_call.hpp"
#include "pybind/call/function_proxy_kernel_args_call.hpp"

#include "stdex/function_traits.h"

#include <type_traits>

namespace pybind
{
    //////////////////////////////////////////////////////////////////////////
    template<class C, class F>
    class method_adapter
        : public method_adapter_interface
    {
    public:
        method_adapter( const char * _name, F _fn )
            : method_adapter_interface( _name )
            , m_fn( _fn )
        {
        }

        ~method_adapter() override
        {
        }

    protected:
        PyObject * call( kernel_interface * _kernel, void * _impl, const class_type_scope_interface_ptr & _scope, PyObject * _args, PyObject * _kwds ) override
        {
            (void)_kwds;

            C * self = _kernel->meta_cast_class_t<C>( _impl, _scope );

            PyObject * ret = method_call<C, F>::call( _kernel, self, m_fn, _args );

            return ret;
        }

    protected:
        F m_fn;
    };
    //////////////////////////////////////////////////////////////////////////
    template<class C, class F>
    class method_adapter_kernel
        : public method_adapter_interface
    {
    public:
        method_adapter_kernel( const char * _name, F _fn )
            : method_adapter_interface( _name )
            , m_fn( _fn )
        {
        }

        ~method_adapter_kernel() override
        {
        }

    protected:
        PyObject * call( kernel_interface * _kernel, void * _impl, const class_type_scope_interface_ptr & _scope, PyObject * _args, PyObject * _kwds ) override
        {
            (void)_kwds;

            C * self = _kernel->meta_cast_class_t<C>( _impl, _scope );

            PyObject * ret = method_kernel_call<C, F>::call( _kernel, self, m_fn, _args );

            return ret;
        }

    protected:
        F m_fn;
    };
    //////////////////////////////////////////////////////////////////////////
    template<class C, class F>
    class method_adapter_args
        : public method_adapter_interface
    {
    public:
        method_adapter_args( const char * _name, F _fn )
            : method_adapter_interface( _name )
            , m_fn( _fn )
        {
        }

        ~method_adapter_args() override
        {
        }

    protected:
        PyObject * call( kernel_interface * _kernel, void * _impl, const class_type_scope_interface_ptr & _scope, PyObject * _args, PyObject * _kwds ) override
        {
            (void)_kwds;

            C * self = _kernel->meta_cast_class_t<C>( _impl, _scope );

            PyObject * ret = method_args_call<C, F>::call( _kernel, self, m_fn, _args );

            return ret;
        }

    protected:
        F m_fn;
    };
    //////////////////////////////////////////////////////////////////////////
    template<class C, class F>
    class method_adapter_deprecated
        : public method_adapter<C, F>
    {
    public:
        method_adapter_deprecated( const char * _name, F _fn, const char * _doc )
            : method_adapter<C, F>( _name, _fn )
            , m_doc( _doc )
        {
        }

        ~method_adapter_deprecated() override
        {
        }

    protected:
        PyObject * call( kernel_interface * _kernel, void * _impl, const class_type_scope_interface_ptr & _scope, PyObject * _args, PyObject * _kwds ) override
        {
            const char * scopeName = _kernel->get_class_type_info_t<C>();

            const char * name = this->getName();

            _kernel->error_traceback( "method '%s::%s' deprecated '%s'"
                , scopeName
                , name
                , m_doc
            );

            PyObject * ret = method_adapter<C, F>::call( _kernel, _impl, _scope, _args, _kwds );

            return ret;
        }

    protected:
        const char * m_doc;
    };
    //////////////////////////////////////////////////////////////////////////
    template<class C, class P, class F>
    class method_adapter_proxy
        : public method_adapter_interface
    {
    public:
        method_adapter_proxy( const char * _name, F _fn, P * _proxy )
            : method_adapter_interface( _name )
            , m_fn( _fn )
            , m_proxy( _proxy )
        {
        }

        ~method_adapter_proxy() override
        {
        }

    protected:
        PyObject * call( kernel_interface * _kernel, void * _impl, const class_type_scope_interface_ptr & _scope, PyObject * _args, PyObject * _kwds ) override
        {
            (void)_kwds;

            C * self = _kernel->meta_cast_class_t<C>( _impl, _scope );

            PyObject * ret = method_proxy_call<P, C, F>::call( _kernel, m_proxy, self, m_fn, _args );

            return ret;
        }

    protected:
        F m_fn;
        P * m_proxy;
    };
    //////////////////////////////////////////////////////////////////////////
    template<class C, class P, class F>
    class method_adapter_proxy_kernel
        : public method_adapter_interface
    {
    public:
        method_adapter_proxy_kernel( const char * _name, F _fn, P * _proxy )
            : method_adapter_interface( _name )
            , m_fn( _fn )
            , m_proxy( _proxy )
        {
        }

        ~method_adapter_proxy_kernel() override
        {
        }

    protected:
        PyObject * call( kernel_interface * _kernel, void * _impl, const class_type_scope_interface_ptr & _scope, PyObject * _args, PyObject * _kwds ) override
        {
            (void)_kwds;

            C * self = _kernel->meta_cast_class_t<C>( _impl, _scope );

            PyObject * ret = method_proxy_kernel_call<P, C, F>::call( _kernel, m_proxy, self, m_fn, _args );

            return ret;
        }

    protected:
        F m_fn;
        P * m_proxy;
    };
    //////////////////////////////////////////////////////////////////////////
    template<class C, class P, class F>
    class method_adapter_proxy_kernel_args
        : public method_adapter_interface
    {
    public:
        method_adapter_proxy_kernel_args( const char * _name, F _fn, P * _proxy )
            : method_adapter_interface( _name )
            , m_fn( _fn )
            , m_proxy( _proxy )
        {
        }

        ~method_adapter_proxy_kernel_args() override
        {
        }

    protected:
        PyObject * call( kernel_interface * _kernel, void * _impl, const class_type_scope_interface_ptr & _scope, PyObject * _args, PyObject * _kwds ) override
        {
            (void)_kwds;

            C * self = _kernel->meta_cast_class_t<C>( _impl, _scope );

            PyObject * ret = method_proxy_kernel_args_call<P, C, F>::call( _kernel, m_proxy, self, m_fn, _args );

            return ret;
        }

    protected:
        F m_fn;
        P * m_proxy;
    };
    //////////////////////////////////////////////////////////////////////////
    template<class C, class P, class F>
    class method_adapter_proxy_deprecated
        : public method_adapter_interface
    {
    public:
        method_adapter_proxy_deprecated( const char * _name, F _fn, P * _proxy, const char * _doc )
            : method_adapter_interface( _name )
            , m_fn( _fn )
            , m_proxy( _proxy )
            , m_doc( _doc )
        {
        }

        ~method_adapter_proxy_deprecated() override
        {
        }

    protected:
        PyObject * call( kernel_interface * _kernel, void * _impl, const class_type_scope_interface_ptr & _scope, PyObject * _args, PyObject * _kwds ) override
        {
            (void)_kwds;

            const char * name = this->getName();

            _kernel->error_traceback( "function '%s' deprecated '%s'"
                , name
                , m_doc
            );

            C * self = _kernel->meta_cast_class_t<C>( _impl, _scope );

            PyObject * ret = method_proxy_call<P, C, F>::call( _kernel, m_proxy, self, m_fn, _args );

            return ret;
        }

    protected:
        F m_fn;
        P * m_proxy;
        const char * m_doc;
    };
    //////////////////////////////////////////////////////////////////////////
    template<class C, class P, class F>
    class method_adapter_proxy_args
        : public method_adapter_interface
    {
    public:
        method_adapter_proxy_args( const char * _name, F _fn, P * _proxy )
            : method_adapter_interface( _name )
            , m_fn( _fn )
            , m_proxy( _proxy )
        {
        }

        ~method_adapter_proxy_args() override
        {
        }

    protected:
        PyObject * call( kernel_interface * _kernel, void * _impl, const class_type_scope_interface_ptr & _scope, PyObject * _args, PyObject * _kwds ) override
        {
            (void)_kwds;

            C * self = _kernel->meta_cast_class_t<C>( _impl, _scope );

            PyObject * ret = method_proxy_args_call<P, C, F>::call( _kernel, m_proxy, self, m_fn, _args );

            return ret;
        }

    protected:
        F m_fn;
        P * m_proxy;
    };
    //////////////////////////////////////////////////////////////////////////
    template<class C, class P, class F>
    class method_adapter_proxy_native
        : public method_adapter_interface
    {
    private:
        typedef typename stdex::function_traits<F>::result f_info;

        static_assert(f_info::method == true, "[pybind] native method proxy bind requires a method");
        static_assert(std::is_convertible<P *, typename f_info::class_type *>::value == true, "[pybind] native method proxy class type is incompatible");
        static_assert(f_info::arity == 3, "[pybind] native method proxy must have object, args and kwds arguments");
        static_assert(std::is_convertible<C *, typename f_info::template iterator_param<0>>::value == true, "[pybind] native method proxy object argument type is incompatible");
        static_assert(std::is_same<typename f_info::template iterator_param<1>, PyObject *>::value == true, "[pybind] native method proxy second argument must be PyObject * args");
        static_assert(std::is_same<typename f_info::template iterator_param<2>, PyObject *>::value == true, "[pybind] native method proxy third argument must be PyObject * kwds");
        static_assert(std::is_same<typename f_info::ret_type, PyObject *>::value == true, "[pybind] native method proxy must return PyObject *");

    public:
        method_adapter_proxy_native( const char * _name, F _fn, P * _proxy )
            : method_adapter_interface( _name )
            , m_fn( _fn )
            , m_proxy( _proxy )
        {
        }

        ~method_adapter_proxy_native() override
        {
        }

    protected:
        PyObject * call( kernel_interface * _kernel, void * _impl, const class_type_scope_interface_ptr & _scope, PyObject * _args, PyObject * _kwds ) override
        {
            C * self = _kernel->meta_cast_class_t<C>( _impl, _scope );

            PyObject * ret = (m_proxy->*m_fn)(self, _args, _kwds);

            return ret;
        }

    protected:
        F m_fn;
        P * m_proxy;
    };
    //////////////////////////////////////////////////////////////////////////
    template<class C, class P, class F>
    class method_adapter_proxy_native_kernel
        : public method_adapter_interface
    {
    private:
        typedef typename stdex::function_traits<F>::result f_info;

        static_assert(f_info::method == true, "[pybind] native method proxy kernel bind requires a method");
        static_assert(std::is_convertible<P *, typename f_info::class_type *>::value == true, "[pybind] native method proxy class type is incompatible");
        static_assert(f_info::arity == 4, "[pybind] native method proxy kernel must have kernel, object, args and kwds arguments");
        static_assert(std::is_same<typename f_info::template iterator_param<0>, pybind::kernel_interface *>::value == true, "[pybind] native method proxy kernel first argument must be kernel_interface *");
        static_assert(std::is_convertible<C *, typename f_info::template iterator_param<1>>::value == true, "[pybind] native method proxy object argument type is incompatible");
        static_assert(std::is_same<typename f_info::template iterator_param<2>, PyObject *>::value == true, "[pybind] native method proxy kernel third argument must be PyObject * args");
        static_assert(std::is_same<typename f_info::template iterator_param<3>, PyObject *>::value == true, "[pybind] native method proxy kernel fourth argument must be PyObject * kwds");
        static_assert(std::is_same<typename f_info::ret_type, PyObject *>::value == true, "[pybind] native method proxy kernel must return PyObject *");

    public:
        method_adapter_proxy_native_kernel( const char * _name, F _fn, P * _proxy )
            : method_adapter_interface( _name )
            , m_fn( _fn )
            , m_proxy( _proxy )
        {
        }

        ~method_adapter_proxy_native_kernel() override
        {
        }

    protected:
        PyObject * call( kernel_interface * _kernel, void * _impl, const class_type_scope_interface_ptr & _scope, PyObject * _args, PyObject * _kwds ) override
        {
            C * self = _kernel->meta_cast_class_t<C>( _impl, _scope );

            PyObject * ret = (m_proxy->*m_fn)(_kernel, self, _args, _kwds);

            return ret;
        }

    protected:
        F m_fn;
        P * m_proxy;
    };
    //////////////////////////////////////////////////////////////////////////
    template<class C, class F>
    class method_adapter_proxy_function
        : public method_adapter_interface
    {
    public:
        method_adapter_proxy_function( const char * _name, F _fn )
            : method_adapter_interface( _name )
            , m_fn( _fn )
        {
        }

        ~method_adapter_proxy_function() override
        {
        }

    protected:
        PyObject * call( kernel_interface * _kernel, void * _impl, const class_type_scope_interface_ptr & _scope, PyObject * _args, PyObject * _kwds ) override
        {
            (void)_kwds;

            C * self = _kernel->meta_cast_class_t<C>( _impl, _scope );

            PyObject * ret = function_proxy_call<F, C>::call( _kernel, m_fn, _args, self );

            return ret;
        }

    protected:
        F m_fn;
    };
    //////////////////////////////////////////////////////////////////////////
    template<class C, class F>
    class method_adapter_proxy_function_deprecated
        : public method_adapter_interface
    {
    public:
        method_adapter_proxy_function_deprecated( const char * _name, F _fn, const char * _doc )
            : method_adapter_interface( _name )
            , m_fn( _fn )
            , m_doc( _doc )
        {
        }

        ~method_adapter_proxy_function_deprecated() override
        {
        }

    protected:
        PyObject * call( kernel_interface * _kernel, void * _impl, const class_type_scope_interface_ptr & _scope, PyObject * _args, PyObject * _kwds ) override
        {
            (void)_kwds;

            const char * scopeName = _kernel->get_class_type_info_t<C>();

            const char * name = this->getName();

            _kernel->error_traceback( "method '%s::%s' deprecated '%s'"
                , scopeName
                , name
                , m_doc
            );

            C * self = _kernel->meta_cast_class_t<C>( _impl, _scope );

            PyObject * ret = function_proxy_call<F, C>::call( _kernel, m_fn, _args, self );

            return ret;
        }

    protected:
        F m_fn;

        const char * m_doc;
    };
    //////////////////////////////////////////////////////////////////////////
    template<class C, class F>
    class method_adapter_proxy_function_args
        : public method_adapter_interface
    {
    public:
        method_adapter_proxy_function_args( const char * _name, F _fn )
            : method_adapter_interface( _name )
            , m_fn( _fn )
        {
        }

        ~method_adapter_proxy_function_args() override
        {
        }

    protected:
        PyObject * call( kernel_interface * _kernel, void * _impl, const class_type_scope_interface_ptr & _scope, PyObject * _args, PyObject * _kwds ) override
        {
            (void)_kwds;

            C * self = _kernel->meta_cast_class_t<C>( _impl, _scope );

            PyObject * ret = function_proxy_args_call<F, C>::call( _kernel, self, m_fn, _args );

            return ret;
        }

    protected:
        F m_fn;
    };
    //////////////////////////////////////////////////////////////////////////
    template<class C, class F>
    class method_adapter_proxy_function_kernel_args
        : public method_adapter_interface
    {
    public:
        method_adapter_proxy_function_kernel_args( const char * _name, F _fn )
            : method_adapter_interface( _name )
            , m_fn( _fn )
        {
        }

        ~method_adapter_proxy_function_kernel_args() override
        {
        }

    protected:
        PyObject * call( kernel_interface * _kernel, void * _impl, const class_type_scope_interface_ptr & _scope, PyObject * _args, PyObject * _kwds ) override
        {
            (void)_kwds;

            C * self = _kernel->meta_cast_class_t<C>( _impl, _scope );

            PyObject * ret = function_proxy_kernel_args_call<C, F>::call( _kernel, self, m_fn, _args );

            return ret;
        }

    protected:
        F m_fn;
    };
    //////////////////////////////////////////////////////////////////////////
    template<class C, class F>
    class method_adapter_proxy_function_kernel
        : public method_adapter_interface
    {
    public:
        method_adapter_proxy_function_kernel( const char * _name, F _fn )
            : method_adapter_interface( _name )
            , m_fn( _fn )
        {
        }

        ~method_adapter_proxy_function_kernel() override
        {
        }

    protected:
        PyObject * call( kernel_interface * _kernel, void * _impl, const class_type_scope_interface_ptr & _scope, PyObject * _args, PyObject * _kwds ) override
        {
            (void)_kwds;

            C * self = _kernel->meta_cast_class_t<C>( _impl, _scope );

            PyObject * ret = function_proxy_kernel_call<C, F>::call( _kernel, self, m_fn, _args );

            return ret;
        }

    protected:
        F m_fn;
    };
    //////////////////////////////////////////////////////////////////////////
    template<class C, class F>
    class method_adapter_native
        : public method_adapter_interface
    {
    private:
        typedef typename stdex::function_traits<F>::result f_info;

        static_assert(f_info::method == true, "[pybind] native method bind requires a method");
        static_assert(std::is_convertible<C *, typename f_info::class_type *>::value == true, "[pybind] native method class type is incompatible");
        static_assert(f_info::arity == 2, "[pybind] native method must have args and kwds arguments");
        static_assert(std::is_same<typename f_info::template iterator_param<0>, PyObject *>::value == true, "[pybind] native method first argument must be PyObject * args");
        static_assert(std::is_same<typename f_info::template iterator_param<1>, PyObject *>::value == true, "[pybind] native method second argument must be PyObject * kwds");
        static_assert(std::is_same<typename f_info::ret_type, PyObject *>::value == true, "[pybind] native method must return PyObject *");

    public:
        method_adapter_native( const char * _name, F _fn )
            : method_adapter_interface( _name )
            , m_fn( _fn )
        {
        }

        ~method_adapter_native() override
        {
        }

    protected:
        PyObject * call( kernel_interface * _kernel, void * _impl, const class_type_scope_interface_ptr & _scope, PyObject * _args, PyObject * _kwds ) override
        {
            C * self = _kernel->meta_cast_class_t<C>( _impl, _scope );

            PyObject * ret = (self->*m_fn)(_args, _kwds);

            return ret;
        }

    protected:
        F m_fn;
    };
    //////////////////////////////////////////////////////////////////////////
    template<class C, class F>
    class method_adapter_native_kernel
        : public method_adapter_interface
    {
    private:
        typedef typename stdex::function_traits<F>::result f_info;

        static_assert(f_info::method == true, "[pybind] native method kernel bind requires a method");
        static_assert(std::is_convertible<C *, typename f_info::class_type *>::value == true, "[pybind] native method class type is incompatible");
        static_assert(f_info::arity == 3, "[pybind] native method kernel must have kernel, args and kwds arguments");
        static_assert(std::is_same<typename f_info::template iterator_param<0>, pybind::kernel_interface *>::value == true, "[pybind] native method kernel first argument must be kernel_interface *");
        static_assert(std::is_same<typename f_info::template iterator_param<1>, PyObject *>::value == true, "[pybind] native method kernel second argument must be PyObject * args");
        static_assert(std::is_same<typename f_info::template iterator_param<2>, PyObject *>::value == true, "[pybind] native method kernel third argument must be PyObject * kwds");
        static_assert(std::is_same<typename f_info::ret_type, PyObject *>::value == true, "[pybind] native method kernel must return PyObject *");

    public:
        method_adapter_native_kernel( const char * _name, F _fn )
            : method_adapter_interface( _name )
            , m_fn( _fn )
        {
        }

        ~method_adapter_native_kernel() override
        {
        }

    protected:
        PyObject * call( kernel_interface * _kernel, void * _impl, const class_type_scope_interface_ptr & _scope, PyObject * _args, PyObject * _kwds ) override
        {
            C * self = _kernel->meta_cast_class_t<C>( _impl, _scope );

            PyObject * ret = (self->*m_fn)(_kernel, _args, _kwds);

            return ret;
        }

    protected:
        F m_fn;
    };
    //////////////////////////////////////////////////////////////////////////
    template<class C, class F>
    class method_adapter_static_native
        : public method_adapter_interface
    {
    private:
        typedef typename stdex::function_traits<F>::result f_info;

        static_assert(f_info::method == false, "[pybind] static native method bind requires a function");
        static_assert(f_info::arity == 3, "[pybind] static native method must have object, args and kwds arguments");
        static_assert(std::is_convertible<C *, typename f_info::template iterator_param<0>>::value == true, "[pybind] static native method object argument type is incompatible");
        static_assert(std::is_same<typename f_info::template iterator_param<1>, PyObject *>::value == true, "[pybind] static native method second argument must be PyObject * args");
        static_assert(std::is_same<typename f_info::template iterator_param<2>, PyObject *>::value == true, "[pybind] static native method third argument must be PyObject * kwds");
        static_assert(std::is_same<typename f_info::ret_type, PyObject *>::value == true, "[pybind] static native method must return PyObject *");

    public:
        method_adapter_static_native( const char * _name, F _fn )
            : method_adapter_interface( _name )
            , m_fn( _fn )
        {
        }

        ~method_adapter_static_native() override
        {
        }

    protected:
        PyObject * call( kernel_interface * _kernel, void * _impl, const class_type_scope_interface_ptr & _scope, PyObject * _args, PyObject * _kwds ) override
        {
            C * self = _kernel->meta_cast_class_t<C>( _impl, _scope );

            PyObject * ret = (*m_fn)(self, _args, _kwds);

            return ret;
        }

    protected:
        F m_fn;
    };
    //////////////////////////////////////////////////////////////////////////
    template<class C, class F>
    class method_adapter_static_native_kernel
        : public method_adapter_interface
    {
    private:
        typedef typename stdex::function_traits<F>::result f_info;

        static_assert(f_info::method == false, "[pybind] static native method kernel bind requires a function");
        static_assert(f_info::arity == 4, "[pybind] static native method kernel must have kernel, object, args and kwds arguments");
        static_assert(std::is_same<typename f_info::template iterator_param<0>, pybind::kernel_interface *>::value == true, "[pybind] static native method kernel first argument must be kernel_interface *");
        static_assert(std::is_convertible<C *, typename f_info::template iterator_param<1>>::value == true, "[pybind] static native method object argument type is incompatible");
        static_assert(std::is_same<typename f_info::template iterator_param<2>, PyObject *>::value == true, "[pybind] static native method kernel third argument must be PyObject * args");
        static_assert(std::is_same<typename f_info::template iterator_param<3>, PyObject *>::value == true, "[pybind] static native method kernel fourth argument must be PyObject * kwds");
        static_assert(std::is_same<typename f_info::ret_type, PyObject *>::value == true, "[pybind] static native method kernel must return PyObject *");

    public:
        method_adapter_static_native_kernel( const char * _name, F _fn )
            : method_adapter_interface( _name )
            , m_fn( _fn )
        {
        }

        ~method_adapter_static_native_kernel() override
        {
        }

    protected:
        PyObject * call( kernel_interface * _kernel, void * _impl, const class_type_scope_interface_ptr & _scope, PyObject * _args, PyObject * _kwds ) override
        {
            C * self = _kernel->meta_cast_class_t<C>( _impl, _scope );

            PyObject * ret = (*m_fn)(_kernel, self, _args, _kwds);

            return ret;
        }

    protected:
        F m_fn;
    };
    //////////////////////////////////////////////////////////////////////////
}

