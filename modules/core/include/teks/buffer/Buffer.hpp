#pragma once

#include <string>
#include <string_view>

#ifndef TEKS_BUFFER_IMPL_STRING
#error "TEKS_BUFFER_IMPL_STRING must be defined by build configuration"
#endif

#if TEKS_BUFFER_IMPL_STRING
#include <teks/buffer/internal/StringBuffer.hpp>
#else
#error "Unknown buffer implementation selected"
#endif

namespace teks::buffer {
#if TEKS_BUFFER_IMPL_STRING
    using Buffer = StringBuffer;
#else
#error "Unknown buffer implementation selected"
#endif

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
