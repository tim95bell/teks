#include <teks/buffer/types.hpp>
#include <teks/buffer/Buffer.hpp>
#include <gtest/gtest.h>

#include <optional>
#include <string>
#include <string_view>
#include <vector>
#include <functional>

using namespace teks::buffer;

namespace {
    Range make_range_start_size(Offset::ValueType start, Bytes::ValueType size) {
        return Range::make_unchecked(Offset(start), Bytes(size));
    }

    Range make_range_start_end(Offset::ValueType start, Offset::ValueType end) {
        return Range::make_unchecked(Offset(start), Offset(end));
    }

    Range make_range_start_empty(Offset::ValueType start) {
        return Range::make_unchecked(Offset(start), Bytes(0));
    }

    Range make_range_size(Offset::ValueType size) {
        return Range(Bytes(size));
    }

    template <typename Case>
    std::string param_case_name(const ::testing::TestParamInfo<Case>& info) {
        return std::string(info.param.test_name);
    }

    struct AssertNoMutation {
        AssertNoMutation(Buffer& buffer)
            : buffer(buffer)
            , before_content(read_all_string(buffer))
            , before_size(buffer.size()) {}

        ~AssertNoMutation() {
            test();
        }

        void test() {
            ASSERT_EQ(read_all_string(buffer), before_content);
            ASSERT_EQ(buffer.size(), before_size);
        }

    private:
        Buffer& buffer;
        const std::string before_content;
        const Bytes before_size;
    };

    const std::size_t large_content_len = 4096;

    template <typename T>
    struct Labeled {
        T value;
        std::string label;
    };

    using MakeAt = std::function<Offset(std::string_view)>;
    using MakeRange = std::function<Range(std::string_view)>;

    struct LabeledRange {
        MakeRange value;
        std::string label;
        // semantic/coverage gate, not construction validity
        std::size_t min_initial_size{0};
    };

    const Labeled<std::string> initial_empty_labeled("", "initial_empty");
    const Labeled<std::string> initial_len5_labeled("Hello", "initial_len5");
    const Labeled<std::string> initial_embedded_nul_len11_labeled(
        std::string("Hell\0 W\0rld", 11),
        "initial_embedded_nul_len11"
    );

    const Labeled<std::string> content_empty_labeled("", "content_empty");
    const Labeled<std::string> content_len1_labeled("A", "content_len1");
    const Labeled<std::string> content_len5_labeled("World", "content_len5");
    const Labeled<std::string> content_embedded_nul_len7_labeled(
        std::string{"W\0or\0ld", 7},
        "content_embedded_nul_len7"
    );
    const Labeled<std::string> content_len4096_labeled(std::string(large_content_len, 'L'), "content_len4096");

    const auto at_start_labeled = Labeled<MakeAt>([](std::string_view content) { return Offset(0); }, "at_start");
    const auto at_midpoint_labeled = Labeled<MakeAt>(
        [](std::string_view content) { return Offset(content.size() / 2); },
        "at_midpoint"
    );
    const auto at_end_labeled = Labeled<MakeAt>([](std::string_view content) { return Offset(content.size()); }, "at_end");
    const auto at_end_plus1_labeled = Labeled<MakeAt>(
        [](std::string_view content) { return Offset(content.size() + 1); },
        "at_end_plus1"
    );

    const LabeledRange range_start_len0_labeled{
        [](std::string_view content) { return make_range_start_empty(0); },
        "range_start_len0",
        0
    };
    const LabeledRange range_midpoint_len0_labeled{
        [](std::string_view content) { return make_range_start_empty(content.size() / 2); },
        "range_midpoint_len0",
        0
    };
    const LabeledRange range_end_len0_labeled{
        [](std::string_view content) { return make_range_start_empty(content.size()); },
        "range_end_len0",
        0
    };
    const LabeledRange range_end_plus1_len0_labeled{
        [](std::string_view content) { return make_range_start_empty(content.size() + 1); },
        "range_end_plus1_len0",
        0
    };

    const LabeledRange range_start_to_midpoint_labeled{
        [](std::string_view content) { return make_range_start_size(0, content.size() / 2); },
        "range_start_to_midpoint",
        2
    };
    const LabeledRange range_start_to_end_labeled{
        [](std::string_view content) { return make_range_start_end(0, content.size()); },
        "range_start_to_end",
        1
    };
    const LabeledRange range_start_to_end_plus1_labeled{
        [](std::string_view content) { return make_range_start_end(0, content.size() + 1); },
        "range_start_to_end_plus1",
        0
    };

    const LabeledRange range_plus1_to_minus1_labeled{
        [](std::string_view content) { return make_range_start_end(1, content.size() - 1); },
        "range_plus1_to_minus1",
        3
    };
    const LabeledRange range_plus1_to_end_labeled{
        [](std::string_view content) { return make_range_start_end(1, content.size()); },
        "range_plus1_to_end",
        2
    };
    const LabeledRange range_plus1_to_end_plus1_labeled{
        [](std::string_view content) { return make_range_start_end(1, content.size() + 1); },
        "range_plus1_to_end_plus1",
        2
    };

    const LabeledRange range_end_to_end_plus1_labeled{
        [](std::string_view content) { return make_range_start_end(content.size(), content.size() + 1); },
        "range_end_to_end_plus1",
        1
    };

    const LabeledRange range_end_plus1_to_end_plus5_labeled{
        [](std::string_view content) { return make_range_start_end(content.size() + 1, content.size() + 5); },
        "range_end_plus1_to_end_plus5",
        0
    };
} // namespace

#define TEKS_CAT_IMPL(a, b) a##b
#define TEKS_CAT(a, b) TEKS_CAT_IMPL(a, b)

#define ASSERT_NO_MUTATION(buffer) \
    [[maybe_unused]] const AssertNoMutation \
    TEKS_CAT(assert_no_mutation_scope_, __COUNTER__){buffer}

TEST(teks_buffer_Buffer, default_constructor) {
    Buffer buffer;
    ASSERT_EQ(read_all_string(buffer), "");
}

TEST(teks_buffer_Buffer, string_constructor) {
    Buffer buffer("Hello");
    ASSERT_EQ(read_all_string(buffer), "Hello");
}

TEST(teks_buffer_Buffer, copy_constructor) {
    Buffer buffer("Hello");
    Buffer bufferCopy(buffer);
    ASSERT_EQ(read_all_string(buffer), read_all_string(bufferCopy));
    buffer.erase(make_range_start_size(2, 2));
    ASSERT_NE(read_all_string(buffer), read_all_string(bufferCopy));
    ASSERT_EQ(read_all_string(bufferCopy), "Hello");
}

TEST(teks_buffer_Buffer, move_constructor) {
    Buffer buffer("Hello");
    Buffer bufferCopy(std::move(buffer));
    ASSERT_EQ(read_all_string(bufferCopy), "Hello");
}

TEST(teks_buffer_Buffer, copy_assignment_into_empty_buffer) {
    Buffer buffer("Hello");
    Buffer bufferCopy;
    bufferCopy = buffer;
    ASSERT_EQ(read_all_string(buffer), read_all_string(bufferCopy));
    buffer.erase(make_range_start_size(2, 2));
    ASSERT_NE(read_all_string(buffer), read_all_string(bufferCopy));
    ASSERT_EQ(read_all_string(bufferCopy), "Hello");
}

TEST(teks_buffer_Buffer, move_assignment_into_empty_buffer) {
    Buffer buffer("Hello");
    Buffer bufferCopy;
    bufferCopy = std::move(buffer);
    ASSERT_EQ(read_all_string(bufferCopy), "Hello");
}

TEST(teks_buffer_Buffer, copy_assignment_into_non_empty_buffer) {
    Buffer buffer("Hello");
    Buffer bufferCopy("World");
    bufferCopy = buffer;
    ASSERT_EQ(read_all_string(buffer), read_all_string(bufferCopy));
    buffer.erase(make_range_start_size(2, 2));
    ASSERT_NE(read_all_string(buffer), read_all_string(bufferCopy));
    ASSERT_EQ(read_all_string(bufferCopy), "Hello");
}

TEST(teks_buffer_Buffer, move_assignment_into_non_empty_buffer) {
    Buffer buffer("Hello");
    Buffer bufferCopy("World");
    bufferCopy = std::move(buffer);
    ASSERT_EQ(read_all_string(bufferCopy), "Hello");
}

TEST(teks_buffer_Buffer, size_and_empty_with_empty_buffer) {
    Buffer buffer;
    ASSERT_EQ(buffer.size(), Bytes(0));
    ASSERT_TRUE(buffer.empty());
}

TEST(teks_buffer_Buffer, size_and_empty_with_size_one_buffer) {
    Buffer buffer("A");
    ASSERT_EQ(buffer.size(), Bytes(1));
    ASSERT_FALSE(buffer.empty());
}

TEST(teks_buffer_Buffer, size_and_empty_with_size_three_buffer) {
    Buffer buffer("ABC");
    ASSERT_EQ(buffer.size(), Bytes(3));
    ASSERT_FALSE(buffer.empty());
}

TEST(teks_buffer_Buffer, size_and_empty_with_large_buffer) {
    Buffer buffer(std::string(large_content_len, 'L'));
    ASSERT_EQ(buffer.size(), Bytes(large_content_len));
    ASSERT_FALSE(buffer.empty());
}

namespace { // insert matrix
    struct BufferInsertCase {
        BufferInsertCase(
            Labeled<std::string> initial_and_label,
            Labeled<std::string> content_and_label,
            Labeled<MakeAt> make_at_and_label)
            : test_name(initial_and_label.label + "_and_" + content_and_label.label + "_and_" + make_at_and_label.label)
            , initial(initial_and_label.value)
            , content(content_and_label.value)
            , at(make_at_and_label.value(initial_and_label.value)) {}

        const std::string test_name;
        const std::string initial;
        const std::string content;
        const Offset at;
    };

    void insert_insert_case_permutations(
        std::vector<BufferInsertCase>& cases,
        const std::vector<Labeled<std::string>>& initial,
        const std::vector<Labeled<std::string>>& content,
        const std::vector<Labeled<MakeAt>>& make_at) {
        for (const auto& i : initial) {
            for (const auto& c : content) {
                for (const auto& a : make_at) {
                    cases.emplace_back(i, c, a);
                }
            }
        }
    }

    std::vector<BufferInsertCase> make_insert_cases() {
        std::vector<BufferInsertCase> cases;
        insert_insert_case_permutations(
            cases,
            std::vector{initial_empty_labeled},
            std::vector{content_empty_labeled, content_len5_labeled, content_embedded_nul_len7_labeled, content_len4096_labeled},
            std::vector{at_start_labeled, at_end_plus1_labeled}
        );
        insert_insert_case_permutations(
            cases,
            std::vector{initial_len5_labeled},
            std::vector{content_empty_labeled, content_len5_labeled, content_embedded_nul_len7_labeled, content_len4096_labeled},
            std::vector{at_start_labeled, at_midpoint_labeled, at_end_labeled, at_end_plus1_labeled}
        );
        insert_insert_case_permutations(
            cases,
            std::vector{initial_embedded_nul_len11_labeled},
            std::vector{content_len5_labeled, content_empty_labeled},
            std::vector{at_start_labeled, at_midpoint_labeled, at_end_labeled, at_end_plus1_labeled}
        );
        return cases;
    }

    struct teks_buffer_Buffer_insert_test : public ::testing::TestWithParam<BufferInsertCase> {};
} // namespace

TEST_P(teks_buffer_Buffer_insert_test, case_matrix) {
    const BufferInsertCase& test_case = GetParam();

    const Offset::ValueType initial_size = test_case.initial.size();
    const bool expected_result = test_case.at.raw() <= initial_size;
    const std::string expected_content = expected_result
        ? std::string(test_case.initial).insert(test_case.at.raw(), test_case.content)
        : test_case.initial;

    Buffer buffer(test_case.initial);
    const std::string before_content = read_all_string(buffer);
    const Bytes before_size = buffer.size();

    const bool result = buffer.insert(test_case.at, test_case.content);
    const std::string after_content = read_all_string(buffer);

    ASSERT_EQ(result, expected_result);
    ASSERT_EQ(after_content, expected_content);

    if (result) {
        const Bytes expected_size = before_size + Bytes(test_case.content.size());
        ASSERT_EQ(buffer.size(), expected_size);
    } else {
        ASSERT_EQ(after_content, before_content);
        ASSERT_EQ(buffer.size(), before_size);
    }

    ASSERT_EQ(buffer.size(), Bytes(expected_content.size()));
}

INSTANTIATE_TEST_SUITE_P(
    teks_buffer_Buffer,
    teks_buffer_Buffer_insert_test,
    ::testing::ValuesIn(make_insert_cases()),
    param_case_name<BufferInsertCase>
);

TEST(teks_buffer_Buffer, insert_sequential_operations) {
    Buffer buffer;

    auto assert_failed_insert_no_mutation = [&](Offset at, std::string_view content) {
        ASSERT_NO_MUTATION(buffer);
        ASSERT_FALSE(buffer.insert(at, content));
    };

    ASSERT_TRUE(insert_start(buffer, "34"));
    ASSERT_EQ(read_all_string(buffer), "34");

    ASSERT_TRUE(insert_start(buffer, "12"));
    ASSERT_EQ(read_all_string(buffer), "1234");

    ASSERT_TRUE(insert_end(buffer, "78"));
    ASSERT_EQ(read_all_string(buffer), "123478");

    ASSERT_TRUE(buffer.insert(Offset(4), "56"));
    ASSERT_EQ(read_all_string(buffer), "12345678");

    assert_failed_insert_no_mutation(Offset(buffer.size()) + Bytes(1), "");
    assert_failed_insert_no_mutation(Offset(buffer.size()) + Bytes(1), "dfsd");

    ASSERT_TRUE(buffer.insert(Offset(2), ""));
    ASSERT_EQ(read_all_string(buffer), "12345678");
}

namespace { // erase
    struct BufferEraseCase {
        BufferEraseCase(Labeled<std::string> initial_and_label, LabeledRange make_range_and_label)
            : test_name(initial_and_label.label + "_and_" + make_range_and_label.label)
            , initial(initial_and_label.value)
            , make_range(make_range_and_label.value)
            , min_initial_size(make_range_and_label.min_initial_size) {}

        const std::string test_name;
        const std::string initial;
        const MakeRange make_range;
        const std::size_t min_initial_size;
    };

    void insert_erase_case_permutations(
        std::vector<BufferEraseCase>& cases,
        const std::vector<Labeled<std::string>>& initial,
        const std::vector<LabeledRange>& range
    ) {
        for (const auto& i : initial) {
            for (const auto& r : range) {
                cases.emplace_back(i, r);
            }
        }
    }

    std::vector<BufferEraseCase> make_erase_cases() {
        std::vector<BufferEraseCase> cases;
        insert_erase_case_permutations(
            cases,
            std::vector{initial_empty_labeled},
            std::vector{
                range_start_len0_labeled,
                range_end_plus1_len0_labeled,
                range_start_to_end_plus1_labeled,
                range_end_plus1_to_end_plus5_labeled
            }
        );
        insert_erase_case_permutations(
            cases,
            std::vector{initial_len5_labeled},
            std::vector{
                range_start_len0_labeled,
                range_midpoint_len0_labeled,
                range_end_len0_labeled,
                range_end_plus1_len0_labeled,
                range_start_to_midpoint_labeled,
                range_start_to_end_labeled,
                range_start_to_end_plus1_labeled,
                range_plus1_to_minus1_labeled,
                range_plus1_to_end_labeled,
                range_plus1_to_end_plus1_labeled,
                range_end_to_end_plus1_labeled,
                range_end_plus1_to_end_plus5_labeled
            }
        );
        insert_erase_case_permutations(
            cases,
            std::vector{initial_embedded_nul_len11_labeled},
            std::vector{range_start_to_midpoint_labeled, range_plus1_to_end_labeled, range_end_plus1_to_end_plus5_labeled}
        );
        return cases;
    }

    struct teks_buffer_Buffer_erase_test : public ::testing::TestWithParam<BufferEraseCase> {};
}

TEST_P(teks_buffer_Buffer_erase_test, case_matrix) {
    const BufferEraseCase& test_case = GetParam();

    // validate test_case
    ASSERT_GE(test_case.initial.size(), test_case.min_initial_size);
    const Range range = test_case.make_range(test_case.initial);

    const Offset::ValueType initial_size = test_case.initial.size();
    const bool expected_result = range.end().raw() <= initial_size;
    const std::string expected_content = expected_result
        ?  std::string(test_case.initial).erase(range.start().raw(), range.size().raw())
        : test_case.initial;

    Buffer buffer(test_case.initial);
    const std::string before_content = read_all_string(buffer);
    const Bytes before_size = buffer.size();

    const bool result = buffer.erase(range);
    const std::string after_content = read_all_string(buffer);

    ASSERT_EQ(result, expected_result);
    ASSERT_EQ(after_content, expected_content);

    if (!result) {
        ASSERT_EQ(after_content, before_content);
        ASSERT_EQ(buffer.size(), before_size);
    }

    ASSERT_EQ(buffer.size(), Bytes(expected_content.size()));
}

INSTANTIATE_TEST_SUITE_P(
    teks_buffer_Buffer,
    teks_buffer_Buffer_erase_test,
    ::testing::ValuesIn(make_erase_cases()),
    param_case_name<BufferEraseCase>
);

TEST(teks_buffer_Buffer, erase_sequential_operations) {
    Buffer buffer("Hello World");

    auto assert_failed_erase_no_mutation = [&](Range range) {
        ASSERT_NO_MUTATION(buffer);
        ASSERT_FALSE(buffer.erase(range));
    };

    assert_failed_erase_no_mutation(Range(buffer.size() + Bytes(1)));

    ASSERT_TRUE(buffer.erase(make_range_start_empty(buffer.size().raw())));
    ASSERT_EQ("Hello World", read_all_string(buffer));
    ASSERT_TRUE(buffer.erase(make_range_start_size(2, 2)));
    ASSERT_EQ("Heo World", read_all_string(buffer));
    ASSERT_TRUE(buffer.erase(make_range_size(1)));
    ASSERT_EQ("eo World", read_all_string(buffer));
    ASSERT_TRUE(buffer.erase(make_range_start_size(6, 2)));
    ASSERT_EQ("eo Wor", read_all_string(buffer));
    assert_failed_erase_no_mutation(Range(buffer.size() + Bytes(1)));
    ASSERT_TRUE(buffer.erase(make_range_start_empty(buffer.size().raw())));
    ASSERT_EQ("eo Wor", read_all_string(buffer));
    ASSERT_TRUE(buffer.erase(make_range_start_size(3, 2)));
    ASSERT_EQ("eo r", read_all_string(buffer));
    ASSERT_TRUE(buffer.erase(make_range_size(2)));
    ASSERT_EQ(" r", read_all_string(buffer));
    ASSERT_TRUE(buffer.erase(make_range_start_size(1, 1)));
    ASSERT_EQ(" ", read_all_string(buffer));
    ASSERT_TRUE(buffer.erase(make_range_size(1)));
    ASSERT_EQ("", read_all_string(buffer));
    ASSERT_TRUE(buffer.empty());
}

namespace { // replace
    struct BufferReplaceCase {
        BufferReplaceCase(Labeled<std::string> initial, LabeledRange make_range, Labeled<std::string> content)
            : test_name(initial.label + "_and_" + make_range.label + "_and_" + content.label)
            , initial(initial.value)
            , make_range(make_range.value)
            , min_initial_size(make_range.min_initial_size)
            , content(content.value) {}

        const std::string test_name;
        const std::string initial;
        const MakeRange make_range;
        const std::size_t min_initial_size;
        const std::string content;
    };

    void insert_replace_case_permutations(
        std::vector<BufferReplaceCase>& cases,
        const std::vector<Labeled<std::string>>& initial,
        const std::vector<LabeledRange>& make_range,
        const std::vector<Labeled<std::string>>& content
    ) {
        for (const auto& i : initial) {
            for (const auto& r : make_range) {
                for (const auto& c : content) {
                    cases.emplace_back(i, r, c);
                }
            }
        }
    }

    std::vector<BufferReplaceCase> make_replace_cases() {
        std::vector<BufferReplaceCase> cases;
        insert_replace_case_permutations(
            cases,
            std::vector{initial_empty_labeled},
            std::vector{
                range_start_len0_labeled,
                range_end_plus1_len0_labeled,
                range_start_to_end_plus1_labeled,
                range_end_plus1_to_end_plus5_labeled
            },
            std::vector{content_empty_labeled, content_len1_labeled, content_len5_labeled}
        );
        insert_replace_case_permutations(
            cases,
            std::vector{initial_len5_labeled},
            std::vector{
                range_start_len0_labeled,
                range_midpoint_len0_labeled,
                range_end_len0_labeled,
                range_end_plus1_len0_labeled,
                range_start_to_midpoint_labeled,
                range_start_to_end_labeled,
                range_start_to_end_plus1_labeled,
                range_plus1_to_minus1_labeled,
                range_plus1_to_end_labeled,
                range_plus1_to_end_plus1_labeled,
                range_end_to_end_plus1_labeled,
                range_end_plus1_to_end_plus5_labeled
            },
            std::vector{content_empty_labeled, content_len1_labeled, content_len5_labeled}
        );
        cases.emplace_back(initial_len5_labeled, range_plus1_to_minus1_labeled, content_embedded_nul_len7_labeled);
        cases.emplace_back(initial_len5_labeled, range_plus1_to_minus1_labeled, content_len4096_labeled);
        insert_replace_case_permutations(
            cases,
            std::vector{initial_embedded_nul_len11_labeled},
            std::vector{range_plus1_to_minus1_labeled},
            std::vector{content_len1_labeled, content_embedded_nul_len7_labeled}
        );

        return cases;
    }

    struct teks_buffer_Buffer_replace_test : public ::testing::TestWithParam<BufferReplaceCase> {};
} // namespace

TEST_P(teks_buffer_Buffer_replace_test, case_matrix) {
    const BufferReplaceCase& test_case = GetParam();

    // validate test_case
    ASSERT_GE(test_case.initial.size(), test_case.min_initial_size);
    const Range range = test_case.make_range(test_case.initial);

    const Offset::ValueType initial_size = test_case.initial.size();
    const bool expected_result = range.end().raw() <= initial_size;
    const std::string expected_content = expected_result ?
        std::string(test_case.initial)
            .replace(range.start().raw(), range.size().raw(), test_case.content)
        : test_case.initial;

    Buffer buffer(test_case.initial);
    const std::string before_content = read_all_string(buffer);
    const Bytes before_size = buffer.size();

    const bool result = buffer.replace(range, test_case.content);
    const std::string after_content = read_all_string(buffer);

    ASSERT_EQ(result, expected_result);
    ASSERT_EQ(after_content, expected_content);

    if (!result) {
        ASSERT_EQ(after_content, before_content);
        ASSERT_EQ(buffer.size(), before_size);
    } else {
        const Bytes expected_size =
            before_size - range.size() + Bytes(test_case.content.size());
        ASSERT_EQ(buffer.size(), expected_size);
    }

    ASSERT_EQ(buffer.size(), Bytes(expected_content.size()));
}

INSTANTIATE_TEST_SUITE_P(
    teks_buffer_Buffer,
    teks_buffer_Buffer_replace_test,
    ::testing::ValuesIn(make_replace_cases()),
    param_case_name<BufferReplaceCase>
);

TEST(teks_buffer_Buffer, replace_sequential_operations) {
    Buffer buffer("Hello World");

    auto assert_failed_replace_no_mutation = [&](Range range, std::string_view content) {
        ASSERT_NO_MUTATION(buffer);
        ASSERT_FALSE(buffer.replace(range, content));
    };

    assert_failed_replace_no_mutation(make_range_start_empty(12), "");

    ASSERT_TRUE(buffer.replace(make_range_start_size(5, 1), ""));
    ASSERT_EQ(read_all_string(buffer), "HelloWorld");

    ASSERT_TRUE(buffer.replace(make_range_start_empty(5), " "));
    ASSERT_EQ(read_all_string(buffer), "Hello World");

    ASSERT_TRUE(buffer.replace(make_range_start_size(6, 5), "Earth"));
    ASSERT_EQ(read_all_string(buffer), "Hello Earth");

    assert_failed_replace_no_mutation(make_range_start_end(9, 20), "X");

    ASSERT_TRUE(buffer.replace(make_range_size(6), "Hi "));
    ASSERT_EQ(read_all_string(buffer), "Hi Earth");

    ASSERT_TRUE(buffer.replace(make_range_start_size(3, 5), ""));
    ASSERT_EQ(read_all_string(buffer), "Hi ");
}

namespace { // read_string
    struct BufferReadStringCase {
        BufferReadStringCase(Labeled<std::string> initial, LabeledRange make_range)
            : test_name(initial.label + "_and_" + make_range.label)
            , initial(initial.value)
            , make_range(make_range.value)
            , min_initial_size(make_range.min_initial_size) {}

        const std::string test_name;
        const std::string initial;
        const MakeRange make_range;
        const std::size_t min_initial_size;
    };

    void insert_read_string_case_permutations(
        std::vector<BufferReadStringCase>& cases,
        const std::vector<Labeled<std::string>>& initial,
        const std::vector<LabeledRange>& make_range
    ) {
        for (const auto& i : initial) {
            for (const auto& r : make_range) {
                cases.emplace_back(i, r);
            }
        }
    }

    std::vector<BufferReadStringCase> make_read_string_cases() {
        std::vector<BufferReadStringCase> cases;
        insert_read_string_case_permutations(
            cases,
            std::vector{initial_empty_labeled},
            std::vector{
                range_start_len0_labeled,
                range_start_to_end_plus1_labeled,
                range_end_plus1_len0_labeled,
                range_end_plus1_to_end_plus5_labeled
            }
        );
        insert_read_string_case_permutations(
            cases,
            std::vector{initial_len5_labeled},
            std::vector{
                range_start_len0_labeled,
                range_midpoint_len0_labeled,
                range_end_len0_labeled,
                range_end_plus1_len0_labeled,
                range_start_to_midpoint_labeled,
                range_start_to_end_labeled,
                range_start_to_end_plus1_labeled,
                range_plus1_to_minus1_labeled,
                range_plus1_to_end_labeled,
                range_plus1_to_end_plus1_labeled,
                range_end_to_end_plus1_labeled,
                range_end_plus1_to_end_plus5_labeled
            }
        );
        cases.emplace_back(initial_embedded_nul_len11_labeled, range_start_to_end_labeled);

        return cases;
    }

    struct teks_buffer_Buffer_read_string_test : public ::testing::TestWithParam<BufferReadStringCase> {};
} // namespace

TEST_P(teks_buffer_Buffer_read_string_test, case_matrix) {
    const BufferReadStringCase& test_case = GetParam();

    // validate test_case
    ASSERT_GE(test_case.initial.size(), test_case.min_initial_size);
    const Range range = test_case.make_range(test_case.initial);

    const Offset::ValueType initial_size = test_case.initial.size();
    const bool expected_result = range.end().raw() <= initial_size;

    Buffer buffer(test_case.initial);
    const std::string before_content = read_all_string(buffer);
    const Bytes before_size = buffer.size();

    const std::optional<std::string> result = buffer.read_string(range);

    if (expected_result) {
        ASSERT_TRUE(result.has_value());
        const std::string expected_content = test_case.initial.substr(
            range.start().raw(),
            range.size().raw()
        );
        ASSERT_EQ(*result, expected_content);
        ASSERT_EQ(result->size(), range.size().raw());
    } else {
        ASSERT_EQ(result, std::nullopt);
    }

    ASSERT_EQ(read_all_string(buffer), before_content);
    ASSERT_EQ(buffer.size(), before_size);
}

INSTANTIATE_TEST_SUITE_P(
    teks_buffer_Buffer,
    teks_buffer_Buffer_read_string_test,
    ::testing::ValuesIn(make_read_string_cases()),
    param_case_name<BufferReadStringCase>
);
