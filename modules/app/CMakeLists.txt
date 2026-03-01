
set(name "${app_name}")

find_package("Qt6" REQUIRED COMPONENTS Widgets)

set(
    source_files
    "src/main.cpp"
    "src/teks/editor/DocumentView.cpp"
    "src/teks/editor/Document.cpp"
    "src/teks/app/MainWindow.cpp"
)

set(
    header_files
    "src/teks/editor/DocumentView.hpp"
    "src/teks/editor/Document.hpp"
    "src/teks/app/MainWindow.hpp"
)

add_executable(
    "${name}"
    ${source_files}
)

source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${source_files} ${header_files})

target_include_directories(
    "${name}"
    PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

teks_apply_defaults("${name}")
target_link_libraries("${name}" PRIVATE "${core_name}" Qt6::Widgets)

add_custom_target(
    "run_${name}"
    COMMAND "$<TARGET_FILE:${name}>"
    DEPENDS "${name}"
    USES_TERMINAL
)

if(CMAKE_GENERATOR STREQUAL "Xcode")
    set_target_properties("${name}" PROPERTIES XCODE_ATTRIBUTE_ONLY_ACTIVE_ARCH[variant=Debug] YES)
endif()
