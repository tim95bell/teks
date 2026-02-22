#include <teks/buffer/types.hpp>
#include <gtest/gtest.h>

using namespace teks::buffer;

TEST(teks_buffer_Range, default_construction) {
    Range r;
    ASSERT_EQ(r.start(), Offset(0));
    ASSERT_EQ(r.end(), Offset(0));
    ASSERT_EQ(r.size(), Bytes(0));
}

TEST(teks_buffer_Range, start_end_try_make) {
    auto result = Range::try_make(Offset(3), Offset(5));
    ASSERT_TRUE(result.has_value());
    Range r = result.value();
    ASSERT_EQ(r.start(), Offset(3));
    ASSERT_EQ(r.end(), Offset(5));
    ASSERT_EQ(r.size(), Bytes(2));
    ASSERT_EQ(Range::try_make(Offset(5), Offset(4)), std::nullopt);
}

TEST(teks_buffer_Range, start_size_try_make) {
    auto result = Range::try_make(Offset(3), Bytes(5));
    ASSERT_TRUE(result.has_value());
    Range r = result.value();
    ASSERT_EQ(r.start(), Offset(3));
    ASSERT_EQ(r.end(), Offset(8));
    ASSERT_EQ(r.size(), Bytes(5));
    ASSERT_EQ(Range::try_make(Offset(Bytes::max_value()), Bytes(1)), std::nullopt);
}

TEST(teks_buffer_Range, start_end_make_unchecked) {
    Range r = Range::make_unchecked(Offset(3), Offset(5));
    ASSERT_EQ(r.start(), Offset(3));
    ASSERT_EQ(r.end(), Offset(5));
    ASSERT_EQ(r.size(), Bytes(2));
    ASSERT_DEBUG_DEATH({(void)Range::make_unchecked(Offset(5), Offset(4));}, ".*");
}

TEST(teks_buffer_Range, start_size_make_unchecked) {
    Range r = Range::make_unchecked(Offset(3), Bytes(5));
    ASSERT_EQ(r.start(), Offset(3));
    ASSERT_EQ(r.end(), Offset(8));
    ASSERT_EQ(r.size(), Bytes(5));
    ASSERT_DEBUG_DEATH({(void)Range::make_unchecked(Offset(Bytes::max_value()), Bytes(1));}, ".*");
}

TEST(teks_buffer_Range, end_construction) {
    Range r(Offset(3));
    ASSERT_EQ(r.start(), Offset(0));
    ASSERT_EQ(r.end(), Offset(3));
    ASSERT_EQ(r.size(), Bytes(3));
}

TEST(teks_buffer_Range, size_construction) {
    Range r(Bytes(3));
    ASSERT_EQ(r.start(), Offset(0));
    ASSERT_EQ(r.end(), Offset(3));
    ASSERT_EQ(r.size(), Bytes(3));
}

TEST(teks_buffer_Range, comparison) {
    const Range a(Offset(1));
    const Range b(Offset(2));
    const Range c(Offset(1));

    ASSERT_EQ(a, c);
    ASSERT_NE(a, b);
    ASSERT_NE(b, c);
}
