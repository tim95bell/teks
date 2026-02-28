#pragma once

#include <string_view>
#include <string>
#include <optional>
#include <teks/types.hpp>
#include <teks/buffer/types.hpp>

namespace teks::buffer {
    // Test-only reference implementation for contract validation.
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
        bool insert(Offset at, std::string_view content);
        bool erase(Range range);
        bool replace(Range range, std::string_view content);
        [[nodiscard]] std::optional<std::string> readString(Range range) const;

    private:
        std::string value_;
    };
} // namespace teks::buffer
