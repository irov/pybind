#pragma once

#ifndef NDEBUG
#   define PYBIND_DEBUG
#else
#   define PYBIND_RELEASE
#endif

#ifndef PYBIND_PYTHON_VERSION
#define PYBIND_PYTHON_VERSION 276
#endif

#ifndef PYBIND_BASES_COUNT
#define PYBIND_BASES_COUNT 16
#endif

#ifndef PYBIND_METHOD_COUNT
#define PYBIND_METHOD_COUNT 256
#endif

#ifndef PYBIND_MEMBER_COUNT
#define PYBIND_MEMBER_COUNT 256
#endif

#ifndef PYBIND_TYPE_COUNT
#define PYBIND_TYPE_COUNT 512
#endif

#ifndef PYBIND_TYPE_COUNT_HASH
#define PYBIND_TYPE_COUNT_HASH 1021
#endif

#ifndef PYBIND_FUNCTION_COUNT
#define PYBIND_FUNCTION_COUNT 512
#endif

#ifndef PYBIND_FUNCTOR_COUNT
#define PYBIND_FUNCTOR_COUNT 512
#endif

#ifndef PYBIND_STL_NOT_SUPPORT
#define PYBIND_STL_SUPPORT
#endif

#ifndef PYBIND_OBJECT_POD_SIZE
#define PYBIND_OBJECT_POD_SIZE 256
#endif

#ifndef PYBIND_MEMCPY
#   include "stdex/memorycopy.h"
#	define PYBIND_MEMCPY(Dest, Src, Size) stdex::memorycopy(Dest, Src, Size)
#endif