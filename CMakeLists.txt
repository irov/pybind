cmake_minimum_required(VERSION 3.16)

if(POLICY CMP0025)
  cmake_policy(SET CMP0025 NEW)
endif ()

if(POLICY CMP0114)
  cmake_policy(SET CMP0114 NEW)
endif ()

OPTION(PYBIND_EXTERNAL_BUILD "PYBIND_EXTERNAL_BUILD"  OFF)
OPTION(PYBIND_STL_NOT_SUPPORT "PYBIND_STL_NOT_SUPPORT" OFF)
OPTION(PYBIND_PYTHON_ERROR_FORMAT_FLAG "PYBIND_PYTHON_ERROR_FORMAT" OFF)
OPTION(PYBIND_SANDBOX "PYBIND_SANDBOX" ON)
OPTION(PYBIND_INSTALL "PYBIND_INSTALL" ON)
SET(PYBIND_BACKEND "python" CACHE STRING "PYBIND_BACKEND")
SET_PROPERTY(CACHE PYBIND_BACKEND PROPERTY STRINGS python tinypy)

IF(NOT PYBIND_BACKEND STREQUAL "python" AND NOT PYBIND_BACKEND STREQUAL "tinypy")
    MESSAGE(FATAL_ERROR "PYBIND_BACKEND must be python or tinypy")
ENDIF()

set(PYBIND_PROJECT_NAME "pybind" CACHE STRING "PYBIND_PROJECT_NAME")
set(PYBIND_PYTHON_VERSION "278" CACHE STRING "PYBIND_PYTHON_VERSION")
set(PYBIND_PYTHON_INCLUDE_PATH "" CACHE STRING "PYBIND_PYTHON_INCLUDE_PATH")
set(PYBIND_PYTHON_CONFIG_INCLUDE_PATH "" CACHE STRING "PYBIND_PYTHON_CONFIG_INCLUDE_PATH")
set(PYBIND_STDEX_INCLUDE_PATH "" CACHE STRING "PYBIND_STDEX_INCLUDE_PATH")
set(PYBIND_TINYPY_SOURCE_DIR "" CACHE PATH "PYBIND_TINYPY_SOURCE_DIR")

PROJECT(${PYBIND_PROJECT_NAME})

IF(NOT PYBIND_EXTERNAL_BUILD)
    set(CMAKE_CXX_STANDARD 17)

    add_definitions(-D_CRT_SECURE_NO_DEPRECATE -D_SCL_SECURE_NO_WARNINGS)
    
    if(WIN32)
        if(MINGW)
            set(CMAKE_STATIC_LIBRARY_PREFIX lib)
            set(CMAKE_STATIC_LIBRARY_SUFFIX .a)
        else()
            set(CMAKE_STATIC_LIBRARY_PREFIX)
            set(CMAKE_STATIC_LIBRARY_SUFFIX .lib)
        endif()
    endif()

    if(UNIX AND NOT APPLE)
        set(CMAKE_STATIC_LIBRARY_PREFIX lib)
        set(CMAKE_STATIC_LIBRARY_SUFFIX .a)
    endif()

    if(APPLE)
        set(CMAKE_STATIC_LIBRARY_PREFIX lib)
        set(CMAKE_STATIC_LIBRARY_SUFFIX .a)
    endif()    
    
    if(${CMAKE_CXX_COMPILER_ID} STREQUAL Clang)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wextra -Werror -Wno-register -Wno-ignored-attributes -Wno-missing-field-initializers -Wno-cast-function-type-mismatch")
    elseif(${CMAKE_CXX_COMPILER_ID} STREQUAL AppleClang)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wextra -Werror -Wno-register -Wno-ignored-attributes -Wno-missing-field-initializers -Wno-cast-function-type-mismatch")
    elseif(${CMAKE_CXX_COMPILER_ID} STREQUAL GNU)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wextra -Werror")
    elseif(${CMAKE_CXX_COMPILER_ID} STREQUAL Intel)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wextra -Werror")
    elseif(${CMAKE_CXX_COMPILER_ID} STREQUAL MSVC)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4 /WX")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd4710") #function not inlined 
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd4738") #storing 32-bit float result in memory, possible loss of performance
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd5033")
    endif()
ENDIF()

IF(NOT PYBIND_EXTERNAL_BUILD)
    set(DEPENDENCIES_DIR "${CMAKE_CURRENT_SOURCE_DIR}/dependencies")
   
    IF(PYBIND_BACKEND STREQUAL "python")
        include(cmake/python_download.cmake)

        set(PYBIND_PYTHON_INCLUDE_PATH ${PYTHON_INCLUDE_DIR} CACHE STRING "PYBIND_PYTHON_INCLUDE_PATH" FORCE)
        message(${PYBIND_PYTHON_INCLUDE_PATH})

        IF(WIN32)
            set(PYBIND_PYTHON_CONFIG_INCLUDE_PATH ${PYTHON_INCLUDE_DIR}/PC CACHE STRING "PYBIND_PYTHON_CONFIG_INCLUDE_PATH" FORCE)
        ENDIF()

        IF(WIN32)
            add_definitions(-D_CRT_SECURE_NO_WARNINGS)
        ENDIF()
    ENDIF()
    
    include(cmake/stdex_download.cmake)
    set(PYBIND_STDEX_INCLUDE_PATH ${STDEX_INCLUDE_DIR} CACHE STRING "PYBIND_STDEX_INCLUDE_PATH" FORCE)
ENDIF()

IF(PYBIND_BACKEND STREQUAL "tinypy" AND NOT TARGET tinypy::tinypy)
    IF(PYBIND_TINYPY_SOURCE_DIR)
        SET(PYBIND_BUILD_TESTING ${BUILD_TESTING})
        SET(BUILD_TESTING OFF)
        ADD_SUBDIRECTORY(${PYBIND_TINYPY_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}/tinypy EXCLUDE_FROM_ALL)
        SET(BUILD_TESTING ${PYBIND_BUILD_TESTING})
    ELSEIF(PYBIND_EXTERNAL_BUILD)
        MESSAGE(FATAL_ERROR "tinypy::tinypy or PYBIND_TINYPY_SOURCE_DIR is required")
    ELSE()
        INCLUDE(FetchContent)
        FetchContent_Declare(tinypy GIT_REPOSITORY https://github.com/irov/tinypy.git GIT_TAG master)
        FetchContent_MakeAvailable(tinypy)
    ENDIF()
ENDIF()

set(CONFIG_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include/config)
set(PUBLIC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include/pybind)
set(PRIVATE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)

file(GLOB files_config_headers ${CONFIG_DIR}/*.hpp)
file(GLOB files_public_adapter_headers ${PUBLIC_DIR}/adapter/*.hpp)
file(GLOB files_public_helper_headers ${PUBLIC_DIR}/helper/*.hpp)
file(GLOB files_public_call_headers ${PUBLIC_DIR}/call/*.hpp)
file(GLOB files_public_generator_headers ${PUBLIC_DIR}/generator/*.hpp)
file(GLOB files_private_generator_sources ${PRIVATE_DIR}/generator/*.cpp)
file(GLOB files_public_headers ${PUBLIC_DIR}/*.hpp)
file(GLOB files_private_headers ${PRIVATE_DIR}/*.hpp)
file(GLOB files_private_sources ${PRIVATE_DIR}/*.cpp)
file(GLOB files_private_python_headers ${PRIVATE_DIR}/python/*.hpp)
file(GLOB files_private_python_sources ${PRIVATE_DIR}/python/*.cpp)
file(GLOB files_private_tinypy_headers ${PRIVATE_DIR}/tinypy/*.hpp)
file(GLOB files_private_tinypy_sources ${PRIVATE_DIR}/tinypy/*.cpp)
file(GLOB files_public_stl_headers ${PUBLIC_DIR}/stl/*.hpp)
file(GLOB files_private_stl_sources ${PRIVATE_DIR}/stl/*.cpp)
file(GLOB files_public_mutex_headers ${PUBLIC_DIR}/mutex/*.hpp)

SOURCE_GROUP(config FILES ${files_config_headers})
SOURCE_GROUP(adapter FILES ${files_public_adapter_headers})
SOURCE_GROUP(helper FILES ${files_public_helper_headers})
SOURCE_GROUP(call FILES ${files_public_call_headers})
SOURCE_GROUP(generator FILES ${files_public_generator_headers} ${files_private_generator_sources})
SOURCE_GROUP(python FILES ${files_private_python_headers} ${files_private_python_sources})
SOURCE_GROUP(tinypy FILES ${files_private_tinypy_headers} ${files_private_tinypy_sources})
SOURCE_GROUP(stl FILES ${files_public_stl_headers} ${files_private_stl_sources})

SOURCE_GROUP(mutex FILES ${files_public_mutex_headers})

INCLUDE_DIRECTORIES(${PROJECT_NAME} include)

if(PYBIND_PYTHON_CONFIG_INCLUDE_PATH)
    include_directories(${PYBIND_PYTHON_CONFIG_INCLUDE_PATH})
endif()

if(PYBIND_PYTHON_INCLUDE_PATH)
    include_directories(${PYBIND_PYTHON_INCLUDE_PATH})
endif()

if(PYBIND_STDEX_INCLUDE_PATH)
    include_directories(${PYBIND_STDEX_INCLUDE_PATH})
endif()

if(CMAKE_BUILD_TYPE MATCHES Debug)
    add_definitions(-DPYBIND_CALL_DEBUG)
endif()

if(PYBIND_STL_NOT_SUPPORT)
    add_definitions(-DPYBIND_STL_NOT_SUPPORT)
endif()

add_definitions(-DPYBIND_PYTHON_VERSION=${PYBIND_PYTHON_VERSION})

if(PYBIND_PYTHON_ERROR_FORMAT_FLAG)
    add_definitions(-DPYBIND_PYTHON_ERROR_FORMAT_FLAG)
endif()

IF(PYBIND_BACKEND STREQUAL "python")
    SET(files_private_backend_headers ${files_private_python_headers})
    SET(files_private_backend_sources ${files_private_python_sources})
    add_definitions(-DPYBIND_BACKEND_PYTHON)
ELSE()
    SET(files_private_backend_headers ${files_private_tinypy_headers})
    SET(files_private_backend_sources ${files_private_tinypy_sources})
    add_definitions(-DPYBIND_BACKEND_TINYPY)
ENDIF()

ADD_LIBRARY(${PROJECT_NAME} STATIC
	${files_config_headers}
	${files_public_headers}
	${files_public_adapter_headers}
	${files_public_helper_headers}
	${files_public_call_headers}
    ${files_public_generator_headers}
    ${files_private_generator_sources}
    ${files_private_backend_headers}
    ${files_private_backend_sources}
	${files_private_headers}
	${files_private_sources}
    ${files_public_stl_headers}
    ${files_private_stl_sources}
    ${files_public_mutex_headers}
	)
    
IF(PYBIND_BACKEND STREQUAL "python")
add_definitions(-DMS_NO_COREDLL)
add_definitions(-DPy_NO_ENABLE_SHARED)
ELSE()
    TARGET_LINK_LIBRARIES(${PROJECT_NAME} tinypy::tinypy)
ENDIF()
    
IF(NOT PYBIND_EXTERNAL_BUILD)
    add_dependencies(${PROJECT_NAME} stdex_download)
    IF(PYBIND_BACKEND STREQUAL "python")
    add_dependencies(${PROJECT_NAME} python_download)
    ENDIF()
ENDIF()

if(PYBIND_SANDBOX)
    add_definitions(-DPYBIND_SANDBOX_SCRIPTS_PATH="${CMAKE_CURRENT_SOURCE_DIR}/sandbox")

    IF(PYBIND_BACKEND STREQUAL "python")
        SET(files_private_sandbox_sources ${CMAKE_CURRENT_SOURCE_DIR}/sandbox/main.cpp)
        SET(PYBIND_SANDBOX_NAME sandbox)
    ELSE()
        SET(files_private_sandbox_sources ${CMAKE_CURRENT_SOURCE_DIR}/sandbox/tinypy_main.cpp)
        SET(PYBIND_SANDBOX_NAME sandbox_tinypy)
    ENDIF()

    SOURCE_GROUP(sandbox FILES ${files_private_sandbox_sources})

    if(PYBIND_PYTHON_INCLUDE_PATH)
        include_directories(${PYBIND_PYTHON_INCLUDE_PATH})
    endif()

    if(PYBIND_STDEX_INCLUDE_PATH)
        include_directories(${PYBIND_STDEX_INCLUDE_PATH})
    endif()

    ADD_EXECUTABLE(${PYBIND_SANDBOX_NAME}
        ${files_private_sandbox_sources}
        )
        
    TARGET_LINK_LIBRARIES(${PYBIND_SANDBOX_NAME} ${PROJECT_NAME})
    add_dependencies(${PYBIND_SANDBOX_NAME} ${PROJECT_NAME})

    IF(PYBIND_BACKEND STREQUAL "python")
        TARGET_LINK_LIBRARIES(${PYBIND_SANDBOX_NAME} python)
        add_dependencies(${PYBIND_SANDBOX_NAME} python_download)
    ENDIF()

    TARGET_LINK_LIBRARIES(${PYBIND_SANDBOX_NAME} stdex)
    add_dependencies(${PYBIND_SANDBOX_NAME} stdex_download)
endif()

if(PYBIND_INSTALL)
    install(DIRECTORY include
        DESTINATION .
        FILES_MATCHING PATTERN "*.hpp" PATTERN "*.h")

    install(TARGETS ${PROJECT_NAME}
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib)
endif()
