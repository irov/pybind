#include "pybind/stl_type_cast.hpp"

#ifdef PYBIND_STL_SUPPORT

#include <string>
#include <vector>

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

                if( _kernel->string_check( _obj ) == true )
                {
                    uint32_t size;
                    const std::string::value_type * string_char = _kernel->string_to_char_and_size( _obj, size );

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

                const std::string::value_type * value_str = _value.c_str();
                std::string::size_type value_size = _value.size();

                PyObject * py_value = _kernel->string_from_char_size( value_str, (uint32_t)value_size );

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

                if( _kernel->unicode_check( _obj ) == true )
                {
                    uint32_t size = 0;
                    const wchar_t * value_char = _kernel->unicode_to_wchar_and_size( _obj, size );

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

                const std::wstring::value_type * value_str = _value.c_str();
                std::wstring::size_type value_size = _value.size();

                PyObject * py_value = _kernel->unicode_from_wchar_size( value_str, (uint32_t)value_size );

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

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void finalize_stl_type_cast( kernel_interface * _kernel )
    {
        pybind::unregistration_stl_vector_type_cast<std::string, std::vector<std::string>>(_kernel);
        pybind::unregistration_stl_vector_type_cast<std::wstring, std::vector<std::wstring>>(_kernel);
    }
}

#endif