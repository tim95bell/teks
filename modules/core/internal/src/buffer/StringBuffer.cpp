#include <teks/buffer/internal/StringBuffer.hpp>
#include <optional>

namespace teks::buffer {
    StringBuffer::StringBuffer(std::string initialValue) : value_(std::move(initialValue)) {}

    Bytes StringBuffer::size() const {
        return Bytes(value_.size());
    }

    bool StringBuffer::empty() const {
        return value_.empty();
    }

    bool StringBuffer::insert(Offset at, std::string_view content) {
        if (at.raw() <= value_.size()) {
            value_.insert(at.raw(), content);
            return true;
        }
        return false;
    }

    bool StringBuffer::erase(Range range) {
        if (range.end().raw() <= value_.size()) {
            value_.erase(range.start().raw(), range.size().raw());
            return true;
        }

        return false;
    }

    bool StringBuffer::replace(Range range, std::string_view content) {
        if (range.end().raw() <= value_.size()) {
            value_.replace(range.start().raw(), range.size().raw(), content);
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
} // namespace teks::buffer
