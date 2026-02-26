
set(name "${core_name}_test")

set(
    test_files
    assert_test.cpp
    buffer/buffer_contract_test.cpp
    buffer/Bytes_test.cpp
    buffer/Offset_test.cpp
    buffer/Range_test.cpp
)

add_executable("${name}" ${test_files})
teks_apply_defaults("${name}")

add_test(
    NAME "${name}"
    COMMAND "${name}"
)

target_link_libraries(${name} PRIVATE GTest::gtest_main "${core_name}")

if(CMAKE_GENERATOR STREQUAL "Xcode")
    set_target_properties("${name}" PROPERTIES XCODE_ATTRIBUTE_ONLY_ACTIVE_ARCH[variant=Debug] YES)
endif()
