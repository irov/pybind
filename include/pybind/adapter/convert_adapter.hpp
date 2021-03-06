#pragma once

#include "pybind/adapter_interface.hpp"

namespace pybind
{
    //////////////////////////////////////////////////////////////////////////
    class convert_adapter_interface
        : public adapter_interface
    {
    public:
        virtual bool convert( pybind::kernel_interface * _kernel, PyObject * _obj, void * _place ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef intrusive_ptr<convert_adapter_interface> convert_adapter_interface_ptr;
    //////////////////////////////////////////////////////////////////////////
    template<class C, class F>
    class convert_adapter
        : public convert_adapter_interface
    {
    public:
        convert_adapter( F _convert, void * _user )
            : m_convert( _convert )
            , m_user( _user )
        {
        }

    public:
        bool convert( pybind::kernel_interface * _kernel, PyObject * _obj, void * _place ) override
        {
            C * c = static_cast<C *>(_place);

            bool result = (*m_convert)(_kernel, _obj, c, m_user);

            return result;
        }

    protected:
        F m_convert;
        void * m_user;
    };
}

