#pragma once

#include <teks/types.hpp>
#include <initializer_list>

namespace teks::buffer {
    struct NewlineStyleSet {
        using Type = u8;

        enum class Style : Type {
            Cr = 1 << 0,
            Lf = 1 << 1,
            Crlf = 1 << 2
        };

        static NewlineStyleSet of(std::initializer_list<Style> styles);

        bool has(Style) const;
        bool hasExactly(std::initializer_list<Style> styles) const;
        void add(Style);

    private:
        Type bits_{0};
    };
}
