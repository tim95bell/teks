#pragma once

#include <teks/buffer/Buffer.hpp>
#include <teks/buffer/NewlineStyleSet.hpp>
#include <optional>
#include <filesystem>

namespace teks::editor {
    struct Document {
        static std::optional<Document> openFile(std::filesystem::path path);

        Document();
        Document(teks::buffer::Buffer);
        Document(teks::buffer::Buffer, std::filesystem::path);

        teks::buffer::Buffer& buffer();
        const teks::buffer::Buffer& buffer() const;

    private:
        teks::buffer::Buffer buffer_;
        std::filesystem::path path_;
        buffer::NewlineStyleSet newLineStyleSet_;

        Document(teks::buffer::Buffer, std::filesystem::path, buffer::NewlineStyleSet);
    };
}
