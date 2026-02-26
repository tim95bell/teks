#pragma once

#include <concepts>
#include <optional>
#include <string>
#include <string_view>
#include <teks/buffer/types.hpp>

#ifndef TEKS_BUFFER_IMPL_STRING
#error "TEKS_BUFFER_IMPL_STRING must be defined by build configuration"
#endif

#if TEKS_BUFFER_IMPL_STRING
#include <teks/buffer/internal/StringBuffer.hpp>
#else
#error "Unknown buffer implementation selected"
#endif

namespace teks::buffer {
    namespace concepts {
        template <typename T>
        concept Buffer = requires(T buffer, const T const_buffer, Offset at, Range range, std::string_view content) {
            { const_buffer.size() } -> std::same_as<Bytes>;
            { const_buffer.empty() } -> std::same_as<bool>;

            // `insert` succeeds if `at` is in `[0, size()]`
            // On success `content` is inserted at `at`; on failure no changes are made.
            // Returns success.
            { buffer.insert(at, content) } -> std::same_as<bool>;

            // `erase` succeeds if `range` is in `[0, size()]`.
            // On success `[range.start(), range.end())` is removed; on failure no changes are made.
            // Returns success.
            { buffer.erase(range) } -> std::same_as<bool>;

            // `replace` succeeds if `range` is in `[0, size()]`.
            // On success `[range.start(), range.end())` is replaced with `content`; on failure no changes are made.
            // Returns success.
            { buffer.replace(range, content) } -> std::same_as<bool>;

            // `read_string` succeeds if `range` is in `[0, size()]`.
            // On success it returns the bytes in `range`; on failure it returns `std::nullopt`.
            { const_buffer.read_string(range) } -> std::same_as<std::optional<std::string>>;
        };
    } // namespace concepts

#if TEKS_BUFFER_IMPL_STRING
    using Buffer = StringBuffer;
#else
#error "Unknown buffer implementation selected"
#endif

    static_assert(concepts::Buffer<Buffer>, "Selected buffer implementation must satisfy teks::buffer::concepts::Buffer");

    [[nodiscard]] inline std::string read_all_string(const Buffer& buffer) {
        return buffer.read_string(Range(buffer.size())).value();
    }

    inline bool insert_start(Buffer& buffer, std::string_view content) {
        return buffer.insert(Offset(0), content);
    }

    inline bool insert_end(Buffer& buffer, std::string_view content) {
        return buffer.insert(Offset(buffer.size()), content);
    }

    [[nodiscard]] inline Range range(const Buffer& buffer) {
        return Range(buffer.size());
    }
} // namespace teks::buffer
