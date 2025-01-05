#include "pybind/helper.hpp"
#include "pybind/tuple.hpp"

namespace pybind
{
    //////////////////////////////////////////////////////////////////////////
    detail::extract_operator_t list_getitem_t( kernel_interface * _kernel, PyObject * _list, uint32_t _it )
    {
        PyObject * py_item = _kernel->list_getitem( _list, _it );

        return detail::extract_operator_t( _kernel, py_item );
    }
    //////////////////////////////////////////////////////////////////////////
    bool list_setitem_i( kernel_interface * _kernel, PyObject * _list, uint32_t _it, const detail::import_operator_t & _item )
    {
        bool result = _kernel->list_setitem( _list, _it, _item );

        return result;
    }
    //////////////////////////////////////////////////////////////////////////
    bool list_appenditem_i( kernel_interface * _kernel, PyObject * _obj, const detail::import_operator_t & _item )
    {
        bool result = _kernel->list_appenditem( _obj, _item );

        return result;
    }
    //////////////////////////////////////////////////////////////////////////
    bool dict_setstring_i( kernel_interface * _kernel, PyObject * _dict, const char * _key, const detail::import_operator_t & _value )
    {
        bool result = _kernel->dict_setstring( _dict, _key, _value );

        return result;
    }
    //////////////////////////////////////////////////////////////////////////
    bool dict_setobject_i( kernel_interface * _kernel, PyObject * _dict, PyObject * _key, const detail::import_operator_t & _value )
    {
        bool result = _kernel->dict_set( _dict, _key, _value );

        return result;
    }
    //////////////////////////////////////////////////////////////////////////
    args make_args_t( kernel_interface * _kernel, PyObject * _tuple, uint32_t _size )
    {
        PyObject * py_args = _kernel->tuple_slice_tail( _tuple, _size );

        return args( _kernel, py_args, pybind::borrowed );
    }
    //////////////////////////////////////////////////////////////////////////
    detail::extract_operator_t tuple_getitem_t( kernel_interface * _kernel, PyObject * _tuple, uint32_t _it )
    {
        PyObject * py_item = _kernel->tuple_getitem( _tuple, _it );

        return detail::extract_operator_t( _kernel, py_item );
    }
    //////////////////////////////////////////////////////////////////////////
    bool tuple_setitem_i( kernel_interface * _kernel, PyObject * _tuple, uint32_t _it, const detail::extract_operator_t & _item )
    {
        bool result = _kernel->tuple_setitem( _tuple, _it, _item );

        return result;
    }
    //////////////////////////////////////////////////////////////////////////
    bool tuple_setitem_i( kernel_interface * _kernel, PyObject * _tuple, uint32_t _it, const detail::import_operator_t & _item )
    {
        bool result = _kernel->tuple_setitem( _tuple, _it, _item );

        return result;
    }
    //////////////////////////////////////////////////////////////////////////
    bool dict_set_i( kernel_interface * _kernel, PyObject * _dict, const detail::import_operator_t & _name, const detail::import_operator_t & _value )
    {
        bool result = _kernel->dict_set( _dict, _name, _value );

        return result;
    }
    //////////////////////////////////////////////////////////////////////////
    detail::extract_operator_t dict_get_i( kernel_interface * _kernel, PyObject * _dict, const detail::import_operator_t & _key )
    {
        PyObject * py_item = _kernel->dict_get( _dict, _key );

        if( py_item == nullptr )
        {
            PyObject * py_none = _kernel->ret_none();

            return detail::extract_operator_t( _kernel, py_none, pybind::borrowed );
        }

        return detail::extract_operator_t( _kernel, py_item );
    }
    //////////////////////////////////////////////////////////////////////////
    bool dict_remove_i( kernel_interface * _kernel, PyObject * _dict, const detail::import_operator_t & _key )
    {
        bool successful = _kernel->dict_remove( _dict, _key );

        return successful;
    }
    //////////////////////////////////////////////////////////////////////////
    detail::extract_operator_t ask_tuple_t( kernel_interface * _kernel, PyObject * _obj, const pybind::tuple & _tuple )
    {
        PyObject * py_result = _kernel->ask_native( _obj, _tuple.ptr() );

        return detail::extract_operator_t( _kernel, py_result, pybind::borrowed );
    }
    //////////////////////////////////////////////////////////////////////////
    detail::extract_operator_t call_t( kernel_interface * _kernel, PyObject * _obj )
    {
        pybind::tuple args = pybind::make_tuple_t( _kernel );

        return pybind::ask_tuple_t( _kernel, _obj, args );
    }
    //////////////////////////////////////////////////////////////////////////	
    detail::extract_operator_t call_i( kernel_interface * _kernel, PyObject * _obj, std::initializer_list<detail::import_operator_t> && _t )
    {
        pybind::tuple args = pybind::make_tuple_i( _kernel, std::forward<std::initializer_list<detail::import_operator_t>>( _t ) );

        return pybind::ask_tuple_t( _kernel, _obj, args );
    }
    //////////////////////////////////////////////////////////////////////////
    detail::extract_operator_t call_args_i( kernel_interface * _kernel, PyObject * _obj, std::initializer_list<detail::import_operator_t> && _t, const args & _args )
    {
        pybind::tuple args = pybind::make_tuple_args_i( _kernel, std::forward<std::initializer_list<detail::import_operator_t>>( _t ), _args );

        return pybind::ask_tuple_t( _kernel, _obj, args );
    }
    //////////////////////////////////////////////////////////////////////////
    detail::extract_operator_t extract_t( kernel_interface * _kernel, PyObject * _obj )
    {
        return detail::extract_operator_t( _kernel, _obj );
    }
    //////////////////////////////////////////////////////////////////////////
}