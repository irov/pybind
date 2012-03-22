#	pragma once

#	define Py_USING_UNICODE
#	ifndef PYBIND_PYTHON_3
#	include "../../Python/Include/Python.h"
#	include "../../Python/Include/structmember.h"
#	include "../../Python/Include/osdefs.h"
#	else
#	include "../../Python32/include/Python.h"
#	include "../../Python32/include/structmember.h"
#	include "../../Python32/include/osdefs.h"
#	endif

