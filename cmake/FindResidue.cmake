#
# CMake module for Residue cryptography library
#
# Creates ${RESIDUE_INCLUDE_DIR} and ${RESIDUE_LIBRARY}
#
# If ${Residue_USE_STATIC_LIBS} is ON then static libs are preferred over shared
#
# (c) 2017-present Muflihun Labs
#
# https://github.com/muflihun/residue
# https://muflihun.com
#

set(RESIDUE_PATHS ${Residue_ROOT} $ENV{RESIDUE_ROOT})

message ("-- Residue: Searching Easylogging++")
find_package(EASYLOGGINGPP REQUIRED)
include_directories (${EASYLOGGINGPP_INCLUDE_DIR})

add_definitions(-DELPP_FEATURE_ALL)
add_definitions(-DELPP_THREAD_SAFE)
add_definitions(-DELPP_STL_LOGGING)
add_definitions(-DELPP_FORCE_USE_STD_THREAD)
add_definitions(-DELPP_DEFAULT_LOGGING_FLAGS=4096)
add_definitions(-DELPP_NO_DEFAULT_LOG_FILE)

message ("-- Residue: Searching...")
find_path(RESIDUE_INCLUDE_DIR_LOCAL
    Residue.h
    PATH_SUFFIXES include
    PATHS ${RESIDUE_PATHS}
)

set (RESIDUE_EXTRA_LIBRARIES "")
set (RESIDUE_EXTRA_INCLUDE_DIRS "")

if (Residue_USE_STATIC_LIBS)
    message ("-- Residue: Static linking")
    find_library(RESIDUE_LIBRARY_LOCAL
        NAMES libresidue-static-full.a libresidue-static.a libresidue.a
        HINTS "${CMAKE_PREFIX_PATH}/lib"
    )
else()
    message ("-- Residue: Dynamic linking")
    find_library(RESIDUE_LIBRARY_LOCAL
        NAMES libresidue.dylib libresidue.so libresidue residue
        HINTS "${CMAKE_PREFIX_PATH}/lib"
    )
endif()

find_package(ZLIB REQUIRED)
if (ZLIB_FOUND)
    message ("-- Residue: libz: " ${ZLIB_LIBRARIES} " version: " ${ZLIB_VERSION_STRING})
    set (RESIDUE_EXTRA_LIBRARIES ${ZLIB_LIBRARIES})
    set (RESIDUE_EXTRA_INCLUDE_DIRS ${ZLIB_INCLUDE_DIRS})
else()
    message ("Residue: zlib not found which is required with static linking")
endif(ZLIB_FOUND)

## pthreads required by async networking
find_package(Threads REQUIRED)
set (RESIDUE_EXTRA_LIBRARIES ${RESIDUE_EXTRA_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})

set (RESIDUE_INCLUDE_DIR
    ${RESIDUE_EXTRA_INCLUDE_DIRS}
    ${EASYLOGGINGPP_INCLUDE_DIR}
    ${RESIDUE_INCLUDE_DIR_LOCAL}
)
set (RESIDUE_LIBRARY 
    ${RESIDUE_LIBRARY_LOCAL}
    ${RESIDUE_EXTRA_LIBRARIES}
)

message ("-- Residue: Include: " ${RESIDUE_INCLUDE_DIR} ", Binary: " ${RESIDUE_LIBRARY})

include(FindPackageHandleStandardArgs)

find_package_handle_standard_args(Residue REQUIRED_VARS RESIDUE_INCLUDE_DIR RESIDUE_LIBRARY)
