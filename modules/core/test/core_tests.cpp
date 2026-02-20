#include <teks/teks.hpp>
#include <gtest/gtest.h>

namespace teks {
    TEST(engine, demoTest) {
        ASSERT_EQ(test(),  "Hello from Teks");
    }
}
