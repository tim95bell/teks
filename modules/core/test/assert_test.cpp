#include <gtest/gtest.h>
#include <teks/assert.hpp>

namespace teks {
    TEST(teks_assert_macros, compileSmoke) {
        int value = 1;
        TEKS_REQUIRE(value == 1);
        TEKS_REQUIRE_MSG(value == 1, "require with message");
#if TEKS_DEBUG
        TEKS_ASSERT(value == 1);
        TEKS_ASSERT_MSG(value == 1, "assert with message");
#else
        int side_effect = 0;
        TEKS_ASSERT_MSG(++side_effect == 1, "must not evaluate");
        ASSERT_EQ(side_effect, 0);
#endif
    }

    TEST(teks_assert_macros, requireFailsInAllBuilds) {
        ASSERT_DEATH({ TEKS_REQUIRE_MSG(false, "require fail path"); }, ".*");
    }

#if TEKS_DEBUG
    TEST(teks_assert_macros, assertFailsInDebug) {
        ASSERT_DEATH({ TEKS_ASSERT_MSG(false, "assert fail path"); }, ".*");
    }
#else
    TEST(teks_assert_macros, assertIsNoopInRelease) {
        int side_effect = 0;
        TEKS_ASSERT_MSG(++side_effect == 1, "must not evaluate");
        ASSERT_EQ(side_effect, 0);
    }
#endif
}
