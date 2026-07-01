include(ExternalProject)

ExternalProject_Add(stdex_download PREFIX stdex
    GIT_REPOSITORY https://github.com/irov/stdex.git
    GIT_PROGRESS TRUE
    
    UPDATE_COMMAND ""

    CMAKE_ARGS
        -DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>
        -DSTDEX_EXTERNAL_BUILD=ON
        -DSTDEX_INSTALL=ON
)

ExternalProject_Get_Property(stdex_download INSTALL_DIR)
set(STDEX_INCLUDE_DIR ${INSTALL_DIR}/include)
set(STDEX_LIBRARY_DIR ${INSTALL_DIR}/lib)

set(STDEX_INCLUDE_DIR ${STDEX_INCLUDE_DIR} CACHE STRING "")
set(STDEX_LIBRARY_DIR ${STDEX_LIBRARY_DIR} CACHE STRING "")

add_library(stdex INTERFACE)
target_include_directories(stdex INTERFACE ${STDEX_INCLUDE_DIR})
