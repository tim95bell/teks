
set(name "${core_name}")

set(
    source_files
    "src/teks.cpp"
    "src/buffer/Buffer.cpp"
)

# internal_source_files are not compiled, they are potentially included in a source_file
set(
    internal_source_files
    "internal/src/buffer/StringBuffer.cpp"
)

set(
    include_files
    "include/teks/teks.hpp"
    "include/teks/assert.hpp"
    "include/teks/types.hpp"
    "include/teks/buffer/types.hpp"
    "include/teks/buffer/Buffer.hpp"
)

set(
    internal_include_files
    "include/teks/buffer/internal/StringBuffer.hpp"
)

add_library("${name}" STATIC ${source_files})
teks_apply_defaults("${name}")

target_sources(
    "${name}"
    PUBLIC
    ${include_files}
    PRIVATE
    ${internal_include_files}
)

source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${source_files} ${internal_source_files} ${include_files} ${internal_include_files})

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
