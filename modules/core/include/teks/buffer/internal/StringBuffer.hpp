#pragma once

#include <string_view>
#include <string>
#include <optional>
#include <teks/types.hpp>
#include <teks/buffer/types.hpp>

namespace teks::buffer {
    struct StringBuffer {
        StringBuffer() = default;
        StringBuffer(std::string);
        StringBuffer(const StringBuffer&) = default;
        StringBuffer(StringBuffer&&) noexcept = default;

        ~StringBuffer() = default;

        StringBuffer& operator=(const StringBuffer&) = default;
        StringBuffer& operator=(StringBuffer&&) noexcept = default;

        [[nodiscard]] Bytes size() const;
        [[nodiscard]] bool empty() const;
        // succeeds if at <= size
        bool insert(Offset at, std::string_view content);
        // succeeds if range end <= size
        bool erase(Range range);
        // succeeds if range end <= size
        bool replace(Range range, std::string_view content);
        // succeeds if range end <= size
        [[nodiscard]] std::optional<std::string> read_string(Range range) const;

    private:
        std::string value;
    };
} // namespace teks::buffer
