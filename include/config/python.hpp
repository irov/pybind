#	pragma once

#	ifndef PYBIND_PYTHON_3
#	define Py_USING_UNICODE

#	include "../../../Python/include/Python.h"
#	include "../../../Python/include/osdefs.h"
#	include "../../../Python/include/structmember.h"
#	else
#	include "../../../Python32/include/Python.h"
#	include "../../../Python32/include/osdefs.h"
#	include "../../../Python32/include/structmember.h"
#	endif