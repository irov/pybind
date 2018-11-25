cmake_minimum_required(VERSION 2.4.5 FATAL_ERROR)

configure_file(${PYBIND_DIR}/cmake/python_config.c ${CMAKE_CURRENT_SOURCE_DIR}/config.c COPYONLY)
configure_file(${PYBIND_DIR}/cmake/python_config.h  ${CMAKE_CURRENT_SOURCE_DIR}/PC/pyconfig.h COPYONLY)

project(python)

REMOVE_DEFINITIONS(-Doverride)

set(MOD_SRCS )

# this macro let's you decide whether you want to build the macro 
# and if yes, whether you want to build it statically or as module
macro(ADD_PYTHON_MODULE _NAME _STATIC_SRCS)
  option(MODULE_${_NAME}_ENABLE "Add module ${_NAME}" TRUE)

  if(MODULE_${_NAME}_ENABLE)
    option(MODULE_${_NAME}_SHARED "Add module ${_NAME} shared" FALSE)
    if(MODULE_${_NAME}_SHARED)
      message(STATUS "NOT Adding ${_NAME} as shared module (not yet implemented)")
    else()
      message(STATUS "Adding ${_NAME} as static module")
      foreach (_current_FILE ${ARGN})
        set(${_STATIC_SRCS} ${${_STATIC_SRCS}} ${_current_FILE})
      endforeach (_current_FILE ${ARGN})
      set(PY_STATIC_MODULES_INIT_FUNCTIONS "${PY_STATIC_MODULES_INIT_FUNCTIONS}extern void init${_NAME}(void);\n")
      set(PY_STATIC_MODULES_INITTAB        "${PY_STATIC_MODULES_INITTAB}  {\"${_NAME}\", init${_NAME}},\n")
    endif()
  endif()
endmacro()

#add_python_module(posix  MOD_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/Modules/posixmodule.c)
add_python_module(signal    MOD_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/Modules/signalmodule.c)
add_python_module(errno     MOD_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/Modules/errnomodule.c)
#add_python_module(pwd       MOD_SRCS ${TRUE_DIR}/Modules/pwdmodule.c)
add_python_module(_sre      MOD_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/Modules/_sre.c)
add_python_module(_codecs   MOD_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/Modules/_codecsmodule.c)
#add_python_module(zipimport MOD_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/Modules/zipimport.c)
add_python_module(_symtable MOD_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/Modules/symtablemodule.c)
add_python_module(xxsubtype MOD_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/Modules/xxsubtype.c)
add_python_module(_struct MOD_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/Modules/_struct.c)
add_python_module(binascii MOD_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/Modules/binascii.c)
add_python_module(unicodedata MOD_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/Modules/unicodedata.c)
add_python_module(cPickle MOD_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/Modules/cPickle.c)
add_python_module(cStringIO MOD_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/Modules/cStringIO.c)
add_python_module(cStringIO MOD_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/Modules/_weakref.c)


set( HAVE_CHROOT 0 )
set( HAVE_FSYNC 0 )
set( HAVE_WAIT3 0 )
set( HAVE_WAIT4 0 )
set( HAVE_DEVICE_MACROS 0 )

############################## 

set(PARSER_SRCS      ${CMAKE_CURRENT_SOURCE_DIR}/Parser/acceler.c
                ${CMAKE_CURRENT_SOURCE_DIR}/Parser/grammar1.c
                ${CMAKE_CURRENT_SOURCE_DIR}/Parser/listnode.c
                ${CMAKE_CURRENT_SOURCE_DIR}/Parser/node.c
                ${CMAKE_CURRENT_SOURCE_DIR}/Parser/parser.c
                ${CMAKE_CURRENT_SOURCE_DIR}/Parser/parsetok.c
                ${CMAKE_CURRENT_SOURCE_DIR}/Parser/bitset.c
                ${CMAKE_CURRENT_SOURCE_DIR}/Parser/metagrammar.c
                ${CMAKE_CURRENT_SOURCE_DIR}/Parser/firstsets.c
                ${CMAKE_CURRENT_SOURCE_DIR}/Parser/grammar.c
                ${CMAKE_CURRENT_SOURCE_DIR}/Parser/pgen.c
                ${CMAKE_CURRENT_SOURCE_DIR}/Parser/myreadline.c
                ${CMAKE_CURRENT_SOURCE_DIR}/Parser/tokenizer.c 
                )


set(OBJECT_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/Objects/abstract.c
                ${CMAKE_CURRENT_SOURCE_DIR}/Objects/boolobject.c
                ${CMAKE_CURRENT_SOURCE_DIR}/Objects/bufferobject.c
				${CMAKE_CURRENT_SOURCE_DIR}/Objects/bytes_methods.c
				${CMAKE_CURRENT_SOURCE_DIR}/Objects/bytearrayobject.c
                ${CMAKE_CURRENT_SOURCE_DIR}/Objects/cellobject.c
                ${CMAKE_CURRENT_SOURCE_DIR}/Objects/classobject.c
                ${CMAKE_CURRENT_SOURCE_DIR}/Objects/cobject.c
                ${CMAKE_CURRENT_SOURCE_DIR}/Objects/codeobject.c
                ${CMAKE_CURRENT_SOURCE_DIR}/Objects/complexobject.c
                ${CMAKE_CURRENT_SOURCE_DIR}/Objects/descrobject.c
                ${CMAKE_CURRENT_SOURCE_DIR}/Objects/enumobject.c
                ${CMAKE_CURRENT_SOURCE_DIR}/Objects/exceptions.c
                ${CMAKE_CURRENT_SOURCE_DIR}/Objects/genobject.c
                ${CMAKE_CURRENT_SOURCE_DIR}/Objects/fileobject.c
                ${CMAKE_CURRENT_SOURCE_DIR}/Objects/floatobject.c
                ${CMAKE_CURRENT_SOURCE_DIR}/Objects/frameobject.c
                ${CMAKE_CURRENT_SOURCE_DIR}/Objects/funcobject.c
                ${CMAKE_CURRENT_SOURCE_DIR}/Objects/intobject.c
                ${CMAKE_CURRENT_SOURCE_DIR}/Objects/iterobject.c
                ${CMAKE_CURRENT_SOURCE_DIR}/Objects/listobject.c
                ${CMAKE_CURRENT_SOURCE_DIR}/Objects/longobject.c
                ${CMAKE_CURRENT_SOURCE_DIR}/Objects/dictobject.c
                ${CMAKE_CURRENT_SOURCE_DIR}/Objects/methodobject.c
                ${CMAKE_CURRENT_SOURCE_DIR}/Objects/moduleobject.c
                ${CMAKE_CURRENT_SOURCE_DIR}/Objects/object.c
                ${CMAKE_CURRENT_SOURCE_DIR}/Objects/obmalloc.c
                ${CMAKE_CURRENT_SOURCE_DIR}/Objects/rangeobject.c
                ${CMAKE_CURRENT_SOURCE_DIR}/Objects/setobject.c
                ${CMAKE_CURRENT_SOURCE_DIR}/Objects/sliceobject.c
                ${CMAKE_CURRENT_SOURCE_DIR}/Objects/stringobject.c
                ${CMAKE_CURRENT_SOURCE_DIR}/Objects/structseq.c
                ${CMAKE_CURRENT_SOURCE_DIR}/Objects/tupleobject.c
                ${CMAKE_CURRENT_SOURCE_DIR}/Objects/typeobject.c
                ${CMAKE_CURRENT_SOURCE_DIR}/Objects/weakrefobject.c
                ${CMAKE_CURRENT_SOURCE_DIR}/Objects/capsule.c
                ${CMAKE_CURRENT_SOURCE_DIR}/Objects/memoryobject.c
                
                
   )

set(OBJECT_SRCS ${OBJECT_SRCS} ${CMAKE_CURRENT_SOURCE_DIR}/Objects/unicodeobject.c ${CMAKE_CURRENT_SOURCE_DIR}/Objects/unicodectype.c)   

set(PYTHON_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/Python/_warnings.c
				${CMAKE_CURRENT_SOURCE_DIR}/Python/Python-ast.c 
                ${CMAKE_CURRENT_SOURCE_DIR}/Python/asdl.c 
                ${CMAKE_CURRENT_SOURCE_DIR}/Python/ast.c 
                ${CMAKE_CURRENT_SOURCE_DIR}/Python/bltinmodule.c 
                ${CMAKE_CURRENT_SOURCE_DIR}/Python/ceval.c 
                ${CMAKE_CURRENT_SOURCE_DIR}/Python/compile.c 
                ${CMAKE_CURRENT_SOURCE_DIR}/Python/codecs.c 
                ${CMAKE_CURRENT_SOURCE_DIR}/Python/errors.c 
                ${CMAKE_CURRENT_SOURCE_DIR}/Python/frozen.c 
                ${CMAKE_CURRENT_SOURCE_DIR}/Python/frozenmain.c 
                ${CMAKE_CURRENT_SOURCE_DIR}/Python/future.c 
                ${CMAKE_CURRENT_SOURCE_DIR}/Python/getargs.c 
                ${CMAKE_CURRENT_SOURCE_DIR}/Python/getcompiler.c 
                ${CMAKE_CURRENT_SOURCE_DIR}/Python/getcopyright.c 
                ${CMAKE_CURRENT_SOURCE_DIR}/Python/getplatform.c 
                ${CMAKE_CURRENT_SOURCE_DIR}/Python/getversion.c 
                ${CMAKE_CURRENT_SOURCE_DIR}/Python/graminit.c 
                ${CMAKE_CURRENT_SOURCE_DIR}/Python/import.c 
                ${CMAKE_CURRENT_SOURCE_DIR}/Python/importdl.c 
                ${CMAKE_CURRENT_SOURCE_DIR}/Python/marshal.c 
                ${CMAKE_CURRENT_SOURCE_DIR}/Python/modsupport.c 
                ${CMAKE_CURRENT_SOURCE_DIR}/Python/mystrtoul.c 
                ${CMAKE_CURRENT_SOURCE_DIR}/Python/mysnprintf.c 
				${CMAKE_CURRENT_SOURCE_DIR}/Python/peephole.c
                ${CMAKE_CURRENT_SOURCE_DIR}/Python/pyarena.c 
                ${CMAKE_CURRENT_SOURCE_DIR}/Python/pyfpe.c 
				${CMAKE_CURRENT_SOURCE_DIR}/Python/pymath.c
                ${CMAKE_CURRENT_SOURCE_DIR}/Python/pystate.c 
                ${CMAKE_CURRENT_SOURCE_DIR}/Python/pythonrun.c 
                ${CMAKE_CURRENT_SOURCE_DIR}/Python/structmember.c 
                ${CMAKE_CURRENT_SOURCE_DIR}/Python/symtable.c 
                ${CMAKE_CURRENT_SOURCE_DIR}/Python/sysmodule.c 
                ${CMAKE_CURRENT_SOURCE_DIR}/Python/traceback.c 
                ${CMAKE_CURRENT_SOURCE_DIR}/Python/getopt.c 
				${CMAKE_CURRENT_SOURCE_DIR}/Python/pystrcmp.c
                ${CMAKE_CURRENT_SOURCE_DIR}/Python/pystrtod.c 
				${CMAKE_CURRENT_SOURCE_DIR}/Python/formatter_unicode.c
				${CMAKE_CURRENT_SOURCE_DIR}/Python/formatter_string.c
                ${CMAKE_CURRENT_SOURCE_DIR}/Python/pyctype.c                
                ${CMAKE_CURRENT_SOURCE_DIR}/Python/dtoa.c
                ${CMAKE_CURRENT_SOURCE_DIR}/Python/random.c
                ${CMAKE_CURRENT_SOURCE_DIR}/Python/thread.c
                )

set(MODULE_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/Modules/main.c 
                ${CMAKE_CURRENT_SOURCE_DIR}/Modules/gcmodule.c
                ${CMAKE_CURRENT_SOURCE_DIR}/Modules/mathmodule.c
                ${CMAKE_CURRENT_SOURCE_DIR}/Modules/getbuildinfo.c
                ${CMAKE_CURRENT_SOURCE_DIR}/Modules/_math.c
                ${MOD_SRCS} )
				
if( NOT WIN32 )
	set( MODULE_SRCS ${MODULE_SRCS} ${CMAKE_CURRENT_SOURCE_DIR}/Modules/getpath.c )
else()
    set( MODULE_SRCS ${MODULE_SRCS} ${CMAKE_CURRENT_SOURCE_DIR}/PC/dl_nt.c )
	set( MODULE_SRCS ${MODULE_SRCS} ${CMAKE_CURRENT_SOURCE_DIR}/PC/getpathp.c )
endif()

if(WIN32)
    set(PYTHON_SRCS ${PYTHON_SRCS} ${CMAKE_CURRENT_SOURCE_DIR}/Python/dynload_win.c)
endif()

set( MODULE_SRCS ${MODULE_SRCS} ${CMAKE_CURRENT_SOURCE_DIR}/config.c )

set_source_files_properties(${CMAKE_CURRENT_SOURCE_DIR}/Python/getplatform.c PROPERTIES COMPILE_FLAGS -DPLATFORM=\\"${PY_PLATFORM}\\")
set_source_files_properties(${CMAKE_CURRENT_SOURCE_DIR}/Modules/getpath.c PROPERTIES COMPILE_FLAGS  "-DPREFIX=\\\"\\\" -DEXEC_PREFIX=\\\"\\\" -DVERSION=\\\"2.7.14\\\" -DVPATH=\\\"..\\\"  -DPYTHONPATH=\\\"\\\" " )

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/Include ${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/Python)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/PC)

if(WIN32)  
  if(MSVC)
	  add_definitions(-DWIN32 -D_WINDOWS)
  else()
	  add_definitions(-D_WINDOWS)
  endif()
endif()

add_definitions(-DMS_NO_COREDLL)
add_definitions(-DPy_NO_ENABLE_SHARED)
add_definitions(-DPy_BUILD_CORE)
			  
add_library(${PROJECT_NAME} STATIC			  
              ${PARSER_SRCS}
              ${OBJECT_SRCS}
              ${PYTHON_SRCS}
              ${MODULE_SRCS})

install(DIRECTORY Include
    DESTINATION ./include
    FILES_MATCHING PATTERN "*.hpp" PATTERN "*.h")
    
install(DIRECTORY PC
    DESTINATION ./include
    FILES_MATCHING PATTERN "*.hpp" PATTERN "*.h")
    
install(TARGETS ${PROJECT_NAME}
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib)
