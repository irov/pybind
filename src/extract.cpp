#include "pybind/extract.hpp"

#include "pybind/bindable.hpp"

#include "pybind/object.hpp"
#include "pybind/list.hpp"
#include "pybind/tuple.hpp"
#include "pybind/dict.hpp"

namespace pybind
{
    //////////////////////////////////////////////////////////////////////////
    bool extract_value( kernel_interface * _kernel, PyObject * _obj, bool & _value, bool _nothrow )
    {
        (void)_kernel;
        (void)_nothrow;

        bool successful = _kernel->extract_bool( _obj, _value );

        return successful;
    }
    //////////////////////////////////////////////////////////////////////////
    bool extract_value( kernel_interface * _kernel, PyObject * _obj, int8_t & _value, bool _nothrow )
    {
        (void)_kernel;
        (void)_nothrow;

        bool successful = _kernel->extract_int8( _obj, _value );

        return successful;
    }
    //////////////////////////////////////////////////////////////////////////
    bool extract_value( kernel_interface * _kernel, PyObject * _obj, int16_t & _value, bool _nothrow )
    {
        (void)_kernel;
        (void)_nothrow;

        bool successful = _kernel->extract_int16( _obj, _value );

        return successful;
    }
    //////////////////////////////////////////////////////////////////////////
    bool extract_value( kernel_interface * _kernel, PyObject * _obj, int32_t & _value, bool _nothrow )
    {
        (void)_kernel;
        (void)_nothrow;

        bool successful = _kernel->extract_int32( _obj, _value );

        return successful;
    }
    //////////////////////////////////////////////////////////////////////////
    bool extract_value( kernel_interface * _kernel, PyObject * _obj, int64_t & _value, bool _nothrow )
    {
        (void)_kernel;
        (void)_nothrow;

        bool successful = _kernel->extract_int64( _obj, _value );

        return successful;
    }
    //////////////////////////////////////////////////////////////////////////
    bool extract_value( kernel_interface * _kernel, PyObject * _obj, uint8_t & _value, bool _nothrow )
    {
        (void)_kernel;
        (void)_nothrow;

        bool successful = _kernel->extract_uint8( _obj, _value );

        return successful;
    }
    //////////////////////////////////////////////////////////////////////////
    bool extract_value( kernel_interface * _kernel, PyObject * _obj, uint16_t & _value, bool _nothrow )
    {
        (void)_kernel;
        (void)_nothrow;

        bool successful = _kernel->extract_uint16( _obj, _value );

        return successful;
    }
    //////////////////////////////////////////////////////////////////////////
    bool extract_value( kernel_interface * _kernel, PyObject * _obj, uint32_t & _value, bool _nothrow )
    {
        (void)_kernel;
        (void)_nothrow;

        bool successful = _kernel->extract_uint32( _obj, _value );

        return successful;
    }
    //////////////////////////////////////////////////////////////////////////
    bool extract_value( kernel_interface * _kernel, PyObject * _obj, uint64_t & _value, bool _nothrow )
    {
        (void)_kernel;
        (void)_nothrow;

        bool successful = _kernel->extract_uint64( _obj, _value );

        return successful;
    }
    //////////////////////////////////////////////////////////////////////////
    bool extract_value( kernel_interface * _kernel, PyObject * _obj, wchar_t & _value, bool _nothrow )
    {
        (void)_kernel;
        (void)_nothrow;

        bool successful = _kernel->extract_wchar( _obj, _value );

        return successful;
    }
    //////////////////////////////////////////////////////////////////////////
    bool extract_value( kernel_interface * _kernel, PyObject * _obj, float & _value, bool _nothrow )
    {
        (void)_kernel;
        (void)_nothrow;

        bool successful = _kernel->extract_float( _obj, _value );

        return successful;
    }
    //////////////////////////////////////////////////////////////////////////
    bool extract_value( kernel_interface * _kernel, PyObject * _obj, double & _value, bool _nothrow )
    {
        (void)_kernel;
        (void)_nothrow;

        bool successful = _kernel->extract_double( _obj, _value );

        return successful;
    }
    //////////////////////////////////////////////////////////////////////////
    bool extract_value( kernel_interface * _kernel, PyObject * _obj, const char *& _value, bool _nothrow )
    {
        (void)_kernel;
        (void)_nothrow;

        if( _obj == nullptr )
        {
            return false;
        }

        if( _kernel->string_check( _obj ) == true )
        {
            const char * ch_buff = _kernel->string_to_char( _obj );

            if( ch_buff == nullptr )
            {
                return false;
            }

            _value = ch_buff;
        }
        else
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool extract_value( kernel_interface * _kernel, PyObject * _obj, const wchar_t *& _value, bool _nothrow )
    {
        (void)_kernel;
        (void)_nothrow;

        if( _obj == nullptr )
        {
            return false;
        }

        if( _kernel->unicode_check( _obj ) == true )
        {
            const wchar_t * ch_buff = _kernel->unicode_to_wchar( _obj );

            if( ch_buff == nullptr )
            {
                return false;
            }

            _value = ch_buff;
        }
        else
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool extract_value( kernel_interface * _kernel, PyObject * _obj, PyObject *& _value, bool _nothrow )
    {
        (void)_kernel;
        (void)_nothrow;

        if( _obj == nullptr )
        {
            return false;
        }

        _value = _obj;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool extract_value( kernel_interface * _kernel, PyObject * _obj, PyTypeObject *& _value, bool _nothrow )
    {
        (void)_kernel;
        (void)_nothrow;

        if( _obj == nullptr )
        {
            return false;
        }

        _value = (PyTypeObject *)_obj;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool extract_value( kernel_interface * _kernel, PyObject * _obj, pybind::object & _value, bool _nothrow )
    {
        (void)_nothrow;

        if( _obj == nullptr )
        {
            return false;
        }

        _value = pybind::object( _kernel, _obj );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool extract_value( kernel_interface * _kernel, PyObject * _obj, pybind::list & _value, bool _nothrow )
    {
        (void)_nothrow;

        if( _obj == nullptr )
        {
            return false;
        }

        if( _kernel->list_check( _obj ) == false )
        {
            return false;
        }

        _value = pybind::list( _kernel, _obj );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool extract_value( kernel_interface * _kernel, PyObject * _obj, pybind::tuple & _value, bool _nothrow )
    {
        (void)_nothrow;

        if( _obj == nullptr )
        {
            return false;
        }

        if( _kernel->tuple_check( _obj ) == false )
        {
            return false;
        }

        _value = pybind::tuple( _kernel, _obj );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool extract_value( kernel_interface * _kernel, PyObject * _obj, pybind::dict & _value, bool _nothrow )
    {
        (void)_nothrow;

        if( _obj == nullptr )
        {
            return false;
        }

        if( _kernel->dict_check( _obj ) == false )
        {
            return false;
        }

        _value = pybind::dict( _kernel, _obj );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * ptr_throw_i( kernel_interface * _kernel, bool _value )
    {
        return _kernel->ptr_bool( _value );
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * ptr_throw_i( kernel_interface * _kernel, int8_t _value )
    {
        return _kernel->ptr_int8( _value );
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * ptr_throw_i( kernel_interface * _kernel, int16_t _value )
    {
        return _kernel->ptr_int16( _value );
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * ptr_throw_i( kernel_interface * _kernel, int32_t _value )
    {
        return _kernel->ptr_int32( _value );
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * ptr_throw_i( kernel_interface * _kernel, int64_t _value )
    {
        return _kernel->ptr_int64( _value );
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * ptr_throw_i( kernel_interface * _kernel, uint8_t _value )
    {
        return _kernel->ptr_uint8( _value );
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * ptr_throw_i( kernel_interface * _kernel, uint16_t _value )
    {
        (void)_kernel;

        return _kernel->ptr_uint16( _value );
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * ptr_throw_i( kernel_interface * _kernel, uint32_t _value )
    {
        return _kernel->ptr_uint32( _value );
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * ptr_throw_i( kernel_interface * _kernel, uint64_t _value )
    {
        return _kernel->ptr_uint64( _value );
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * ptr_throw_i( kernel_interface * _kernel, wchar_t _value )
    {
        (void)_kernel;

        wchar_t tmp[2];
        tmp[0] = _value;
        tmp[1] = 0;

        return _kernel->unicode_from_wchar( tmp );
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * ptr_throw_i( kernel_interface * _kernel, float _value )
    {
        (void)_kernel;

        return _kernel->ptr_float( _value );
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * ptr_throw_i( kernel_interface * _kernel, double _value )
    {
        (void)_kernel;

        return _kernel->ptr_double( _value );
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * ptr_throw_i( kernel_interface * _kernel, char * _value )
    {
        return _kernel->ptr_string( _value );
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * ptr_throw_i( kernel_interface * _kernel, wchar_t * _value )
    {
        return _kernel->ptr_unicode( _value );
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * ptr_throw_i( kernel_interface * _kernel, const char * _value )
    {
        return _kernel->ptr_string( _value );
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * ptr_throw_i( kernel_interface * _kernel, const wchar_t * _value )
    {
        return _kernel->ptr_unicode( _value );
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * ptr_throw_i( kernel_interface * _kernel, PyObject * _value )
    {
        _kernel->incref( _value );

        return _value;
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * ptr_throw_i( kernel_interface * _kernel, PyTypeObject * _value )
    {
        _kernel->incref( (PyObject *)_value );

        return (PyObject *)_value;
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * ptr_throw_i( kernel_interface * _kernel, pybind::bindable * _value )
    {
        if( _value == nullptr )
        {
            return _kernel->ret_none();
        }

        PyObject * py_obj = _value->getEmbed( _kernel );

        return py_obj;
    }
    ////////////////////////////////////////////////////////////////////////////
    PyObject * ptr_throw_i( kernel_interface * _kernel, const pybind::object & _value )
    {
        PyObject * obj = _value.ptr();

        _kernel->incref( obj );

        return obj;
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * ptr_throw_i( kernel_interface * _kernel, const pybind::list & _value )
    {
        PyObject * obj = _value.ptr();

        _kernel->incref( obj );

        return obj;
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * ptr_throw_i( kernel_interface * _kernel, const pybind::tuple & _value )
    {
        PyObject * obj = _value.ptr();

        _kernel->incref( obj );

        return obj;
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * ptr_throw_i( kernel_interface * _kernel, const pybind::dict & _value )
    {
        PyObject * obj = _value.ptr();

        _kernel->incref( obj );

        return obj;
    }
}