#include <teks/buffer/types.hpp>
#include <gtest/gtest.h>

using namespace teks::buffer;

TEST(teks_buffer_Bytes, comparison) {
    ASSERT_EQ(Bytes(0), Bytes(0));
    ASSERT_NE(Bytes(0), Bytes(1));
    ASSERT_LT(Bytes(1), Bytes(2));
    ASSERT_GT(Bytes(3), Bytes(2));
}

TEST(teks_buffer_Bytes, addition) {
    ASSERT_EQ(Bytes(1) + Bytes(2), Bytes(3));
    ASSERT_EQ(Bytes(3) + Bytes(0), Bytes(3));
    ASSERT_EQ(Bytes(0) + Bytes(3), Bytes(3));
    ASSERT_NO_FATAL_FAILURE({
        Bytes b(Bytes::max_value());
        ASSERT_EQ(b.raw(), Bytes::max_value());
    });
    ASSERT_DEBUG_DEATH({(void)(Bytes(Bytes::max_value()) + Bytes(1));}, ".*");
}

TEST(teks_buffer_Bytes, subtraction) {
    ASSERT_EQ(Bytes(5) - Bytes(3), Bytes(2));
    ASSERT_EQ(Bytes(5) - Bytes(5), Bytes(0));
    ASSERT_EQ(Bytes(3) - Bytes(0), Bytes(3));
    ASSERT_DEBUG_DEATH({(void)(Bytes(5) - Bytes(6));}, ".*");
}
