#include <teks/buffer/internal/StringBuffer.hpp>
#include <optional>
#include <vector>
#include <algorithm>
#include <tuple>

namespace {
    inline std::tuple<
        std::string,
        std::vector<teks::buffer::Offset>,
        teks::buffer::NewlineStyleSet
    > normalizeLineEndings(std::string s) {
        std::vector<teks::buffer::Offset> lineStarts;
        lineStarts.push_back(teks::buffer::Offset(0));
        teks::buffer::NewlineStyleSet newlineStyleSet;
        constexpr const char* newlineChars = "\r\n";
        std::size_t newlineIndex = s.find_first_of(newlineChars);
        if (newlineIndex == std::string::npos) {
            return std::tuple(std::move(s), std::move(lineStarts), newlineStyleSet);
        }

        std::string result;
        result.reserve(s.size());
        std::size_t fromIndex = 0;
        do {
            result.append(s, fromIndex, newlineIndex - fromIndex);
            result.push_back('\n');
            lineStarts.push_back(teks::buffer::Offset(result.size()));

            const std::size_t afterNewlineIndex = newlineIndex + 1;
            if (s[newlineIndex] == '\r') {
                if (afterNewlineIndex < s.size() && s[afterNewlineIndex] == '\n') {
                    newlineStyleSet.add(teks::buffer::NewlineStyleSet::Style::Crlf);
                    fromIndex = afterNewlineIndex + 1;
                } else {
                    newlineStyleSet.add(teks::buffer::NewlineStyleSet::Style::Cr);
                    fromIndex = afterNewlineIndex;
                }
            } else {
                newlineStyleSet.add(teks::buffer::NewlineStyleSet::Style::Lf);
                fromIndex = afterNewlineIndex;
            }

            newlineIndex = s.find_first_of(newlineChars, fromIndex);
        } while (newlineIndex != std::string::npos);
        result.append(s, fromIndex, s.size() - fromIndex);
        return std::tuple(std::move(result), std::move(lineStarts), newlineStyleSet);
    }
}

namespace teks::buffer {
    std::pair<StringBuffer, NewlineStyleSet> StringBuffer::fromRawText(std::string text) {
        auto [content, lineStarts, newlineStyleSet] = normalizeLineEndings(std::move(text));
        return std::pair(StringBuffer(std::move(content), std::move(lineStarts)), newlineStyleSet);
    }

    StringBuffer::StringBuffer(std::string text)
        : StringBuffer(fromRawText(text).first)
    {}

    StringBuffer::StringBuffer(std::string text, std::vector<Offset> lineStarts)
        : value_(std::move(text))
        , lineStarts_(std::move(lineStarts))
    {}

    Bytes StringBuffer::size() const {
        return Bytes(value_.size());
    }

    bool StringBuffer::empty() const {
        return value_.empty();
    }

    usize StringBuffer::firstLineIndexAfterOffset(Offset at) const {
        for (usize i = 1; i < lineStarts_.size(); ++i) {
            if (lineStarts_[i] > at) {
                return i;
            }
        }

        return lineStarts_.size();
    }

    bool StringBuffer::insert(Offset at, std::string_view content) {
        if (at.raw() <= value_.size()) {
            std::string contentString(content);
            auto [normalizedContent, contentLineStarts, _]
                = normalizeLineEndings(contentString);
            for (Offset& lineStart : contentLineStarts) {
                lineStart += Bytes(at);
            }
            const usize insertLineStartsAt = firstLineIndexAfterOffset(at);
            for (usize i = insertLineStartsAt; i < lineStarts_.size(); ++i) {
                lineStarts_[i] += Bytes(normalizedContent.size());
            }
            lineStarts_.insert(
                lineStarts_.begin() + static_cast<decltype(lineStarts_)::difference_type>(insertLineStartsAt),
                // +1 to drop the initial lineStart that is not a newline
                contentLineStarts.begin() + 1,
                contentLineStarts.end()
            );
            value_.insert(at.raw(), normalizedContent);
            return true;
        }
        return false;
    }

    bool StringBuffer::erase(Range range) {
        if (range.end().raw() <= value_.size()) {
            const Offset start = range.start();
            const Offset end = range.end();
            const Bytes size = range.size();
            const auto left = std::find_if(
                lineStarts_.begin(),
                lineStarts_.end(),
                // x is the line start, the newline is at x - 1 (as long as its not the fist line start), that is why > is used rather than >=
                [start](Offset x) { return x > start; }
            );
            if (left != lineStarts_.end()) {
                const auto rLeft = std::make_reverse_iterator(left);
                const auto rRight = std::find_if(
                    lineStarts_.rbegin(),
                    rLeft,
                    // x is the line start, the newline is at x - 1 (as long as its not the fist line start), that is why <= is used rather than <
                    [end](Offset x) { return x <= end; }
                );
                if (rRight != rLeft) {
                    const auto right = rRight.base();
                    const auto firstAfterErase = lineStarts_.erase(left, right);
                    for (auto i = firstAfterErase; i != lineStarts_.end(); ++i) {
                        *i -= size;
                    }
                } else {
                    for (auto i = left; i != lineStarts_.end(); ++i) {
                        *i -= size;
                    }
                }
            }
            value_.erase(range.start().raw(), range.size().raw());
            return true;
        }

        return false;
    }

    bool StringBuffer::replace(Range range, std::string_view content) {
        if (range.end().raw() <= value_.size()) {
            erase(range);
            insert(range.start(), content);
            return true;
        }

        return false;
    }

    std::optional<std::string> StringBuffer::readString(Range range) const {
        if (range.end().raw() <= value_.size()) {
            return value_.substr(range.start().raw(), range.size().raw());
        }
        return std::nullopt;
    }

    usize StringBuffer::lineCount() const {
        return lineStarts_.size();
    }

    std::optional<buffer::Range> StringBuffer::lineRange(usize line) const {
        // `lineStarts_.size() - 1` is safe because `lineStarts_.size()` >= 1
        if (line < lineStarts_.size() - 1) {
            // not the last line
            const Offset start = lineStarts_[line];
            // minus 1 gets to the newline, range end is exclusive so that newline is excluded
            const Offset end = lineStarts_[line + 1] - Bytes(1);
            return Range::makeUnchecked(start, end);
        }

        if (line < lineStarts_.size()) {
            // the last line
            const Offset start = lineStarts_[line];
            const auto end = Offset(size());
            return Range::makeUnchecked(start, end);
        }

        return std::nullopt;
    }
} // namespace teks::buffer
