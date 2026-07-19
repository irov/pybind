#include "pybind/pickle.hpp"

#include "pybind/extract.hpp"
#include "pybind/helper.hpp"
#include "pybind/object.hpp"

namespace pybind
{
    //////////////////////////////////////////////////////////////////////////
    static constexpr uint8_t PICKLE_NONE = 0;
    static constexpr uint8_t PICKLE_FALSE = 1;
    static constexpr uint8_t PICKLE_TRUE = 2;
    static constexpr uint8_t PICKLE_INT = 3;
    static constexpr uint8_t PICKLE_FLOAT = 4;
    static constexpr uint8_t PICKLE_STRING = 5;
    static constexpr uint8_t PICKLE_UNICODE = 6;
    static constexpr uint8_t PICKLE_TUPLE = 7;
    static constexpr uint8_t PICKLE_LIST = 8;
    static constexpr uint8_t PICKLE_DICT = 9;
    static constexpr uint8_t PICKLE_OBJECT = 10;
    static constexpr uint8_t PICKLE_LONG = 11;
    static constexpr uint8_t PICKLE_SET = 12;
    //////////////////////////////////////////////////////////////////////////
    template<class T>
    static void s_write_buffer_t( void * _buffer, size_t _capacity, T _t, size_t * const _offset )
    {
        size_t size = sizeof( T );

        if( _buffer != nullptr )
        {
            if( *_offset + size > _capacity )
            {
                pybind::throw_exception( "pickle buffer out of capacity %zu + %zu > %zu"
                    , *_offset
                    , size
                    , _capacity
                );

                return;
            }

            uint8_t * write_buffer = static_cast<uint8_t *>(_buffer);

            PYBIND_MEMCPY( write_buffer + *_offset, &_t, size );
        }

        *_offset += size;
    }
    //////////////////////////////////////////////////////////////////////////
    template<class T>
    static void s_write_buffer_tn( void * _buffer, size_t _capacity, const T * _t, size_t _count, size_t * const _offset )
    {
        size_t size = sizeof( T ) * _count;

        if( _buffer != nullptr )
        {
            if( *_offset + size > _capacity )
            {
                pybind::throw_exception( "pickle buffer out of capacity %zu + %zu > %zu"
                    , *_offset
                    , size
                    , _capacity
                );

                return;
            }

            uint8_t * write_buffer = static_cast<uint8_t *>(_buffer);

            PYBIND_MEMCPY( write_buffer + *_offset, _t, size );
        }

        *_offset += size;
    }
    //////////////////////////////////////////////////////////////////////////
    static void s_write_size_t( void * _buffer, size_t _capacity, size_t _t, size_t * const _offset )
    {
        if( _t < 255 )
        {
            uint8_t write_t = (uint8_t)_t;
            s_write_buffer_t( _buffer, _capacity, write_t, _offset );
        }
        else
        {
            uint8_t write_dummy = 255;
            s_write_buffer_t( _buffer, _capacity, write_dummy, _offset );

            uint32_t write_t = (uint32_t)_t;
            s_write_buffer_t( _buffer, _capacity, write_t, _offset );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    static void s_obj_pickle( kernel_interface * _kernel, PyObject * _obj, PyObject * _types, void * _buffer, size_t _capacity, size_t * const _offset )
    {
        if( _kernel->is_none( _obj ) == true )
        {
            uint8_t type = PICKLE_NONE;

            s_write_buffer_t( _buffer, _capacity, type, _offset );
        }
        else if( _kernel->bool_check( _obj ) == true )
        {
            uint8_t type = _kernel->is_true( _obj ) == true ? PICKLE_TRUE : PICKLE_FALSE;

            s_write_buffer_t( _buffer, _capacity, type, _offset );
        }
        else if( _kernel->int_check( _obj ) == true )
        {
            uint8_t type = PICKLE_INT;

            s_write_buffer_t( _buffer, _capacity, type, _offset );

            int32_t value = pybind::extract_throw<int32_t>( _kernel, _obj );

            s_write_buffer_t( _buffer, _capacity, value, _offset );
        }
        else if( _kernel->long_check( _obj ) == true )
        {
            uint8_t type = PICKLE_LONG;

            s_write_buffer_t( _buffer, _capacity, type, _offset );

            int64_t value = pybind::extract_throw<int64_t>( _kernel, _obj );

            s_write_buffer_t( _buffer, _capacity, value, _offset );
        }
        else if( _kernel->float_check( _obj ) == true )
        {
            uint8_t type = PICKLE_FLOAT;

            s_write_buffer_t( _buffer, _capacity, type, _offset );

            float value = pybind::extract_throw<float>( _kernel, _obj );

            s_write_buffer_t( _buffer, _capacity, value, _offset );
        }
        else if( _kernel->string_check( _obj ) == true )
        {
            uint8_t type = PICKLE_STRING;

            s_write_buffer_t( _buffer, _capacity, type, _offset );

            size_t str_size;
            const char * str = _kernel->string_to_char_and_size( _obj, &str_size );

            s_write_size_t( _buffer, _capacity, str_size, _offset );
            s_write_buffer_tn( _buffer, _capacity, str, str_size, _offset );
        }
        else if( _kernel->unicode_check( _obj ) == true )
        {
            uint8_t type = PICKLE_UNICODE;

            s_write_buffer_t( _buffer, _capacity, type, _offset );

            PyObject * py_utf8 = _kernel->unicode_encode_utf8( _obj );

            if( py_utf8 == nullptr )
            {
                pybind::throw_exception( "pickle invalid unicode value" );

                return;
            }

            pybind::object utf8( _kernel, py_utf8, pybind::borrowed );

            size_t str_size;
            const char * str = _kernel->string_to_char_and_size( py_utf8, &str_size );

            s_write_size_t( _buffer, _capacity, str_size, _offset );
            s_write_buffer_tn( _buffer, _capacity, str, str_size, _offset );
        }
        else if( _kernel->tuple_check( _obj ) == true )
        {
            uint8_t type = PICKLE_TUPLE;

            s_write_buffer_t( _buffer, _capacity, type, _offset );

            size_t count = _kernel->tuple_size( _obj );

            s_write_size_t( _buffer, _capacity, count, _offset );

            for( size_t i = 0; i != count; ++i )
            {
                PyObject * element = _kernel->tuple_getitem( _obj, i );

                s_obj_pickle( _kernel, element, _types, _buffer, _capacity, _offset );
            }
        }
        else if( _kernel->list_check( _obj ) == true )
        {
            uint8_t type = PICKLE_LIST;

            s_write_buffer_t( _buffer, _capacity, type, _offset );

            size_t count = _kernel->list_size( _obj );

            s_write_size_t( _buffer, _capacity, count, _offset );

            for( size_t i = 0; i != count; ++i )
            {
                PyObject * element = _kernel->list_getitem( _obj, i );

                s_obj_pickle( _kernel, element, _types, _buffer, _capacity, _offset );
            }
        }
        else if( _kernel->dict_check( _obj ) == true )
        {
            uint8_t type = PICKLE_DICT;

            s_write_buffer_t( _buffer, _capacity, type, _offset );

            size_t count = _kernel->dict_size( _obj );

            s_write_size_t( _buffer, _capacity, count, _offset );

            size_t pos = 0;

            PyObject * key;
            PyObject * value;
            while( _kernel->dict_next( _obj, &pos, &key, &value ) == true )
            {
                s_obj_pickle( _kernel, key, _types, _buffer, _capacity, _offset );
                s_obj_pickle( _kernel, value, _types, _buffer, _capacity, _offset );
            }
        }
        else if( _kernel->set_check( _obj ) == true )
        {
            uint8_t type = PICKLE_SET;

            s_write_buffer_t( _buffer, _capacity, type, _offset );

            size_t count = _kernel->set_size( _obj );

            s_write_size_t( _buffer, _capacity, count, _offset );

            PyObject * py_iterator;
            if( _kernel->iterator_get( _obj, &py_iterator ) == false )
            {
                pybind::throw_exception( "pickle obj '%s' type '%s' has set but iterator get failed!"
                    , _kernel->object_repr( _obj ).c_str()
                    , _kernel->object_repr_type( _obj ).c_str()
                );

                return;
            }

            pybind::object iterator( _kernel, py_iterator, pybind::borrowed );

            PyObject * py_element;
            while( _kernel->iterator_next( py_iterator, &py_element ) == true )
            {
                pybind::object element( _kernel, py_element, pybind::borrowed );

                s_obj_pickle( _kernel, py_element, _types, _buffer, _capacity, _offset );
            }
        }
        else if( _kernel->has_attrstring( _obj, "value" ) == true )
        {
            uint8_t type = PICKLE_OBJECT;

            s_write_buffer_t( _buffer, _capacity, type, _offset );

            size_t type_count = _kernel->list_size( _types );

            for( size_t index = 0; index != type_count; ++index )
            {
                PyObject * element = _kernel->list_getitem( _types, index );

                PyTypeObject * element_type = (PyTypeObject *)(element);

                if( _kernel->is_instanceof( _obj, element_type ) == false )
                {
                    continue;
                }

                s_write_size_t( _buffer, _capacity, index, _offset );

                PyObject * py_obj_value = _kernel->get_attrstring( _obj, "value" );
                pybind::object obj_value( _kernel, py_obj_value, pybind::borrowed );

                s_obj_pickle( _kernel, py_obj_value, _types, _buffer, _capacity, _offset );

                return;
            }

            pybind::throw_exception( "pickle obj '%s' type '%s' has 'value' but not type in types!"
                , _kernel->object_repr( _obj ).c_str()
                , _kernel->object_repr_type( _obj ).c_str()
            );
        }
        else
        {
            pybind::throw_exception( "pickle obj '%s' type '%s' invalid pickle"
                , _kernel->object_repr( _obj ).c_str()
                , _kernel->object_repr_type( _obj ).c_str()
            );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    bool pickle( kernel_interface * _kernel, PyObject * _obj, PyObject * _types, void * _buffer, size_t _capacity, size_t * const _size )
    {
        if( _kernel->list_check( _types ) == false )
        {
            return false;
        }

        try
        {
            *_size = 0;

            s_obj_pickle( _kernel, _obj, _types, _buffer, _capacity, _size );
        }
        catch( const pybind::pybind_exception & _ex )
        {
            _kernel->error_message( "ptr value: %s"
                , _ex.what()
            );

            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    template<class T>
    static void s_read_buffer_t( const void * _buffer, size_t _capacity, size_t * const _carriage, T & _t )
    {
        size_t size = sizeof( T );

        if( *_carriage + size > _capacity )
        {
            pybind::throw_exception( "unpickle out of buffer capacity %zu + %zu > %zu"
                , *_carriage
                , size
                , _capacity
            );

            return;
        }

        const void * src_buffer = (static_cast<const uint8_t *>(_buffer) + *_carriage);

        PYBIND_MEMCPY( &_t, src_buffer, size );

        *_carriage += size;
    }
    //////////////////////////////////////////////////////////////////////////
    template<class T>
    static void s_read_buffer_tn( const void * _buffer, size_t _capacity, size_t * const _carriage, T * _t, size_t _count )
    {
        size_t size = sizeof( T ) * _count;

        if( *_carriage + size > _capacity )
        {
            pybind::throw_exception( "unpickle out of buffer capacity %zu + %zu > %zu"
                , *_carriage
                , size
                , _capacity
            );

            return;
        }

        const void * src_buffer = static_cast<const uint8_t *>(_buffer) + *_carriage;

        PYBIND_MEMCPY( _t, src_buffer, size );

        *_carriage += size;
    }
    //////////////////////////////////////////////////////////////////////////
    static void s_read_size_t( const void * _buffer, size_t _capacity, size_t * const _carriage, size_t * const _t )
    {
        uint8_t size_base;
        s_read_buffer_t( _buffer, _capacity, _carriage, size_base );

        if( size_base < 255 )
        {
            *_t = (size_t)size_base;
        }
        else
        {
            uint32_t size_full;
            s_read_buffer_t( _buffer, _capacity, _carriage, size_full );

            *_t = (size_t)size_full;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    static PyObject * s_obj_unpickle( kernel_interface * _kernel, const void * _buffer, size_t _capacity, size_t * const _carriage, PyObject * _types )
    {
        uint8_t type;
        s_read_buffer_t( _buffer, _capacity, _carriage, type );

        switch( type )
        {
        case PICKLE_NONE:
            {
                PyObject * obj = _kernel->ret_none();

                return obj;
            }break;
        case PICKLE_FALSE:
            {
                PyObject * obj = _kernel->ret_bool( false );

                return obj;
            }break;
        case PICKLE_TRUE:
            {
                PyObject * obj = _kernel->ret_bool( true );

                return obj;
            }break;
        case PICKLE_INT:
            {
                int32_t value;
                s_read_buffer_t( _buffer, _capacity, _carriage, value );

                PyObject * obj = pybind::ptr_throw( _kernel, value );

                return obj;
            }break;
        case PICKLE_LONG:
            {
                int64_t value;
                s_read_buffer_t( _buffer, _capacity, _carriage, value );

                PyObject * obj = pybind::ptr_throw( _kernel, value );

                return obj;
            }break;
        case PICKLE_FLOAT:
            {
                float value;
                s_read_buffer_t( _buffer, _capacity, _carriage, value );

                PyObject * obj = pybind::ptr_throw( _kernel, value );

                return obj;
            }break;
        case PICKLE_STRING:
            {
                size_t str_size;
                s_read_size_t( _buffer, _capacity, _carriage, &str_size );

                if( *_carriage + str_size > _capacity )
                {
                    pybind::throw_exception( "unpickle object string out of buffer %zu + %zu > %zu"
                        , *_carriage
                        , str_size
                        , _capacity
                    );

                    return nullptr;
                }

                const char * str_buffer = (const char *)(static_cast<const uint8_t *>(_buffer) + *_carriage);

                *_carriage += str_size;

                PyObject * obj = _kernel->string_from_char_size( str_buffer, str_size );

                return obj;
            }break;
        case PICKLE_UNICODE:
            {
                size_t utf8_size;
                s_read_size_t( _buffer, _capacity, _carriage, &utf8_size );

                if( *_carriage + utf8_size > _capacity )
                {
                    pybind::throw_exception( "unpickle object unicode out of buffer %zu + %zu > %zu"
                        , _carriage
                        , utf8_size
                        , _capacity
                    );

                    return nullptr;
                }

                const char * utf8_buffer = (const char *)(static_cast<const uint8_t *>(_buffer) + *_carriage);

                *_carriage += utf8_size;

                PyObject * obj = _kernel->unicode_from_utf8_size( utf8_buffer, utf8_size );

                return obj;
            }break;
        case PICKLE_TUPLE:
            {
                size_t count;
                s_read_size_t( _buffer, _capacity, _carriage, &count );

                PyObject * py_obj = _kernel->tuple_new( count );
                pybind::object obj( _kernel, py_obj, pybind::borrowed );

                for( size_t i = 0; i != count; ++i )
                {
                    PyObject * py_element = s_obj_unpickle( _kernel, _buffer, _capacity, _carriage, _types );
                    pybind::object element( _kernel, py_element, pybind::borrowed );

                    if( py_element == nullptr || _kernel->tuple_setitem( py_obj, i, py_element ) == false )
                    {
                        pybind::throw_exception( "unpickle tuple invalid element %zu", i );

                        return nullptr;
                    }
                }

                return obj.ret();
            }break;
        case PICKLE_LIST:
            {
                size_t count;
                s_read_size_t( _buffer, _capacity, _carriage, &count );

                PyObject * py_obj = _kernel->list_new( count );
                pybind::object obj( _kernel, py_obj, pybind::borrowed );

                for( size_t i = 0; i != count; ++i )
                {
                    PyObject * py_element = s_obj_unpickle( _kernel, _buffer, _capacity, _carriage, _types );
                    pybind::object element( _kernel, py_element, pybind::borrowed );

                    if( py_element == nullptr || _kernel->list_setitem( py_obj, i, py_element ) == false )
                    {
                        pybind::throw_exception( "unpickle list invalid element %zu", i );

                        return nullptr;
                    }
                }

                return obj.ret();
            }break;
        case PICKLE_DICT:
            {
                size_t count;
                s_read_size_t( _buffer, _capacity, _carriage, &count );

                PyObject * py_obj = _kernel->dict_new_presized( count );
                pybind::object obj( _kernel, py_obj, pybind::borrowed );

                for( uint32_t i = 0; i != count; ++i )
                {
                    PyObject * py_key = s_obj_unpickle( _kernel, _buffer, _capacity, _carriage, _types );
                    pybind::object key( _kernel, py_key, pybind::borrowed );

                    PyObject * py_value = s_obj_unpickle( _kernel, _buffer, _capacity, _carriage, _types );
                    pybind::object value( _kernel, py_value, pybind::borrowed );

                    if( py_key == nullptr || py_value == nullptr || _kernel->dict_set( py_obj, py_key, py_value ) == false )
                    {
                        pybind::throw_exception( "unpickle dict invalid item %u", i );

                        return nullptr;
                    }
                }

                return obj.ret();
            }break;
        case PICKLE_SET:
            {
                size_t count;
                s_read_size_t( _buffer, _capacity, _carriage, &count );

                PyObject * py_obj = _kernel->set_new();
                pybind::object obj( _kernel, py_obj, pybind::borrowed );

                for( uint32_t i = 0; i != count; ++i )
                {
                    PyObject * py_element = s_obj_unpickle( _kernel, _buffer, _capacity, _carriage, _types );
                    pybind::object element( _kernel, py_element, pybind::borrowed );

                    if( py_element == nullptr || _kernel->set_set( py_obj, py_element ) == false )
                    {
                        pybind::throw_exception( "unpickle set invalid element %u", i );

                        return nullptr;
                    }
                }

                return obj.ret();
            }break;
        case PICKLE_OBJECT:
            {
                size_t index;
                s_read_size_t( _buffer, _capacity, _carriage, &index );

                size_t types_size = _kernel->list_size( _types );

                if( index >= types_size )
                {
                    pybind::throw_exception( "unpickle object has type index %zu more of size types %zu"
                        , index
                        , types_size
                    );
                }

                PyObject * py_value = s_obj_unpickle( _kernel, _buffer, _capacity, _carriage, _types );
                pybind::object value( _kernel, py_value, pybind::borrowed );

                PyObject * object_type = _kernel->list_getitem( _types, index );

                PyObject * py_args = _kernel->tuple_new( 1 );
                pybind::object args( _kernel, py_args, pybind::borrowed );

                if( py_value == nullptr || _kernel->tuple_setitem( py_args, 0, py_value ) == false )
                {
                    pybind::throw_exception( "unpickle invalid object value" );

                    return nullptr;
                }

                PyObject * obj = _kernel->ask_native( object_type, py_args );

                if( obj == nullptr )
                {
                    pybind::throw_exception( "unpickle invalid create object '%s' type index %zu"
                        , _kernel->object_repr_type( object_type ).c_str()
                        , index
                    );
                }

                return obj;
            }break;
        default:
            {
                pybind::throw_exception( "unpickle invalid type %u"
                    , type
                );
            }
        }

        return nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    PYBIND_API PyObject * unpickle( kernel_interface * _kernel, const void * _buffer, size_t _capacity, PyObject * _types )
    {
        size_t carriage = 0;

        try
        {
            PyObject * obj = s_obj_unpickle( _kernel, _buffer, _capacity, &carriage, _types );

            return obj;
        }
        catch( const pybind::pybind_exception & _ex )
        {
            _kernel->error_message( "ptr value: %s"
                , _ex.what()
            );
        }

        return nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
}
