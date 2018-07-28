#pragma once

#include "Include/Python.h"
#include "Include/structmember.h"
#include "Include/osdefs.h"
#include "Include/marshal.h"
#include "Include/object.h"
#include "Include/frameobject.h"
#include "Include/pydebug.h"

#if PYBIND_PYTHON_VERSION >= 350
#   include "Include/pylifecycle.h"
#endif