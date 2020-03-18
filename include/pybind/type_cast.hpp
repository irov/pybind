#pragma once

#include "pybind/exports.hpp"
#include "pybind/types.hpp"

#include "pybind/factorable.hpp"

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

    protected:
        static bool type_info_cast( kernel_interface * _kernel, PyObject * _obj, uint32_t _tinfo, uint32_t _tptrinfo, void ** _impl );
    };
    //////////////////////////////////////////////////////////////////////////
    typedef intrusive_ptr<type_cast> type_cast_ptr;
    //////////////////////////////////////////////////////////////////////////
    template<class T>
    type_cast_ptr make_type_cast( kernel_interface * _kernel )
    {
        allocator_interface * allocator = _kernel->get_allocator();

        return allocator->newT<T>();
    }
    //////////////////////////////////////////////////////////////////////////
}