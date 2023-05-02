cmake_minimum_required(VERSION 3.6 FATAL_ERROR)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
# set_property(GLOBAL PROPERTY EXPORT_COMPILE_COMMANDS ON)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/tools/cmake")
include(fst_version)
include(fst_compile_options)
include(fst_platform)

# Parse version file.
set(FST_VERSION_FILE_PATH "${CMAKE_SOURCE_DIR}/tools/version.txt")
fst_parse_version(${FST_VERSION_FILE_PATH})

# Setup fst project.
project("fst" VERSION ${FST_VERSION_MAJOR}.${FST_VERSION_MINOR}.${FST_VERSION_PATCH} DESCRIPTION "Generic C++ tools"
        HOMEPAGE_URL "https://github.com/alxarsenault/fst")

# Options
option(FST_BUILD_TESTS "Build tests" ON)
option(FST_BUILD_EXAMPLES "Build examples" ON)
option(FST_BUILD_BENCHS "Build benchmarks" OFF)
option(FST_USE_CUSTOM_CONFIG "Use custom config header" OFF)
option(FST_USE_STL "Use the stl" OFF)
option(FST_CONFIG_USE_PROFILER "Use profiler" ON)
option(FST_CONFIG_PROFILER_WARNING_IS_ERROR "Generate error on profiler warning" ON)
option(FST_USE_RP_MALLOC "Use rpmalloc for the simd_memory_zone" OFF)
option(FST_USE_RELEASE_ASSERT "Assert in release" ON)
option(FST_USE_ASSERT_COLOR "Use color in assert message" ON)
option(FST_USE_ASSERT_MSG "Assert message" ON)
option(FST_BUILD_PRIVATE "Build private projects" OFF)
option(FST_USE_SIMD_128 "Use simd 128" OFF)
option(FST_USE_SIMD_256 "Use simd 256" OFF)
option(FST_USE_SIMD_512 "Use simd 512" OFF)
option(FST_USE_NEON "Use neon" OFF)
option(FST_CLANG_FORMAT_TARGET "Generate clang-format targets" OFF)

# No reason to set CMAKE_CONFIGURATION_TYPES if it's not a multiconfig generator.
get_property(IS_MULTI_CONFIG GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)
if(IS_MULTI_CONFIG)
    set(CMAKE_CONFIGURATION_TYPES "Debug;Release;RelWithDebInfo" CACHE STRING "" FORCE)
else()
    if(NOT CMAKE_BUILD_TYPE)
        set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Build type" FORCE)
    endif()
endif()

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/output/lib")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/output/lib")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/output/bin")

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG "${CMAKE_BINARY_DIR}/output/lib/Debug")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG "${CMAKE_BINARY_DIR}/output/lib/Debug")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_BINARY_DIR}/output/bin/Debug")

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/output/lib/Release")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/output/lib/Release")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/output/bin/Release")

fst_platform_info()

# Output compiler information
if(CMAKE_CXX_COMPILER_LOADED)
    message(STATUS "Compiler path: ${CMAKE_CXX_COMPILER}")
    message(STATUS "Compiler ID: ${CMAKE_CXX_COMPILER_ID}")
    message(STATUS "Compiler version: ${CMAKE_CXX_COMPILER_VERSION}")
    message(STATUS "Compiler is part of GCC: ${CMAKE_COMPILER_IS_GNUCXX}")
endif()

# Profiler
set(FST_PROFILER_LOG_DIR "${CMAKE_BINARY_DIR}/profiler")
set(FST_PROFILER_LOG_PATH "${FST_PROFILER_LOG_DIR}/profiler.log")
file(MAKE_DIRECTORY "${FST_PROFILER_LOG_DIR}")

# Config file
if(FST_USE_CUSTOM_CONFIG)
    add_compile_definitions(-DFST_USE_CUSTOM_CONFIG=1)

else()
    set(FST_DEFAULT_CONFIG_DEFAULT TRUE)

    # namespace
    set(FST_NAMESPACE "fst")

    #
    set(FST_CONFIG_ASSERT_BREAK "#define FST_ASSERT_BREAK() FST_DEBUGTRAP()")

    #
    set(FST_CONFIG_ASSERT_HOOK "#define FST_ASSERT_HOOK(Expr, File, Line) FST_UNUSED(Expr)")

    # set(FST_USE_SIMD_128 OFF)
    # set(FST_USE_SIMD_256 OFF)
    # set(FST_USE_SIMD_512 OFF)

    set(FST_USE_PROFILER ${FST_CONFIG_USE_PROFILER})
    set(FST_PROFILER_WARNING_IS_ERROR ${FST_CONFIG_PROFILER_WARNING_IS_ERROR})

    fst_generate_config_file()
endif()

set(CMAKE_CXX_STANDARD 20)

#
fst_remove_exceptions()

#
set(FST_STATIC_LIB_NAME fst)
set(FST_DYNAMIC_LIB_NAME fst_shared)

# fst lib sources.
set(FST_INCLUDE_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(FST_SOURCE_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/src)

file(GLOB_RECURSE FST_HEADER_FILES "${FST_INCLUDE_DIRECTORY}/fst/*.h")
file(GLOB_RECURSE FST_SOURCE_FILES "${FST_SOURCE_DIRECTORY}/*.cpp" "${FST_SOURCE_DIRECTORY}/*.h")

source_group(TREE ${FST_INCLUDE_DIRECTORY} PREFIX include FILES ${FST_HEADER_FILES})
source_group(TREE ${FST_SOURCE_DIRECTORY} PREFIX src FILES ${FST_SOURCE_FILES})

# fst static lib
add_library(${FST_STATIC_LIB_NAME} STATIC ${FST_HEADER_FILES} ${FST_SOURCE_FILES})
target_include_directories(${FST_STATIC_LIB_NAME} PUBLIC ${FST_INCLUDE_DIRECTORY})
target_compile_features(${FST_STATIC_LIB_NAME} PUBLIC ${FST_CPP_VERSION})

fst_set_compile_options(${FST_STATIC_LIB_NAME} PUBLIC)
fst_add_version_definition(${FST_STATIC_LIB_NAME})
fst_include_config(${FST_STATIC_LIB_NAME})

if(WIN32)
    set_target_properties(${FST_STATIC_LIB_NAME} PROPERTIES COMPILE_PDB_NAME fst_static PDB_NAME fst_static)
endif()

fst_link_dependencies(${FST_STATIC_LIB_NAME})

# Clang-format
if(FST_CLANG_FORMAT_TARGET)
    include(fst_clang_format)
    target_clangformat_setup(${FST_STATIC_LIB_NAME})
endif()

# Tests
if(FST_BUILD_TESTS)
    enable_testing()
    set(FST_TESTS_NAME fst_tests)
    add_subdirectory(tests)
endif()

# Examples
if(FST_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

# Private
if(FST_BUILD_PRIVATE)
    add_subdirectory(private)
endif()

set(CMAKE_SUPPRESS_REGENERATION true)
