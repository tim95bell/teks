#include <teks/buffer/internal/StringBuffer.hpp>
#include <optional>

namespace teks::buffer {
    StringBuffer::StringBuffer(std::string value) : value(std::move(value)) {}

    Bytes StringBuffer::size() const {
        return Bytes(value.size());
    }

    bool StringBuffer::empty() const {
        return value.empty();
    }

    bool StringBuffer::insert(Offset at, std::string_view content) {
        if (at.raw() <= value.size()) {
            value.insert(at.raw(), content);
            return true;
        }
        return false;
    }

    bool StringBuffer::erase(Range range) {
        if (range.end().raw() <= value.size()) {
            value.erase(range.start().raw(), range.size().raw());
            return true;
        }

        return false;
    }

    bool StringBuffer::replace(Range range, std::string_view content) {
        if (range.end().raw() <= value.size()) {
            value.replace(range.start().raw(), range.size().raw(), content);
            return true;
        }

        return false;
    }

    std::optional<std::string> StringBuffer::read_string(Range range) const {
        if (range.end().raw() <= value.size()) {
            return value.substr(range.start().raw(), range.size().raw());
        }
        return std::nullopt;
    }
} // namespace teks::buffer
