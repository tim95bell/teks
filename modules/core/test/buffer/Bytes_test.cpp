#include <teks/buffer/types.hpp>
#include <gtest/gtest.h>

using namespace teks::buffer;

TEST(teksBufferBytes, comparison) {
    ASSERT_EQ(Bytes(0), Bytes(0));
    ASSERT_NE(Bytes(0), Bytes(1));
    ASSERT_LT(Bytes(1), Bytes(2));
    ASSERT_GT(Bytes(3), Bytes(2));
}

TEST(teksBufferBytes, addition) {
    ASSERT_EQ(Bytes(1) + Bytes(2), Bytes(3));
    ASSERT_EQ(Bytes(3) + Bytes(0), Bytes(3));
    ASSERT_EQ(Bytes(0) + Bytes(3), Bytes(3));
    ASSERT_NO_FATAL_FAILURE({
        Bytes b(Bytes::maxValue());
        ASSERT_EQ(b.raw(), Bytes::maxValue());
    });
    ASSERT_DEBUG_DEATH({(void)(Bytes(Bytes::maxValue()) + Bytes(1));}, ".*");
}

TEST(teksBufferBytes, subtraction) {
    ASSERT_EQ(Bytes(5) - Bytes(3), Bytes(2));
    ASSERT_EQ(Bytes(5) - Bytes(5), Bytes(0));
    ASSERT_EQ(Bytes(3) - Bytes(0), Bytes(3));
    ASSERT_DEBUG_DEATH({(void)(Bytes(5) - Bytes(6));}, ".*");
}
