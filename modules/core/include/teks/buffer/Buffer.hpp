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
        // Buffer newlines must be normalized to LF style, and this must be maintained by all mutating methods
        template <typename T>
        concept Buffer = requires(
            T buffer,
            const T constBuffer,
            Offset at,
            Range range,
            std::string_view content,
            u64 line
        ) {
            { constBuffer.size() } -> std::same_as<Bytes>;
            { constBuffer.empty() } -> std::same_as<bool>;

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

            // `readString` succeeds if `range` is in `[0, size()]`.
            // On success it returns the bytes in `range`; on failure it returns `std::nullopt`.
            { constBuffer.readString(range) } -> std::same_as<std::optional<std::string>>;

            { constBuffer.lineCount() } -> std::same_as<usize>;

            { constBuffer.lineRange(line) } -> std::same_as<std::optional<Range>>;
        };
    } // namespace concepts

#if TEKS_BUFFER_IMPL_STRING
    using Buffer = StringBuffer;
#else
#error "Unknown buffer implementation selected"
#endif

    static_assert(
        concepts::Buffer<Buffer>,
        "Selected buffer implementation must satisfy teks::buffer::concepts::Buffer"
    );

    [[nodiscard]] inline std::string readAllString(const Buffer& buffer) {
        return buffer.readString(Range(buffer.size())).value();
    }

    inline bool insertStart(Buffer& buffer, std::string_view content) {
        return buffer.insert(Offset(0), content);
    }

    inline bool insertEnd(Buffer& buffer, std::string_view content) {
        return buffer.insert(Offset(buffer.size()), content);
    }

    [[nodiscard]] inline Range range(const Buffer& buffer) {
        return Range(buffer.size());
    }

    [[nodiscard]] inline std::optional<std::string> readLine(const Buffer& buffer, usize line) {
        const auto range = buffer.lineRange(line);
        if (range.has_value()) {
            return buffer.readString(range.value());
        }
        return std::nullopt;
    }
} // namespace teks::buffer
