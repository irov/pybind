include(ExternalProject)

ExternalProject_Add(python_download PREFIX python
    GIT_REPOSITORY https://github.com/python/cpython.git
    GIT_TAG "v2.7.15"
    GIT_PROGRESS TRUE
    
    UPDATE_COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/cmake/python_CMakeLists.cmake ${CMAKE_CURRENT_BINARY_DIR}/python/src/python_download/CMakeLists.txt
    
    CMAKE_ARGS 
        -DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>
        -DPYBIND_DIR=${CMAKE_CURRENT_SOURCE_DIR}
)

    #CONFIGURE_COMMAND ${CMAKE_CURRENT_BINARY_DIR}/python/src/configure
    #BUILD_COMMAND make
    #BUILD_IN_SOURCE 1



ExternalProject_Get_Property(python_download INSTALL_DIR)

set(PYTHON_INCLUDE_DIR ${INSTALL_DIR}/include)
set(PYTHON_LIBRARY_DIR ${INSTALL_DIR}/lib)

set(PYTHON_INCLUDE_DIR ${PYTHON_INCLUDE_DIR} CACHE STRING "")
set(PYTHON_LIBRARY_DIR ${PYTHON_LIBRARY_DIR} CACHE STRING "")


add_library(python STATIC IMPORTED)

set_target_properties(python PROPERTIES IMPORTED_LOCATION ${PYTHON_LIBRARY_DIR}/${CMAKE_STATIC_LIBRARY_PREFIX}python${CMAKE_STATIC_LIBRARY_SUFFIX})