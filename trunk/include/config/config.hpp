#	pragma once

#   ifndef PYBIND_PYTHON_VERSION
#   define PYBIND_PYTHON_VERSION 274
#   endif

//#   define PYBIND_VISIT_OBJECTS

namespace pybind
{
#   ifdef PYBIND_MARMALADE
    typedef wchar_t pybind_unicode_t;
#   else
    typedef wchar_t pybind_unicode_t;
#   endif
}