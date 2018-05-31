#	pragma once

#	include "pybind/system.hpp"

#   include "pybind/return_operator.hpp"
#   include "pybind/import_operator.hpp"
#   include "pybind/extract_operator.hpp"
#   include "pybind/args.hpp"

namespace pybind
{
    //////////////////////////////////////////////////////////////////////////
    template<>
    struct ptr_throw_specialized <detail::import_operator_t>;
    //////////////////////////////////////////////////////////////////////////
    PYBIND_API args make_args_t( kernel_interface * _kernel, PyObject * _tuple, uint32_t _size );
    
    PYBIND_API detail::extract_operator_t list_getitem_t( kernel_interface * _kernel, PyObject * _list, uint32_t _it );
    PYBIND_API bool list_setitem_i( kernel_interface * _kernel, PyObject * _list, uint32_t _it, const detail::import_operator_t & _item );
    PYBIND_API bool list_appenditem_i( kernel_interface * _kernel, PyObject * _obj, const detail::import_operator_t & _item );
    
    //////////////////////////////////////////////////////////////////////////
    template<class T>
    bool list_setitem_t( kernel_interface * _kernel, PyObject * _list, uint32_t _it, const T & _item )
    {
        return list_setitem_i( _kernel, _list, _it, detail::import_operator_t( _kernel, _item ) );
    }
    //////////////////////////////////////////////////////////////////////////
    template<class T>
    bool list_appenditem_t( kernel_interface * _kernel, PyObject * _obj, const T & _item )
    {
        return list_appenditem_i( _kernel, _obj, detail::import_operator_t( _kernel, _item ) );
    }
    //////////////////////////////////////////////////////////////////////////
    template<class It>
    inline bool list_appenditems_t( kernel_interface * _kernel, PyObject * _obj, It _begin, It _end )
    {
        for( It it = _begin; it != _end; ++it )
        {
            if( pybind::list_appenditem_t( _obj, *it ) == false )
            {
                return false;
            }
        }

        return true;
    }

    PYBIND_API detail::extract_operator_t tuple_getitem_t( kernel_interface * _kernel, PyObject * _tuple, uint32_t _it );
    PYBIND_API bool tuple_setitem_i( kernel_interface * _kernel, PyObject * _tuple, uint32_t _it, const detail::import_operator_t & _item );
    //////////////////////////////////////////////////////////////////////////	
    template<class T>
    bool tuple_setitem_t( kernel_interface * _kernel, PyObject * _tuple, uint32_t _it, const T & _item )
    {
        return tuple_setitem_i( _kernel, _tuple, _it, detail::import_operator_t( _kernel, _item ) );
    }
    
    PYBIND_API bool dict_setstring_i( kernel_interface * _kernel, PyObject * _dict, const char * _key, const detail::import_operator_t & _value );
    PYBIND_API bool dict_setobject_i( kernel_interface * _kernel, PyObject * _dict, PyObject * _key, const detail::import_operator_t & _value );
    PYBIND_API detail::extract_operator_t dict_get_i( kernel_interface * _kernel, PyObject * _dict, const detail::import_operator_t & _key );
    PYBIND_API bool dict_set_i( kernel_interface * _kernel, PyObject * _dict, const detail::import_operator_t & _name, const detail::import_operator_t & _value );
    PYBIND_API bool dict_remove_i( kernel_interface * _kernel, PyObject * _dict, const detail::import_operator_t & _key );
	//////////////////////////////////////////////////////////////////////////
	template<class V>
	bool dict_setstring_t( kernel_interface * _kernel, PyObject * _dict, const char * _name, const V & _value )
	{
		return dict_setstring_i( _kernel, _dict, _name, detail::import_operator_t( _kernel, _value ) );
	}
	//////////////////////////////////////////////////////////////////////////
	template<class K, class V>
	bool dict_set_t( kernel_interface * _kernel, PyObject * _dict, const K & _name, const V & _value )
	{
		return dict_set_i( _kernel, _dict, detail::import_operator_t( _kernel, _name ), detail::import_operator_t( _kernel, _value ) );
	}
	//////////////////////////////////////////////////////////////////////////
	template<class K>
	detail::extract_operator_t dict_get_t( kernel_interface * _kernel, PyObject * _dict, const K & _key )
	{
		return dict_get_i( _kernel, _dict, detail::import_operator_t( _kernel, _key ) );
	}
	//////////////////////////////////////////////////////////////////////////
	template<class K>
	bool dict_remove_t( kernel_interface * _kernel, PyObject * _dict, const K & _key )
	{
		return dict_remove_i( _kernel, _dict, detail::import_operator_t( _kernel, _key ) );
	}
	//////////////////////////////////////////////////////////////////////////
	template<class K, class V>
	inline bool dict_next_t( kernel_interface * _kernel, PyObject * _dict, uint32_t & _pos, K & _key, V & _value )
	{
		PyObject * py_key;
		PyObject * py_value;

		if( pybind::dict_next( _dict, _pos, &py_key, &py_value ) == false )
		{
			return false;
		}

		_key = pybind::extract<K>( _kernel, py_key );
		_value = pybind::extract<V>( _kernel, py_value );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	PYBIND_API detail::extract_operator_t ask_tuple_t( kernel_interface * _kernel, PyObject * _obj, const pybind::tuple & _tuple );
	//////////////////////////////////////////////////////////////////////////
	PYBIND_API detail::extract_operator_t call_t( kernel_interface * _kernel, PyObject * _obj );
	PYBIND_API detail::extract_operator_t call_i( kernel_interface * _kernel, PyObject * _obj, const detail::import_operator_t & _t0 );
	PYBIND_API detail::extract_operator_t call_i( kernel_interface * _kernel, PyObject * _obj, const detail::import_operator_t & _t0, const detail::import_operator_t & _t1 );
	PYBIND_API detail::extract_operator_t call_i( kernel_interface * _kernel, PyObject * _obj, const detail::import_operator_t & _t0, const detail::import_operator_t & _t1, const detail::import_operator_t & _t2 );
	PYBIND_API detail::extract_operator_t call_i( kernel_interface * _kernel, PyObject * _obj, const detail::import_operator_t & _t0, const detail::import_operator_t & _t1, const detail::import_operator_t & _t2, const detail::import_operator_t & _t3 );
    PYBIND_API detail::extract_operator_t call_i( kernel_interface * _kernel, PyObject * _obj, const detail::import_operator_t & _t0, const detail::import_operator_t & _t1, const detail::import_operator_t & _t2, const detail::import_operator_t & _t3, const detail::import_operator_t & _t4, const detail::import_operator_t & _t5 );
	PYBIND_API detail::extract_operator_t call_i( kernel_interface * _kernel, PyObject * _obj, const detail::import_operator_t & _t0, const detail::import_operator_t & _t1, const detail::import_operator_t & _t2, const detail::import_operator_t & _t3, const detail::import_operator_t & _t4 );
	PYBIND_API detail::extract_operator_t call_i( kernel_interface * _kernel, PyObject * _obj, const detail::import_operator_t & _t0, const detail::import_operator_t & _t1, const detail::import_operator_t & _t2, const detail::import_operator_t & _t3, const detail::import_operator_t & _t4, const detail::import_operator_t & _t5, const detail::import_operator_t & _t6 );
	PYBIND_API detail::extract_operator_t call_i( kernel_interface * _kernel, PyObject * _obj, const detail::import_operator_t & _t0, const detail::import_operator_t & _t1, const detail::import_operator_t & _t2, const detail::import_operator_t & _t3, const detail::import_operator_t & _t4, const detail::import_operator_t & _t5, const detail::import_operator_t & _t6, const detail::import_operator_t & _t7 );
    //////////////////////////////////////////////////////////////////////////
    template<class T0>
    detail::extract_operator_t call_t( kernel_interface * _kernel, PyObject * _obj, const T0 & _t0 )
    {
        return call_i( _kernel, _obj
            , detail::import_operator_t( _kernel, _t0 )
        );
    }
    //////////////////////////////////////////////////////////////////////////
    template<class T0, class T1>
    detail::extract_operator_t call_t( kernel_interface * _kernel, PyObject * _obj, const T0 & _t0, const T1 & _t1 )
    {
        return call_i( _kernel, _obj
            , detail::import_operator_t( _kernel, _t1 )
            , detail::import_operator_t( _kernel, _t0 )
        );
    }
    //////////////////////////////////////////////////////////////////////////
    template<class T0, class T1, class T2>
    detail::extract_operator_t call_t( kernel_interface * _kernel, PyObject * _obj, const T0 & _t0, const T1 & _t1, const T2 & _t2 )
    {
        return call_i( _kernel, _obj
            , detail::import_operator_t( _kernel, _t0 )
            , detail::import_operator_t( _kernel, _t1 )
            , detail::import_operator_t( _kernel, _t2 )
        );
    }
    //////////////////////////////////////////////////////////////////////////
    template<class T0, class T1, class T2, class T3>
    detail::extract_operator_t call_t( kernel_interface * _kernel, PyObject * _obj, const T0 & _t0, const T1 & _t1, const T2 & _t2, const T3 & _t3 )
    {
        return call_i( _kernel, _obj
            , detail::import_operator_t( _kernel, _t0 )
            , detail::import_operator_t( _kernel, _t1 )
            , detail::import_operator_t( _kernel, _t2 )
            , detail::import_operator_t( _kernel, _t3 )
        );
    }
    //////////////////////////////////////////////////////////////////////////
    template<class T0, class T1, class T2, class T3, class T4>
    detail::extract_operator_t call_t( kernel_interface * _kernel, PyObject * _obj, const T0 & _t0, const T1 & _t1, const T2 & _t2, const T3 & _t3, const T4 & _t4 )
    {
        return call_i( _kernel, _obj
            , detail::import_operator_t( _kernel, _t0 )
            , detail::import_operator_t( _kernel, _t1 )
            , detail::import_operator_t( _kernel, _t2 )
            , detail::import_operator_t( _kernel, _t3 )
            , detail::import_operator_t( _kernel, _t4 )
        );
    }
    //////////////////////////////////////////////////////////////////////////
    template<class T0, class T1, class T2, class T3, class T4, class T5>
    detail::extract_operator_t call_t( kernel_interface * _kernel, PyObject * _obj, const T0 & _t0, const T1 & _t1, const T2 & _t2, const T3 & _t3, const T4 & _t4, const T5 & _t5 )
    {
        return call_i( _kernel, _obj
            , detail::import_operator_t( _kernel, _t0 )
            , detail::import_operator_t( _kernel, _t1 )
            , detail::import_operator_t( _kernel, _t2 )
            , detail::import_operator_t( _kernel, _t3 )
            , detail::import_operator_t( _kernel, _t4 )
            , detail::import_operator_t( _kernel, _t5 )
        );
    }
    //////////////////////////////////////////////////////////////////////////
    template<class T0, class T1, class T2, class T3, class T4, class T5, class T6>
    detail::extract_operator_t call_t( kernel_interface * _kernel, PyObject * _obj, const T0 & _t0, const T1 & _t1, const T2 & _t2, const T3 & _t3, const T4 & _t4, const T5 & _t5, const T6 & _t6 )
    {
        return call_i( _kernel, _obj
            , detail::import_operator_t( _kernel, _t0 )
            , detail::import_operator_t( _kernel, _t1 )
            , detail::import_operator_t( _kernel, _t2 )
            , detail::import_operator_t( _kernel, _t3 )
            , detail::import_operator_t( _kernel, _t4 )
            , detail::import_operator_t( _kernel, _t5 )
            , detail::import_operator_t( _kernel, _t6 )
        );
    }
    //////////////////////////////////////////////////////////////////////////
    template<class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7>
    detail::extract_operator_t call_t( kernel_interface * _kernel, PyObject * _obj, const T0 & _t0, const T1 & _t1, const T2 & _t2, const T3 & _t3, const T4 & _t4, const T5 & _t5, const T6 & _t6, const T7 & _t7 )
    {
        return call_i( _kernel, _obj
            , detail::import_operator_t( _kernel, _t0 )
            , detail::import_operator_t( _kernel, _t1 )
            , detail::import_operator_t( _kernel, _t2 )
            , detail::import_operator_t( _kernel, _t3 )
            , detail::import_operator_t( _kernel, _t4 )
            , detail::import_operator_t( _kernel, _t5 )
            , detail::import_operator_t( _kernel, _t6 )
            , detail::import_operator_t( _kernel, _t7 )
        );
    }
	//////////////////////////////////////////////////////////////////////////
	PYBIND_API detail::extract_operator_t call_args_t( kernel_interface * _kernel, PyObject * _obj, const args & _args );
	PYBIND_API detail::extract_operator_t call_args_i( kernel_interface * _kernel, PyObject * _obj, const detail::import_operator_t & _t0, const args & _args );
	PYBIND_API detail::extract_operator_t call_args_i( kernel_interface * _kernel, PyObject * _obj, const detail::import_operator_t & _t0, const detail::import_operator_t & _t1, const args & _args );
	PYBIND_API detail::extract_operator_t call_args_i( kernel_interface * _kernel, PyObject * _obj, const detail::import_operator_t & _t0, const detail::import_operator_t & _t1, const detail::import_operator_t & _t2, const args & _args );
	PYBIND_API detail::extract_operator_t call_args_i( kernel_interface * _kernel, PyObject * _obj, const detail::import_operator_t & _t0, const detail::import_operator_t & _t1, const detail::import_operator_t & _t2, const detail::import_operator_t & _t3, const args & _args );
	PYBIND_API detail::extract_operator_t call_args_i( kernel_interface * _kernel, PyObject * _obj, const detail::import_operator_t & _t0, const detail::import_operator_t & _t1, const detail::import_operator_t & _t2, const detail::import_operator_t & _t3, const detail::import_operator_t & _t4, const args & _args );
	PYBIND_API detail::extract_operator_t call_args_i( kernel_interface * _kernel, PyObject * _obj, const detail::import_operator_t & _t0, const detail::import_operator_t & _t1, const detail::import_operator_t & _t2, const detail::import_operator_t & _t3, const detail::import_operator_t & _t4, const detail::import_operator_t & _t5, const args & _args );
	PYBIND_API detail::extract_operator_t call_args_i( kernel_interface * _kernel, PyObject * _obj, const detail::import_operator_t & _t0, const detail::import_operator_t & _t1, const detail::import_operator_t & _t2, const detail::import_operator_t & _t3, const detail::import_operator_t & _t4, const detail::import_operator_t & _t5, const detail::import_operator_t & _t6, const args & _args );
	PYBIND_API detail::extract_operator_t call_args_i( kernel_interface * _kernel, PyObject * _obj, const detail::import_operator_t & _t0, const detail::import_operator_t & _t1, const detail::import_operator_t & _t2, const detail::import_operator_t & _t3, const detail::import_operator_t & _t4, const detail::import_operator_t & _t5, const detail::import_operator_t & _t6, const detail::import_operator_t & _t7, const args & _args );
    //////////////////////////////////////////////////////////////////////////
    template<class T0>
    detail::extract_operator_t call_args_t( kernel_interface * _kernel, PyObject * _obj, const T0 & _t0, const args & _args )
    {
        return call_args_i( _kernel, _obj
            , detail::import_operator_t( _kernel, _t0 )
            , _args
        );
    }
    //////////////////////////////////////////////////////////////////////////
    template<class T0, class T1>
    detail::extract_operator_t call_args_t( kernel_interface * _kernel, PyObject * _obj, const T0 & _t0, const T1 & _t1, const args & _args )
    {
        return call_args_i( _kernel, _obj
            , detail::import_operator_t( _kernel, _t0 )
            , detail::import_operator_t( _kernel, _t1 )
            , _args
        );
    }
    //////////////////////////////////////////////////////////////////////////
    template<class T0, class T1, class T2>
    detail::extract_operator_t call_args_t( kernel_interface * _kernel, PyObject * _obj, const T0 & _t0, const T1 & _t1, const T2 & _t2, const args & _args )
    {
        return call_args_i( _kernel, _obj
            , detail::import_operator_t( _kernel, _t0 )
            , detail::import_operator_t( _kernel, _t1 )
            , detail::import_operator_t( _kernel, _t2 )
            , _args
        );
    }
    //////////////////////////////////////////////////////////////////////////
    template<class T0, class T1, class T2, class T3>
    detail::extract_operator_t call_args_t( kernel_interface * _kernel, PyObject * _obj, const T0 & _t0, const T1 & _t1, const T2 & _t2, const T3 & _t3, const args & _args )
    {
        return call_args_i( _kernel, _obj
            , detail::import_operator_t( _kernel, _t0 )
            , detail::import_operator_t( _kernel, _t1 )
            , detail::import_operator_t( _kernel, _t2 )
            , detail::import_operator_t( _kernel, _t3 )
            , _args
        );
    }
    //////////////////////////////////////////////////////////////////////////
    template<class T0, class T1, class T2, class T3, class T4>
    detail::extract_operator_t call_args_t( kernel_interface * _kernel, PyObject * _obj, const T0 & _t0, const T1 & _t1, const T2 & _t2, const T3 & _t3, const T4 & _t4, const args & _args )
    {
        return call_args_i( _kernel, _obj
            , detail::import_operator_t( _kernel, _t0 )
            , detail::import_operator_t( _kernel, _t1 )
            , detail::import_operator_t( _kernel, _t2 )
            , detail::import_operator_t( _kernel, _t3 )
            , detail::import_operator_t( _kernel, _t4 )
            , _args
        );
    }
    //////////////////////////////////////////////////////////////////////////
    template<class T0, class T1, class T2, class T3, class T4, class T5>
    detail::extract_operator_t call_args_t( kernel_interface * _kernel, PyObject * _obj, const T0 & _t0, const T1 & _t1, const T2 & _t2, const T3 & _t3, const T4 & _t4, const T5 & _t5, const args & _args )
    {
        return call_args_i( _kernel, _obj
            , detail::import_operator_t( _kernel, _t0 )
            , detail::import_operator_t( _kernel, _t1 )
            , detail::import_operator_t( _kernel, _t2 )
            , detail::import_operator_t( _kernel, _t3 )
            , detail::import_operator_t( _kernel, _t4 )
            , detail::import_operator_t( _kernel, _t5 )
            , _args
        );
    }
    //////////////////////////////////////////////////////////////////////////
    template<class T0, class T1, class T2, class T3, class T4, class T5, class T6>
    detail::extract_operator_t call_args_t( kernel_interface * _kernel, PyObject * _obj, const T0 & _t0, const T1 & _t1, const T2 & _t2, const T3 & _t3, const T4 & _t4, const T5 & _t5, const T6 & _t6, const args & _args )
    {
        return call_args_i( _kernel, _obj
            , detail::import_operator_t( _kernel, _t0 )
            , detail::import_operator_t( _kernel, _t1 )
            , detail::import_operator_t( _kernel, _t2 )
            , detail::import_operator_t( _kernel, _t3 )
            , detail::import_operator_t( _kernel, _t4 )
            , detail::import_operator_t( _kernel, _t5 )
            , detail::import_operator_t( _kernel, _t6 )
            , _args
        );
    }
    //////////////////////////////////////////////////////////////////////////
    template<class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7>
    detail::extract_operator_t call_args_t( kernel_interface * _kernel, PyObject * _obj, const T0 & _t0, const T1 & _t1, const T2 & _t2, const T3 & _t3, const T4 & _t4, const T5 & _t5, const T6 & _t6, const T7 & _t7, const args & _args )
    {
        return call_args_i( _kernel, _obj
            , detail::import_operator_t( _kernel, _t0 )
            , detail::import_operator_t( _kernel, _t1 )
            , detail::import_operator_t( _kernel, _t2 )
            , detail::import_operator_t( _kernel, _t3 )
            , detail::import_operator_t( _kernel, _t4 )
            , detail::import_operator_t( _kernel, _t5 )
            , detail::import_operator_t( _kernel, _t6 )
            , detail::import_operator_t( _kernel, _t7 )
            , _args
        );
    }
	//////////////////////////////////////////////////////////////////////////
	PYBIND_API detail::extract_operator_t extract_t( kernel_interface * _kernel, PyObject * _obj );
}