#pragma once

#include "pybind/exports.hpp"
#include "pybind/types.hpp"

#include "pybind/intrusive_ptr_base.hpp"

namespace pybind
{
    //////////////////////////////////////////////////////////////////////////
    class kernel_interface;
    //////////////////////////////////////////////////////////////////////////
    class type_cast
        : public intrusive_ptr_base
    {
    public:
        type_cast();
        ~type_cast() override;

    protected:
        static bool type_info_cast( kernel_interface * _kernel, PyObject * _obj, uint32_t _tinfo, uint32_t _tptrinfo, void ** _impl );
    };
    //////////////////////////////////////////////////////////////////////////
    typedef stdex::intrusive_ptr<type_cast> type_cast_ptr;
    //////////////////////////////////////////////////////////////////////////
    template<class T>
    type_cast_ptr make_type_cast()
    {
        return type_cast_ptr( new T() );
    }
}