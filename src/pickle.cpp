#	include "pybind/pickle.hpp"

#	include "pybind/system.hpp"
#	include "pybind/extract.hpp"
#	include "pybind/helper.hpp"

#	include "stdex/memorycopy.h"

namespace pybind
{
	//////////////////////////////////////////////////////////////////////////
	static const uint8_t PICKLE_NONE = 0;
	static const uint8_t PICKLE_FALSE = 1;
	static const uint8_t PICKLE_TRUE = 2;
	static const uint8_t PICKLE_INT = 3;	
	static const uint8_t PICKLE_FLOAT = 4;
	static const uint8_t PICKLE_STRING = 5;
	static const uint8_t PICKLE_UNICODE = 6;
	static const uint8_t PICKLE_TUPLE = 7;
	static const uint8_t PICKLE_LIST = 8;
	static const uint8_t PICKLE_DICT = 9;
	static const uint8_t PICKLE_OBJECT = 10;
	static const uint8_t PICKLE_LONG = 11;
	//////////////////////////////////////////////////////////////////////////
	template<class T>
	static void s_write_buffer_t( void * _buffer, size_t _capacity, const T & _t, size_t & _offset )
	{
		const size_t size = sizeof(T);

		if( _buffer != nullptr )
		{
			if( _offset + size > _capacity )
			{
				pybind::throw_exception("pickle buffer out of capacity %d + %d > %d"
					, _offset
					, size
					, _capacity
					);
			}

			uint8_t * write_buffer = static_cast<uint8_t *>(_buffer);

			stdex::memorycopy( write_buffer, _offset, &_t, size );
		}

		_offset += size;
	}
	//////////////////////////////////////////////////////////////////////////
	template<class T>
	static void s_write_buffer_tn( void * _buffer, size_t _capacity, const T * _t, size_t _count, size_t & _offset )
	{
		const size_t size = sizeof(T) * _count;

		if( _buffer != nullptr )
		{
			if( _offset + size > _capacity )
			{
				pybind::throw_exception("pickle buffer out of capacity %d + %d > %d"
					, _offset
					, size
					, _capacity
					);
			}

			uint8_t * write_buffer = static_cast<uint8_t *>(_buffer);

			stdex::memorycopy( write_buffer, _offset, _t, size );
		}

		_offset += size;
	}
	//////////////////////////////////////////////////////////////////////////
	static void s_write_size_t( void * _buffer, size_t _capacity, const size_t _t, size_t & _offset )
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
	static void s_obj_pickle( kernel_interface * _kernel, PyObject * _obj, PyObject * _types, void * _buffer, size_t _capacity, size_t & _offset )
	{
		if( pybind::is_none( _obj ) == true )
		{
			uint8_t type = PICKLE_NONE;

			s_write_buffer_t( _buffer, _capacity, type, _offset );
		}
		else if( pybind::bool_check( _obj ) == true )
		{
			uint8_t type = pybind::is_true( _obj ) == true ? PICKLE_TRUE : PICKLE_FALSE;
			
			s_write_buffer_t( _buffer, _capacity, type, _offset );
		}
		else if( pybind::int_check( _obj ) == true )
		{
			uint8_t type = PICKLE_INT;

			s_write_buffer_t( _buffer, _capacity, type, _offset );

			int32_t value = pybind::extract_throw<int32_t>( _kernel, _obj );

			s_write_buffer_t( _buffer, _capacity, value, _offset );
		}
		else if( pybind::long_check( _obj ) == true )
		{
			uint8_t type = PICKLE_LONG;

			s_write_buffer_t( _buffer, _capacity, type, _offset );

			int64_t value = pybind::extract_throw<int64_t>( _kernel, _obj );

			s_write_buffer_t( _buffer, _capacity, value, _offset );
		}
		else if( pybind::float_check( _obj ) == true )
		{
			uint8_t type = PICKLE_FLOAT;

			s_write_buffer_t( _buffer, _capacity, type, _offset );

			float value = pybind::extract_throw<float>( _kernel, _obj );

			s_write_buffer_t( _buffer, _capacity, value, _offset );
		}
		else if( pybind::string_check( _obj ) == true )
		{
			uint8_t type = PICKLE_STRING;

			s_write_buffer_t( _buffer, _capacity, type, _offset );

			size_t str_size;
			const char * str = pybind::string_to_char_and_size( _obj, str_size );

			s_write_size_t( _buffer, _capacity, str_size, _offset );			
			s_write_buffer_tn( _buffer, _capacity, str, str_size, _offset );
		}
		else if( pybind::unicode_check( _obj ) == true )
		{
			uint8_t type = PICKLE_UNICODE;

			s_write_buffer_t( _buffer, _capacity, type, _offset );

			size_t str_size;
			const char * str = pybind::unicode_to_utf8_and_size( _obj, str_size );

			s_write_size_t( _buffer, _capacity, str_size, _offset );
			s_write_buffer_tn( _buffer, _capacity, str, str_size, _offset );
		}
		else if( pybind::tuple_check( _obj ) == true )
		{
			uint8_t type = PICKLE_TUPLE;

			s_write_buffer_t( _buffer, _capacity, type, _offset );

			size_t count = pybind::tuple_size( _obj );

			s_write_size_t( _buffer, _capacity, count, _offset );

			for( size_t i = 0; i != count; ++i )
			{
				PyObject * element = pybind::tuple_getitem( _obj, i );

				s_obj_pickle( _kernel, element, _types, _buffer, _capacity, _offset );
			}
		}
		else if( pybind::list_check( _obj ) == true )
		{
			uint8_t type = PICKLE_LIST;

			s_write_buffer_t( _buffer, _capacity, type, _offset );

			size_t count = pybind::list_size( _obj );

			s_write_size_t( _buffer, _capacity, count, _offset );

			for( size_t i = 0; i != count; ++i )
			{
				PyObject * element = pybind::list_getitem( _obj, i );

				s_obj_pickle( _kernel, element, _types, _buffer, _capacity, _offset );
			}
		}
		else if( pybind::dict_check( _obj ) == true )
		{
			uint8_t type = PICKLE_DICT;

			s_write_buffer_t( _buffer, _capacity, type, _offset );

			size_t count = pybind::dict_size( _obj );

			s_write_size_t( _buffer, _capacity, count, _offset );

			size_t pos = 0;

			PyObject * key;
			PyObject * value;
			while( pybind::dict_next( _obj, pos, &key, &value ) == true )
			{
				s_obj_pickle( _kernel, key, _types, _buffer, _capacity, _offset );
				s_obj_pickle( _kernel, value, _types, _buffer, _capacity, _offset );
			}
		}
		else if( pybind::has_attrstring( _obj, "value" ) == true )
		{
			uint8_t type = PICKLE_OBJECT;

			s_write_buffer_t( _buffer, _capacity, type, _offset );

			size_t type_count = pybind::list_size( _types );

			for( size_t index = 0; index != type_count; ++index )
			{
				PyObject * element = pybind::list_getitem( _types, index );

				PyTypeObject * element_type = (PyTypeObject *)( element );

				if( pybind::is_instanceof( _obj, element_type ) == false )
				{
					continue;
				}

				s_write_size_t( _buffer, _capacity, index, _offset );
				
				PyObject * obj_value = pybind::get_attrstring( _obj, "value" );

				s_obj_pickle( _kernel, obj_value, _types, _buffer, _capacity, _offset );

				pybind::decref( obj_value );

				return;
			}

			pybind::throw_exception("pickle obj %s type %s has 'value' but not type in types!"
				, pybind::object_repr(_obj)
				, pybind::object_repr_type(_obj)
				);
		}
		else
		{
			pybind::throw_exception("pickle obj %s type %s invalid pickle"
				, pybind::object_repr(_obj)
				, pybind::object_repr_type(_obj)
				);
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool pickle( kernel_interface * _kernel, PyObject * _obj, PyObject * _types, void * _buffer, size_t _capacity, size_t & _size )
	{		
		if( pybind::list_check( _types ) == false )
		{
			return false;
		}

		try
		{
			_size = 0;
			s_obj_pickle( _kernel, _obj, _types, _buffer, _capacity, _size );
		}
		catch( const pybind::pybind_exception & _ex )
		{
			pybind::error_message("ptr value %s"
				, _ex.what()
				);

			pybind::check_error();

			return false;
		}
		
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	template<class T>
	static void s_read_buffer_t( const void * _buffer, const size_t _capacity, size_t & _carriage, T & _t )
	{
		const size_t size = sizeof(T);

		if( _carriage + size > _capacity )
		{
			pybind::throw_exception("unpickle out of buffer capacity"
				);
		}

		const void * src_buffer = (static_cast<const uint8_t *>(_buffer) + _carriage);
		stdex::memorycopy( &_t, 0, src_buffer, size );

		_carriage += size;
	}
	//////////////////////////////////////////////////////////////////////////
	template<class T>
	static void s_read_buffer_tn( const void * _buffer, const size_t _capacity, size_t & _carriage, T * _t, size_t _count )
	{
		const size_t size = sizeof(T) * _count;

		if( _carriage + size > _capacity )
		{
			pybind::throw_exception("unpickle out of buffer capacity"
				);
		}

		const void * src_buffer = static_cast<const uint8_t *>(_buffer) + _carriage;
		stdex::memorycopy( _t, 0, src_buffer, size );

		_carriage += size;
	}
	//////////////////////////////////////////////////////////////////////////
	static void s_read_size_t( const void * _buffer, const size_t _capacity, size_t & _carriage, size_t & _t )
	{
		uint8_t size_base;
		s_read_buffer_t( _buffer, _capacity, _carriage, size_base );
		
		if( size_base < 255 )
		{			
			_t = (size_t)size_base;
		}
		else
		{
			uint32_t size_full;
			s_read_buffer_t( _buffer, _capacity, _carriage, size_full );

			_t = (size_t)size_full;
		}		
	}	
	//////////////////////////////////////////////////////////////////////////
	static PyObject * s_obj_unpickle( kernel_interface * _kernel, const void * _buffer, const size_t _capacity, size_t & _carriage, PyObject * _types )
	{
		uint8_t type;
		s_read_buffer_t( _buffer, _capacity, _carriage, type );

		switch( type )
		{
		case PICKLE_NONE:
			{
				PyObject * obj = pybind::ret_none();

				return obj;
			}break;
		case PICKLE_FALSE:
			{
				PyObject * obj = pybind::ret_bool( false );

				return obj;
			}break;
		case PICKLE_TRUE:
			{
				PyObject * obj = pybind::ret_bool( true );

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
				s_read_size_t( _buffer, _capacity, _carriage, str_size );

				const char * str_buffer = (const char *)(static_cast<const uint8_t *>(_buffer) + _carriage);

				if( _carriage + str_size > _capacity )
				{
					pybind::throw_exception("unpickle object string out of buffer %d + %d > %d"
						, _carriage
						, str_size
						, _capacity
						);
				}

				_carriage += str_size;

				PyObject * obj = pybind::string_from_char_size( str_buffer, str_size );

				return obj;
			}break;
		case PICKLE_UNICODE:
			{
				size_t utf8_size;
				s_read_size_t( _buffer, _capacity, _carriage, utf8_size );

				const char * utf8_buffer = (const char *)(static_cast<const uint8_t *>(_buffer) + _carriage);

				if( _carriage + utf8_size > _capacity )
				{
					pybind::throw_exception("unpickle object unicode out of buffer %d + %d > %d"
						, _carriage
						, utf8_size
						, _capacity
						);
				}

				_carriage += utf8_size;

				PyObject * obj = pybind::unicode_from_utf8_size( utf8_buffer, utf8_size );

				return obj;
			}break;
		case PICKLE_TUPLE:
			{
				size_t count;
				s_read_size_t( _buffer, _capacity, _carriage, count );

				PyObject * obj = pybind::tuple_new( count );

				for( size_t i = 0; i != count; ++i )
				{
					PyObject * element = s_obj_unpickle( _kernel, _buffer, _capacity, _carriage, _types );
					
					pybind::tuple_setitem_t( _kernel, obj, i, element );
				}

				return obj;
			}break;
		case PICKLE_LIST:
			{
				size_t count;
				s_read_size_t( _buffer, _capacity, _carriage, count );

				PyObject * obj = pybind::list_new( count );

				for( size_t i = 0; i != count; ++i )
				{
					PyObject * element = s_obj_unpickle( _kernel, _buffer, _capacity, _carriage, _types );

					pybind::list_setitem_t( _kernel, obj, i, element );
				}

				return obj;
			}break;
		case PICKLE_DICT:
			{
				size_t count;
				s_read_size_t( _buffer, _capacity, _carriage, count );

				PyObject * obj = pybind::dict_new_presized( count );

				for( size_t i = 0; i != count; ++i )
				{
					PyObject * key = s_obj_unpickle( _kernel, _buffer, _capacity, _carriage, _types );
					PyObject * value = s_obj_unpickle( _kernel, _buffer, _capacity, _carriage, _types );

					pybind::dict_set( obj, key, value );
				}

				return obj;
			}break;
		case PICKLE_OBJECT:
			{
				size_t index;
				s_read_size_t( _buffer, _capacity, _carriage, index );

				size_t types_size = pybind::list_size( _types );

				if( index >= types_size )
				{
					pybind::throw_exception("unpickle object has type index %d more of size types %d"
						, index
						, types_size
						);
				}

				PyObject * value = s_obj_unpickle( _kernel, _buffer, _capacity, _carriage, _types );
				
				PyObject * object_type = pybind::list_getitem( _types, index );

				PyObject * obj = pybind::call_t( _kernel, object_type, value );

				if( obj == nullptr )
				{
					pybind::throw_exception("unpickle invalid create object %s type index %d"
						, pybind::object_repr_type( object_type )
						, index
						);
				}

				return obj;
			}break;
		default:
			{
				pybind::throw_exception("unpickle invalid type %d"
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
			PyObject * obj = s_obj_unpickle( _kernel, _buffer, _capacity, carriage, _types );

			return obj;
		}
		catch( const pybind::pybind_exception & _ex )
		{
			pybind::error_message("ptr value %s"
				, _ex.what()
				);
		}

		return nullptr;
	}
}