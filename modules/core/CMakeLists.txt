
set(name "${core_name}")

set(
    source_files
    "src/teks.cpp"
)

set(
    include_files
    "include/teks/teks.hpp"
    "include/teks/assert.hpp"
)

add_library("${name}" STATIC ${source_files})
teks_apply_defaults("${name}")

target_sources(
    "${name}"
    PUBLIC
    ${include_files}
)

source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${include_files})

target_include_directories(
    "${name}"
    PUBLIC
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
    "$<INSTALL_INTERFACE:include>"
)

if(CMAKE_GENERATOR STREQUAL "Xcode")
    set_target_properties("${name}" PROPERTIES XCODE_ATTRIBUTE_ONLY_ACTIVE_ARCH[variant=Debug] YES)
endif()

if(TEKS_UNIT_TEST)
    add_subdirectory(test)
endif()
