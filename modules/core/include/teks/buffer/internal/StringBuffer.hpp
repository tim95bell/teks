#pragma once

#include <teks/types.hpp>
#include <teks/buffer/types.hpp>
#include <teks/buffer/NewlineStyleSet.hpp>
#include <string_view>
#include <string>
#include <optional>
#include <vector>

namespace teks::buffer {
    // Buffer text is expected to be LF-normalized, and mutating methods must preserve that invariant
    struct StringBuffer {
        static std::pair<StringBuffer, NewlineStyleSet> fromRawText(std::string);

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
        [[nodiscard]] usize lineCount() const;
        [[nodiscard]] std::optional<Range> lineRange(usize line) const;

    private:
        std::string value_;
        std::vector<Offset> lineStarts_{1, Offset(0)};

        StringBuffer(std::string text, std::vector<Offset> lineStarts);

        usize firstLineIndexAfterOffset(Offset at) const;
    };
} // namespace teks::buffer
