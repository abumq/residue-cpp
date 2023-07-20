#
# CMake module for Residue cryptography library
#
# Creates ${RESIDUE_INCLUDE_DIR} and ${RESIDUE_LIBRARY}
#
# If ${Residue_USE_STATIC_LIBS} is ON then static libs are preferred over shared
#
# Specify ${RESIDUE_ROOT} if you wish to specify root path manually, e.g, -DRESIDUE_ROOT=/usr/local
#
# (c) 2017-present @abumq (Majid Q.)
#
# https://github.com/abumq/residue-cpp
#

## Can't use not defined because of old cmake
if (RESIDUE_ROOT)
else()
    if (ENV{RESIDUE_ROOT})
    else()
        set(RESIDUE_ROOT /usr/local)
    endif()
endif()

set(RESIDUE_PATHS
    ${RESIDUE_ROOT}
    $ENV{RESIDUE_ROOT}
    "${RESIDUE_ROOT}/include/residue"
    "${RESIDUE_ROOT}/lib"
)

add_definitions(-DELPP_FEATURE_ALL)
add_definitions(-DELPP_THREAD_SAFE)
add_definitions(-DELPP_STL_LOGGING)
add_definitions(-DELPP_FORCE_USE_STD_THREAD)
add_definitions(-DELPP_DEFAULT_LOGGING_FLAGS=4096)
add_definitions(-DELPP_NO_DEFAULT_LOG_FILE)

message ("-- Residue: Searching...")
find_path(RESIDUE_INCLUDE_DIR_LOCAL
    Residue.h
    PATHS ${RESIDUE_PATHS}
)

set (RESIDUE_EXTRA_LIBRARIES "")
set (RESIDUE_EXTRA_INCLUDE_DIRS "")

if (Residue_USE_STATIC_LIBS)
    message ("-- Residue: Static linking")
    find_library(RESIDUE_LIBRARY_LOCAL
        NAMES libresidue-static-full.a libresidue-static.a libresidue.a
        HINTS "${RESIDUE_PATHS}/lib"
    )
else()
    message ("-- Residue: Dynamic linking")
    find_library(RESIDUE_LIBRARY_LOCAL
        NAMES libresidue.dylib libresidue.so libresidue residue
        HINTS "${RESIDUE_PATHS}/lib"
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
