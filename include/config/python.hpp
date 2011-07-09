#	pragma once

#	define Py_USING_UNICODE

#	ifndef PYBIND_PYTHON_3
#	include "../../../../Python-2.6/Include/Python.h"
#	include "../../../../Python-2.6/Include/structmember.h"
#	include "../../../../Python-2.6/Include/osdefs.h"
#	else
#	include "../../../../Python-3.2/Include/Python.h"
#	include "../../../../Python-3.2/Include/structmember.h"
#	include "../../../../Python-3.2/Include/osdefs.h"
#	endif