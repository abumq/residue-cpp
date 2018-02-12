cmake_minimum_required (VERSION 2.8.12)

project (Residue-C++)

option (test "Builds all tests" ON)
option (build_sample_app "Builds detailed-cmake sample" OFF)
option (profiling "Turns profiling on for various scenarios" OFF)
option (travis "Whether its travis build or not" OFF)
option (BUILD_SHARED_LIBS "build shared libraries" ON)
option (special_edition "Special edition build" OFF)

set (RESIDUE_MAJOR "1")
set (RESIDUE_MINOR "2")
set (RESIDUE_PATCH "1")

set (RESIDUE_SOVERSION "${RESIDUE_MAJOR}.${RESIDUE_MINOR}.${RESIDUE_PATCH}")
set (RESIDUE_NAME "Residue-C++")

add_definitions (-DRESIDUE_SOVERSION="${RESIDUE_SOVERSION}")
add_definitions (-DRIPE_VERSION="4.1.1-static")

set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake")
include(residue-dev)

if (profiling)
    add_definitions (-DRESIDUE_PROFILING)
endif()

if (special_edition)
    add_definitions (-DRESIDUE_SPECIAL_EDITION)
endif()

add_definitions (-DASIO_STANDALONE)

include (FindPackageHandleStandardArgs)

#require_cpp11()
check_apple()

list (APPEND CMAKE_CXX_FLAGS " -std=c++11 -O3 -Wall -Wno-unused-local-typedef -Wno-undefined-var-template -Wno-unknown-warning-option -Wno-pessimizing-move -Wno-deprecated-declarations ")

find_package(ZLIB REQUIRED)
if (ZLIB_FOUND)
    include_directories(${ZLIB_INCLUDE_DIRS})
    message ("-- libz: " ${ZLIB_LIBRARIES} " version: " ${ZLIB_VERSION_STRING})
else()
    message ("--==> libz not found")
endif(ZLIB_FOUND)

include_directories(${CMAKE_SOURCE_DIR}/deps)

# Check for cryptopp (static)
set(CryptoPP_USE_STATIC_LIBS ON)
find_package(CryptoPP REQUIRED)
message ("-- Crypto++ binary: " ${CRYPTOPP_LIBRARY})
include_directories (${CRYPTOPP_INCLUDE_DIRS})

thread_packages_check()

set (THIRD_PARTY_REQUIRED_LIBS
    ${ZLIB_LIBRARIES}
    ${CRYPTOPP_LIBRARIES}
)

############# RESIDUE (STATIC) CLIENT LIB ###############

set(LIB_RESIDUE_SOURCE_FILES
    deps/ripe/Ripe.cc
    src/dispatcher.cc
    src/network-client.cc
    src/internal-logger.cc
    src/residue.cc
    src/easylogging++.cc
)


add_library (residue-static STATIC ${LIB_RESIDUE_SOURCE_FILES})

set_target_properties (residue-static PROPERTIES
    VERSION ${RESIDUE_SOVERSION}
)

target_link_libraries (residue-static ${THIRD_PARTY_REQUIRED_LIBS})

target_compile_definitions (residue-static PUBLIC
    ELPP_FORCE_USE_STD_THREAD
    ELPP_THREAD_SAFE
    ELPP_NO_LOG_TO_FILE
    ELPP_NO_DEFAULT_LOG_FILE
    ELPP_DEFAULT_LOGGING_FLAGS=4096
    ELPP_FEATURE_ALL
    ELPP_STL_LOGGING
    ELPP_LOG_STD_ARRAY
    ELPP_LOG_UNORDERED_MAP
    ELPP_LOG_UNORDERED_SET
)

# Set RPATH to library install path.
set (CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
install (TARGETS residue-static DESTINATION lib)

############## RESIDUE (SHARED) CLIENT LIB #################

if (BUILD_SHARED_LIBS)
    add_library (residue-lib ${LIB_RESIDUE_SOURCE_FILES})

    set_target_properties (residue-lib PROPERTIES
        VERSION ${RESIDUE_SOVERSION}
    )
    target_link_libraries (residue-lib
        residue-static
    )

    target_compile_definitions (residue-lib PRIVATE
        ELPP_FORCE_USE_STD_THREAD
        ELPP_THREAD_SAFE
        ELPP_NO_LOG_TO_FILE
        ELPP_NO_DEFAULT_LOG_FILE
        ELPP_DEFAULT_LOGGING_FLAGS=4096
        ELPP_FEATURE_ALL
        ELPP_STL_LOGGING
        ELPP_LOG_STD_ARRAY
        ELPP_LOG_UNORDERED_MAP
        ELPP_LOG_UNORDERED_SET
    )

    set_target_properties (residue-lib PROPERTIES OUTPUT_NAME "residue")
    install (TARGETS residue-lib DESTINATION lib)
endif()

############# INCLUDES ###############

install (FILES include/residue.h DESTINATION "include/residue/")
install (FILES include/easylogging++.h DESTINATION "include/residue/")


############# SAMPLE CLIENT APP ###############

if (build_sample_app)
    add_definitions (-DROOT_SAMPLE_BUILD)
    add_subdirectory (samples/detailed-cmake/)
endif()

############## Cmake Package #################

# Packaging config.
set (CPACK_PACKAGE_NAME "Residue")
set (CPACK_PACKAGE_VERSION_MAJOR ${RESIDUE_MAJOR})
set (CPACK_PACKAGE_VERSION_MINOR ${RESIDUE_MINOR})
set (CPACK_SOURCE_GENERATOR TGZ)
set (CPACK_SOURCE_IGNORE_FILES
    "/build/"
)
include (CPack)

# Compile-time configuration.
configure_file (
    ${CMAKE_SOURCE_DIR}/cmake/config.h.cmake
    ${CMAKE_BINARY_DIR}/config.h
)

include_directories (${CMAKE_BINARY_DIR})
include_directories (${CMAKE_SOURCE_DIR})


################# Test #####################
if (test)

    find_package (gtest REQUIRED)

    include_directories (${GTEST_INCLUDE_DIRS})

    enable_testing()

    add_executable (residue-cpp-unit-tests
        test/main.cc
        src/easylogging++.cc
    )

    target_compile_definitions (residue-cpp-unit-tests PUBLIC
        ELPP_FEATURE_ALL
        ELPP_DEFAULT_LOG_FILE="/tmp/logs/residue_cpp_unit_tests.log"
        ELPP_DEFAULT_LOGGING_FLAGS=4096
    )

    # Standard linking to gtest stuff.
    target_link_libraries (residue-cpp-unit-tests ${GTEST_LIBRARIES})

    target_link_libraries (residue-cpp-unit-tests residue-lib)

    add_test (NAME residueCppUnitTests COMMAND residue-cpp-unit-tests)
endif() ## test

