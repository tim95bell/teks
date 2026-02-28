#include <teks/buffer/types.hpp>
#include <gtest/gtest.h>

using namespace teks::buffer;

TEST(teksBufferRange, defaultConstruction) {
    Range r;
    ASSERT_EQ(r.start(), Offset(0));
    ASSERT_EQ(r.end(), Offset(0));
    ASSERT_EQ(r.size(), Bytes(0));
}

TEST(teksBufferRange, startEndTryMake) {
    auto result = Range::tryMake(Offset(3), Offset(5));
    ASSERT_TRUE(result.has_value());
    Range r = result.value();
    ASSERT_EQ(r.start(), Offset(3));
    ASSERT_EQ(r.end(), Offset(5));
    ASSERT_EQ(r.size(), Bytes(2));
    ASSERT_EQ(Range::tryMake(Offset(5), Offset(4)), std::nullopt);
}

TEST(teksBufferRange, startSizeTryMake) {
    auto result = Range::tryMake(Offset(3), Bytes(5));
    ASSERT_TRUE(result.has_value());
    Range r = result.value();
    ASSERT_EQ(r.start(), Offset(3));
    ASSERT_EQ(r.end(), Offset(8));
    ASSERT_EQ(r.size(), Bytes(5));
    ASSERT_EQ(Range::tryMake(Offset(Bytes::maxValue()), Bytes(1)), std::nullopt);
}

TEST(teksBufferRange, startEndMakeUnchecked) {
    Range r = Range::makeUnchecked(Offset(3), Offset(5));
    ASSERT_EQ(r.start(), Offset(3));
    ASSERT_EQ(r.end(), Offset(5));
    ASSERT_EQ(r.size(), Bytes(2));
    ASSERT_DEBUG_DEATH({(void)Range::makeUnchecked(Offset(5), Offset(4));}, ".*");
}

TEST(teksBufferRange, startSizeMakeUnchecked) {
    Range r = Range::makeUnchecked(Offset(3), Bytes(5));
    ASSERT_EQ(r.start(), Offset(3));
    ASSERT_EQ(r.end(), Offset(8));
    ASSERT_EQ(r.size(), Bytes(5));
    ASSERT_DEBUG_DEATH({(void)Range::makeUnchecked(Offset(Bytes::maxValue()), Bytes(1));}, ".*");
}

TEST(teksBufferRange, endConstruction) {
    Range r(Offset(3));
    ASSERT_EQ(r.start(), Offset(0));
    ASSERT_EQ(r.end(), Offset(3));
    ASSERT_EQ(r.size(), Bytes(3));
}

TEST(teksBufferRange, sizeConstruction) {
    Range r(Bytes(3));
    ASSERT_EQ(r.start(), Offset(0));
    ASSERT_EQ(r.end(), Offset(3));
    ASSERT_EQ(r.size(), Bytes(3));
}

TEST(teksBufferRange, comparison) {
    const Range a(Offset(1));
    const Range b(Offset(2));
    const Range c(Offset(1));

    ASSERT_EQ(a, c);
    ASSERT_NE(a, b);
    ASSERT_NE(b, c);
}
