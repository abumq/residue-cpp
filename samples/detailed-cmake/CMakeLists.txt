cmake_minimum_required(VERSION 2.8.12)

project(ResidueSampleApp)

option (use_static_residue "static linking residue" OFF)

set (APP_VERSION "1.0")

set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake")

include_directories (${CMAKE_BINARY_DIR})
include_directories (${CMAKE_SOURCE_DIR})

include(FindPackageHandleStandardArgs)
include(residue-dev)

require_cpp11()
check_apple()

set (Residue_USE_STATIC_LIBS ${use_static_residue})

find_package(Residue REQUIRED)
include_directories (${RESIDUE_INCLUDE_DIR})

#######################################################

add_executable (sample-app-bin src/app.cc)

target_link_libraries (sample-app-bin
    ${RESIDUE_LIBRARY}
    ${CMAKE_THREAD_LIBS_INIT}
)

set_target_properties (sample-app-bin PROPERTIES
    OUTPUT_NAME "residue-sample-app"
    VERSION ${APP_VERSION}
)
