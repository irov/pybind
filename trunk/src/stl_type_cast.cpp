#	include "pybind/stl_type_cast.hpp"
#	include "pybind/system.hpp"

#	include <string>

namespace pybind
{
	namespace
	{
		struct extract_String_type
			: public pybind::type_cast_result<std::string>
		{
		public:
			bool apply( PyObject * _obj, std::string & _value ) override
			{
				if( pybind::string_check( _obj ) == true )
				{
					size_t size;
					const std::string::value_type * string_char = pybind::string_to_char_and_size( _obj, size  );

					if( string_char == 0 )
					{
						return false;
					}

					_value.assign( string_char, size );
				}
				else
				{
					return false;
				}

				return true;
			}

			PyObject * wrap( pybind::type_cast_result<std::string>::TCastRef _value ) override
			{
				PyObject * py_value = pybind::string_from_char_size( _value.c_str(), _value.size() );

				return py_value;
			}
		};

		struct extract_WString_type
			: public pybind::type_cast_result<std::wstring>
		{
		public:
			bool apply( PyObject * _obj, std::wstring & _value ) override
			{
				if( pybind::unicode_check( _obj ) == true )
				{
					size_t size = 0;
					const pybind::pybind_unicode_t * value_char = pybind::unicode_to_wchar_and_size( _obj, size );

					if( value_char == nullptr )
					{
						return false;
					}

					_value.assign( value_char, size );
				}
				else
				{
					return false;
				}

				return true;
			}

			PyObject * wrap( pybind::type_cast_result<std::wstring>::TCastRef _value ) override
			{
				PyObject * py_value = pybind::unicode_from_wchar_size( _value.c_str(), _value.size() );

				return py_value;
			}
		};
	}
	//////////////////////////////////////////////////////////////////////////
	bool initialize_stl_type_cast()
	{
		pybind::registration_type_cast<std::string>( new extract_String_type );
		pybind::registration_type_cast<std::wstring>( new extract_WString_type );

		pybind::registration_stl_vector_type_cast<std::string>();
		pybind::registration_stl_vector_type_cast<std::wstring>();
		
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void finalize_stl_type_cast()
	{
		pybind::unregistration_stl_vector_type_cast<std::string>();
		pybind::unregistration_stl_vector_type_cast<std::wstring>();
	}
}