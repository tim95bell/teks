#include <teks/buffer/NewlineStyleSet.hpp>

namespace teks::buffer {
    NewlineStyleSet NewlineStyleSet::of(std::initializer_list<Style> styles) {
        NewlineStyleSet result;
        for (Style style : styles) {
            result.add(style);
        }
        return result;
    }

    bool NewlineStyleSet::has(NewlineStyleSet::Style style) const {
        return (bits_ & static_cast<Type>(style)) != 0;
    }

    bool NewlineStyleSet::hasExactly(std::initializer_list<Style> styles) const {
        Type expectedBits = 0;
        for (Style style : styles) {
            expectedBits |= static_cast<Type>(style);
        }
        return bits_ == expectedBits;
    }

    void NewlineStyleSet::add(Style style) {
        bits_ = bits_ | static_cast<Type>(style);
    }
}
