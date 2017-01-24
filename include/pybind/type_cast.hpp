#	pragma once

#	include "pybind/exports.hpp"
#	include "pybind/types.hpp"

#	include "config/stdex.hpp"

namespace pybind
{
	//////////////////////////////////////////////////////////////////////////
    class type_cast
		: public stdex::intrusive_ptr_base<type_cast>
    {
    public:
        type_cast()
        {
        }

        virtual ~type_cast()
        {
        }

    public:
		static void intrusive_ptr_destroy( type_cast * _ptr );

	public:
		void * operator new ( size_t _size );
		void operator delete ( void * _ptr, size_t _size );

    protected:
		static bool type_info_cast( PyObject * _obj, uint32_t _tinfo, uint32_t _tptrinfo, void ** _impl );
    };
	//////////////////////////////////////////////////////////////////////////
    typedef stdex::intrusive_ptr<type_cast> type_cast_ptr;
}