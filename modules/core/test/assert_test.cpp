#include <gtest/gtest.h>
#include <teks/assert.hpp>

namespace teks {
    TEST(teksAssertMacros, compileSmoke) {
        int value = 1;
        TEKS_REQUIRE(value == 1);
        TEKS_REQUIRE_MSG(value == 1, "require with message");
#if TEKS_DEBUG
        TEKS_ASSERT(value == 1);
        TEKS_ASSERT_MSG(value == 1, "assert with message");
#else
        int sideEffect = 0;
        TEKS_ASSERT_MSG(++sideEffect == 1, "must not evaluate");
        ASSERT_EQ(sideEffect, 0);
#endif
    }

    TEST(teksAssertMacros, requireFailsInAllBuilds) {
        ASSERT_DEATH({ TEKS_REQUIRE_MSG(false, "require fail path"); }, ".*");
    }

#if TEKS_DEBUG
    TEST(teksAssertMacros, assertFailsInDebug) {
        ASSERT_DEATH({ TEKS_ASSERT_MSG(false, "assert fail path"); }, ".*");
    }
#else
    TEST(teksAssertMacros, assertIsNoopInRelease) {
        int sideEffect = 0;
        TEKS_ASSERT_MSG(++sideEffect == 1, "must not evaluate");
        ASSERT_EQ(sideEffect, 0);
    }
#endif
}
