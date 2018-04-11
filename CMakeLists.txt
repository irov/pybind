cmake_minimum_required(VERSION 3.0)

OPTION(PYBIND_PROJECT_NAME  "PYBIND_PROJECT_NAME" ON)

PROJECT(${PYBIND_PROJECT_NAME})

OPTION(PYBIND_EXTERNAL_BUILD  "PYBIND_EXTERNAL_BUILD"  OFF)
OPTION(PYBIND_PYTHON_INCLUDE_PATH  "PYBIND_PYTHON_INCLUDE_PATH" OFF)
OPTION(PYBIND_PYTHON_CONFIG_INCLUDE_PATH  "PYBIND_PYTHON_CONFIG_INCLUDE_PATH" OFF)
OPTION(PYBIND_STDEX_INCLUDE_PATH  "PYBIND_STDEX_INCLUDE_PATH" OFF)
OPTION(PYBIND_PYTHON_VERSION "PYBIND_PYTHON_VERSION" 278)

IF( NOT PYBIND_EXTERNAL_BUILD )
    set( CMAKE_CXX_STANDARD 11 )
    
    if(${CMAKE_CXX_COMPILER_ID} STREQUAL Clang)
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Werror")
    elseif(${CMAKE_CXX_COMPILER_ID} STREQUAL AppleClang)
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Werror")
    elseif(${CMAKE_CXX_COMPILER_ID} STREQUAL GNU)
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Werror")
    elseif(${CMAKE_CXX_COMPILER_ID} STREQUAL Intel)
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Werror")
    elseif(${CMAKE_CXX_COMPILER_ID} STREQUAL MSVC)
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /Wall /WX")
        set(CMAKE_C_FLAGS "${CMAKE_CXX_FLAGS} /wd4710") #function not inlined 
        set(CMAKE_C_FLAGS "${CMAKE_CXX_FLAGS} /wd4738") #storing 32-bit float result in memory, possible loss of performance
    endif()

    SET( CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/build_temp/${ENGINE_LIB_DIR} )
    SET( CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin/pybind_win32 )
    SET( CMAKE_TEMP_DIR ${CMAKE_CURRENT_SOURCE_DIR}/CMakeTemp/pybind_win32/${CMAKE_GENERATOR}/${CMAKE_BUILD_TYPE} )
    
    set(DEPENDENCIES_DIR "${CMAKE_CURRENT_SOURCE_DIR}/dependencies")

    IF(NOT EXISTS ${DEPENDENCIES_DIR}/Python)
        message("Downloading python, this may take some time!")
        file(DOWNLOAD https://www.python.org/ftp/python/2.7.14/Python-2.7.14.tgz ${DEPENDENCIES_DIR}/Python-2.7.14.tgz SHOW_PROGRESS)
        
        execute_process(
            COMMAND ${CMAKE_COMMAND} -E tar x ${DEPENDENCIES_DIR}/Python-2.7.14.tgz
            WORKING_DIRECTORY "${DEPENDENCIES_DIR}"
        )
        
        execute_process(
            COMMAND ${CMAKE_COMMAND} -E rename
            ${DEPENDENCIES_DIR}/Python-2.7.14 ${DEPENDENCIES_DIR}/Python
        )
        
        message("Downloading pyconfig.h, this may take some time!")
        file(DOWNLOAD https://raw.githubusercontent.com/irov/Mengine/master/CMake/Dependencies/Python/ModifiedMac/pyconfig.h ${DEPENDENCIES_DIR}/PythonConfig/pyconfig.h SHOW_PROGRESS)
    ENDIF()
    
    SET(PYBIND_PYTHON_INCLUDE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/Python CACHE STRING "PYBIND_PYTHON_INCLUDE_PATH" FORCE)
    
    IF(WIN32)
        SET(PYBIND_PYTHON_CONFIG_INCLUDE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/Python/PC CACHE STRING "PYBIND_PYTHON_CONFIG_INCLUDE_PATH" FORCE)
    ELSE()
        SET(PYBIND_PYTHON_CONFIG_INCLUDE_PATH ${DEPENDENCIES_DIR}/PythonConfig CACHE STRING "PYBIND_PYTHON_CONFIG_INCLUDE_PATH" FORCE)
    ENDIF()
    
    IF(WIN32)
        add_definitions(-D_CRT_SECURE_NO_WARNINGS)
    ENDIF()
    
    include(ExternalProject)
    
    ExternalProject_Add(stdex_download PREFIX stdex
        GIT_REPOSITORY https://github.com/irov/stdex.git
        
        UPDATE_COMMAND ""
        INSTALL_COMMAND ""
  
        CMAKE_ARGS
            "-DSTDEX_EXTERNAL_BUILD=ON"
            "-DCMAKE_CXX_STANDARD=11"
    )
    
    ExternalProject_Get_Property(stdex_download SOURCE_DIR)
    set(STDEX_INCLUDE_DIR ${SOURCE_DIR}/include)
    set(STDEX_LIBRARY_DIR ${SOURCE_DIR}/lib)
    
    set(STDEX_INCLUDE_DIR ${STDEX_INCLUDE_DIR} CACHE STRING "")
    set(STDEX_LIBRARY_DIR ${STDEX_LIBRARY_DIR} CACHE STRING "")
    
    SET(PYBIND_STDEX_INCLUDE_PATH ${STDEX_INCLUDE_DIR} CACHE STRING "PYBIND_STDEX_INCLUDE_PATH" FORCE)
ENDIF()

SET( CONFIG_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include/config )
SET( PUBLIC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include/pybind )
SET( PRIVATE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src )

file(GLOB files_config_headers ${CONFIG_DIR}/*.hpp)
file(GLOB files_public_headers ${PUBLIC_DIR}/*.hpp)
file(GLOB files_private_headers ${PRIVATE_DIR}/*.hpp)
file(GLOB files_private_sources ${PRIVATE_DIR}/*.cpp)

INCLUDE_DIRECTORIES( ${PROJECT_NAME} include )

if(PYBIND_PYTHON_INCLUDE_PATH)
    include_directories(${PYBIND_PYTHON_INCLUDE_PATH})
endif()

if(PYBIND_PYTHON_CONFIG_INCLUDE_PATH)
    include_directories(${PYBIND_PYTHON_CONFIG_INCLUDE_PATH})
endif()

if(PYBIND_STDEX_INCLUDE_PATH)
    include_directories(${PYBIND_STDEX_INCLUDE_PATH})
endif()

if( CMAKE_BUILD_TYPE MATCHES Debug )
    add_definitions(-DPYBIND_CALL_DEBUG)
endif()

add_definitions(-DMS_NO_COREDLL)
add_definitions(-DPy_NO_ENABLE_SHARED)


add_definitions(-DPYBIND_PYTHON_VERSION=${PYBIND_PYTHON_VERSION})

ADD_LIBRARY( ${PROJECT_NAME} STATIC ${files_config_headers} ${files_public_headers} ${files_private_headers} ${files_private_sources} )

IF( NOT PYBIND_EXTERNAL_BUILD )
    TARGET_LINK_LIBRARIES( ${PROJECT_NAME} stdex )
    add_dependencies(${PROJECT_NAME} stdex_download)
ENDIF()