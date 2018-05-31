#	include "pybind/stl_type_cast.hpp"
#	include "pybind/system.hpp"

#	include "config/stdex.hpp"

#	include <string>
#	include <vector>

namespace pybind
{
    namespace helper
    {
        //////////////////////////////////////////////////////////////////////////
        struct extract_stl_string_type
            : public pybind::type_cast_result<std::string>
        {
        public:
            bool apply( kernel_interface * _kernel, PyObject * _obj, std::string & _value, bool _nothrow ) override
            {
                (void)_kernel;
                (void)_nothrow;

                if( pybind::string_check( _obj ) == true )
                {
                    size_t size;
                    const std::string::value_type * string_char = pybind::string_to_char_and_size( _obj, size );

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

        public:
            PyObject * wrap( kernel_interface * _kernel, pybind::type_cast_result<std::string>::TCastRef _value ) override
            {
                (void)_kernel;

                PyObject * py_value = pybind::string_from_char_size( _value.c_str(), _value.size() );

                return py_value;
            }
        };
        //////////////////////////////////////////////////////////////////////////
        struct extract_stl_wstring_type
            : public pybind::type_cast_result<std::wstring>
        {
        public:
            bool apply( kernel_interface * _kernel, PyObject * _obj, std::wstring & _value, bool _nothrow ) override
            {
                (void)_kernel;
                (void)_nothrow;

                if( pybind::unicode_check( _obj ) == true )
                {
                    size_t size = 0;
                    const wchar_t * value_char = pybind::unicode_to_wchar_and_size( _obj, size );

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

        public:
            PyObject * wrap( kernel_interface * _kernel, pybind::type_cast_result<std::wstring>::TCastRef _value ) override
            {
                (void)_kernel;

                PyObject * py_value = pybind::unicode_from_wchar_size( _value.c_str(), _value.size() );

                return py_value;
            }
        };
        //////////////////////////////////////////////////////////////////////////
        struct extract_stdex_stl_string_type
            : public pybind::type_cast_result<stdex::string>
        {
        public:
            bool apply( kernel_interface * _kernel, PyObject * _obj, stdex::string & _value, bool _nothrow ) override
            {
                (void)_kernel;
                (void)_nothrow;

                if( pybind::string_check( _obj ) == true )
                {
                    size_t size;
                    const stdex::string::value_type * string_char = pybind::string_to_char_and_size( _obj, size );

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

        public:
            PyObject * wrap( kernel_interface * _kernel, pybind::type_cast_result<stdex::string>::TCastRef _value ) override
            {
                (void)_kernel;
                PyObject * py_value = pybind::string_from_char_size( _value.c_str(), _value.size() );

                return py_value;
            }
        };
        //////////////////////////////////////////////////////////////////////////
        struct extract_stdex_stl_wstring_type
            : public pybind::type_cast_result<stdex::wstring>
        {
        public:
            bool apply( kernel_interface * _kernel, PyObject * _obj, stdex::wstring & _value, bool _nothrow ) override
            {
                (void)_kernel;
                (void)_nothrow;

                if( pybind::unicode_check( _obj ) == true )
                {
                    size_t size = 0;
                    const wchar_t * value_char = pybind::unicode_to_wchar_and_size( _obj, size );

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

        public:
            PyObject * wrap( kernel_interface * _kernel, pybind::type_cast_result<stdex::wstring>::TCastRef _value ) override
            {
                (void)_kernel;
                PyObject * py_value = pybind::unicode_from_wchar_size( _value.c_str(), _value.size() );

                return py_value;
            }
        };
    }
    //////////////////////////////////////////////////////////////////////////
    bool initialize_stl_type_cast( kernel_interface * _kernel )
    {
        pybind::registration_type_cast<std::string>(_kernel, new helper::extract_stl_string_type);
        pybind::registration_type_cast<std::wstring>(_kernel, new helper::extract_stl_wstring_type);

        pybind::registration_stl_vector_type_cast<std::string, std::vector<std::string>>(_kernel);
        pybind::registration_stl_vector_type_cast<std::wstring, std::vector<std::wstring>>(_kernel);

        pybind::registration_type_cast<stdex::string>(_kernel, new helper::extract_stdex_stl_string_type);
        pybind::registration_type_cast<stdex::wstring>(_kernel, new helper::extract_stdex_stl_wstring_type);

        pybind::registration_stl_vector_type_cast<stdex::string, stdex::vector<stdex::string>>(_kernel);
        pybind::registration_stl_vector_type_cast<stdex::wstring, stdex::vector<stdex::wstring>>(_kernel);

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void finalize_stl_type_cast( kernel_interface * _kernel )
    {
        pybind::unregistration_stl_vector_type_cast<std::string, std::vector<std::string>>(_kernel);
        pybind::unregistration_stl_vector_type_cast<std::wstring, std::vector<std::wstring>>(_kernel);

        pybind::unregistration_stl_vector_type_cast<stdex::string, stdex::vector<stdex::string>>(_kernel);
        pybind::unregistration_stl_vector_type_cast<stdex::wstring, stdex::vector<stdex::wstring>>(_kernel);
    }
}