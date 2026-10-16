#pragma once

#include "pybind/exports.hpp"
#include "pybind/types.hpp"
#include "pybind/factorable.hpp"

#include <typeinfo>

namespace pybind
{
    //////////////////////////////////////////////////////////////////////////
    class kernel_interface;
    //////////////////////////////////////////////////////////////////////////
    class type_cast
        : public factorable
    {
    public:
        type_cast();
        ~type_cast() override;

    public:
        // The type a type_cast_result converts, with a pointer to const read as a pointer.
        virtual const std::type_info & get_value_type() const = 0;

    protected:
        static bool type_info_cast( kernel_interface * _kernel, PyObject * _obj, typeid_t _tinfo, typeid_t _tptrinfo, void ** _impl );
    };
    //////////////////////////////////////////////////////////////////////////
    typedef intrusive_ptr<type_cast> type_cast_ptr;
    //////////////////////////////////////////////////////////////////////////
}