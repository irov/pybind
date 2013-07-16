#	pragma once

#   ifndef PYBIND_PYTHON_VERSION
#   define PYBIND_PYTHON_VERSION 274
#   endif

//#   define PYBIND_VISIT_OBJECTS

#   define PYBIND_TYPE_COUNT 256
#   define PYBIND_FUNCTION_COUNT 256
#   define PYBIND_FUNCTOR_COUNT 256

namespace pybind
{
#   ifdef PYBIND_MARMALADE
    typedef wchar_t pybind_unicode_t;
#   else
    typedef wchar_t pybind_unicode_t;
#   endif
}