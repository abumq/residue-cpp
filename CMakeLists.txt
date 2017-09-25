cmake_minimum_required (VERSION 2.8.12)

project (Residue-C++)

option (test "Builds all tests" ON)
option (build_sample_app "Builds detailed-cmake sample" OFF)
option (profiling "Turns profiling on for various scenarios" OFF)
option (travis "Whether its travis build or not" OFF)
option (static_ripe "Prefer static ripe library" OFF)
option (BUILD_SHARED_LIBS "build shared libraries" ON)

set (RESIDUE_MAJOR "1")
set (RESIDUE_MINOR "0")
set (RESIDUE_PATCH "0-beta.17")

set (RESIDUE_SOVERSION "${RESIDUE_MAJOR}.${RESIDUE_MINOR}.${RESIDUE_PATCH}")
set (RESIDUE_NAME "Residue-C++")

add_definitions (-DRESIDUE_SOVERSION="${RESIDUE_SOVERSION}")

set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake")
include(residue-dev)

if (profiling)
    add_definitions (-DRESIDUE_PROFILING)
endif()

include (FindPackageHandleStandardArgs)

require_cpp11()
check_apple()

list (APPEND CMAKE_CXX_FLAGS " -Wall -Werror -Wno-unknown-warning-option -Wno-pessimizing-move ")

if (travis)
    message ("-- Building on travis")
    set (static_boost OFF)
else()
    set (static_boost ON)
endif()
set (static_boost OFF)

message ("-- Check for Easylogging++")
find_package (EASYLOGGINGPP REQUIRED)
include_directories (${EASYLOGGINGPP_INCLUDE_DIR})

set (Ripe_USE_STATIC_LIBS ${static_ripe})
find_package (Ripe REQUIRED)
include_directories (${RIPE_INCLUDE_DIR})

message ("-- Check for Boost System (static)")
## We always use static linking for boost stuffs
## Note this does not work when residue library is being build statically
set (Boost_USE_STATIC_LIBS ${static_boost})
unset(Boost_INCLUDE_DIR CACHE)
unset(Boost_LIBRARY_DIRS CACHE)
find_package (Boost REQUIRED COMPONENTS system)
include_directories (${Boost_INCLUDE_DIR})
message ("-- Boost binary: " ${Boost_LIBRARIES})

thread_packages_check()

############# RESIDUE CLIENT LIB ###############

set(LIB_RESIDUE_SOURCE_FILES
    include/Residue.h
    include/log.h
    src/Residue.cc
    ${EASYLOGGINGPP_INCLUDE_DIR}/easylogging++.cc
)

add_library (residue-lib ${LIB_RESIDUE_SOURCE_FILES})

set_target_properties (residue-lib PROPERTIES
    VERSION ${RESIDUE_SOVERSION}
)
target_link_libraries (residue-lib
    ${Boost_LIBRARIES}
    ${RIPE_LIBRARY}
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
install (FILES include/Residue.h DESTINATION "include")


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

    include_directories (${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})

    enable_testing()

    add_executable (residue-cpp-unit-tests
        test/main.cc
        ${EASYLOGGINGPP_INCLUDE_DIR}/easylogging++.cc
    )

    target_compile_definitions (residue-cpp-unit-tests PUBLIC
        ELPP_FEATURE_ALL
        ELPP_DEFAULT_LOG_FILE="/tmp/logs/residue_cpp_unit_tests.log"
        ELPP_DEFAULT_LOGGING_FLAGS=4096
    )

    # Standard linking to gtest stuff.
    target_link_libraries (residue-cpp-unit-tests gtest gtest_main)

    target_link_libraries (residue-cpp-unit-tests residue-lib)

    add_test (NAME residueCppUnitTests COMMAND residue-cpp-unit-tests)
endif() ## test

