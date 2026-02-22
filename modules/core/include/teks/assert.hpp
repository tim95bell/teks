#pragma once

#include <boost/assert.hpp>
#include <csignal>
#include <cstdio>
#include <cstdlib>

#ifndef TEKS_DEBUG
#error "TEKS_DEBUG must be defined by build configuration"
#endif

namespace teks::detail {
    inline void debug_break() noexcept {
#if TEKS_DEBUG
#if defined(_MSC_VER)
        __debugbreak();
#elif defined(__has_builtin)
#if __has_builtin(__builtin_debugtrap)
        __builtin_debugtrap();
#elif __has_builtin(__builtin_trap)
        __builtin_trap();
#elif defined(SIGTRAP)
        std::raise(SIGTRAP);
#else
        std::abort();
#endif
#elif defined(SIGTRAP)
        std::raise(SIGTRAP);
#else
        std::abort();
#endif
#endif
    }

    inline void log_require_message(const char* message) noexcept {
#if !TEKS_DEBUG
        const char* safe_message = message == nullptr ? "(null)" : message;
        std::fprintf(stderr, "TEKS_REQUIRE failed: %s\n", safe_message);
        std::fflush(stderr);
#endif
    }
} // namespace teks::detail

#define TEKS_REQUIRE(cond) \
    do { \
        if (!(cond)) { \
            ::teks::detail::debug_break(); \
            BOOST_ASSERT_MSG(false, #cond); \
            std::abort(); \
        } \
    } while (false)

#define TEKS_REQUIRE_MSG(cond, msg) \
    do { \
        if (!(cond)) { \
            ::teks::detail::log_require_message(msg); \
            ::teks::detail::debug_break(); \
            BOOST_ASSERT_MSG(false, msg); \
            std::abort(); \
        } \
    } while (false)

#if TEKS_DEBUG
#define TEKS_ASSERT(cond) \
    do { \
        if (!(cond)) { \
            ::teks::detail::debug_break(); \
            BOOST_ASSERT_MSG(false, #cond); \
            std::abort(); \
        } \
    } while (false)

#define TEKS_ASSERT_MSG(cond, msg) \
    do { \
        if (!(cond)) { \
            ::teks::detail::debug_break(); \
            BOOST_ASSERT_MSG(false, msg); \
            std::abort(); \
        } \
    } while (false)
#else
#define TEKS_ASSERT(cond) ((void)0)
#define TEKS_ASSERT_MSG(cond, msg) ((void)0)
#endif
