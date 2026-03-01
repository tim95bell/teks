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
    Range makeRangeStartSize(Offset::ValueType start, Bytes::ValueType size) {
        return Range::makeUnchecked(Offset(start), Bytes(size));
    }

    Range makeRangeStartEnd(Offset::ValueType start, Offset::ValueType end) {
        return Range::makeUnchecked(Offset(start), Offset(end));
    }

    Range makeRangeStartEmpty(Offset::ValueType start) {
        return Range::makeUnchecked(Offset(start), Bytes(0));
    }

    Range makeRangeSize(Offset::ValueType size) {
        return Range(Bytes(size));
    }

    template <typename Case>
    std::string paramCaseName(const ::testing::TestParamInfo<Case>& info) {
        return std::string(info.param.testName);
    }

    std::vector<Range> calcLineRanges(const Buffer& buffer) {
        const teks::usize lines = buffer.lineCount();
        std::vector<Range> result;
        result.reserve(lines);
        for (teks::usize i = 0; i < lines; ++i) {
            std::optional<Range> range = buffer.lineRange(i);
            EXPECT_TRUE(range.has_value());
            result.push_back(range.value());
        }
        return result;
    }

    void assertLineRangesSizesPlusNewlineCountEqualsContentSize(const Buffer& buffer) {
        const teks::usize lines = buffer.lineCount();
        teks::usize accumulatedLinesLength = 0;
        for (teks::usize i = 0; i < lines; ++i) {
            const auto range = buffer.lineRange(i);
            ASSERT_TRUE(range.has_value());
            accumulatedLinesLength += range.value().size().raw();
        }
        ASSERT_EQ(buffer.size().raw(), (lines - 1) + accumulatedLinesLength);
    }

    struct AssertNoMutation {
        AssertNoMutation(Buffer& buffer)
            : buffer_(buffer)
            , beforeContent_(readAllString(buffer))
            , beforeSize_(buffer.size())
            , beforeLines_(buffer.lineCount())
            , beforeLineRanges_(calcLineRanges(buffer))
            {}

        ~AssertNoMutation() {
            test();
        }

        void test() {
            ASSERT_EQ(readAllString(buffer_), beforeContent_);
            ASSERT_EQ(buffer_.size(), beforeSize_);
            ASSERT_EQ(buffer_.lineCount(), beforeLines_);
            ASSERT_EQ(calcLineRanges(buffer_), beforeLineRanges_);
            ASSERT_EQ(buffer_.lineRange(buffer_.lineCount()), std::nullopt);
        }

    private:
        Buffer& buffer_;
        const std::string beforeContent_;
        const Bytes beforeSize_;
        const teks::usize beforeLines_;
        const std::vector<Range> beforeLineRanges_;
    };

    const std::size_t largeContentLen = 4096;

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
        std::size_t minInitialSize{0};
    };

    const Labeled<std::string> initialEmptyLabeled("", "initial_empty");
    const Labeled<std::string> initialLen5Labeled("Hello", "initial_len5");
    const Labeled<std::string> initialEmbeddedNulLen11Labeled(
        std::string("Hell\0 W\0rld", 11),
        "initial_embedded_nul_len11"
    );

    const Labeled<std::string> contentEmptyLabeled("", "content_empty");
    const Labeled<std::string> contentLen1Labeled("A", "content_len1");
    const Labeled<std::string> contentLen5Labeled("World", "content_len5");
    const Labeled<std::string> contentEmbeddedNulLen7Labeled(
        std::string{"W\0or\0ld", 7},
        "content_embedded_nul_len7"
    );
    const Labeled<std::string> contentLen4096Labeled(
        std::string(largeContentLen, 'L'),
        "content_len4096"
    );

    const auto atStartLabeled = Labeled<MakeAt>(
        [](std::string_view content) { return Offset(0); },
        "at_start"
    );
    const auto atMidpointLabeled = Labeled<MakeAt>(
        [](std::string_view content) { return Offset(content.size() / 2); },
        "at_midpoint"
    );
    const auto atEndLabeled = Labeled<MakeAt>(
        [](std::string_view content) { return Offset(content.size()); },
        "at_end"
    );
    const auto atEndPlus1Labeled = Labeled<MakeAt>(
        [](std::string_view content) { return Offset(content.size() + 1); },
        "at_end_plus1"
    );

    const LabeledRange rangeStartLen0Labeled{
        [](std::string_view content) { return makeRangeStartEmpty(0); },
        "range_start_len0",
        0
    };
    const LabeledRange rangeMidpointLen0Labeled{
        [](std::string_view content) { return makeRangeStartEmpty(content.size() / 2); },
        "range_midpoint_len0",
        0
    };
    const LabeledRange rangeEndLen0Labeled{
        [](std::string_view content) { return makeRangeStartEmpty(content.size()); },
        "range_end_len0",
        0
    };
    const LabeledRange rangeEndPlus1Len0Labeled{
        [](std::string_view content) { return makeRangeStartEmpty(content.size() + 1); },
        "range_end_plus1_len0",
        0
    };

    const LabeledRange rangeStartToMidpointLabeled{
        [](std::string_view content) { return makeRangeStartSize(0, content.size() / 2); },
        "range_start_to_midpoint",
        2
    };
    const LabeledRange rangeStartToEndLabeled{
        [](std::string_view content) { return makeRangeStartEnd(0, content.size()); },
        "range_start_to_end",
        1
    };
    const LabeledRange rangeStartToEndPlus1Labeled{
        [](std::string_view content) { return makeRangeStartEnd(0, content.size() + 1); },
        "range_start_to_end_plus1",
        0
    };

    const LabeledRange rangePlus1ToMinus1Labeled{
        [](std::string_view content) { return makeRangeStartEnd(1, content.size() - 1); },
        "range_plus1_to_minus1",
        3
    };
    const LabeledRange rangePlus1ToEndLabeled{
        [](std::string_view content) { return makeRangeStartEnd(1, content.size()); },
        "range_plus1_to_end",
        2
    };
    const LabeledRange rangePlus1ToEndPlus1Labeled{
        [](std::string_view content) { return makeRangeStartEnd(1, content.size() + 1); },
        "range_plus1_to_end_plus1",
        2
    };

    const LabeledRange rangeEndToEndPlus1Labeled{
        [](std::string_view content) {
            return makeRangeStartEnd(content.size(), content.size() + 1);
        },
        "range_end_to_end_plus1",
        1
    };

    const LabeledRange rangeEndPlus1ToEndPlus5Labeled{
        [](std::string_view content) {
            return makeRangeStartEnd(content.size() + 1, content.size() + 5);
        },
        "range_end_plus1_to_end_plus5",
        0
    };
} // namespace

#define TEKS_CAT_IMPL(a, b) a##b
#define TEKS_CAT(a, b) TEKS_CAT_IMPL(a, b)

#define ASSERT_NO_MUTATION(buffer) \
    [[maybe_unused]] const AssertNoMutation \
    TEKS_CAT(assertNoMutationScope_, __COUNTER__){buffer}

TEST(teksBufferBuffer, defaultConstructor) {
    Buffer buffer;
    ASSERT_EQ(readAllString(buffer), "");
}

TEST(teksBufferBuffer, stringConstructor) {
    Buffer buffer("Hello");
    ASSERT_EQ(readAllString(buffer), "Hello");
}

TEST(teksBufferBuffer, stringConstructorNormalizesNewlinesAndRecordsLineStarts) {
    Buffer buffer("12\r34\r\n56\n78");
    ASSERT_EQ(readAllString(buffer), "12\n34\n56\n78");
    ASSERT_EQ(buffer.size(), Bytes(11));
    ASSERT_FALSE(buffer.empty());
    ASSERT_EQ(buffer.lineCount(), 4);
    ASSERT_EQ(calcLineRanges(buffer), (std::vector{
        makeRangeStartSize(0, 2),
        makeRangeStartSize(3, 2),
        makeRangeStartSize(6, 2),
        makeRangeStartSize(9, 2)
    }));
    ASSERT_EQ(buffer.lineRange(buffer.lineCount()), std::nullopt);
}

TEST(teksBufferBuffer, copyConstructor) {
    Buffer buffer("Hello");
    Buffer bufferCopy(buffer);
    ASSERT_EQ(readAllString(buffer), readAllString(bufferCopy));
    buffer.erase(makeRangeStartSize(2, 2));
    ASSERT_NE(readAllString(buffer), readAllString(bufferCopy));
    ASSERT_EQ(readAllString(bufferCopy), "Hello");
}

TEST(teksBufferBuffer, moveConstructor) {
    Buffer buffer("Hello");
    Buffer bufferCopy(std::move(buffer));
    ASSERT_EQ(readAllString(bufferCopy), "Hello");
}

TEST(teksBufferBuffer, copyAssignmentIntoEmptyBuffer) {
    Buffer buffer("Hello");
    Buffer bufferCopy;
    bufferCopy = buffer;
    ASSERT_EQ(readAllString(buffer), readAllString(bufferCopy));
    buffer.erase(makeRangeStartSize(2, 2));
    ASSERT_NE(readAllString(buffer), readAllString(bufferCopy));
    ASSERT_EQ(readAllString(bufferCopy), "Hello");
}

TEST(teksBufferBuffer, moveAssignmentIntoEmptyBuffer) {
    Buffer buffer("Hello");
    Buffer bufferCopy;
    bufferCopy = std::move(buffer);
    ASSERT_EQ(readAllString(bufferCopy), "Hello");
}

TEST(teksBufferBuffer, copyAssignmentIntoNonEmptyBuffer) {
    Buffer buffer("Hello");
    Buffer bufferCopy("World");
    bufferCopy = buffer;
    ASSERT_EQ(readAllString(buffer), readAllString(bufferCopy));
    buffer.erase(makeRangeStartSize(2, 2));
    ASSERT_NE(readAllString(buffer), readAllString(bufferCopy));
    ASSERT_EQ(readAllString(bufferCopy), "Hello");
}

TEST(teksBufferBuffer, moveAssignmentIntoNonEmptyBuffer) {
    Buffer buffer("Hello");
    Buffer bufferCopy("World");
    bufferCopy = std::move(buffer);
    ASSERT_EQ(readAllString(bufferCopy), "Hello");
}

TEST(teksBufferBuffer, sizeAndEmptyWithEmptyBuffer) {
    Buffer buffer;
    ASSERT_EQ(buffer.size(), Bytes(0));
    ASSERT_TRUE(buffer.empty());
}

TEST(teksBufferBuffer, sizeAndEmptyWithSizeOneBuffer) {
    Buffer buffer("A");
    ASSERT_EQ(buffer.size(), Bytes(1));
    ASSERT_FALSE(buffer.empty());
}

TEST(teksBufferBuffer, sizeAndEmptyWithSizeThreeBuffer) {
    Buffer buffer("ABC");
    ASSERT_EQ(buffer.size(), Bytes(3));
    ASSERT_FALSE(buffer.empty());
}

TEST(teksBufferBuffer, sizeAndEmptyWithMixedNewlineStyleBuffer) {
    Buffer buffer("A\nB\rC\r\nD");
    ASSERT_EQ(buffer.size(), Bytes(7));
    ASSERT_FALSE(buffer.empty());
}

TEST(teksBufferBuffer, sizeAndEmptyWithSingleNewlineBuffer) {
    Buffer buffer("\n");
    ASSERT_EQ(buffer.size(), Bytes(1));
    ASSERT_FALSE(buffer.empty());
}

TEST(teksBufferBuffer, sizeAndEmptyWithLargeBuffer) {
    Buffer buffer(std::string(largeContentLen, 'L'));
    ASSERT_EQ(buffer.size(), Bytes(largeContentLen));
    ASSERT_FALSE(buffer.empty());
}

TEST(teksBufferBuffer, lineCountWithEmptyBuffer) {
    Buffer buffer;
    ASSERT_EQ(buffer.lineCount(), 1);
}

TEST(teksBufferBuffer, lineCountWithOnlyNewline) {
    Buffer buffer("\n");
    ASSERT_EQ(buffer.lineCount(), 2);
}

TEST(teksBufferBuffer, lineCountWithOnlyTwoNewlines) {
    Buffer buffer("\n\n");
    ASSERT_EQ(buffer.lineCount(), 3);
}

TEST(teksBufferBuffer, lineCountWithOnlyText) {
    Buffer buffer("Hello World");
    ASSERT_EQ(buffer.lineCount(), 1);
}

TEST(teksBufferBuffer, lineCountWithNewlineAtTheStartOfText) {
    Buffer buffer("\nHello World");
    ASSERT_EQ(buffer.lineCount(), 2);
}

TEST(teksBufferBuffer, lineCountWithNewlineInTheMiddleOfText) {
    Buffer buffer("Hello\nWorld");
    ASSERT_EQ(buffer.lineCount(), 2);
}

TEST(teksBufferBuffer, lineCountWithNewlineAtTheEndOfText) {
    Buffer buffer("Hello World\n");
    ASSERT_EQ(buffer.lineCount(), 2);
}

TEST(teksBufferBuffer, lineRangeWithFirstLineOnEmptyBuffer) {
    Buffer buffer;
    ASSERT_EQ(buffer.lineRange(0), makeRangeStartEmpty(0));
}

TEST(teksBufferBuffer, lineRangeWithOobLineOnEmptyBuffer) {
    Buffer buffer;
    ASSERT_EQ(buffer.lineRange(1), std::nullopt);
}

TEST(teksBufferBuffer, lineRangeWithFirstLineOnSingleLineBuffer) {
    Buffer buffer("Hello");
    ASSERT_EQ(buffer.lineRange(0), makeRangeStartSize(0, 5));
}

TEST(teksBufferBuffer, lineRangeWithOobLineOnSingleLineBuffer) {
    Buffer buffer("Hello World");
    ASSERT_EQ(buffer.lineRange(1), std::nullopt);
}

TEST(teksBufferBuffer, lineRangeWithFirstLineOnMultiLineBuffer) {
    Buffer buffer("\nHello\nWorld\n!\n");
    ASSERT_EQ(buffer.lineRange(0), makeRangeStartEmpty(0));
}

TEST(teksBufferBuffer, lineRangeWithMiddleLineOnMultiLineBuffer) {
    Buffer buffer("\nHello\nWorld\n!\n");
    ASSERT_EQ(buffer.lineRange(1), makeRangeStartSize(1, 5));
}

TEST(teksBufferBuffer, lineRangeWithLastLineOnMultiLineBuffer) {
    Buffer buffer("\nHello\nWorld\n!\n");
    ASSERT_EQ(buffer.lineRange(4), makeRangeStartSize(15, 0));
}

TEST(teksBufferBuffer, lineRangeWithOobLineOnMultiLineBuffer) {
    Buffer buffer("\nHello\nWorld\n!\n");
    ASSERT_EQ(buffer.lineRange(5), std::nullopt);
}

TEST(teksBufferBuffer, lineRangeWithFirstLineOnMultiLineBufferWithNonEmptyFirstLine) {
    Buffer buffer("Hello\n");
    ASSERT_EQ(buffer.lineRange(0), makeRangeStartSize(0, 5));
}

TEST(teksBufferBuffer, lineRangeWithLastLineOnMultiLineBufferWithNonEmptyLastLine) {
    Buffer buffer("\nHello");
    ASSERT_EQ(buffer.lineRange(1), makeRangeStartSize(1, 5));
}

TEST(teksBufferBuffer, fromRawTextWithEmptyBufferDetectesNewlineStylesAndCountsLinesAndRecordsLineStarts) {
    const auto [buffer, newlineStyles] = Buffer::fromRawText("");
    ASSERT_TRUE(newlineStyles.hasExactly({}));
    ASSERT_EQ(buffer.lineCount(), 1);
    ASSERT_EQ(readAllString(buffer), "");
    ASSERT_EQ(buffer.size(), Bytes(0));
    ASSERT_TRUE(buffer.empty());
    ASSERT_EQ(buffer.lineRange(0), makeRangeStartEmpty(0));
    ASSERT_EQ(buffer.lineRange(1), std::nullopt);
    assertLineRangesSizesPlusNewlineCountEqualsContentSize(buffer);
}

TEST(teksBufferBuffer, fromRawTextWithSingleCharacterLFBufferDetectesNewlineStylesAndCountsLinesAndRecordsLineStarts) {
    const auto [buffer, newlineStyles] = Buffer::fromRawText("\n");
    ASSERT_TRUE(newlineStyles.hasExactly({NewlineStyleSet::Style::Lf}));
    ASSERT_EQ(buffer.lineCount(), 2);
    ASSERT_EQ(readAllString(buffer), "\n");
    ASSERT_EQ(buffer.size(), Bytes(1));
    ASSERT_FALSE(buffer.empty());
    ASSERT_EQ(buffer.lineRange(0), makeRangeStartEmpty(0));
    ASSERT_EQ(buffer.lineRange(1), makeRangeStartEmpty(1));
    ASSERT_EQ(buffer.lineRange(2), std::nullopt);
    assertLineRangesSizesPlusNewlineCountEqualsContentSize(buffer);
}

TEST(teksBufferBuffer, fromRawTextWithSingleCharacterCRBufferDetectesNewlineStylesAndCountsLinesAndNormalizesNewlinesAndRecordsLineStarts) {
    const auto [buffer, newlineStyles] = Buffer::fromRawText("\r");
    ASSERT_TRUE(newlineStyles.hasExactly({NewlineStyleSet::Style::Cr}));
    ASSERT_EQ(buffer.lineCount(), 2);
    ASSERT_EQ(readAllString(buffer), "\n");
    ASSERT_EQ(buffer.size(), Bytes(1));
    ASSERT_FALSE(buffer.empty());
    ASSERT_EQ(buffer.lineRange(0), makeRangeStartEmpty(0));
    ASSERT_EQ(buffer.lineRange(1), makeRangeStartEmpty(1));
    ASSERT_EQ(buffer.lineRange(2), std::nullopt);
    assertLineRangesSizesPlusNewlineCountEqualsContentSize(buffer);
}

TEST(teksBufferBuffer, fromRawTextWithSingleCharacterCRLFBufferDetectesNewlineStylesAndCountsNewlinesAndNormalizesNewlinesAndRecordsLineStarts) {
    const auto [buffer, newlineStyles] = Buffer::fromRawText("\r\n");
    ASSERT_TRUE(newlineStyles.hasExactly({NewlineStyleSet::Style::Crlf}));
    ASSERT_EQ(buffer.lineCount(), 2);
    ASSERT_EQ(readAllString(buffer), "\n");
    ASSERT_EQ(buffer.size(), Bytes(1));
    ASSERT_FALSE(buffer.empty());
    ASSERT_EQ(buffer.lineRange(0), makeRangeStartEmpty(0));
    ASSERT_EQ(buffer.lineRange(1), makeRangeStartEmpty(1));
    ASSERT_EQ(buffer.lineRange(2), std::nullopt);
    assertLineRangesSizesPlusNewlineCountEqualsContentSize(buffer);
}

namespace {
    void assertFromRawTextWithMultiLineStringContentAndLineStarts(const Buffer& buffer) {
        ASSERT_EQ(buffer.lineCount(), 6);
        ASSERT_EQ(readAllString(buffer), "some\nlines\nseperated\nby\nnewlines\n");
        ASSERT_EQ(buffer.size(), Bytes(33));
        ASSERT_FALSE(buffer.empty());
        ASSERT_EQ(calcLineRanges(buffer), (std::vector{
            makeRangeStartSize(0, 4),
            makeRangeStartSize(5, 5),
            makeRangeStartSize(11, 9),
            makeRangeStartSize(21, 2),
            makeRangeStartSize(24, 8),
            makeRangeStartSize(33, 0)
        }));
        ASSERT_EQ(buffer.lineRange(6), std::nullopt);
        assertLineRangesSizesPlusNewlineCountEqualsContentSize(buffer);
    }

    TEST(teksBufferBuffer, fromRawTextWithLFBufferDetectesNewlineStylesAndCountsNewlinesAndRecordsLineStarts) {
        const auto [buffer, newlineStyles] = Buffer::fromRawText("some\nlines\nseperated\nby\nnewlines\n");
        ASSERT_TRUE(newlineStyles.hasExactly({NewlineStyleSet::Style::Lf}));
        assertFromRawTextWithMultiLineStringContentAndLineStarts(buffer);
        assertLineRangesSizesPlusNewlineCountEqualsContentSize(buffer);
    }

    TEST(teksBufferBuffer, fromRawTextWithCRBufferDetectesNewlineStylesAndCountsNewlinesAndNormalizesNewlinesAndRecordsLineStarts) {
        const auto [buffer, newlineStyles] = Buffer::fromRawText("some\rlines\rseperated\rby\rnewlines\r");
        ASSERT_TRUE(newlineStyles.hasExactly({NewlineStyleSet::Style::Cr}));
        assertFromRawTextWithMultiLineStringContentAndLineStarts(buffer);
        assertLineRangesSizesPlusNewlineCountEqualsContentSize(buffer);
    }

    TEST(teksBufferBuffer, fromRawTextWithCRLFBufferDetectesNewlineStylesAndCountsNewlinesAndNormalizesNewlinesAndRecordsLineStarts) {
        const auto [buffer, newlineStyles] = Buffer::fromRawText("some\r\nlines\r\nseperated\r\nby\r\nnewlines\r\n");
        ASSERT_TRUE(newlineStyles.hasExactly({NewlineStyleSet::Style::Crlf}));
        assertFromRawTextWithMultiLineStringContentAndLineStarts(buffer);
        assertLineRangesSizesPlusNewlineCountEqualsContentSize(buffer);
    }

    TEST(teksBufferBuffer, fromRawTextWithMixedNewlineBufferDetectesNewlineStylesAndCountsNewlinesAndNormalizesNewlinesAndRecordsLineStarts) {
        const auto [buffer, newlineStyles] = Buffer::fromRawText("some\nlines\r\nseperated\rby\r\nnewlines\n");
        ASSERT_TRUE(newlineStyles.hasExactly({NewlineStyleSet::Style::Crlf, NewlineStyleSet::Style::Cr, NewlineStyleSet::Style::Lf}));
        assertFromRawTextWithMultiLineStringContentAndLineStarts(buffer);
        assertLineRangesSizesPlusNewlineCountEqualsContentSize(buffer);
    }
}

namespace { // insert matrix
    struct BufferInsertCase {
        BufferInsertCase(
            Labeled<std::string> initialAndLabel,
            Labeled<std::string> contentAndLabel,
            Labeled<MakeAt> makeAtAndLabel)
            : testName(initialAndLabel.label
                + "_and_" + contentAndLabel.label
                + "_and_" + makeAtAndLabel.label)
            , initial(initialAndLabel.value)
            , content(contentAndLabel.value)
            , at(makeAtAndLabel.value(initialAndLabel.value)) {}

        const std::string testName;
        const std::string initial;
        const std::string content;
        const Offset at;
    };

    void insertInsertCasePermutations(
        std::vector<BufferInsertCase>& cases,
        const std::vector<Labeled<std::string>>& initial,
        const std::vector<Labeled<std::string>>& content,
        const std::vector<Labeled<MakeAt>>& makeAt) {
        for (const auto& i : initial) {
            for (const auto& c : content) {
                for (const auto& a : makeAt) {
                    cases.emplace_back(i, c, a);
                }
            }
        }
    }

    std::vector<BufferInsertCase> makeInsertCases() {
        std::vector<BufferInsertCase> cases;
        insertInsertCasePermutations(
            cases,
            std::vector{initialEmptyLabeled},
            std::vector{
                contentEmptyLabeled,
                contentLen5Labeled,
                contentEmbeddedNulLen7Labeled,
                contentLen4096Labeled
            },
            std::vector{atStartLabeled, atEndPlus1Labeled}
        );
        insertInsertCasePermutations(
            cases,
            std::vector{initialLen5Labeled},
            std::vector{
                contentEmptyLabeled,
                contentLen5Labeled,
                contentEmbeddedNulLen7Labeled,
                contentLen4096Labeled
            },
            std::vector{atStartLabeled, atMidpointLabeled, atEndLabeled, atEndPlus1Labeled}
        );
        insertInsertCasePermutations(
            cases,
            std::vector{initialEmbeddedNulLen11Labeled},
            std::vector{contentLen5Labeled, contentEmptyLabeled},
            std::vector{atStartLabeled, atMidpointLabeled, atEndLabeled, atEndPlus1Labeled}
        );
        return cases;
    }

    struct TeksBufferBufferInsertTest
        : public ::testing::TestWithParam<BufferInsertCase>
    {};
} // namespace

TEST_P(TeksBufferBufferInsertTest, caseMatrix) {
    const BufferInsertCase& testCase = GetParam();

    const Offset::ValueType initialSize = testCase.initial.size();
    const bool expectedResult = testCase.at.raw() <= initialSize;
    const std::string expectedContent = expectedResult
        ? std::string(testCase.initial).insert(testCase.at.raw(), testCase.content)
        : testCase.initial;

    Buffer buffer(testCase.initial);
    const std::string beforeContent = readAllString(buffer);
    const Bytes beforeSize = buffer.size();

    const bool result = buffer.insert(testCase.at, testCase.content);
    const std::string afterContent = readAllString(buffer);

    ASSERT_EQ(result, expectedResult);
    ASSERT_EQ(afterContent, expectedContent);

    if (result) {
        const Bytes expectedSize = beforeSize + Bytes(testCase.content.size());
        ASSERT_EQ(buffer.size(), expectedSize);
    } else {
        ASSERT_EQ(afterContent, beforeContent);
        ASSERT_EQ(buffer.size(), beforeSize);
    }

    ASSERT_EQ(buffer.size(), Bytes(expectedContent.size()));
}

INSTANTIATE_TEST_SUITE_P(
    teksBufferBuffer,
    TeksBufferBufferInsertTest,
    ::testing::ValuesIn(makeInsertCases()),
    paramCaseName<BufferInsertCase>
);

TEST(teksBufferBuffer, insertSequentialOperations) {
    Buffer buffer;

    auto assertFailedInsertNoMutation = [&](Offset at, std::string_view content) {
        ASSERT_NO_MUTATION(buffer);
        ASSERT_FALSE(buffer.insert(at, content));
    };

    ASSERT_TRUE(insertStart(buffer, "34"));
    ASSERT_EQ(readAllString(buffer), "34");

    ASSERT_TRUE(insertStart(buffer, "12"));
    ASSERT_EQ(readAllString(buffer), "1234");

    ASSERT_TRUE(insertEnd(buffer, "78"));
    ASSERT_EQ(readAllString(buffer), "123478");

    ASSERT_TRUE(buffer.insert(Offset(4), "56"));
    ASSERT_EQ(readAllString(buffer), "12345678");

    assertFailedInsertNoMutation(Offset(buffer.size()) + Bytes(1), "");
    assertFailedInsertNoMutation(Offset(buffer.size()) + Bytes(1), "dfsd");

    ASSERT_TRUE(buffer.insert(Offset(2), ""));
    ASSERT_EQ(readAllString(buffer), "12345678");
}

TEST(teksBufferBuffer, insertMixedNewLineStyles) {
    Buffer buffer("12\n9A");
    ASSERT_TRUE(buffer.insert(Offset(3), "34\r\n56\r78\n"));
    ASSERT_EQ(readAllString(buffer), "12\n34\n56\n78\n9A");
    ASSERT_EQ(buffer.size(), Bytes(14));
    ASSERT_FALSE(buffer.empty());
    ASSERT_EQ(buffer.lineCount(), 5);
    ASSERT_EQ(calcLineRanges(buffer), (std::vector{
        makeRangeStartSize(0, 2),
        makeRangeStartSize(3, 2),
        makeRangeStartSize(6, 2),
        makeRangeStartSize(9, 2),
        makeRangeStartSize(12, 2)
    }));
    ASSERT_EQ(buffer.lineRange(5), std::nullopt);
    assertLineRangesSizesPlusNewlineCountEqualsContentSize(buffer);
}

namespace { // erase
    struct BufferEraseCase {
        BufferEraseCase(Labeled<std::string> initialAndLabel, LabeledRange makeRangeAndLabel)
            : testName(initialAndLabel.label + "_and_" + makeRangeAndLabel.label)
            , initial(initialAndLabel.value)
            , makeRange(makeRangeAndLabel.value)
            , minInitialSize(makeRangeAndLabel.minInitialSize) {}

        const std::string testName;
        const std::string initial;
        const MakeRange makeRange;
        const std::size_t minInitialSize;
    };

    void insertEraseCasePermutations(
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

    std::vector<BufferEraseCase> makeEraseCases() {
        std::vector<BufferEraseCase> cases;
        insertEraseCasePermutations(
            cases,
            std::vector{initialEmptyLabeled},
            std::vector{
                rangeStartLen0Labeled,
                rangeEndPlus1Len0Labeled,
                rangeStartToEndPlus1Labeled,
                rangeEndPlus1ToEndPlus5Labeled
            }
        );
        insertEraseCasePermutations(
            cases,
            std::vector{initialLen5Labeled},
            std::vector{
                rangeStartLen0Labeled,
                rangeMidpointLen0Labeled,
                rangeEndLen0Labeled,
                rangeEndPlus1Len0Labeled,
                rangeStartToMidpointLabeled,
                rangeStartToEndLabeled,
                rangeStartToEndPlus1Labeled,
                rangePlus1ToMinus1Labeled,
                rangePlus1ToEndLabeled,
                rangePlus1ToEndPlus1Labeled,
                rangeEndToEndPlus1Labeled,
                rangeEndPlus1ToEndPlus5Labeled
            }
        );
        insertEraseCasePermutations(
            cases,
            std::vector{initialEmbeddedNulLen11Labeled},
            std::vector{
                rangeStartToMidpointLabeled,
                rangePlus1ToEndLabeled,
                rangeEndPlus1ToEndPlus5Labeled
            }
        );
        return cases;
    }

    struct TeksBufferBufferEraseTest
        : public ::testing::TestWithParam<BufferEraseCase>
    {};
}

TEST_P(TeksBufferBufferEraseTest, caseMatrix) {
    const BufferEraseCase& testCase = GetParam();

    // validate testCase
    ASSERT_GE(testCase.initial.size(), testCase.minInitialSize);
    const Range range = testCase.makeRange(testCase.initial);

    const Offset::ValueType initialSize = testCase.initial.size();
    const bool expectedResult = range.end().raw() <= initialSize;
    const std::string expectedContent = expectedResult
        ?  std::string(testCase.initial).erase(range.start().raw(), range.size().raw())
        : testCase.initial;

    Buffer buffer(testCase.initial);
    const std::string beforeContent = readAllString(buffer);
    const Bytes beforeSize = buffer.size();

    const bool result = buffer.erase(range);
    const std::string afterContent = readAllString(buffer);

    ASSERT_EQ(result, expectedResult);
    ASSERT_EQ(afterContent, expectedContent);

    if (!result) {
        ASSERT_EQ(afterContent, beforeContent);
        ASSERT_EQ(buffer.size(), beforeSize);
    }

    ASSERT_EQ(buffer.size(), Bytes(expectedContent.size()));
}

INSTANTIATE_TEST_SUITE_P(
    teksBufferBuffer,
    TeksBufferBufferEraseTest,
    ::testing::ValuesIn(makeEraseCases()),
    paramCaseName<BufferEraseCase>
);

TEST(teksBufferBuffer, eraseSequentialOperations) {
    Buffer buffer("Hello World");

    auto assertFailedEraseNoMutation = [&](Range range) {
        ASSERT_NO_MUTATION(buffer);
        ASSERT_FALSE(buffer.erase(range));
    };

    assertFailedEraseNoMutation(Range(buffer.size() + Bytes(1)));

    ASSERT_TRUE(buffer.erase(makeRangeStartEmpty(buffer.size().raw())));
    ASSERT_EQ("Hello World", readAllString(buffer));
    ASSERT_TRUE(buffer.erase(makeRangeStartSize(2, 2)));
    ASSERT_EQ("Heo World", readAllString(buffer));
    ASSERT_TRUE(buffer.erase(makeRangeSize(1)));
    ASSERT_EQ("eo World", readAllString(buffer));
    ASSERT_TRUE(buffer.erase(makeRangeStartSize(6, 2)));
    ASSERT_EQ("eo Wor", readAllString(buffer));
    assertFailedEraseNoMutation(Range(buffer.size() + Bytes(1)));
    ASSERT_TRUE(buffer.erase(makeRangeStartEmpty(buffer.size().raw())));
    ASSERT_EQ("eo Wor", readAllString(buffer));
    ASSERT_TRUE(buffer.erase(makeRangeStartSize(3, 2)));
    ASSERT_EQ("eo r", readAllString(buffer));
    ASSERT_TRUE(buffer.erase(makeRangeSize(2)));
    ASSERT_EQ(" r", readAllString(buffer));
    ASSERT_TRUE(buffer.erase(makeRangeStartSize(1, 1)));
    ASSERT_EQ(" ", readAllString(buffer));
    ASSERT_TRUE(buffer.erase(makeRangeSize(1)));
    ASSERT_EQ("", readAllString(buffer));
    ASSERT_TRUE(buffer.empty());
}

TEST(teksBufferBuffer, eraseSingleNewlineAdjustsLineStarts) {
    Buffer buffer("12\n34");
    ASSERT_TRUE(buffer.erase(makeRangeStartSize(2, 1)));
    ASSERT_EQ(buffer.lineCount(), 1);
    ASSERT_EQ(readAllString(buffer), "1234");
    ASSERT_EQ(buffer.size(), Bytes(4));
    ASSERT_FALSE(buffer.empty());
    ASSERT_EQ(buffer.lineRange(0), makeRangeStartSize(0, 4));
    ASSERT_EQ(buffer.lineRange(1), std::nullopt);
    assertLineRangesSizesPlusNewlineCountEqualsContentSize(buffer);
}

TEST(teksBufferBuffer, eraseMultipleNewlineAdjustsLineStarts) {
    Buffer buffer("12\n34\n56\n78\n9A");
    ASSERT_TRUE(buffer.erase(makeRangeStartSize(5, 4)));
    ASSERT_EQ(buffer.lineCount(), 3);
    ASSERT_EQ(readAllString(buffer), "12\n3478\n9A");
    ASSERT_EQ(buffer.size(), Bytes(10));
    ASSERT_FALSE(buffer.empty());
    ASSERT_EQ(calcLineRanges(buffer), (std::vector{
        makeRangeStartSize(0, 2),
        makeRangeStartSize(3, 4),
        makeRangeStartSize(8, 2)
    }));
    ASSERT_EQ(buffer.lineRange(3), std::nullopt);
    assertLineRangesSizesPlusNewlineCountEqualsContentSize(buffer);
}

TEST(teksBufferBuffer, eraseMultipleNewlinesToEndAdjustsLineStarts) {
    Buffer buffer("12\n34\n56");
    ASSERT_TRUE(buffer.erase(makeRangeStartSize(2, 6)));
    ASSERT_EQ(buffer.lineCount(), 1);
    ASSERT_EQ(readAllString(buffer), "12");
    ASSERT_EQ(buffer.size(), Bytes(2));
    ASSERT_FALSE(buffer.empty());
    ASSERT_EQ(buffer.lineRange(0), makeRangeStartSize(0, 2));
    ASSERT_EQ(buffer.lineRange(1), std::nullopt);
    assertLineRangesSizesPlusNewlineCountEqualsContentSize(buffer);
}


namespace { // replace
    struct BufferReplaceCase {
        BufferReplaceCase(
            Labeled<std::string> initial,
            LabeledRange makeRange,
            Labeled<std::string> content
        ) : testName(initial.label + "_and_" + makeRange.label + "_and_" + content.label)
            , initial(initial.value)
            , makeRange(makeRange.value)
            , minInitialSize(makeRange.minInitialSize)
            , content(content.value)
        {}

        const std::string testName;
        const std::string initial;
        const MakeRange makeRange;
        const std::size_t minInitialSize;
        const std::string content;
    };

    void insertReplaceCasePermutations(
        std::vector<BufferReplaceCase>& cases,
        const std::vector<Labeled<std::string>>& initial,
        const std::vector<LabeledRange>& makeRange,
        const std::vector<Labeled<std::string>>& content
    ) {
        for (const auto& i : initial) {
            for (const auto& r : makeRange) {
                for (const auto& c : content) {
                    cases.emplace_back(i, r, c);
                }
            }
        }
    }

    std::vector<BufferReplaceCase> makeReplaceCases() {
        std::vector<BufferReplaceCase> cases;
        insertReplaceCasePermutations(
            cases,
            std::vector{initialEmptyLabeled},
            std::vector{
                rangeStartLen0Labeled,
                rangeEndPlus1Len0Labeled,
                rangeStartToEndPlus1Labeled,
                rangeEndPlus1ToEndPlus5Labeled
            },
            std::vector{contentEmptyLabeled, contentLen1Labeled, contentLen5Labeled}
        );
        insertReplaceCasePermutations(
            cases,
            std::vector{initialLen5Labeled},
            std::vector{
                rangeStartLen0Labeled,
                rangeMidpointLen0Labeled,
                rangeEndLen0Labeled,
                rangeEndPlus1Len0Labeled,
                rangeStartToMidpointLabeled,
                rangeStartToEndLabeled,
                rangeStartToEndPlus1Labeled,
                rangePlus1ToMinus1Labeled,
                rangePlus1ToEndLabeled,
                rangePlus1ToEndPlus1Labeled,
                rangeEndToEndPlus1Labeled,
                rangeEndPlus1ToEndPlus5Labeled
            },
            std::vector{contentEmptyLabeled, contentLen1Labeled, contentLen5Labeled}
        );
        cases.emplace_back(
            initialLen5Labeled,
            rangePlus1ToMinus1Labeled,
            contentEmbeddedNulLen7Labeled
        );
        cases.emplace_back(initialLen5Labeled, rangePlus1ToMinus1Labeled, contentLen4096Labeled);
        insertReplaceCasePermutations(
            cases,
            std::vector{initialEmbeddedNulLen11Labeled},
            std::vector{rangePlus1ToMinus1Labeled},
            std::vector{contentLen1Labeled, contentEmbeddedNulLen7Labeled}
        );

        return cases;
    }

    struct TeksBufferBufferReplaceTest
        : public ::testing::TestWithParam<BufferReplaceCase>
    {};
} // namespace

TEST_P(TeksBufferBufferReplaceTest, caseMatrix) {
    const BufferReplaceCase& testCase = GetParam();

    // validate testCase
    ASSERT_GE(testCase.initial.size(), testCase.minInitialSize);
    const Range range = testCase.makeRange(testCase.initial);

    const Offset::ValueType initialSize = testCase.initial.size();
    const bool expectedResult = range.end().raw() <= initialSize;
    const std::string expectedContent = expectedResult ?
        std::string(testCase.initial)
            .replace(range.start().raw(), range.size().raw(), testCase.content)
        : testCase.initial;

    Buffer buffer(testCase.initial);
    const std::string beforeContent = readAllString(buffer);
    const Bytes beforeSize = buffer.size();

    const bool result = buffer.replace(range, testCase.content);
    const std::string afterContent = readAllString(buffer);

    ASSERT_EQ(result, expectedResult);
    ASSERT_EQ(afterContent, expectedContent);

    if (!result) {
        ASSERT_EQ(afterContent, beforeContent);
        ASSERT_EQ(buffer.size(), beforeSize);
    } else {
        const Bytes expectedSize =
            beforeSize - range.size() + Bytes(testCase.content.size());
        ASSERT_EQ(buffer.size(), expectedSize);
    }

    ASSERT_EQ(buffer.size(), Bytes(expectedContent.size()));
}

INSTANTIATE_TEST_SUITE_P(
    teksBufferBuffer,
    TeksBufferBufferReplaceTest,
    ::testing::ValuesIn(makeReplaceCases()),
    paramCaseName<BufferReplaceCase>
);

TEST(teksBufferBuffer, replaceSequentialOperations) {
    Buffer buffer("Hello World");

    auto assertFailedReplaceNoMutation = [&](Range range, std::string_view content) {
        ASSERT_NO_MUTATION(buffer);
        ASSERT_FALSE(buffer.replace(range, content));
    };

    assertFailedReplaceNoMutation(makeRangeStartEmpty(12), "");

    ASSERT_TRUE(buffer.replace(makeRangeStartSize(5, 1), ""));
    ASSERT_EQ(readAllString(buffer), "HelloWorld");

    ASSERT_TRUE(buffer.replace(makeRangeStartEmpty(5), " "));
    ASSERT_EQ(readAllString(buffer), "Hello World");

    ASSERT_TRUE(buffer.replace(makeRangeStartSize(6, 5), "Earth"));
    ASSERT_EQ(readAllString(buffer), "Hello Earth");

    assertFailedReplaceNoMutation(makeRangeStartEnd(9, 20), "X");

    ASSERT_TRUE(buffer.replace(makeRangeSize(6), "Hi "));
    ASSERT_EQ(readAllString(buffer), "Hi Earth");

    ASSERT_TRUE(buffer.replace(makeRangeStartSize(3, 5), ""));
    ASSERT_EQ(readAllString(buffer), "Hi ");
}

TEST(teksBufferBuffer, replaceSingleNewlineWithContentContainingNoNewlines) {
    Buffer buffer("12\n56");
    ASSERT_TRUE(buffer.replace(makeRangeStartSize(2, 1), "34"));
    ASSERT_EQ(readAllString(buffer), "123456");
    ASSERT_EQ(buffer.size(), Bytes(6));
    ASSERT_FALSE(buffer.empty());
    ASSERT_EQ(buffer.lineCount(), 1);
    ASSERT_EQ(buffer.lineRange(0), makeRangeStartSize(0, 6));
    ASSERT_EQ(buffer.lineRange(1), std::nullopt);
    assertLineRangesSizesPlusNewlineCountEqualsContentSize(buffer);
}

TEST(teksBufferBuffer, replaceSingleNewlineWithContentContainingMixedNewlineStyles) {
    Buffer buffer("12\n9A");
    ASSERT_TRUE(buffer.replace(makeRangeStartSize(2, 1), "\r\n34\n56\r78\r"));
    ASSERT_EQ(readAllString(buffer), "12\n34\n56\n78\n9A");
    ASSERT_EQ(buffer.size(), Bytes(14));
    ASSERT_FALSE(buffer.empty());
    ASSERT_EQ(buffer.lineCount(), 5);
    ASSERT_EQ(calcLineRanges(buffer), (std::vector{
        makeRangeStartSize(0, 2),
        makeRangeStartSize(3, 2),
        makeRangeStartSize(6, 2),
        makeRangeStartSize(9, 2),
        makeRangeStartSize(12, 2)
    }));
    ASSERT_EQ(buffer.lineRange(5), std::nullopt);
    assertLineRangesSizesPlusNewlineCountEqualsContentSize(buffer);
}

TEST(teksBufferBuffer, replaceMultipleNewlinesWithContentContainingNoNewlines) {
    Buffer buffer("12\n34\n56\n78\n9A");
    ASSERT_TRUE(buffer.replace(makeRangeStartSize(5, 4), "56"));
    ASSERT_EQ(readAllString(buffer), "12\n345678\n9A");
    ASSERT_EQ(buffer.size(), Bytes(12));
    ASSERT_FALSE(buffer.empty());
    ASSERT_EQ(buffer.lineCount(), 3);
    ASSERT_EQ(calcLineRanges(buffer), (std::vector{
        makeRangeStartSize(0, 2),
        makeRangeStartSize(3, 6),
        makeRangeStartSize(10, 2)
    }));
    ASSERT_EQ(buffer.lineRange(3), std::nullopt);
    assertLineRangesSizesPlusNewlineCountEqualsContentSize(buffer);
}

TEST(teksBufferBuffer, replaceMultipleNewlinesWithContentContainingMixedNewlineStyles) {
    Buffer buffer("12\n34\n56\n78\n9A");
    ASSERT_TRUE(buffer.replace(makeRangeStartSize(5, 4), "\r\n\n56\n\r"));
    ASSERT_EQ(readAllString(buffer), "12\n34\n\n56\n\n78\n9A");
    ASSERT_EQ(buffer.size(), Bytes(16));
    ASSERT_FALSE(buffer.empty());
    ASSERT_EQ(buffer.lineCount(), 7);
    ASSERT_EQ(calcLineRanges(buffer), (std::vector{
        makeRangeStartSize(0, 2),
        makeRangeStartSize(3, 2),
        makeRangeStartSize(6, 0),
        makeRangeStartSize(7, 2),
        makeRangeStartSize(10, 0),
        makeRangeStartSize(11, 2),
        makeRangeStartSize(14, 2)
    }));
    ASSERT_EQ(buffer.lineRange(7), std::nullopt);
    assertLineRangesSizesPlusNewlineCountEqualsContentSize(buffer);
}

namespace { // readString
    struct BufferReadStringCase {
        BufferReadStringCase(Labeled<std::string> initial, LabeledRange makeRange)
            : testName(initial.label + "_and_" + makeRange.label)
            , initial(initial.value)
            , makeRange(makeRange.value)
            , minInitialSize(makeRange.minInitialSize) {}

        const std::string testName;
        const std::string initial;
        const MakeRange makeRange;
        const std::size_t minInitialSize;
    };

    void insertReadStringCasePermutations(
        std::vector<BufferReadStringCase>& cases,
        const std::vector<Labeled<std::string>>& initial,
        const std::vector<LabeledRange>& makeRange
    ) {
        for (const auto& i : initial) {
            for (const auto& r : makeRange) {
                cases.emplace_back(i, r);
            }
        }
    }

    std::vector<BufferReadStringCase> makeReadStringCases() {
        std::vector<BufferReadStringCase> cases;
        insertReadStringCasePermutations(
            cases,
            std::vector{initialEmptyLabeled},
            std::vector{
                rangeStartLen0Labeled,
                rangeStartToEndPlus1Labeled,
                rangeEndPlus1Len0Labeled,
                rangeEndPlus1ToEndPlus5Labeled
            }
        );
        insertReadStringCasePermutations(
            cases,
            std::vector{initialLen5Labeled},
            std::vector{
                rangeStartLen0Labeled,
                rangeMidpointLen0Labeled,
                rangeEndLen0Labeled,
                rangeEndPlus1Len0Labeled,
                rangeStartToMidpointLabeled,
                rangeStartToEndLabeled,
                rangeStartToEndPlus1Labeled,
                rangePlus1ToMinus1Labeled,
                rangePlus1ToEndLabeled,
                rangePlus1ToEndPlus1Labeled,
                rangeEndToEndPlus1Labeled,
                rangeEndPlus1ToEndPlus5Labeled
            }
        );
        cases.emplace_back(initialEmbeddedNulLen11Labeled, rangeStartToEndLabeled);

        return cases;
    }

    struct TeksBufferBufferReadStringTest
        : public ::testing::TestWithParam<BufferReadStringCase>
    {};
} // namespace

TEST_P(TeksBufferBufferReadStringTest, caseMatrix) {
    const BufferReadStringCase& testCase = GetParam();

    // validate testCase
    ASSERT_GE(testCase.initial.size(), testCase.minInitialSize);
    const Range range = testCase.makeRange(testCase.initial);

    const Offset::ValueType initialSize = testCase.initial.size();
    const bool expectedResult = range.end().raw() <= initialSize;

    Buffer buffer(testCase.initial);
    const std::string beforeContent = readAllString(buffer);
    const Bytes beforeSize = buffer.size();

    const std::optional<std::string> result = buffer.readString(range);

    if (expectedResult) {
        ASSERT_TRUE(result.has_value());
        const std::string expectedContent = testCase.initial.substr(
            range.start().raw(),
            range.size().raw()
        );
        ASSERT_EQ(*result, expectedContent);
        ASSERT_EQ(result->size(), range.size().raw());
    } else {
        ASSERT_EQ(result, std::nullopt);
    }

    ASSERT_EQ(readAllString(buffer), beforeContent);
    ASSERT_EQ(buffer.size(), beforeSize);
}

INSTANTIATE_TEST_SUITE_P(
    teksBufferBuffer,
    TeksBufferBufferReadStringTest,
    ::testing::ValuesIn(makeReadStringCases()),
    paramCaseName<BufferReadStringCase>
);

TEST(teksBufferBuffer, readStringWithRangeContainingNewlines) {
    Buffer buffer("12\n34\n56\n78");
    const auto result = buffer.readString(makeRangeStartSize(2, 7));
    ASSERT_EQ(result, "\n34\n56\n");
}
