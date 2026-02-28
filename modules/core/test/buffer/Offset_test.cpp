#include <teks/buffer/types.hpp>
#include <gtest/gtest.h>
#include <functional>
#include <type_traits>

using namespace teks::buffer;

static_assert(std::is_invocable_v<std::less<>, Offset, Bytes>);
static_assert(std::is_invocable_v<std::less_equal<>, Offset, Bytes>);
static_assert(std::is_invocable_v<std::greater<>, Offset, Bytes>);
static_assert(std::is_invocable_v<std::greater_equal<>, Offset, Bytes>);

static_assert(!std::is_invocable_v<std::less<>, Bytes, Offset>);
static_assert(!std::is_invocable_v<std::less_equal<>, Bytes, Offset>);
static_assert(!std::is_invocable_v<std::greater<>, Bytes, Offset>);
static_assert(!std::is_invocable_v<std::greater_equal<>, Bytes, Offset>);

TEST(teksBufferOffset, comparison) {
    ASSERT_EQ(Offset(0), Offset(0));
    ASSERT_NE(Offset(0), Offset(1));
    ASSERT_LT(Offset(1), Offset(2));
    ASSERT_GT(Offset(3), Offset(2));
}

TEST(teksBufferOffset, additionWithBytes) {
    ASSERT_EQ(Offset(3) + Bytes(2), Offset(5));
    ASSERT_EQ(Offset(0) + Bytes(2), Offset(2));
    ASSERT_EQ(Offset(4) + Bytes(0), Offset(4));
    ASSERT_NO_FATAL_FAILURE({
        Offset o(Bytes::maxValue());
        ASSERT_EQ(o.raw(), Bytes::maxValue());
    });
    ASSERT_DEBUG_DEATH({(void)(Offset(Bytes::maxValue()) + Bytes(1));}, ".*");
}

TEST(teksBufferOffset, subtractionWithBytes) {
    ASSERT_EQ(Offset(3) - Bytes(2), Offset(1));
    ASSERT_EQ(Offset(3) - Bytes(3), Offset(0));
    ASSERT_EQ(Offset(4) - Bytes(0), Offset(4));
    ASSERT_DEBUG_DEATH({(void)(Offset(5) - Bytes(6));}, ".*");
}

TEST(teksBufferOffset, subtraction) {
    ASSERT_EQ(Offset(3) - Offset(2), Bytes(1));
    ASSERT_EQ(Offset(3) - Offset(3), Bytes(0));
    ASSERT_EQ(Offset(4) - Offset(0), Bytes(4));
    ASSERT_DEBUG_DEATH({(void)(Offset(5) - Offset(6));}, ".*");
}

TEST(teksBufferOffset, comparisonWithBytesRhs) {
    ASSERT_TRUE(Offset(1) < Bytes(2));
    ASSERT_TRUE(Offset(2) <= Bytes(2));
    ASSERT_TRUE(Offset(3) > Bytes(2));
    ASSERT_TRUE(Offset(2) >= Bytes(2));
}
