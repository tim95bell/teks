#include "Document.hpp"
#include <teks/buffer/Buffer.hpp>
#include <teks/buffer/NewlineStyleSet.hpp>
#include <utility>
#include <fstream>
#include <optional>

namespace teks::editor {
    std::optional<Document> Document::openFile(std::filesystem::path path) {
        std::ifstream file(path, std::ios::binary);
        if (!file) {
            return std::nullopt;
        }

        auto [buffer, newlineStyleSet] = buffer::Buffer::fromRawText(
            std::string(
                (std::istreambuf_iterator<char>(file)),
                std::istreambuf_iterator<char>()
            )
        );

        return std::optional<Document>(
            Document(
                std::move(buffer),
                std::move(path),
                newlineStyleSet
            )
        );
    }

    Document::Document()
        : Document(buffer::Buffer())
    {}

    Document::Document(teks::buffer::Buffer buffer)
        : Document(std::move(buffer), std::filesystem::path())
    {}

    Document::Document(teks::buffer::Buffer buffer, std::filesystem::path path)
        : Document(
            std::move(buffer),
            std::move(path),
            buffer::NewlineStyleSet::of({buffer::NewlineStyleSet::Style::Lf})
        )
    {}

    Document::Document(
        teks::buffer::Buffer buffer,
        std::filesystem::path path,
        buffer::NewlineStyleSet newLineStyleSet
    ) : buffer_(std::move(buffer))
        , path_(std::move(path))
        , newLineStyleSet_(newLineStyleSet)
    {}

    teks::buffer::Buffer& Document::buffer() {
        return buffer_;
    }

    const teks::buffer::Buffer& Document::buffer() const {
        return buffer_;
    }
}
