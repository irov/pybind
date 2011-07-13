#	pragma once

#	define Py_USING_UNICODE

#	ifndef PYBIND_PYTHON_3
#	include "../../Python/Include/Python.h"
#	include "../../Python/Include/structmember.h"
#	include "../../Python/Include/osdefs.h"
#	else
#	include "../../../../Python-3.2/Include/Python.h"
#	include "../../../../Python-3.2/Include/structmember.h"
#	include "../../../../Python-3.2/Include/osdefs.h"
#	endif