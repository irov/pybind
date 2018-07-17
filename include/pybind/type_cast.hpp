#pragma once

#include "pybind/exports.hpp"
#include "pybind/types.hpp"

#include "config/stdex.hpp"

namespace pybind
{
	//////////////////////////////////////////////////////////////////////////
	class kernel_interface;
	//////////////////////////////////////////////////////////////////////////
    class type_cast
		: public stdex::intrusive_ptr_base
    {
    public:
        type_cast()
        {
        }

        virtual ~type_cast()
        {
        }

    public:
		void destroy() override;

	public:
		void * operator new ( size_t _size );
		void operator delete ( void * _ptr, size_t _size );

    protected:
		static bool type_info_cast( kernel_interface * _kernel, PyObject * _obj, uint32_t _tinfo, uint32_t _tptrinfo, void ** _impl );
    };
	//////////////////////////////////////////////////////////////////////////
    typedef stdex::intrusive_ptr<type_cast> type_cast_ptr;
}