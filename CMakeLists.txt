cmake_minimum_required(VERSION 3.0)

if(POLICY CMP0025)
  cmake_policy(SET CMP0025 NEW)
endif ()

OPTION(PYBIND_EXTERNAL_BUILD "PYBIND_EXTERNAL_BUILD"  OFF)
OPTION(PYBIND_STL_NOT_SUPPORT "PYBIND_STL_NOT_SUPPORT" OFF)
OPTION(PYBIND_PYTHON_ERROR_FORMAT_FLAG "PYBIND_PYTHON_ERROR_FORMAT" OFF)
OPTION(PYBIND_SANDBOX "PYBIND_SANDBOX" ON)
OPTION(PYBIND_INSTALL "PYBIND_INSTALL" ON)

set(PYBIND_PROJECT_NAME "pybind" CACHE STRING "PYBIND_PROJECT_NAME")
set(PYBIND_PYTHON_VERSION "278" CACHE STRING "PYBIND_PYTHON_VERSION")
set(PYBIND_PYTHON_INCLUDE_PATH "" CACHE STRING "PYBIND_PYTHON_INCLUDE_PATH")
set(PYBIND_PYTHON_CONFIG_INCLUDE_PATH "" CACHE STRING "PYBIND_PYTHON_CONFIG_INCLUDE_PATH")
set(PYBIND_STDEX_INCLUDE_PATH "" CACHE STRING "PYBIND_STDEX_INCLUDE_PATH")

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
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wextra -Werror")
    elseif(${CMAKE_CXX_COMPILER_ID} STREQUAL AppleClang)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wextra -Werror")
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
   
    include(cmake/python_download.cmake)
    
    set(PYBIND_PYTHON_INCLUDE_PATH ${PYTHON_INCLUDE_DIR} CACHE STRING "PYBIND_PYTHON_INCLUDE_PATH" FORCE)
    message(${PYBIND_PYTHON_INCLUDE_PATH})
    
    IF(WIN32)
        set(PYBIND_PYTHON_CONFIG_INCLUDE_PATH ${PYTHON_INCLUDE_DIR}/PC CACHE STRING "PYBIND_PYTHON_CONFIG_INCLUDE_PATH" FORCE)
    ENDIF()
    
    IF(WIN32)
        add_definitions(-D_CRT_SECURE_NO_WARNINGS)
    ENDIF()
    
    include(cmake/stdex_download.cmake)
    set(PYBIND_STDEX_INCLUDE_PATH ${STDEX_INCLUDE_DIR} CACHE STRING "PYBIND_STDEX_INCLUDE_PATH" FORCE)
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
file(GLOB files_public_stl_headers ${PUBLIC_DIR}/stl/*.hpp)
file(GLOB files_private_stl_sources ${PRIVATE_DIR}/stl/*.cpp)
file(GLOB files_public_mutex_headers ${PUBLIC_DIR}/mutex/*.hpp)

SOURCE_GROUP(config FILES ${files_config_headers})
SOURCE_GROUP(adapter FILES ${files_public_adapter_headers})
SOURCE_GROUP(helper FILES ${files_public_helper_headers})
SOURCE_GROUP(call FILES ${files_public_call_headers})
SOURCE_GROUP(generator FILES ${files_public_generator_headers} ${files_private_generator_sources})
SOURCE_GROUP(python FILES ${files_private_python_headers} ${files_private_python_sources})
SOURCE_GROUP(stl FILES ${files_public_stl_headers} ${files_private_stl_sources})

SOURCE_GROUP(mutex FILES ${files_public_mutex_headers})

INCLUDE_DIRECTORIES(${PROJECT_NAME} include)

if(PYBIND_PYTHON_INCLUDE_PATH)
    include_directories(${PYBIND_PYTHON_INCLUDE_PATH})
endif()

if(PYBIND_PYTHON_CONFIG_INCLUDE_PATH)
    include_directories(${PYBIND_PYTHON_CONFIG_INCLUDE_PATH})
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

ADD_LIBRARY(${PROJECT_NAME} STATIC 
	${files_config_headers} 
	${files_public_headers} 
	${files_public_adapter_headers} 
	${files_public_helper_headers} 
	${files_public_call_headers}
    ${files_public_generator_headers}
    ${files_private_generator_sources}
	${files_private_headers} 
	${files_private_sources}
    ${files_public_stl_headers}
    ${files_private_stl_sources}
    ${files_public_mutex_headers}
	)
    
add_definitions(-DMS_NO_COREDLL)
add_definitions(-DPy_NO_ENABLE_SHARED)
    
ADD_LIBRARY(${PROJECT_NAME}_python STATIC 
    ${files_private_python_sources}
    )
    
IF(NOT PYBIND_EXTERNAL_BUILD)
    add_dependencies(${PROJECT_NAME} stdex_download)
    add_dependencies(${PROJECT_NAME} python_download)
    
    add_dependencies(${PROJECT_NAME}_python stdex_download)
    add_dependencies(${PROJECT_NAME}_python python_download)
ENDIF()

if(PYBIND_SANDBOX)
    file(GLOB files_private_sandbox_sources ${CMAKE_CURRENT_SOURCE_DIR}/sandbox/*.cpp)

    SOURCE_GROUP(sandbox FILES ${files_private_sandbox_sources})

    if(PYBIND_PYTHON_INCLUDE_PATH)
        include_directories(${PYBIND_PYTHON_INCLUDE_PATH})
    endif()

    if(PYBIND_STDEX_INCLUDE_PATH)
        include_directories(${PYBIND_STDEX_INCLUDE_PATH})
    endif()

    ADD_EXECUTABLE(sandbox 
        ${files_private_sandbox_sources}
        )
        
    TARGET_LINK_LIBRARIES(sandbox ${PROJECT_NAME} ${PROJECT_NAME}_python)
    add_dependencies(sandbox ${PROJECT_NAME} ${PROJECT_NAME}_python)

    TARGET_LINK_LIBRARIES(sandbox python)
    add_dependencies(sandbox python_download)

    TARGET_LINK_LIBRARIES(sandbox stdex)
    add_dependencies(sandbox stdex_download)
endif()

if(PYBIND_INSTALL)
    install(DIRECTORY include
        DESTINATION .
        FILES_MATCHING PATTERN "*.hpp" PATTERN "*.h")

    install(TARGETS ${PROJECT_NAME}
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib)
endif()