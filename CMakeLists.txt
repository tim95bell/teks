
cmake_minimum_required(VERSION 3.28)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(teks VERSION 0.1.0 LANGUAGES CXX)

set(core_name "${PROJECT_NAME}_core")
set(app_name "${PROJECT_NAME}_app")
set(warnings_name "${PROJECT_NAME}_warnings")
set(options_name "${PROJECT_NAME}_options")

if(NOT CMAKE_CONFIGURATION_TYPES AND NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Build type" FORCE)
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "RelWithDebInfo" "MinSizeRel")
endif()

option(TEKS_UNIT_TEST "Build Unit Tests" OFF)
option(TEKS_WARNINGS_AS_ERRORS "Treat Warnings As Errors" ON)
option(TEKS_WARNING_LEVEL_STRICT "Strict warnings" OFF)
set(TEKS_BUFFER_IMPL "STRING" CACHE STRING "Buffer implementation (STRING)")
set_property(CACHE TEKS_BUFFER_IMPL PROPERTY STRINGS "STRING")

add_library("${options_name}" INTERFACE)
target_compile_features("${options_name}" INTERFACE cxx_std_20)
target_include_directories(
    "${options_name}"
    INTERFACE
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/third_party/boost-assert/include>"
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/third_party/boost-config/include>"
)

if(TEKS_BUFFER_IMPL STREQUAL "STRING")
    target_compile_definitions("${options_name}" INTERFACE "TEKS_BUFFER_IMPL_STRING=1")
else()
    message(FATAL_ERROR "Unsupported TEKS_BUFFER_IMPL='${TEKS_BUFFER_IMPL}'")
endif()

target_compile_definitions(
    "${options_name}"
    INTERFACE
    "TEKS_DEBUG=$<BOOL:$<CONFIG:Debug>>"
)

if(TEKS_UNIT_TEST)
    if(MSVC)
        set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    endif()
    add_subdirectory("third_party/googletest")
    enable_testing()
endif()

include("cmake/warnings.cmake")

function(teks_apply_defaults target_name)
    target_link_libraries("${target_name}" PRIVATE "${options_name}" "${warnings_name}")
endfunction()

add_subdirectory("modules/core")
add_subdirectory("modules/app")
